#include "PeerRetriever.hpp"

#include <curl/curl.h>

#include "utils.hpp"
// #include "cpr/cpr.h"

// #define TRACKER_TIMEOUT 15000

PeerRetriever::PeerRetriever(const std::string& peerId, int port, long long fileSize)
    : peerId(peerId), port(port), fileSize(fileSize)
{
    ;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::vector<Peer> PeerRetriever::retrievePeers(const TorrentFileParser& tfp, long long bytesDownloaded)
{
    // cpr::Response res = cpr::Get(cpr::Url{tfp.getAnnounce()}, cpr::Parameters {
    //         { "info_hash", std::string(hexDecode(tfp.getInfoHash())) },
    //         { "peer_id", std::string("UT2021-484955672893") },
    //         { "port", std::to_string(8080) },
    //         { "uploaded", std::to_string(0) },
    //         { "downloaded", std::to_string(bytesDownloaded) },
    //         { "left", std::to_string(tfp.getLengthOne() - bytesDownloaded) },
    //         { "compact", std::to_string(1) }
    //     }, cpr::Timeout{ TRACKER_TIMEOUT }
    // );
    // std::cerr << res.text << std::endl;

    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        std::string query(tfp.getAnnounce() + "?info_hash=" + urlEncodeHex(hexDecode(tfp.getInfoHash())) +
                          "&peer_id=" + "-UT2021-484955672893"  // peerId
                          + "&port=" + std::to_string(port) + "&uploaded=" + "0" + "&downloaded=" +
                          std::to_string(bytesDownloaded) + "&left=" + std::to_string(tfp.getLengthOne() - bytesDownloaded) + "&compact=1");
        std::cerr << "Query = " << query << std::endl;

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);  // TODO
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::abort();
        curl_easy_cleanup(curl);
        curl = NULL;
        std::cerr << response << std::endl;
    }
    return std::vector<Peer>();
}
