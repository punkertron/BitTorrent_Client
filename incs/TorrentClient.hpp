#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <thread>

#include "SharedQueue.hpp"
#include "TorrentFileParser.hpp"
#include "bencode.hpp"

class TorrentClient
{
   private:
    TorrentFileParser tfp;
    long long interval;
    long long complete;
    long long incomplete;
    bencode::dict peers;

    SharedQueue<std::pair<std::string, long long> > queue;

    std::vector<std::thread> threads;

   public:
    explicit TorrentClient(const char* filePath);
    ~TorrentClient();

    void run();
};

#endif  // TORRENT_CLIENT_HPP