#include <iostream>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "connection.hpp"

int createConnection(const std::string& ip, const long long port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    };

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "error" << std::endl;
        return -1;
    }

    // FIXME: something bad with sockets

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "error in flags" << std::endl;
        return -1;
    }

    flags &= ~O_NONBLOCK; // Clear O_NONBLOCK flag
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        std::cerr << "error in FCNTL" << std::endl;
        return -1;
    }
    
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        std::cerr << "error with setsockopt" << std::endl;
        return -1;
    }

    int connectResult = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectResult == -1)
    {
        std::cerr << "Error connecting to the server." << std::endl;
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void sendData(const int sockfd, const std::string& msg)
{
    int sent = send(sockfd, msg.c_str(), msg.size(), 0);
    if (sent == 1)
    {
        std::cerr << "Error sending data" << std::endl;
        return ;
    }
}

const std::string recieveData(int sockfd, long long size)
{
    char buffer[size]; // FIXME: if size would be 1000000000000000?
    int bytesReceived = recv(sockfd, buffer, size, 0);
    if (bytesReceived == -1) {
        std::cerr << "Error receiving data." << std::endl;
        close(sockfd);
        return std::string("");
    };

    std::string res;
    for (long long l = 0; l < size; ++l)
        res.push_back(buffer[l]);

    return res;
}
