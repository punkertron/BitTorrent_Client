#include <fstream>

#include "TorrentFileParser.hpp"

int main(int argc, char **argv)
{
    if (argv[1] == NULL)
        TorrentFileParser bla("./torrents/one_file.torrent");
    else
        TorrentFileParser tfp(argv[1]);

    return 0;
}
