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

    struct timeval timeout;
    timeout.tv_sec = 5; // 5 seconds timeout
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
        std::abort();

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
    int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        std::cerr << "Error receiving data." << std::endl;
        close(sockfd);
        return std::string("");
    };

    return std::string(buffer);

}
