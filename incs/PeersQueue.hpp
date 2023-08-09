#ifndef PEERS_QUEUE_HPP
#define PEERS_QUEUE_HPP

#include <mutex>
#include <string>
#include <thread>
#include <vector>

class PeersQueue
{
   private:
    int i = 0;
    std::mutex mutexPeerQueue;
    std::vector<std::pair<std::string, long long> > peers;
    std::vector<bool> activePeers;

   public:
    const std::pair<std::string, long long>& getPeer()
    {
        std::lock_guard<std::mutex> lock(mutexPeerQueue);
        while (i == peers.size() || activePeers[i] != false)
        {
            if (i == peers.size())
                i = 0;
            else
                ++i;
        }
        activePeers[i] = true;
        return peers[i++];
    }

    void push_back(const std::pair<std::string, long long>& peer)
    {
        std::lock_guard<std::mutex> lock(mutexPeerQueue);
        peers.push_back(peer);
        activePeers.push_back(false);
    }

    bool hasFreePeers()
    {
        std::lock_guard<std::mutex> lock(mutexPeerQueue);
        for (int i = 0; i < activePeers.size(); ++i)
        {
            if (activePeers[i] == false)
                return true;
        }
        return false;
    }
};

#endif  // PEERS_QUEUE_HPP