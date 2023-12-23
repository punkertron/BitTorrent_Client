#include <filesystem>
#include <fstream>

#include "TorrentClient.hpp"
#include "cxxopts.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Try:\n./torrent_client -h" << std::endl;
        return 1;
    }
    char *homePath = getenv("HOME");
    std::string defaultDownloadPath("./");
    if (homePath)
    {
        defaultDownloadPath = std::string(homePath) + "/Downloads/";
        std::filesystem::path directoryPath(defaultDownloadPath);
        if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath))
            defaultDownloadPath = "./";
    }

    bool enableLogs;
    std::string torrentPath;
    std::string downloadPath;
    try
    {
        cxxopts::Options options("torrent_client", "The best BitTorrent Client! v1.0");
        options.add_options()("t,torrent", "Location of the .torrent", cxxopts::value<std::string>())(
            "d,directory", "Where to save file", cxxopts::value<std::string>()->default_value(defaultDownloadPath))(
            "l,logs", "Enable logs (./logs/logs.txt)", cxxopts::value<bool>()->default_value("false"))("h,help",
                                                                                                       "Print usage");
        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 2;
        }
        enableLogs   = result["logs"].as<bool>();
        downloadPath = result["directory"].as<std::string>();
        if (*downloadPath.crbegin() != '/')
            downloadPath.push_back('/');
        torrentPath = result["torrent"].as<std::string>();
    }
    catch (const cxxopts::exceptions::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 3;
    }

    std::string logPath;
    if (enableLogs)
    {
        char exePath[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
        if (len != -1)
        {
            exePath[len] = '\0';

            // Extracting the directory from the executable path
            const std::string exeDir = std::string(exePath).substr(0, std::string(exePath).find_last_of('/'));
            const std::string logDir = exeDir + "/logs";
            logPath = logDir + "/logs.txt";
            
            if (!std::filesystem::exists(logDir))
            {
                std::filesystem::create_directory(logDir);
            }

            // clean old logs
            try
            {
                std::ofstream ofs(logPath, std::ios::trunc);
                ofs.close();
            }
            catch (...)  // do nothing?
            {
            }
        }

        auto logger = spdlog::basic_logger_mt("logger", logPath);
        logger->flush_on(spdlog::level::info);
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%-5l] [%-5t] [%s/%!]\t%v");
        SPDLOG_INFO("Start of the program");
    }
    else
        spdlog::set_level(spdlog::level::off);

    try
    {
        TorrentClient tc(torrentPath.c_str(), downloadPath.c_str());
        tc.run();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
