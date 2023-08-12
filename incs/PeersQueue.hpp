#ifndef PEERS_QUEUE_HPP
#define PEERS_QUEUE_HPP

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class PeersQueue
{
   private:
    size_t i = 0;
    std::mutex mutexPeerQueue;
    std::vector<std::pair<std::string, long long> > peers;
    std::vector<bool> activePeers;

   public:
    PeersQueue()                                   = default;
    ~PeersQueue()                                  = default;
    PeersQueue& operator=(const PeersQueue& other) = delete;
    PeersQueue(const PeersQueue& other)            = delete;
    PeersQueue(PeersQueue&& other)                 = delete;

    const std::pair<std::string, long long> getPeer();
    void push_back(const std::pair<std::string, long long>& peer);
    bool hasFreePeers();
    size_t size();
};

#endif  // PEERS_QUEUE_HPP