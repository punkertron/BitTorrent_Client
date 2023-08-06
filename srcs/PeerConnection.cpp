#include "PeerConnection.hpp"

#include <arpa/inet.h>

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
        std::cerr << "socket = " << sockfd << std::endl;
        performHandshake();
        Message msg(recieveMessage());
        if (msg.getMessageType() == eMessageType::Bitfield)  // TODO: BitField only?
        {
            pieceManagerPtr->addPeerBitfield(peerPeerId, msg.getPayload());
        }
        else
        {
            throw std::runtime_error("No Bitfield");
        }
        sendInterest();

        // while (!pieceManagerPtr->isComplete())
        // {
        //     msg = recieveMessage();
        //     std::cerr << "Message type = " << (int)msg.getMessageType() << std::endl;
        //     switch (msg.getMessageType())
        //     {
        //         case eMessageType::Choke:
        //             choke = true;
        //             break;
        //         case eMessageType::Unchoke:
        //             choke = false;
        //             std::cerr << "Choke = false" << std::endl;
        //             break;
        //         case eMessageType::Piece:
        //         {
        //             std::cerr << "WOWO" << std::endl;
        //             std::string payload = payload;
        //             int index           = getIntFromStr(payload.substr(0, 4));
        //             int begin           = getIntFromStr(payload.substr(4, 4));
        //             std::string blockStr(payload.substr(8));
        //             pieceManagerPtr->blockReceived(peerId, index, begin, blockStr);
        //         }

        //             // case eMessageType::Have:
        //             //     // TODO:
        //             //     break;

        //         default:
        //             break;
        //     }
        //     if (!choke)
        //     {
        //         requestPiece();
        //         std::cerr << "Boo" << std::endl;
        //     }
        // }
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

Message PeerConnection::recieveMessage()  // TODO: or get bitfield
{
    return Message(recieveData(sockfd, 0));
    // std::cerr << static_cast<int>(msg.getMessageType()) << std::endl;
}

void PeerConnection::sendInterest()
{
    sendData(sockfd, Message(eMessageType::Interested).getMessageStr());
}

void PeerConnection::requestPiece()
{
    Block* block = pieceManagerPtr->nextRequest(peerId);

    char payLoad[12];

    int index  = htonl(block->piece);
    int offset = htonl(block->offset);
    int length = htonl(block->length);
    std::memcpy(payLoad, &index, sizeof(int));
    std::memcpy(payLoad + 4, &offset, sizeof(int));
    std::memcpy(payLoad + 8, &length, sizeof(int));

    std::string reqPayLoad;
    for (int i = 0; i < 12; ++i)
        reqPayLoad.push_back(payLoad[i]);

    std::string request = Message(eMessageType::Request, reqPayLoad).getMessageStr();
    sendData(sockfd, request);
}
