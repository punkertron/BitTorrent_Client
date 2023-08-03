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
    
    
    
    return std::string("\\x13BitTorrent protocol\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\")
        + urlEncodeHex(hexDecode(infoHash))
        + peerId;
}

void PeerConnection::performHandshake()
{
    std::string msg(std::move(createHandshakeMessage()));
    sendData(sockfd, msg);
    std::string response = recieveData(sockfd, msg.length());
    std::cerr << response << std::endl;
    // std::cerr << urlEncodeHex(response.substr(28, 20)) << std::endl;
    // std::cerr << urlEncodeHex(hexDecode(infoHash)) << std::endl;
    std::cerr << msg << std::endl;
    if (response.substr(28, 20) == hexDecode(infoHash))
        std::cerr << "Wow!" << std::endl;
    else
        std::cerr << "bfdls" << std::endl;
}

#define INFO_HASH_STARTING_POS 28
#define PEER_ID_STARTING_POS 48