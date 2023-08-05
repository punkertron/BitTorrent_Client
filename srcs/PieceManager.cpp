#include "PieceManager.hpp"

#include <cmath>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16384  // 2^14
#endif

#ifndef DOWNLOADS_PATH
#define DOWNLOADS_PATH "./downloads/"
#endif

PieceManager::PieceManager(const TorrentFileParser& tfp) : tfp(tfp), missingPieces(initialisePieces())
{
    downloadedFile.open(DOWNLOADS_PATH + tfp.getFileName(), std::ios::binary | std::ios::out);
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
            block->piece                 = i;
            block->status                = missing;
            block->offset                = offset * BLOCK_SIZE;
            if (i == totalPieces - 1 && offset == blockCount - 1)
                block->length = totalLength % BLOCK_SIZE;
            else
                block->length = totalLength;
            blocks.push_back(std::move(block));
        }
        std::unique_ptr<Piece> piece = std::make_unique<Piece>(i, std::move(blocks), pieceHashes[i]);
        res.push_back(std::move(piece));
    }
    return res;
}

bool PieceManager::isComplete()
{
    return havePieces.size() == totalPieces;
}

void PieceManager::blockReceived(const std::string& peerId, const int index, const int begin, const std::string& blockStr)
{
    (void)peerId;
    Piece* ptr = missingPieces[index].get();
    ptr->fillBlock(begin, blockStr);
    if (ptr->isFull())
    {
        if (ptr->isHashMatching())
        {
            writeToFile(ptr);
        }
    }
}

void PieceManager::writeToFile(Piece* ptr)
{
    long long position = tfp.getPieceLength() * ptr->getIndex();
    downloadedFile.seekp(position);
    downloadedFile << ptr->getData();
}

Block* PieceManager::nextOngoing(std::string peerId)
{
    for (int i = 0; i < missingPieces.size(); ++i)
    {
        Block* block = missingPieces[i].get()->nextRequest();
        return block;
    }
}

Block* PieceManager::nextRequest(const std::string& peerId)
{
    (void)peerId;

    Block* block = nextOngoing(peerId);
    return block;
}