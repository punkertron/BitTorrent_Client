#ifndef PIECE_MANAGER_HPP
#define PIECE_MANAGER_HPP

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "TorrentFileParser.hpp"

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
    // Block(){};
    // ~Block(){};
    // Block(const Block& other) = default;
    // Block(Block&& other) = default;
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
};

#endif  // PIECE_MANAGER_HPP