#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

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

   public:
    explicit TorrentClient(const char* filePath);
    ~TorrentClient();

    void run();
};

#endif  // TORRENT_CLIENT_HPP