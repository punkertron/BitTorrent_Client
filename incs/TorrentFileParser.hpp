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

    std::vector<std::string> announceList;
    void fillAnnounceList(const bencode::list& announce_list);

    std::variant<singleFile, multiFile> SingleMultiFile;  // TODO: add correct parsing of multiFile

   public:
    explicit TorrentFileParser(const char* filePath);
    ~TorrentFileParser()                              = default;
    TorrentFileParser(const TorrentFileParser& other) = default;

    TorrentFileParser()                                          = delete;
    TorrentFileParser& operator=(const TorrentFileParser& other) = delete;
    TorrentFileParser(TorrentFileParser&& other)                 = delete;
    TorrentFileParser& operator=(TorrentFileParser&& other)      = delete;

    const std::string& getAnnounce() const;
    const long long& getPieceLength() const;
    const std::string& getPieces() const;
    bool IsSingle() const;
    const std::string& getFileName() const;
    long long getLengthOne() const;
    const std::string& getDirName() const;
    const bencode::list& getFiles() const;
    const std::string& getInfoHash() const;
    size_t getAnnounce_listSize() const;
    const std::string& getAnnounce_listI(size_t i) const;
};

#endif  // TORRENT_FILE_PARSER