#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <sstream>
#include "ctpl_stl.h"
using namespace std;

std::string exec(const char *cmd)
{
    // f = popen("exec bash -c 'shopt -s dotglob; cat -- *'", "r");
    // [200];
    // char *prefix = "exec bash -c \'";
    // char *finalcmd = strcat(prefix, cmd);
    // finalcmd = strcat(prefix, cmd);
    cout<<"Prepared CMD: "<<std::string("bash -c \'"+std::string(cmd)+"\'").c_str()<<"\n";
    std::shared_ptr<FILE> pipe(popen(std::string("bash -c \'"+std::string(cmd)+"\'").c_str(), "r"), pclose);
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get()))
    {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

void handler(int id, int sd, sockaddr_in s_addr, socklen_t s_addr_size)
{
    cout << "In thread " << id << " ..." << endl;
    // buffer to send and receive messages with
    char msg[3500];
    // lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    // also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    // receive a message from the client (listen)
    cout<<"Thread "<<id<< " => Receiving Backend CMD..." << endl;
    memset(&msg, 0, sizeof(msg)); // clear the buffer
    bytesRead += recv(sd, (char *)&msg, sizeof(msg), 0);
    // if(!strcmp(msg, "exit"))
    // {
    //     cout << "Client has quit the session" << endl;
    //     break;
    // }
    cout<<"Thread "<<id<< " => Backend: " << msg << endl;
    string data = exec(std::string(msg).c_str());
    cout<<"Thread "<<id<< " => CMD output: " << data << endl;
    // data = string;
    memset(&msg, 0, sizeof(msg)); // clear the buffer
    strcpy(msg, data.c_str());
    // if(data == "exit")
    // {
    //     //send to the client that server has closed the connection
    //     send(sd, (char*)&msg, strlen(msg), 0);
    //     break;
    // }
    // send the message to client
    bytesWritten += send(sd, (char *)&msg, strlen(msg), 0);
    // we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(sd);
    // cout << "******** thread Session********" << endl;
    cout<<"Thread "<<id<< " => Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    // cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
    //      << " secs" << endl;
    // cout << "Connection closed..." << endl;
}
ctpl::thread_pool *refthreadpool;
int serverSd;
void signal_handler(int signum)
{

    cout << std::endl
         << "Exiting program (cleanup)..." << endl;
    close(serverSd);
    if (refthreadpool != nullptr)
    {
        refthreadpool->stop();
        refthreadpool->~thread_pool();
    }
    exit(0);
}

void timestamp_now()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    cout << timeinfo->tm_year + 1900 << "-" << setfill('0') << setw(2) << timeinfo->tm_mon + 1 << "-" << timeinfo->tm_mday << " " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec;
}
// Server side
int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    short poolSize = 3;
    // for the server, we only need to specify a port number
    if (argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    // grab the port number
    int port = atoi(argv[1]);

    // setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    // open stream oriented socket with internet address
    // also keep track of the socket descriptor
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    // bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr *)&servAddr,
                          sizeof(servAddr));
    if (bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    ctpl::thread_pool threadpool(poolSize /* two threads in the pool */);
    refthreadpool = &threadpool;
    // cout << "Waiting for a client to connect..." << endl;
    // listen for up to 5 requests at a time
    listen(serverSd, 5);
    // receive a request from client using accept
    // we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    while (true)
    {
        cout << "Awaiting backend cmd..." << endl;
        // accept, create a new socket descriptor to
        // handle the new connection with client
        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if (newSd < 0)
        {
            timestamp_now();
            cerr << "Error accepting request from client!" << endl;
            exit(1);
        }
        char cip[INET_ADDRSTRLEN];
        // Get client ip and port
        //  inet_ntop(AF_INET, &(newSockAddr.sin_addr), cip, INET_ADDRSTRLEN);
        cout << "Connected with client: " << inet_ntoa(newSockAddr.sin_addr) << ":" << ntohs(newSockAddr.sin_port) << endl;
        // thread th1(handler, newSd, newSockAddr, newSockAddrSize);
        // th1.join();
        if (threadpool.n_idle() == 0)
        {
            timestamp_now();
            cout << " : All threads are busy! rejecting request " << newSd << endl;
        }
        else
        {
            threadpool.push(handler, newSd, newSockAddr, newSockAddrSize);
            timestamp_now();
            cout << " : Job dispatched, idle threads : " << threadpool.n_idle() << endl;
        }
    }
    close(serverSd);
}