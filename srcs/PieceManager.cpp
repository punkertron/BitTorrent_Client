#include "PieceManager.hpp"

#include <arpa/inet.h>

#include <cmath>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16384  // 2^14
#endif

#ifndef DOWNLOADS_PATH
#define DOWNLOADS_PATH "./downloads/"
#endif

PieceManager::PieceManager(const TorrentFileParser& tfp) : tfp(tfp), Pieces(initialisePieces())
{
    downloadedFile.open(DOWNLOADS_PATH + tfp.getFileName(), std::ios::binary | std::ios::out);
    downloadedFile.seekp(tfp.getLengthOne() - 1);
    downloadedFile.write("", 1);
}

PieceManager::~PieceManager()
{
    downloadedFile.close();
}

static std::vector<std::string> splitPiecesHashes(const std::string& pieces)
{
    std::vector<std::string> res;

    int piecesCount = pieces.size() / 20;  // Length of one HASH = 20
    res.reserve(piecesCount);
    for (int i = 0; i < piecesCount; ++i)
        res.push_back(pieces.substr(i * 20, 20));
    return res;
}

std::vector<std::unique_ptr<Piece> > PieceManager::initialisePieces()
{
    std::vector<std::unique_ptr<Piece> > res;
    std::vector<std::string> pieceHashes(splitPiecesHashes(tfp.getPieces()));
    totalPieces = pieceHashes.size();

    long long totalLength = tfp.getLengthOne();

    long long pieceLength = tfp.getPieceLength();
    int blockCount        = std::ceil(pieceLength / BLOCK_SIZE);

    for (int i = 0; i < totalPieces; ++i)
    {
        if (i == totalPieces - 1)
        {
            ;  // TODO: last piece
        }
        std::vector<std::unique_ptr<Block> > blocks;

        blocks.reserve(blockCount);
        for (int offset = 0; offset < blockCount; ++offset)
        {
            std::unique_ptr<Block> block = std::make_unique<Block>();
            // block->piece                 = i;
            block->status = BlockStatus::missing;
            block->offset = offset * BLOCK_SIZE;
            if (i == totalPieces - 1 && offset == blockCount - 1)
                block->length = totalLength % BLOCK_SIZE;
            else
                block->length = BLOCK_SIZE;
            blocks.push_back(std::move(block));
        }
        std::unique_ptr<Piece> piece = std::make_unique<Piece>(std::move(blocks), pieceHashes[i]);
        res.push_back(std::move(piece));
    }
    return res;
}

void PieceManager::addPeerBitfield(const std::string& peerPeerId, const std::string& payload)
{
    std::vector<bool> bits;
    bits.resize(payload.size() * 8);

    for (int i = 0; i < payload.size(); ++i)
    {
        unsigned char byte = payload[i];
        for (int j = 0; j < 8; ++j)
        {
            int bitPos   = i * 8 + j;
            bits[bitPos] = (byte >> (7 - j)) & 1;
        }
    }

    peerBitfield.insert({peerPeerId, bits});
}

bool PieceManager::isComplete()
{
    return false;  // TODO: havePieces.size() == totalPieces;
}

const std::string PieceManager::requestPiece(const std::string& peerPeerId)
{
    std::vector<bool> bitfield(peerBitfield[peerPeerId]);
    for (int i = 0; i < totalPieces; ++i)
    {
        if (Pieces[i] != nullptr && bitfield[i])
        {
            // std::cerr << "Index = " << i << ' ';
            std::string blockInfo(Pieces[i].get()->requestBlock());

            // uint32_t index = htonl(i);
            // char tmp[4];
            // std::memcpy(tmp, &index, sizeof(uint32_t));
            // std::string res;
            // for (int j = 0; j < 4; ++j)
            //     res =+ tmp[j];
            // std::cerr << "= " << getIntFromStr(blockInfo.substr(0, 4)) << std::endl;
            // std::cerr << "= " << getIntFromStr(blockInfo.substr(4, 4)) << std::endl;
            return intToBytes(htonl(i)) + blockInfo;
        }
    }
    return ("");
}
