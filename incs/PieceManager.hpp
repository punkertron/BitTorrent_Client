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

    std::vector<std::unique_ptr<Piece> > Pieces;

    // std::vector<std::unique_ptr<Piece> > ongoingPieces;
    // std::vector<std::unique_ptr<Piece> > havePieces;

    std::ofstream downloadedFile;

    std::vector<std::unique_ptr<Piece> > initialisePieces();

   public:
    PieceManager(const TorrentFileParser& tfp);
    ~PieceManager();

    void addPeerBitfield(const std::string& peerPeerId, const std::string& payload);

    const std::string requestPiece(const std::string& peerPeerId);

    bool isComplete();
};

#endif  // PIECE_MANAGER_HPP