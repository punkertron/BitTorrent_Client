#include "TorrentClient.hpp"

int main(int argc, char **argv)
{
    if (argv[1] == NULL)
    {
        TorrentClient bla("./resources/debian.torrent");
        bla.run();
    }
    else
    {
        TorrentClient tfp(argv[1]);
        tfp.run();
    }

    return 0;
}
