#include "TorrentClient.hpp"

int main(int argc, char **argv)
{   
    if (argv[1] == NULL)
    {
        TorrentClient bla("./torrents/one_file.torrent");
        bla.run();
    }
    else
        TorrentClient tfp(argv[1]);



    return 0;
}
