#ifndef PIECE_MANAGER_HPP
#define PIECE_MANAGER_HPP

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "Piece.hpp"
#include "TorrentFileParser.hpp"
#include "utils.hpp"

class PieceManager
{
   private:
    const TorrentFileParser tfp;
    const char* downloadPath;
    int totalPieces;
    int totalDownloaded = 0;
    std::unordered_map<std::string, std::vector<bool> > peerBitfield;  // vector<bool> is efficient
    std::vector<std::unique_ptr<Piece> > Pieces;
    std::string downloadFilePath;
    std::ofstream downloadedFile;

    const std::chrono::time_point<std::chrono::steady_clock> startTime =
        std::chrono::steady_clock::now();  // start of the downloading
    float lastCheckFileSize{0.0f};         // bytes. FIXME: It's not good to store in float

    std::mutex mutex;
    std::mutex mutexWrite;

    std::vector<std::unique_ptr<Piece> > initialisePieces();
    void writeDataToFile(int index, const std::string& dataToFile);
    void display(const float currentFileSize, const double n, const int lengthOfSize);

   public:
    explicit PieceManager(const TorrentFileParser& tfp, const char* downloadPath);
    ~PieceManager();

    PieceManager()                                     = delete;
    PieceManager& operator=(const PieceManager& other) = delete;
    PieceManager(const PieceManager& other)            = delete;
    PieceManager(PieceManager&& other)                 = delete;
    PieceManager& operator=(PieceManager&& other)      = delete;

    void addPeerBitfield(const std::string& peerPeerId, const std::string& payload);

    const std::string requestPiece(const std::string& peerPeerId);
    void blockReceived(int index, int begin, const std::string& blockStr);
    void addToBitfield(const std::string& peerPeerId, const std::string& payload);

    bool isComplete();
    void trackProgress();
    void trackSpeed();
};

#endif  // PIECE_MANAGER_HPP