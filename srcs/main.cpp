#include <fstream>

#include "TorrentClient.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    std::ofstream ofs("logs/logs.txt", std::ios::trunc);
    ofs.close();

    // spdlog::set_level(spdlog::level::off);  // disable logs

    auto logger = spdlog::basic_logger_mt("logger", "logs/logs.txt");
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%-5l] [%-5t] [%s/%!]\t%v");
    SPDLOG_INFO("Start of the program");

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
