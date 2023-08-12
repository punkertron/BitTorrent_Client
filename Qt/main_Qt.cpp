#include <QApplication>
#include <filesystem>
#include <fstream>

#include "Window.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::off);

    QApplication app(argc, argv);

    Window window;
    window.show();

    return app.exec();
}
