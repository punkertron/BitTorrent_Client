#include "Piece.hpp"

#include <arpa/inet.h>

#include <cstring>

// Piece::Piece(int index, std::vector<std::unique_ptr<Block> > blocks, std::string hash) :
//     index(index), blocks(std::move(blocks)), hash(hash)
// {
// }

// const int Piece::getIndex() const
// {
//     return index;
// }

Piece::Piece(std::vector<std::unique_ptr<Block> > blocks, std::string hash) : blocks(std::move(blocks)), hash(hash)
{
}

bool Piece::isFull() const
{
    return std::all_of(blocks.begin(), blocks.end(),
                       [](const std::unique_ptr<Block>& block)
                       {
                           return block.get()->status = BlockStatus::retrieved;
                       });
}

const std::string Piece::requestBlock()
{
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->status == BlockStatus::missing)
        {
            // uint32_t offset = htonl(blocks[i].get()->offset);
            // uint32_t length = htonl(blocks[i].get()->length);
            // char tmp[8];
            // std::memcpy(tmp, &offset, sizeof(uint32_t));
            // std::memcpy(tmp + 4, &length, sizeof(uint32_t));
            // std::string blockInfo;
            // for (int i = 0; i < 8; i++)
            // {
            //     blockInfo += tmp[i];
            // }

            // std::cerr << "Offset = " << blocks[i].get()->offset << " length = " << blocks[i].get()->length << std::endl;
            // std::cerr << "= " << getIntFromStr(intToBytes(htonl(blocks[i].get()->length))) << std::endl;
            return intToBytes(htonl(blocks[i].get()->offset)) + intToBytes(htonl(blocks[i].get()->length));
        }
    }
    return ("");
}
