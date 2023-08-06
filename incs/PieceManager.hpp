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
    int totalDownloaded = 0;
    std::unordered_map<std::string, std::vector<bool> > peerBitfield;  // vector<bool> is efficient
    std::vector<std::unique_ptr<Piece> > Pieces;
    std::ofstream downloadedFile;

    std::vector<std::unique_ptr<Piece> > initialisePieces();
    void writeDataToFile(int index, const std::string& dataToFile);

   public:
    PieceManager(const TorrentFileParser& tfp);
    ~PieceManager();

    void addPeerBitfield(const std::string& peerPeerId, const std::string& payload);

    const std::string requestPiece(const std::string& peerPeerId);
    void blockReceived(int index, int begin, const std::string& blockStr);

    bool isComplete();
};

#endif  // PIECE_MANAGER_HPP