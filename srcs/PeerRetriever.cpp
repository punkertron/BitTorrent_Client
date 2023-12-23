#include "PeerRetriever.hpp"

#include <arpa/inet.h>
#include <curl/curl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "spdlog/spdlog.h"
#include "utils.hpp"

PeerRetriever::PeerRetriever(const std::string& peerId, int port, const TorrentFileParser& tfp, long long bytesDownloaded) :
    port(port), fileSize(tfp.getLengthOne()), peerId(peerId)
{
    allPeers = retrievePeers(tfp, bytesDownloaded);
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

    std::string response;
    std::vector<std::pair<std::string, long long> > peers;

    size_t i = 0;
    while (!peers.size())
    {
        curl = curl_easy_init();
        if (curl)
        {
            std::string announce;
            if (i == 0)
                announce = tfp.getAnnounce();
            else
            {
                if (i <= tfp.getAnnounce_listSize())
                    announce = tfp.getAnnounce_listI(i - 1);
                else
                    announce = tfp.getAnnounce();
            }

            std::string query(announce + "?info_hash=" + urlEncodeHex(hexDecode(tfp.getInfoHash())) + "&peer_id=" + peerId +
                              "&port=" + std::to_string(port) + "&uploaded=" + "0" +
                              "&downloaded=" + std::to_string(bytesDownloaded) +
                              "&left=" + std::to_string(tfp.getLengthOne() - bytesDownloaded) + "&compact=1");
            SPDLOG_INFO("Query is {}", query);

            if (query.size() > 3 && query.substr(0, 3) == "udp")
            {
                SPDLOG_INFO("Retrieve peers using CURL");
                // TODO:
            }
            else
            {
                SPDLOG_INFO("Retrieve peers using CURL");
                curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "TorrentClient");
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                    SPDLOG_DEBUG("CURL failed");
                curl_easy_cleanup(curl);
                curl = NULL;
            }
            ++i;
            if (i == 1'000'000)
                break;
        }
        peers = decodeResponse(response);
    }
    return peers;
}

std::vector<std::pair<std::string, long long> > PeerRetriever::decodeResponse(const std::string& response)
{
    std::vector<std::pair<std::string, long long> > result{};

    try
    {
        auto data = bencode::decode(response);
        auto dict = std::get<bencode::dict>(data);
        interval  = std::get<bencode::integer>(dict["interval"]);
        try
        {
            auto peers = std::get<bencode::list>(dict["peers"]);

            for (const auto& peer : peers)
            {
                bencode::dict peerDict    = std::get<bencode::dict>(peer);
                std::string peerIPAddress = std::get<bencode::string>(peerDict["ip"]);
                // std::string peerId = std::get<bencode::string>(peerDict["peer id"]);
                long long peerPort = std::get<bencode::integer>(peerDict["port"]);
                result.push_back(std::make_pair(peerIPAddress, peerPort));
            }
        }
        catch (...)
        {
            auto peers            = std::get<bencode::string>(dict["peers"]);
            const int segmentSize = 6;

            for (size_t i = 0; i + segmentSize <= peers.size(); i += segmentSize)
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
        SPDLOG_ERROR("No peers from tracker OR some other parsing error");
    }
    SPDLOG_INFO("Amount of peers: {}", result.size());
    return result;
}

const std::vector<std::pair<std::string, long long> >& PeerRetriever::getPeers() const
{
    return allPeers;
}

long long PeerRetriever::getInterval() const
{
    return interval;
}
