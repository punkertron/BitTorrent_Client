#ifndef PEERS_QUEUE_HPP
#define PEERS_QUEUE_HPP

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

class PeersQueue
{
   private:
    static const int maxPeersQueueSize = 300;  // if more than 300, we clean current peers

    size_t i = 0;
    std::mutex mutexPeerQueue;

    // ip, port
    std::vector<std::pair<std::string, long long> > peers;
    std::vector<std::pair<std::string, long long> > dummyPeers;  // we've caught errors with them

    std::chrono::time_point<std::chrono::steady_clock> lastPushTime = std::chrono::steady_clock::now();

   public:
    PeersQueue()                                   = default;
    ~PeersQueue()                                  = default;
    PeersQueue& operator=(const PeersQueue& other) = delete;
    PeersQueue(const PeersQueue& other)            = delete;
    PeersQueue(PeersQueue&& other)                 = delete;
    PeersQueue& operator=(PeersQueue&& other)      = delete;

    const std::pair<std::string, long long> getPeer();
    void push_back(const std::pair<std::string, long long>& peer);
    void reportBadPeer(const std::pair<std::string, long long>& peer);
    bool hasFreePeers();
    size_t size();
};

#endif  // PEERS_QUEUE_HPP