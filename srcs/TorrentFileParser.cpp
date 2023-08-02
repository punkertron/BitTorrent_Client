#include "TorrentFileParser.hpp"

#include <fstream>

TorrentFileParser::TorrentFileParser(const char* filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::abort();
    }

    try
    {
        auto data   = bencode::decode(file);
        auto dict   = std::get<bencode::dict>(data);
        announce    = std::get<bencode::string>(dict["announce"]);
        auto info   = std::get<bencode::dict>(dict["info"]);
        pieceLength = std::get<bencode::integer>(info["piece length"]);
        pieces      = std::get<bencode::string>(info["pieces"]);
        try
        {
            files = std::get<bencode::list>(info["files"]);
        }
        catch (...)
        {
            isSingle  = true;
            fileName  = std::get<bencode::string>(info["name"]);
            lengthOne = std::get<bencode::integer>(info["length"]);
            file.close();
            return;
        }
        dirName = std::get<bencode::string>(info["name"]);
        file.close();
    }
    catch (...)
    {
        file.close();
        std::abort();
    }
}
