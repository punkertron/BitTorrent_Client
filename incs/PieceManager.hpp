#ifndef PIECE_MANAGER_HPP
#define PIECE_MANAGER_HPP

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Piece.hpp"
#include "TorrentFileParser.hpp"
#include "utils.hpp"

class PieceManager
{
   private:
    const TorrentFileParser tfp = nullptr;
    int totalPieces;
    std::unordered_map<std::string, std::vector<bool> > peerBitfield;  // vector<bool> is efficient

    std::vector<std::unique_ptr<Piece> > missingPieces;
    std::vector<std::unique_ptr<Piece> > ongoingPieces;
    std::vector<std::unique_ptr<Piece> > havePieces;
    std::ofstream downloadedFile;

    std::vector<std::unique_ptr<Piece> > initialisePieces();

   public:
    PieceManager(const TorrentFileParser& tfp);
    ~PieceManager();

    void addPeerBitfield(const std::string& peerPeerId, const std::string& payload);

    bool isComplete();
    void blockReceived(const std::string& peerId, const int index, const int begin, const std::string& blockStr);
    void writeToFile(Piece* ptr);

    Block* nextRequest(const std::string& peerId);
    Block* nextOngoing(std::string peerId);
};

#endif  // PIECE_MANAGER_HPP