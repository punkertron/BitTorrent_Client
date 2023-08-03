#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

int createConnection(const std::string& ip, const long long port);
void sendData(const int sockfd, const std::string& msg);
const std::string recieveData(int sockfd, long long size);

#endif // CONNECTION_HPP