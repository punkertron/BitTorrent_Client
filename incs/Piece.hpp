#ifndef PIECE_HPP
#define PIECE_HPP

#include <chrono>
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
    enum class eBlockStatus : int
    {
        missing   = 0,
        pending   = 1,
        retrieved = 2
    };

    struct Block
    {
        int offset;
        int length;
        eBlockStatus status;
        std::string data;
        std::chrono::time_point<std::chrono::steady_clock> timeRequest;
    };

    std::vector<std::unique_ptr<Block> > blocks;
    const std::string hash;

    std::vector<std::unique_ptr<Block> > setBlocks(int blockCount, long long totalLength, bool isLastPiece);
    inline bool isReadyToRequest(const Block* ptr);

   public:
    explicit Piece(int blockCount, long long totalLength, const std::string& hash, bool isLastPiece);
    ~Piece() = default;

    Piece()                              = delete;
    Piece& operator=(const Piece& other) = delete;
    Piece(const Piece& other)            = delete;
    Piece(Piece&& other)                 = delete;

    bool isFull() const;
    bool isHashChecked(std::string& dataToFile);
    const std::string requestBlock();
    void fillData(int begin, const std::string& data);
    bool haveBlockToRequest();
    void resetAllBlocksToMissing();
};

#endif  // PIECE_HPP