#ifndef PIECE_HPP
#define PIECE_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "utils.hpp"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16384  // 2^14
#endif

class Piece
{
   private:
    enum class BlockStatus : int
    {
        missing   = 0,
        pending   = 1,
        retrieved = 2
    };

    struct Block
    {
        int offset;
        int length;
        BlockStatus status;
        std::string data;
    };

    std::vector<std::unique_ptr<Block> > blocks;
    const std::string hash;

    std::vector<std::unique_ptr<Block> > setBlocks(int blockCount, long long totalLength, bool isLastPiece);

   public:
    Piece(int blockCount, long long totalLength, const std::string& hash, bool isLastPiece);
    ~Piece() = default;

    bool isFull() const;
    bool isHashChecked(std::string& dataToFile);
    const std::string requestBlock();
    void fillData(int begin, const std::string& data);
    bool haveMissingBlock() const;
};

#endif  // PIECE_HPP