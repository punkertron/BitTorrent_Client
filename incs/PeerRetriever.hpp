#ifndef PEER_RETRIEVER_HPP
#define PEER_RETRIEVER_HPP

#include <string>
#include <vector>

#include "TorrentFileParser.hpp"

struct Peer
{
    std::string ip;
    int port;
};

class PeerRetriever
{
   private:
    PeerRetriever();
    PeerRetriever(const PeerRetriever&);
    PeerRetriever(PeerRetriever&&);

    int port;
    long long fileSize;
    std::string peerId;
    std::vector<Peer> peers;

   public:
    explicit PeerRetriever(const std::string& peerId, int port, long long fileSize);
    ~PeerRetriever() = default;

    std::vector<Peer> retrievePeers(const TorrentFileParser& tfp, long long bytesDownloaded);
};

#endif  // PEER_RETRIEVER_HPP