#include "TorrentClient.hpp"

#include <curl/curl.h>

#include "PeerConnection.hpp"
#include "PeerRetriever.hpp"
#include "PeersQueue.hpp"
#include "PieceManager.hpp"

#ifndef PORT
#define PORT 8080
#endif

#ifndef PEER_ID
#define PEER_ID "-TR1000-000123456789"  // should use random
#endif

#ifndef THREAD_NUM
#define THREAD_NUM 15
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
    PieceManager pieceManager(tfp);
    PeerRetriever p(std::string(PEER_ID), PORT, tfp, 0);
    std::vector<std::pair<std::string, long long> > peers(p.getPeers());
    for (auto& peer : peers)
        peersQueue.push_back(peer);

    // This thread displays download status
    std::thread thread(&PieceManager::trackProgress, std::ref(pieceManager));
    threads.push_back(std::move(thread));

    // These threads download file
    for (int i = 0; i < THREAD_NUM && i < peers.size(); ++i)
    {
        std::thread thread(&PeerConnection::start,
                           PeerConnection(tfp.getInfoHash(), std::string(PEER_ID), &pieceManager, &peersQueue));
        thread.detach();
        threads.push_back(std::move(thread));
    }

    auto lastUpdate = std::chrono::steady_clock::now();
    while (true)
    {
        if (pieceManager.isComplete())
            break;
        auto diff = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - lastUpdate).count();

        if ((diff / 1000) > p.getInterval() || !peersQueue.hasFreePeers())
        {
            lastUpdate = std::chrono::steady_clock::now();
            p          = PeerRetriever(std::string(PEER_ID), PORT, tfp, 0);
            peers      = p.getPeers();
            for (auto peer : peers)
                peersQueue.push_back(peer);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    threads[0].join();
}
