#include <curl/curl.h>

#include "PeerRetriever.hpp"

PeerRetriever::PeerRetriever(const std::string& peerId, int port, long long fileSize):
    peerId(peerId), port(port), fileSize(fileSize)
{
    ;
}

std::vector<Peer> PeerRetriever::retrievePeers(const TorrentFileParser& tfp, long long bytesDownloaded)
{
    CURL* curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if (curl)
    {
        std::string query (tfp.getAnnounce());
        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); // TODO
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    CURLcode res;
}
