#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <thread>

#include "PeersQueue.hpp"
#include "TorrentFileParser.hpp"

class TorrentClient
{
   private:
    TorrentFileParser tfp;
    const char* downloadPath;
    long long interval;
    long long complete;
    long long incomplete;

    PeersQueue peersQueue;

    std::vector<std::thread> threads;

   public:
    explicit TorrentClient(const char* torrentPath, const char* downloadPath);
    ~TorrentClient();

    void run();
};

#endif  // TORRENT_CLIENT_HPP