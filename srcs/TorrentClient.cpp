#include <curl/curl.h>

#include "TorrentClient.hpp"
#include "PeerRetriever.hpp"

#define PORT 8080

TorrentClient::TorrentClient(const char* filePath):
    tfp(filePath)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

TorrentClient::~TorrentClient()
{
    curl_global_cleanup();
}

void TorrentClient::run()
{
    PeerRetriever p(std::string("BLA"), PORT, tfp.getLengthOne());
}
