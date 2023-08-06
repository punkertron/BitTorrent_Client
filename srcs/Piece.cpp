#include "Piece.hpp"

Piece::Piece(int index, std::vector<std::unique_ptr<Block> > blocks, std::string hash) :
    index(index), blocks(std::move(blocks)), hash(hash)
{
}

const int Piece::getIndex() const
{
    return index;
}

bool Piece::isFull() const
{
    return std::all_of(blocks.begin(), blocks.end(),
                       [](const std::unique_ptr<Block>& block)
                       {
                           return block.get()->status = BlockStatus::retrieved;
                       });
}

void Piece::fillBlock(const int begin, const std::string& blockStr)
{
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->offset == begin)
        {
            blocks[i].get()->status = BlockStatus::retrieved;
            blocks[i].get()->data   = blockStr;
            return;
        }
    }
}

bool Piece::isHashMatching() const
{
    return hash == hexDecode(sha1(getData()));
}

const std::string Piece::getData() const
{
    std::string data;
    for (int i = 0; i < blocks.size(); ++i)
        data += blocks[i].get()->data;
    return data;
}

Block* Piece::nextRequest()
{
    for (auto& block : blocks)
    {
        if (block->status == BlockStatus::missing)
        {
            block->status = BlockStatus::pending;
            return block.get();
        }
    }
    return nullptr;
}
