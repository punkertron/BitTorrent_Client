#ifndef PIECE_HPP
#define PIECE_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Block.hpp"
#include "utils.hpp"

class Piece
{
   private:
    const int index;
    std::vector<std::unique_ptr<Block> > blocks;
    const std::string hash;

   public:
    Piece(int index, std::vector<std::unique_ptr<Block> > blocks, std::string hash);
    ~Piece() = default;

    const int getIndex() const;

    bool isFull() const;

    void fillBlock(const int begin, const std::string& blockStr);

    bool isHashMatching() const;

    const std::string getData() const;

    Block* nextRequest();
};

#endif  // PIECE_HPP