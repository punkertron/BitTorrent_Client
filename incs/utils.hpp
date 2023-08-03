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

#endif  // UTILS_HPP