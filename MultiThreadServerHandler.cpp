#include <iostream>
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
// #include <boost/process.hpp>
// #include <boost/process/child.hpp>
#include <vector>
#include <sstream>
#include <thread>
using namespace std;
// namespace bp = boost::process;
// namespace bpo = boost::program_options;
// using boost::property_tree::ptree;
// using boost::format;

std::string exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

void handler(int sd, sockaddr_in s_addr, socklen_t s_addr_size)
{
    // cout<<"In thread "<<std::this_thread::get_id()<<"..."<<endl;
    // buffer to send and receive messages with
    char msg[1500];
    // lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    // also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    // receive a message from the client (listen)
    cout << "Awaiting Backend CMD..." << endl;
    memset(&msg, 0, sizeof(msg)); // clear the buffer
    bytesRead += recv(sd, (char *)&msg, sizeof(msg), 0);
    // if(!strcmp(msg, "exit"))
    // {
    //     cout << "Client has quit the session" << endl;
    //     break;
    // }
    cout << "Backend: " << msg << endl;
    // cout << ">";
    // Prepare client response here
    // 8888888888888888888888888888888888888888
   /* bp::ipstream is; // reading pipe-stream
    bp::ipstream err;
    bp::child c((char*)"/bin/bash", "-c", std::string(msg).c_str(), bp::std_err > err, bp::std_out > is);
    c.wait();
    std::vector<std::string> outputlines, errorlines;
    std::string line;

    while (std::getline(err, line) && !line.empty())
        errorlines.push_back(line);
    line.clear();
    stringstream ss;
    for (auto it = errorlines.begin(); it != errorlines.end(); it++) {
        ss << *it << "\n";
    }
    cout<<"error: "<<ss.str()<<endl;
    ss.clear();
    while (std::getline(is, line) && !line.empty())
        outputlines.push_back(line);
    for (auto it = outputlines.begin(); it != outputlines.end(); it++) {
        ss << *it << "\n";
    }
    cout<<"output: "<<ss.str()<<endl;
*/
    // return data;
    // 8888888888888888888888888888888888888888
    // getline(cin, data);
    // string data = ss.str();
    string data = exec(std::string(msg).c_str());
    cout<<"CMD output: "<<data<<endl;
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
    // cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    // cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
    //      << " secs" << endl;
    // cout << "Connection closed..." << endl;
}

int serverSd;
void signal_handler(int signum)
{

    cout << std::endl
         << "Exiting program (cleanup)..." << endl;
    close(serverSd);
    exit(0);
}
// Server side
int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
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
            cerr << "Error accepting request from client!" << endl;
            exit(1);
        }
char cip[INET_ADDRSTRLEN];
//Get client ip and port
// inet_ntop(AF_INET, &(newSockAddr.sin_addr), cip, INET_ADDRSTRLEN);
        cout << "Connected with client: "<< inet_ntoa(newSockAddr.sin_addr)<<":"<<ntohs(newSockAddr.sin_port)<< endl;
        thread th1(handler, newSd, newSockAddr, newSockAddrSize);
        th1.join();
        cout << "Th1 joined" << endl;
    }
    close(serverSd);
}