#include "PeerRetriever.hpp"

#include <curl/curl.h>

#include "utils.hpp"
// #include "cpr/cpr.h"

// #define TRACKER_TIMEOUT 15000

PeerRetriever::PeerRetriever(const std::string& peerId, int port, const TorrentFileParser& tfp, long long bytesDownloaded) :
    peerId(peerId), port(port), fileSize(tfp.getLengthOne())
{
    allPeers = std::move(retrievePeers(tfp, bytesDownloaded));
    // for (const auto& pair : allPeers)
    // {
    //     std::cerr << "Ip = " << pair.first << "\t Port = " << pair.second << std::endl;
    // }
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::vector<std::pair<std::string, long long> > PeerRetriever::retrievePeers(const TorrentFileParser& tfp,
                                                                             long long bytesDownloaded)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    std::string response;
    if (curl)
    {
        std::string query(tfp.getAnnounce() + "?info_hash=" + urlEncodeHex(hexDecode(tfp.getInfoHash())) +
                          "&peer_id=" + peerId + "&port=" + std::to_string(port) + "&uploaded=" + "0" +
                          "&downloaded=" + std::to_string(bytesDownloaded) +
                          "&left=" + std::to_string(tfp.getLengthOne() - bytesDownloaded) + "&compact=1");
        // std::cerr << "Query = " << query << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "TorrentClient");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Bad curl" << std::endl;
            std::abort();
        }
        curl_easy_cleanup(curl);
        curl = NULL;
    }
    return decodeResponse(response);
}

std::vector<std::pair<std::string, long long> > PeerRetriever::decodeResponse(const std::string& response)
{
    std::vector<std::pair<std::string, long long> > result;

    try
    {
        auto data = bencode::decode(response);
        auto dict = std::get<bencode::dict>(data);
        // long long interval   = std::get<bencode::integer>(dict["interval"]);
        // long long complete   = std::get<bencode::integer>(dict["complete"]);
        // long long incomplete = std::get<bencode::integer>(dict["incomplete"]);

        try
        {
            auto peers = std::get<bencode::dict>(dict["peers"]);
            std::cerr << "Not impemented!" << std::endl;  // TODO:
            std::abort();
        }
        catch (...)
        {
            auto peers            = std::get<bencode::string>(dict["peers"]);
            const int segmentSize = 6;

            for (int i = 0; i + segmentSize <= peers.size(); i += segmentSize)
            {
                std::string segment        = peers.substr(i, segmentSize);
                std::string IPAddressBytes = segment.substr(0, 4);
                std::string portBytes      = segment.substr(4, 2);
                std::string peerIPAddress  = bytesToIPAddress(IPAddressBytes);
                long long peerPort         = bytesToPort(portBytes);

                result.push_back(std::make_pair(peerIPAddress, peerPort));
            }
        }
    }
    catch (...)
    {
        std::cerr << "Something bad with peer list!" << std::endl;
        std::abort();
    }
    return result;
}