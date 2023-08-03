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

    std::variant<singleFile, multiFile> SingleMultiFile;
    // info from SINGLE file mode:

    // info from MULTIPLE file mode

    // friend class PeerRetriever;

   public:
    TorrentFileParser(const char* filePath);
    const std::string& getAnnounce() const { return announce; }

    const long long& getPieceLength() const { return pieceLength; }

    const std::string& getPieces() const { return pieces; }

    const bool IsSingle() const { return isSingle; }

    const std::string& getFileName() const { return std::get<singleFile>(SingleMultiFile).fileName; }

    const long long getLengthOne() const { return std::get<singleFile>(SingleMultiFile).length; }

    const std::string& getDirName() const { return std::get<multiFile>(SingleMultiFile).dirName; }

    const bencode::list& getFiles() const { return std::get<multiFile>(SingleMultiFile).files; }

    const std::string& getInfoHash() const { return infoHash; }
};

#endif  // TORRENT_FILE_PARSER