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
            std::get<multiFile>(SingleMultiFile).files = std::get<bencode::list>(info["files"]);
        }
        catch (...)
        {
            isSingle  = true;
            std::get<singleFile>(SingleMultiFile).fileName  = std::get<bencode::string>(info["name"]);
            std::get<singleFile>(SingleMultiFile).length = std::get<bencode::integer>(info["length"]);
            file.close();
            return ;
        }
        std::get<multiFile>(SingleMultiFile).dirName = std::get<bencode::string>(info["name"]);
        file.close();
    }
    catch (...)
    {
        file.close();
        std::abort();
    }
}
