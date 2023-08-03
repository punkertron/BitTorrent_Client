#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include "TorrentFileParser.hpp"

class TorrentClient
{
   private:
    TorrentFileParser tfp;

   public:
    explicit TorrentClient(const char* filePath);
    ~TorrentClient();

    void run();
};

#endif  // TORRENT_CLIENT_HPP