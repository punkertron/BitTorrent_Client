#include "PeersQueue.hpp"

#include "spdlog/spdlog.h"

const std::pair<std::string, long long> PeersQueue::getPeer()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    while (i < peers.size() && activePeers[i])  // FIXME: How exit from this loop?
    {
        ++i;
    }
    // std::cerr << "--------------" << std::endl;
    if (i == peers.size())
    {
        i = 0;
        return {"", 0};
    }
    activePeers[i] = true;
    return peers[i++];
}

void PeersQueue::push_back(const std::pair<std::string, long long>& peer)
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    peers.emplace_back(peer);
    activePeers.emplace_back(false);
}

bool PeersQueue::hasFreePeers()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    for (int i = 0; i < activePeers.size(); ++i)
    {
        if (activePeers[i] == false)
        {
            SPDLOG_INFO("PeersQueue has free members");
            return true;
        }
    }
    SPDLOG_INFO("PeersQueue has NOT free members");
    return false;
}

size_t PeersQueue::size()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);  // Don't need it?
    return peers.size();
}