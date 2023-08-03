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
    std::vector<std::pair<std::string, long long> > allPeers;

    std::vector<std::pair<std::string, long long> > decodeResponse(const std::string& response);

   public:
    explicit PeerRetriever(const std::string& peerId, int port, const TorrentFileParser& tfp, long long bytesDownloaded);
    ~PeerRetriever() = default;

    std::vector<std::pair<std::string, long long> > retrievePeers(const TorrentFileParser& tfp, long long bytesDownloaded);

    const std::vector<std::pair<std::string, long long> >& getPeers() const { return allPeers; }
};

#endif  // PEER_RETRIEVER_HPP