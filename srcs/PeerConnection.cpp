#include "PeerConnection.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Message.hpp"
#include "connection.hpp"
#include "unistd.h"
#include "utils.hpp"

PeerConnection::PeerConnection(const std::string& infoHash, const std::string& peerId,
                               const std::pair<std::string, long long>& peer, PieceManager* pieceManagerPtr) :
    infoHash(infoHash), peerId(peerId), peer(peer), pieceManagerPtr(pieceManagerPtr)
{
}

PeerConnection::~PeerConnection()
{
    if (sockfd > 0)
        close(sockfd);
}

void PeerConnection::start()
{
    try
    {
        sockfd = createConnection(peer.first, peer.second);
        // std::cerr << "socket = " << sockfd << std::endl;
        performHandshake();
        recieveMessage();  // BitField?
        sendInterest();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
}

inline std::string PeerConnection::createHandshakeMessage() const
{
    std::string protocol = "BitTorrent protocol";
    std::string reserved(8, '\0');  // 8-byte reserved field, all zeros

    uint8_t pstrlen = protocol.size();
    std::string handshakeMessage;
    handshakeMessage += pstrlen;
    handshakeMessage += protocol;
    handshakeMessage += reserved;
    handshakeMessage += hexDecode(infoHash);
    handshakeMessage += peerId;
    return handshakeMessage;
}

void PeerConnection::performHandshake()
{
    std::string msg(std::move(createHandshakeMessage()));
    sendData(sockfd, msg);
    std::string response = recieveData(sockfd, msg.size());

    if (msg.substr(0, 20) != response.substr(0, 20)        // check protocol
        || msg.substr(28, 20) != response.substr(28, 20))  // check infoHash
    {
        throw std::runtime_error("No handshake");
    }
    peerPeerId = response.substr(48, 20);
    // std::cerr << peerPeerId << std::endl;
}

void PeerConnection::recieveMessage()  // TODO: or get bitfield
{
    Message msg(recieveData(sockfd, 0));
    std::cerr << static_cast<int>(msg.getMessageType()) << std::endl;
}

void PeerConnection::sendInterest()
{
    sendData(sockfd, Message(eMessageType::Interested).getMessageStr());
}
