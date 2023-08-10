#ifndef TORRENT_FILE_PARSER
#define TORRENT_FILE_PARSER

#include <memory>
#include <string>

#include "bencode.hpp"

struct singleFile
{
    std::string fileName;
    long long length;
};

struct multiFile
{
    std::string dirName;
    bencode::list files;
};

class TorrentFileParser
{
   private:
    std::string announce;
    long long pieceLength;
    std::string pieces;
    bool isSingle = false;
    std::string infoHash;

    std::variant<singleFile, multiFile> SingleMultiFile;  // TODO: add correct parsing of multiFile

   public:
    TorrentFileParser(const char* filePath);
    const std::string& getAnnounce() const;
    const long long& getPieceLength() const;
    const std::string& getPieces() const;
    bool IsSingle() const;
    const std::string& getFileName() const;
    long long getLengthOne() const;
    const std::string& getDirName() const;
    const bencode::list& getFiles() const;
    const std::string& getInfoHash() const;
};

#endif  // TORRENT_FILE_PARSER