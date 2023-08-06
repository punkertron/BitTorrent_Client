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
            if ((totalLength % pieceLength) != 0)
                blockCount = std::ceil((totalLength % pieceLength) / BLOCK_SIZE);
        }
        res.push_back(std::move(std::make_unique<Piece>(blockCount, totalLength, pieceHashes[i], false)));
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
    // std::cerr << totalDownloaded << '|' << totalPieces << std::endl;
    if (totalDownloaded == totalPieces - 1)
    {
        std::cerr << "Download is done!!!!!" << std::endl;
        exit(EXIT_SUCCESS);  // FIXME:: ?
    }

    if (std::all_of(Pieces.begin(), Pieces.end(),
                    [](const std::unique_ptr<Piece>& piece)
                    {
                        return piece == nullptr;
                    }))
    {
        std::cerr << "Download is done!!!!!" << std::endl;
        exit(EXIT_SUCCESS);  // FIXME:: ?
    }

    return false;
}

const std::string PieceManager::requestPiece(const std::string& peerPeerId)
{
    bool b = isComplete();  // FIXME:: change this
    std::vector<bool> bitfield(peerBitfield[peerPeerId]);
    for (int i = 0; i < totalPieces; ++i)
    {
        if (Pieces[i] != nullptr && bitfield[i])
        {
            std::string blockInfo(Pieces[i].get()->requestBlock());
            return intToBytes(htonl(i)) + blockInfo;
        }
    }
    throw std::runtime_error("No piece");
    return ("");
}

void PieceManager::blockReceived(int index, int begin, const std::string& blockStr)
{
    Piece* ptr = Pieces[index].get();
    ptr->fillData(begin, blockStr);
    std::string dataToFile;
    if (ptr->isFull() && ptr->isHashChecked(dataToFile))
    {
        writeDataToFile(index, dataToFile);
        Pieces[index] = nullptr;
        ++totalDownloaded;
    }
}

void PieceManager::writeDataToFile(int index, const std::string& dataToFile)
{
    downloadedFile.seekp(index * tfp.getPieceLength());
    downloadedFile << dataToFile;
}
