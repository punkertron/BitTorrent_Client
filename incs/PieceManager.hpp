#ifndef PIECE_MANAGER_HPP
#define PIECE_MANAGER_HPP

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "TorrentFileParser.hpp"
#include "utils.hpp"

enum BlockStatus
{
    missing   = 0,
    pending   = 1,
    retrieved = 2
};

struct Block
{
    int piece;
    int offset;
    int length;
    BlockStatus status;
    std::string data;
};

class Piece
{
   private:
    const int index;
    std::vector<std::unique_ptr<Block> > blocks;
    const std::string hash;

   public:
    Piece(int index, std::vector<std::unique_ptr<Block> > blocks, std::string hash) :
        index(index), blocks(std::move(blocks)), hash(hash)
    {
    }
    ~Piece() = default;

    const int getIndex() const
    {
        return index;
    }

    bool isFull() const
    {
        return std::all_of(blocks.begin(), blocks.end(),
                           [](const std::unique_ptr<Block>& block)
                           {
                               return block.get()->status = retrieved;
                           });
    }

    void fillBlock(const int begin, const std::string& blockStr)
    {
        for (int i = 0; i < blocks.size(); ++i)
        {
            if (blocks[i].get()->offset == begin)
            {
                blocks[i].get()->status = retrieved;
                blocks[i].get()->data   = blockStr;
                return;
            }
        }
    }

    bool isHashMatching() const
    {
        return hash == hexDecode(sha1(getData()));
    }

    const std::string getData() const
    {
        std::string data;
        for (int i = 0; i < blocks.size(); ++i)
            data += blocks[i].get()->data;
        return data;
    }

    Block* nextRequest()
    {
        for (auto& block : blocks)
        {
            if (block->status == missing)
            {
                block->status = pending;
                return block.get();
            }
        }
        return nullptr;
    }
};

class PieceManager
{
   private:
    const TorrentFileParser tfp = nullptr;
    int totalPieces;

    std::vector<std::unique_ptr<Piece> > missingPieces;
    std::vector<std::unique_ptr<Piece> > ongoingPieces;
    std::vector<std::unique_ptr<Piece> > havePieces;
    std::ofstream downloadedFile;

    std::vector<std::unique_ptr<Piece> > initialisePieces();

   public:
    PieceManager(const TorrentFileParser& tfp);
    ~PieceManager();

    bool isComplete();
    void blockReceived(const std::string& peerId, const int index, const int begin, const std::string& blockStr);
    void writeToFile(Piece* ptr);

    Block* nextRequest(const std::string& peerId);
    Block* nextOngoing(std::string peerId);
};

#endif  // PIECE_MANAGER_HPP