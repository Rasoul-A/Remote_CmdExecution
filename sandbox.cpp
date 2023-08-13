
void send_tcp_remote_cmd(std::string inputstr, std::string *outputstr )
{

// grab the IP address and port number
char *serverIp = argv[1];
int port = atoi(argv[2]);
// create a message buffer
char msg[1500];
// setup a socket and connection tools
struct hostent *host = gethostbyname(serverIp);
sockaddr_in sendSockAddr;
bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
sendSockAddr.sin_family = AF_INET;
sendSockAddr.sin_addr.s_addr =
    inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
sendSockAddr.sin_port = htons(port);
int clientSd = socket(AF_INET, SOCK_STREAM, 0);
struct timeval tv;
tv.tv_sec = 5;
tv.tv_usec = 0;
setsockopt(clientSd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
// try to connect...
int status = connect(clientSd,
                     (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
if (status < 0)
{
    cout << "Error connecting to socket!" << endl;
    return -1;
}
cout << "Connected to the server!" << endl;
int bytesRead, bytesWritten = 0;
// struct timeval start1, end1;
// gettimeofday(&start1, NULL);
    // cout << ">";
    string data;// Your command
    
    memset(&msg, 0, sizeof(msg)); // clear the buffer
    strcpy(msg, data.c_str());
    bytesWritten += send(clientSd, (char *)&msg, strlen(msg), 0);
    cout << "Awaiting server response..." << endl;
    memset(&msg, 0, sizeof(msg)); // clear the buffer
    bytesRead += recv(clientSd, (char *)&msg, sizeof(msg), 0);
    cout << "Server: " << msg << endl;
// gettimeofday(&end1, NULL);
close(clientSd);

}