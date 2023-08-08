#include "PieceManager.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <cmath>

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
            // std::cerr << "totalLength % pieceLength = " << totalLength % pieceLength << std::endl;
            if ((totalLength % pieceLength) != 0)
                blockCount = std::ceil(static_cast<double>((totalLength % pieceLength)) / BLOCK_SIZE);
            // std::cerr << "BlockCount = " << blockCount << std::endl;
            res.push_back(std::move(std::make_unique<Piece>(blockCount, totalLength, pieceHashes[i], true)));
        }
        else
            res.push_back(std::move(std::make_unique<Piece>(blockCount, totalLength, pieceHashes[i], false)));
    }

    // std::cerr << "RES SIZE = " << res.size() << std::endl;
    // std::cerr << "TOTAL_PIECES = " << totalPieces << std::endl;
    return res;
}

void PieceManager::addPeerBitfield(const std::string& peerPeerId, const std::string& payload)
{
    std::lock_guard<std::mutex> lock(mutex);
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

    // for (auto bit : bits)
    //     std::cerr << bit << ' ';
    // std::cerr << std::endl;
    peerBitfield.insert({peerPeerId, bits});
}

bool PieceManager::isComplete()
{
    std::lock_guard<std::mutex> lock(mutex);
    return totalDownloaded == totalPieces;
}

const std::string PieceManager::requestPiece(const std::string& peerPeerId)
{
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<bool> bitfield(peerBitfield[peerPeerId]);
    for (int i = 0; i < totalPieces; ++i)
    {
        if (Pieces[i] != nullptr && bitfield[i] && Pieces[i]->haveMissingBlock())
        {
            std::string blockInfo(Pieces[i].get()->requestBlock());
            // std::cerr << "Piece index = " << i << " and block offset = " << getIntFromStr(blockInfo.substr(0, 4)) <<
            // std::endl;
            return intToBytes(htonl(i)) + blockInfo;
        }
    }
    throw std::runtime_error("No piece from this peer");
    return ("");
}

void PieceManager::blockReceived(int index, int begin, const std::string& blockStr)
{
    std::lock_guard<std::mutex> lock(mutex);
    Piece* ptr = Pieces[index].get();
    // std::cerr << "Index = " << index << " begin = " << begin << std::endl;
    ptr->fillData(begin, blockStr);
    std::string dataToFile;
    if (ptr->isFull())
    {
        if (ptr->isHashChecked(dataToFile))
        {
            writeDataToFile(index, dataToFile);
            Pieces[index] = nullptr;
            ++totalDownloaded;
        }
        else
        {
            ;  // TODO: if hash is not correct
        }
    }
}

void PieceManager::writeDataToFile(int index, const std::string& dataToFile)
{
    downloadedFile.seekp(index * tfp.getPieceLength());
    downloadedFile << dataToFile;
}

void PieceManager::addToBitfield(const std::string& peerPeerId, const std::string& payload)
{
    int bitPos                       = getIntFromStr(payload);
    peerBitfield[peerPeerId][bitPos] = true;
}
