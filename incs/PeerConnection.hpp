#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <string>

class PeerConnection
{
   private:
    int sockfd;
    const std::string infoHash;
    const std::string peerId;
    std::string peerPeerId;
    std::pair<std::string, long long> peer;
    bool choke = true;

    inline std::string createHandshakeMessage() const;
    void performHandshake();
    void recieveMessage();

   public:
    PeerConnection(const std::string& infoHash, const std::string& peerId, const std::pair<std::string, long long>& peer);
    ~PeerConnection();
    void start();
};

#endif  // PEER_CONNECTION_HPP