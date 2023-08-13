#include <QApplication>
#include <filesystem>
#include <fstream>
#include <string>

#include "Window.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::off);

    QApplication app(argc, argv);

    Window window;
    window.show();

    return app.exec();
}
