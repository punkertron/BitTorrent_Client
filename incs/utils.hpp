#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

const std::string sha1(const std::string& str);

#endif // UTILS_HPP