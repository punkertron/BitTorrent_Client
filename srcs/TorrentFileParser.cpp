#include "TorrentFileParser.hpp"

#include <fstream>

#include "utils.hpp"

TorrentFileParser::TorrentFileParser(const char* filePath) : SingleMultiFile(multiFile())
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Can't open " << filePath << std::endl;
        std::abort();
    }

    try
    {
        auto data   = bencode::decode(file);
        auto dict   = std::get<bencode::dict>(data);
        announce    = std::get<bencode::string>(dict["announce"]);
        auto info   = std::get<bencode::dict>(dict["info"]);
        infoHash    = sha1(bencode::encode(info));
        pieceLength = std::get<bencode::integer>(info["piece length"]);
        pieces      = std::get<bencode::string>(info["pieces"]);

        try
        {
            std::get<multiFile>(SingleMultiFile).files = std::get<bencode::list>(info["files"]);
        }
        catch (...)
        {
            SingleMultiFile                                = singleFile();
            isSingle                                       = true;
            std::get<singleFile>(SingleMultiFile).fileName = std::get<bencode::string>(info["name"]);
            std::get<singleFile>(SingleMultiFile).length   = std::get<bencode::integer>(info["length"]);
            file.close();
            return;
        }
        std::get<multiFile>(SingleMultiFile).dirName = std::get<bencode::string>(info["name"]);
        file.close();
    }

    catch (...)
    {
        std::cerr << "Something really bad!" << std::endl;
        file.close();
        std::abort();
    }
}

const std::string& TorrentFileParser::getAnnounce() const
{
    return announce;
}

const long long& TorrentFileParser::getPieceLength() const
{
    return pieceLength;
}

const std::string& TorrentFileParser::getPieces() const
{
    return pieces;
}

bool TorrentFileParser::IsSingle() const
{
    return isSingle;
}

const std::string& TorrentFileParser::getFileName() const
{
    return std::get<singleFile>(SingleMultiFile).fileName;
}

long long TorrentFileParser::getLengthOne() const
{
    return std::get<singleFile>(SingleMultiFile).length;
}

const std::string& TorrentFileParser::getDirName() const
{
    return std::get<multiFile>(SingleMultiFile).dirName;
}

const bencode::list& TorrentFileParser::getFiles() const
{
    return std::get<multiFile>(SingleMultiFile).files;
}

const std::string& TorrentFileParser::getInfoHash() const
{
    return infoHash;
}
