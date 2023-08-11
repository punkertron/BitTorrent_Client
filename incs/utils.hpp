#ifndef UTILS_HPP
#define UTILS_HPP

#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <string>

const std::string sha1(const std::string& str);
std::string hexDecode(const std::string& value);
std::string urlEncodeHex(const std::string& input);
std::string bytesToIPAddress(const std::string& bytes);
long long bytesToPort(const std::string& bytes);
int getIntFromStr(const std::string& str);
std::string intToBytes(int x);

#endif  // UTILS_HPP