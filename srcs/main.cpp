#include <fstream>

#include "TorrentFileParser.hpp"

int main(int argc, char **argv)
{
    if (argv[1] == NULL)
        TorrentFileParser bla("one_file.torrent");
    TorrentFileParser tfp(argv[1]);

    return 0;
}
