#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <string>

enum class BlockStatus : int
{
    missing   = 0,
    pending   = 1,
    retrieved = 2
};

struct Block
{
    // int piece;
    int offset;
    int length;
    BlockStatus status;
    std::string data;
};

#endif  // BLOCK_HPP