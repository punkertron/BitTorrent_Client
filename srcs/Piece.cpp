#include "Piece.hpp"

#include <arpa/inet.h>

#include <algorithm>
#include <cstring>

#include "spdlog/spdlog.h"

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
        block->status                = eBlockStatus::missing;
        block->offset                = offset * BLOCK_SIZE;
        if (isLastPiece && offset == blockCount - 1)
        {
            block->length = totalLength % BLOCK_SIZE;
            if (!block->length)
                block->length = BLOCK_SIZE;
            SPDLOG_INFO("Last Piece: offset = {}, blockLength = {}", offset, block->length);
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
                           return block.get()->status == eBlockStatus::retrieved;
                       });
}

bool Piece::isReadyToRequest(const Block* ptr)
{
    return (ptr->status == eBlockStatus::missing ||
            (ptr->status == eBlockStatus::pending &&
             std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - ptr->timeRequest).count() >
                 7000));  // If block is pending more than 7 sec
}

const std::string Piece::requestBlock()
{
    for (size_t i = 0; i < blocks.size(); ++i)
    {
        if (isReadyToRequest(blocks[i].get()))
        {
            Block* ptr       = blocks[i].get();
            ptr->status      = eBlockStatus::pending;
            ptr->timeRequest = std::chrono::steady_clock::now();
            return intToBytes(htonl(ptr->offset)) + intToBytes(htonl(ptr->length));
        }
    }
    throw std::runtime_error("No block to request");
}

void Piece::fillData(int begin, const std::string& data)
{
    for (size_t i = 0, n = blocks.size(); i < n; ++i)
    {
        if (blocks[i].get()->offset == begin)
        {
            if (blocks[i].get()->status == eBlockStatus::retrieved)
                throw std::runtime_error("Block already there");
            else
                blocks[i].get()->data = data;
            blocks[i].get()->status = eBlockStatus::retrieved;
            return;
        }
    }
    throw std::runtime_error("No such offset in blocks");
}

bool Piece::isHashChecked(const std::string& dataToFile) const
{
    return hexDecode(sha1(dataToFile)) == hash;
}

bool Piece::haveBlockToRequest()
{
    for (size_t i = 0; i < blocks.size(); ++i)
    {
        if (isReadyToRequest(blocks[i].get()))
            return true;
    }
    return false;
}

void Piece::resetAllBlocksToMissing()
{
    for (size_t i = 0; i < blocks.size(); ++i)
        blocks[i].get()->status = eBlockStatus::missing;
}

void Piece::fillDataToStr(std::string& dataToFile)
{
    for (size_t i = 0; i < blocks.size(); ++i)
        dataToFile += blocks[i].get()->data;
}
