#include "TorrentClient.hpp"

#include <curl/curl.h>

#include "PeerConnection.hpp"
#include "PeerRetriever.hpp"
#include "PieceManager.hpp"

#ifndef PORT
#define PORT 8080
#endif

#ifndef PEER_ID
#define PEER_ID "-TR1000-000123456789"  // should use random
#endif

TorrentClient::TorrentClient(const char* filePath) : tfp(filePath)
{
    if (!tfp.IsSingle())
    {
        std::cerr << "No mitiple files right now!" << std::endl;
        std::abort();
    }
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

TorrentClient::~TorrentClient()
{
    curl_global_cleanup();
}

void TorrentClient::run()
{
    PeerRetriever p(std::string(PEER_ID), PORT, tfp, 0);
    // std::cerr << "There are " << p.getPeers().size() << " peers!" << std::endl;
    PieceManager pieceManager(tfp);
    for (int i = 0; i < p.getPeers().size(); ++i)
    {
        // if (i == 18 || i == 9)
        {
            std::thread thread(&PeerConnection::start,
                               PeerConnection(tfp.getInfoHash(), std::string(PEER_ID), p.getPeers()[i], i, &pieceManager));
            threads.push_back(std::move(thread));
        }

        // PeerConnection pconn(tfp.getInfoHash(), std::string(PEER_ID), p.getPeers()[i], i, &pieceManager);
        // pconn.start();
    }

    for (int i = 0; i < threads.size(); ++i)
        threads[i].join();

    if (pieceManager.isComplete())
        std::cerr << "Downloaded completed!" << std::endl;
    else
        std::cerr << "Download error!" << std::endl;
}
