#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <thread>

#include "PeersQueue.hpp"
#include "PieceManager.hpp"
#include "TorrentFileParser.hpp"

class TorrentClient
{
   private:
    bool trackProgress;
    TorrentFileParser tfp;
    PieceManager pieceManager;
    long long interval;
    long long complete;
    long long incomplete;

    PeersQueue peersQueue;

    std::vector<std::thread> threads;

   public:
    explicit TorrentClient(const char* torrentPath, const char* downloadPath, bool trackProgress = true);
    ~TorrentClient();

    TorrentClient()                                      = delete;
    TorrentClient& operator=(const TorrentClient& other) = delete;
    TorrentClient(const TorrentClient& other)            = delete;
    TorrentClient(TorrentClient&& other)                 = delete;

    void run();

    long long getFileSize() const;
    const std::string& getFileName() const;
};

#endif  // TORRENT_CLIENT_HPP