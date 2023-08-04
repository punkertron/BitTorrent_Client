#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <string>

#include "PieceManager.hpp"

class PeerConnection
{
   private:
    int sockfd = -1;
    const std::string infoHash;
    const std::string peerId;
    std::string peerPeerId;
    std::pair<std::string, long long> peer;
    bool choke = true;
    PieceManager* pieceManagerPtr;

    inline std::string createHandshakeMessage() const;
    void performHandshake();
    void recieveMessage();
    void sendInterest();

   public:
    PeerConnection(const std::string& infoHash, const std::string& peerId, const std::pair<std::string, long long>& peer,
                   PieceManager* pieceManagerPtr);
    ~PeerConnection();
    void start();
};

#endif  // PEER_CONNECTION_HPP