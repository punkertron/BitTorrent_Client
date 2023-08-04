#include "connection.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <stdexcept>

#include "utils.hpp"

int createConnection(const std::string& ip, const long long port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        throw std::runtime_error("Error creating socket");
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        throw std::runtime_error("inet_pton error");
    }

    // FIXME: something bad with sockets ?

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        throw std::runtime_error("error in flags");
    }

    flags &= ~O_NONBLOCK;  // Clear O_NONBLOCK flag
    if (fcntl(sockfd, F_SETFL, flags) == -1)
    {
        throw std::runtime_error("error in FCNTL");
    }

    struct timeval timeout;
    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
    {
        throw std::runtime_error("error with setsockopt");
    }

    int connectResult = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectResult == -1)
    {
        throw std::runtime_error("Error connecting to the server");
    }

    return sockfd;
}

void sendData(const int sockfd, const std::string& msg)
{
    int sent = send(sockfd, msg.c_str(), msg.size(), 0);
    if (sent == 1)
    {
        throw std::runtime_error("Error sending data");
    }
}

const std::string recieveData(int sockfd, int size)
{
    if (!size)
    {
        char buffer[4];
        int bytesReceived = recv(sockfd, buffer, 4, 0);
        if (bytesReceived != 4)
        {
            throw std::runtime_error("Error receiving data");
        }
        std::string res(4, '\0');
        for (int i = 0; i < 4; ++i)
            res[i] = buffer[i];
        size = getLengthFromMessage(res);
        if (size == 0)
            return res;

        std::unique_ptr<char[]> bigBuffer;
        try
        {
            bigBuffer = std::make_unique<char[]>(size);
        }
        catch (const std::bad_alloc&)
        {
            throw std::runtime_error("Bad allocation in receiving data");
        }

        bytesReceived = recv(sockfd, bigBuffer.get(), size, 0);
        if (bytesReceived != size)
        {
            throw std::runtime_error("Error receiving data");
        }

        size += 4;
        std::string bigRes;
        int i = 0;
        for (; i < 4; ++i)
            bigRes.push_back(buffer[i]);
        for (; i < size; ++i)
            bigRes.push_back(bigBuffer[i - 4]);
        return bigRes;
    }
    else
    {
        char buffer[size];  // size = 68
        int bytesReceived = recv(sockfd, buffer, size, 0);
        if (bytesReceived != size)
        {
            throw std::runtime_error("Error receiving data");
        }
        std::string res(size, '\0');
        for (int i = 0; i < size; ++i)
            res[i] = buffer[i];
        return res;
    }
}
