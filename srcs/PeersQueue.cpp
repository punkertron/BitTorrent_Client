#include "PeersQueue.hpp"

#include <algorithm>

#include "spdlog/spdlog.h"

const std::pair<std::string, long long> PeersQueue::getPeer()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    if (i == peers.size())
    {
        return {"", 0};
    }
    return peers[i++];
}

void PeersQueue::push_back(const std::pair<std::string, long long>& peer)
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    if (peers.size() > maxPeersQueueSize)
    {
        SPDLOG_INFO("Clearing PeersQueue beacause the size of the PeersQueue > maxPeersQueueSize");
        peers.clear();
        dummyPeers.clear();
    }
    auto elapsedTime = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - lastPushTime).count();
    int milliseconds = static_cast<int>(elapsedTime);
    int seconds      = milliseconds / 1000;
    if (std::find(dummyPeers.begin(), dummyPeers.end(), peer) == dummyPeers.end())
    {
        peers.emplace_back(peer);
        lastPushTime = std::chrono::steady_clock::now();
    }
    else if (seconds > 10)
    {
        SPDLOG_INFO(
            "Force-push to the PeerQueue new peers, "
            "even if they are exists and marked as bad (because we had problems with them before). "
            "Because the previous successful push was more than 10 seconds ago");
        dummyPeers.clear();
        peers.emplace_back(peer);
        lastPushTime = std::chrono::steady_clock::now();
    }
}

void PeersQueue::reportBadPeer(const std::pair<std::string, long long>& peer)
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    dummyPeers.emplace_back(peer);
}

bool PeersQueue::hasFreePeers()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    return i < peers.size();
}

size_t PeersQueue::size()
{
    std::lock_guard<std::mutex> lock(mutexPeerQueue);
    return peers.size();
}
