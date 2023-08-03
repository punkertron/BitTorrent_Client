#include "TorrentClient.hpp"

#include <curl/curl.h>

#include "PeerRetriever.hpp"

#ifndef PORT
#define PORT 8080
#endif

#ifndef PEER_ID
#define PEER_ID "-TR1000-000123456789"
#endif

TorrentClient::TorrentClient(const char* filePath) : tfp(filePath) { curl_global_init(CURL_GLOBAL_DEFAULT); }

TorrentClient::~TorrentClient() { curl_global_cleanup(); }

void TorrentClient::run() { PeerRetriever p(std::string(PEER_ID), PORT, tfp, 0); }
