#include "PeerConnection.hpp"

#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "Message.hpp"
#include "connection.hpp"
#include "unistd.h"
#include "utils.hpp"

PeerConnection::PeerConnection(const std::string& infoHash, const std::string& peerId, PieceManager* pieceManagerPtr,
                               PeersQueue* peers) :
    infoHash(infoHash), peerId(peerId), pieceManagerPtr(pieceManagerPtr), peers(peers)
{
}

PeerConnection::~PeerConnection()
{
    if (sockfd > 0)
        close(sockfd);
}

void PeerConnection::establishConnection()
{
    sockfd = createConnection(peer.first, peer.second);
    // std::cerr << "socket = " << sockfd << std::endl;
    performHandshake();
    Message msg(recieveMessage());
    if (msg.getMessageType() == eMessageType::Bitfield)
    {
        pieceManagerPtr->addPeerBitfield(peerPeerId, msg.getPayload());
    }
    else
    {
        throw std::runtime_error("No Bitfield");
    }
    sendInterest();
}

void PeerConnection::start()
{
    while (!pieceManagerPtr->isComplete())
    {
        peer = peers->getPeer();

        try
        {
            establishConnection();
            while (!pieceManagerPtr->isComplete())
            {
                Message msg = recieveMessage();
                switch (msg.getMessageType())
                {
                    case eMessageType::Choke:
                        choke = true;
                        break;
                    case eMessageType::Unchoke:
                        choke = false;
                        break;
                    case eMessageType::Piece:
                    {
                        std::string payload = msg.getPayload();
                        int index           = getIntFromStr(payload.substr(0, 4));
                        int begin           = getIntFromStr(payload.substr(4, 4));
                        std::string blockStr(payload.substr(8));
                        pieceManagerPtr->blockReceived(index, begin, blockStr);
                        requestPending = false;
                        break;
                    }
                    case eMessageType::Have:
                        pieceManagerPtr->addToBitfield(peerPeerId, msg.getPayload());
                        break;
                    default:
                        break;
                }

                if (!choke)
                {
                    if (!requestPending)
                    {
                        requestPiece();
                    }
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            // std::cerr << e.what() << std::endl;
            if (sockfd != -1)
                close(sockfd);
        }
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

Message PeerConnection::recieveMessage()  // TODO: or get bitfield
{
    return Message(recieveData(sockfd, 0));
}

void PeerConnection::sendInterest()
{
    sendData(sockfd, Message(eMessageType::Interested).getMessageStr());
}

void PeerConnection::requestPiece()
{
    std::string request = Message(eMessageType::Request, pieceManagerPtr->requestPiece(peerPeerId)).getMessageStr();
    // std::cerr << "-------------" << std::endl;
    // std::cerr << getIntFromStr(request.substr(5, 4)) << std::endl;
    // std::cerr << getIntFromStr(request.substr(9, 4)) << std::endl;
    sendData(sockfd, request);
    requestPending = true;
}
