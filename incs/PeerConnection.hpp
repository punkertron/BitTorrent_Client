#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <string>

#include "Message.hpp"
#include "PeersQueue.hpp"
#include "PieceManager.hpp"

class PeerConnection
{
   private:
    int sockfd = -1;
    const std::string infoHash;
    const std::string peerId;
    std::string peerPeerId;
    std::pair<std::string, long long> peer;
    bool choke          = true;
    bool requestPending = false;
    std::string bitfield;
    PieceManager* pieceManagerPtr;

    PeersQueue* peers;

    inline std::string createHandshakeMessage() const;
    void performHandshake();
    Message recieveMessage();
    void sendInterest();
    void requestPiece();
    void establishConnection();

   public:
    PeerConnection(const std::string& infoHash, const std::string& peerId, PieceManager* pieceManagerPtr, PeersQueue* peers);
    ~PeerConnection();
    void start();
};

#endif  // PEER_CONNECTION_HPP