#include <iostream>
#include <sstream>
#include <cassert>

#include "unistd.h"

#include "PeerConnection.hpp"

#include "utils.hpp"
#include "connection.hpp"

PeerConnection::PeerConnection(const std::string& infoHash, const std::string& peerId, const std::pair<std::string, long long>& peer):
    infoHash(infoHash), peerId(peerId), peer(peer)
{
}

PeerConnection::~PeerConnection()
{
    close(sockfd);
}

void PeerConnection::start()
{
    sockfd = createConnection(peer.first, peer.second);
    // std::cerr << sockfd << std::endl;
    performHandshake();
}

inline std::string PeerConnection::createHandshakeMessage() const
{
    // const std::string protocol = "BitTorrent protocol";
    // std::stringstream buffer;
    // buffer << (char) protocol.length();
    // buffer << protocol;
    // std::string reserved;
    // for (int i = 0; i < 8; i++)
    //     reserved.push_back('\0');
    // buffer << reserved;
    // buffer << hexDecode(infoHash);
    // buffer << peerId;
    // assert (buffer.str().length() == protocol.length() + 49);
    // return buffer.str();
    
    std::string protocol = "BitTorrent protocol";
    std::string reserved(8, '\0'); // 8-byte reserved field, all zeros
    // std::string infoHash = "<your_info_hash>"; // Replace with the actual SHA-1 hash of the info dictionary
    // std::string peerId = "<your_peer_id>"; // Replace with your unique peer ID

    uint8_t pstrlen = protocol.size();
    std::string handshakeMessage;
    handshakeMessage += pstrlen;
    handshakeMessage += protocol;
    handshakeMessage += reserved;
    handshakeMessage += hexDecode(infoHash);
    handshakeMessage += peerId;
    // std::cerr << urlEncodeHex(hexDecode(infoHash)) << std::endl;
    return handshakeMessage;

    return std::string("\\x13BitTorrent protocol\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\")
        + urlEncodeHex(hexDecode(infoHash))
        + peerId;
}

void PeerConnection::performHandshake()
{
    std::string msg(std::move(createHandshakeMessage()));
    sendData(sockfd, msg);
    std::string response = recieveData(sockfd, msg.size());

    if (response.size() && msg.substr(28, 20) == response.substr(28, 20)) // check infoHash
        std::cerr << "Wow!" << std::endl;
    else
        std::cerr << "bfdls" << std::endl;
}

#define INFO_HASH_STARTING_POS 28
#define PEER_ID_STARTING_POS 48