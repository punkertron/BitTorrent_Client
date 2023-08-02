#ifndef TORRENT_FILE_PARSER
#define TORRENT_FILE_PARSER

#include <memory>
#include <string>

#include "bencode.hpp"

// ‘piece length’, ‘name’, ‘pieces’ (hash list), and ‘paths’ and ‘lengths’ of all individual files.

class TorrentFileParser
{
   private:
    std::string announce;
    long long pieceLength;
    std::string pieces;

    bool isSingle = false;

    // info from SINGLE file mode:
    std::string fileName;
    long long lengthOne;

    // info from MULTIPLE file mode
    std::string dirName;
    bencode::list files;

   public:
    TorrentFileParser(const char* filePath);
    const std::string& getAnnounce() const { return announce; }

    const long long& getPieceLength() const { return pieceLength; }

    const std::string& getPieces() const { return pieces; }

    const bool IsSingle() const { return isSingle; }

    const std::string& getFileName() const { return fileName; }

    const long long getLengthOne() const { return lengthOne; }

    const std::string& getDirName() const { return dirName; }

    const bencode::list& getFiles() const { return files; }
};

#endif  // TORRENT_FILE_PARSER