#include "connection.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "utils.hpp"

int createConnection(const std::string& ip, const long long port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        throw std::runtime_error("Error creating socket");

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
        throw std::runtime_error("Error with inet_pton");

    // Set socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("Error in fcntl flags");
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1)
        throw std::runtime_error("Error in fcntl setting socket to non-blocking mode");

    int connectResult = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectResult == -1)
    {
        if (errno == EINPROGRESS)
        {
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(sockfd, &writeSet);

            struct timeval timeout;
            timeout.tv_sec  = 5;
            timeout.tv_usec = 0;

            int selectResult = select(sockfd + 1, NULL, &writeSet, NULL, &timeout);
            if (selectResult == -1)
                throw std::runtime_error("Error in select");
            else if (selectResult == 0)
                throw std::runtime_error("Connection timeout");
        }
        else
            throw std::runtime_error("Error connecting to peer");
    }

    // Connection successful, socket is in blocking mode again
    flags &= ~O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1)
        throw std::runtime_error("Error in fcntl setting socker to blocking mode");

    return sockfd;
}

void sendData(const int sockfd, const std::string& msg)
{
    struct timeval timeout;
    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
        throw std::runtime_error("Error with setsockopt sendTimeout");

    int sent = send(sockfd, msg.c_str(), msg.size(), 0);
    if (sent != static_cast<int>(msg.size()))
        throw std::runtime_error("Error sending data");
}

static void recvData(int sockfd, char* buffer, int size)
{
    int bytesReceived = 0;
    int sumReceived   = 0;
    auto startTime    = std::chrono::steady_clock::now();
    while (sumReceived < size)
    {
        auto diff = std::chrono::steady_clock::now() - startTime;
        if (std::chrono::duration<double, std::milli>(diff).count() > 5000)  // 5 sec
            throw std::runtime_error("Read timeout");
        bytesReceived = recv(sockfd, buffer + sumReceived, size - sumReceived, 0);
        if (bytesReceived == -1)
            throw std::runtime_error("Error receiving data");
        sumReceived += bytesReceived;
    }
}

const std::string recieveData(int sockfd, int size)
{
    struct timeval recvTimeout;
    recvTimeout.tv_sec  = 3;
    recvTimeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout)) == -1)
        throw std::runtime_error("Error with setsockopt recvTimeout");

    if (!size)
    {
        char buffer[4];

        recvData(sockfd, buffer, 4);
        std::string res(4, '\0');
        for (int i = 0; i < 4; ++i)
            res[i] = buffer[i];
        size = getIntFromStr(res);
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

        recvData(sockfd, bigBuffer.get(), size);

        size += 4;
        std::string bigRes;
        int i = 0;
        for (; i < 4; ++i)
            bigRes.push_back(buffer[i]);
        for (; i < size; ++i)
            bigRes.push_back(bigBuffer[i - 4]);
        return bigRes;
    }
    else  // Handshake  - size = 68
    {
        char buffer[size];

        recvData(sockfd, buffer, size);

        std::string res(size, '\0');
        for (int i = 0; i < size; ++i)
            res[i] = buffer[i];
        return res;
    }
}
