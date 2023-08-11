#include "PeersQueue.hpp"

#include "spdlog/spdlog.h"

const std::pair<std::string, long long> PeersQueue::getPeer()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    while (i < peers.size() && activePeers[i])
        ++i;
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
    for (size_t j = 0; j < activePeers.size(); ++j)
    {
        if (activePeers[j] == false)
            return true;
    }
    return false;
}

size_t PeersQueue::size()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);  // Don't need it?
    return peers.size();
}
