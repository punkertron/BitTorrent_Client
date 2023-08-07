#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <string>

#include "Message.hpp"
#include "PieceManager.hpp"

class PeerConnection
{
   private:
    const int peerIndex;
    int sockfd = -1;
    const std::string infoHash;
    const std::string peerId;
    std::string peerPeerId;
    std::pair<std::string, long long> peer;
    bool choke          = true;
    bool requestPending = false;
    std::string bitfield;
    PieceManager* pieceManagerPtr;

    inline std::string createHandshakeMessage() const;
    void performHandshake();
    Message recieveMessage();
    void sendInterest();
    void requestPiece();

   public:
    PeerConnection(const std::string& infoHash, const std::string& peerId, const std::pair<std::string, long long>& peer,
                   const int peerIndex, PieceManager* pieceManagerPtr);
    ~PeerConnection();
    void start();
};

#endif  // PEER_CONNECTION_HPP