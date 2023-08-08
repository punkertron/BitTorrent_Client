#include "Piece.hpp"

#include <arpa/inet.h>

#include <cstring>

Piece::Piece(int blockCount, long long totalLength, const std::string& hash, bool isLastPiece) :
    blocks(setBlocks(blockCount, totalLength, isLastPiece)), hash(hash)
{
}

std::vector<std::unique_ptr<Piece::Block> > Piece::setBlocks(int blockCount, long long totalLength, bool isLastPiece)
{
    std::vector<std::unique_ptr<Block> > blocks;

    blocks.reserve(blockCount);
    for (int offset = 0; offset < blockCount; ++offset)
    {
        std::unique_ptr<Block> block = std::make_unique<Block>();
        block->status                = BlockStatus::missing;
        block->offset                = offset * BLOCK_SIZE;
        if (isLastPiece && offset == blockCount - 1)
        {
            block->length = totalLength % BLOCK_SIZE;
        }
        else
            block->length = BLOCK_SIZE;
        blocks.push_back(std::move(block));
    }
    return blocks;
}

bool Piece::isFull() const
{
    return std::all_of(blocks.begin(), blocks.end(),
                       [](const std::unique_ptr<Block>& block)
                       {
                           return block.get()->status == BlockStatus::retrieved;
                       });
}

const std::string Piece::requestBlock()
{
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->status == BlockStatus::missing)
        {
            blocks[i].get()->status = BlockStatus::pending;
            return intToBytes(htonl(blocks[i].get()->offset)) + intToBytes(htonl(blocks[i].get()->length));
        }
    }
    throw std::runtime_error("No block to request");
}

void Piece::fillData(int begin, const std::string& data)
{
    for (int i = 0, n = blocks.size(); i < n; ++i)
    {
        if (blocks[i].get()->offset == begin)
        {
            if (blocks[i].get()->status == BlockStatus::retrieved)
                throw std::runtime_error("Block already there");
            else
                blocks[i].get()->data = data;
            blocks[i].get()->status = BlockStatus::retrieved;
            return;
        }
    }
    throw std::runtime_error("No such offset in blocks");
}

bool Piece::isHashChecked(std::string& dataToFile)
{
    for (int i = 0; i < blocks.size(); ++i)
        dataToFile += blocks[i].get()->data;

    if (hexDecode(sha1(dataToFile)) != hash)
        throw std::runtime_error("Hash check failed");
    return true;
}

bool Piece::haveMissingBlock() const
{
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->status == BlockStatus::missing)
            return true;
    }
    return false;
}
