#include "PeerConnection.hpp"

#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "Message.hpp"
#include "connection.hpp"
#include "spdlog/spdlog.h"
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
    performHandshake();
    Message msg(recieveMessage());
    if (msg.getMessageType() == eMessageType::Bitfield)
        pieceManagerPtr->addPeerBitfield(peerPeerId, msg.getPayload());
    else
        throw std::runtime_error("No Bitfield");  // But can be without bitfield, right?
    sendInterest();
}

void PeerConnection::start()
{
    while (!pieceManagerPtr->isComplete())
    {
        while ((peer = peers->getPeer()).first == "")
        {
            if (pieceManagerPtr->isComplete())
                return;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

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
                        requestPiece();
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            SPDLOG_ERROR("PeerIp = {}\tPeerPort = {}\tError: {}", peer.first, peer.second, e.what());
            if (sockfd > 0)
                close(sockfd);
            sockfd = -1;
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
        throw std::runtime_error("No handshake");
    peerPeerId = response.substr(48, 20);
}

Message PeerConnection::recieveMessage()
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
    SPDLOG_INFO("Request: pieceIndex = {}\toffset = {}\tblockSize = {}", getIntFromStr(request.substr(5, 4)),
                getIntFromStr(request.substr(9, 4)), getIntFromStr(request.substr(13, 4)));
    sendData(sockfd, request);
    requestPending = true;
}
