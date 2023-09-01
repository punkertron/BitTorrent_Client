#include "Window.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QSize>
#include <QSpacerItem>
#include <QWindow>
#include <filesystem>
#include <stdexcept>
#include <thread>

#include "TorrentClient.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

static const std::string defaultDownloadPath()
{
    char* homePath = getenv("HOME");
    std::string defaultDownloadPath("./");
    if (homePath)
    {
        defaultDownloadPath = std::string(homePath) + "/Downloads/";
        std::filesystem::path directoryPath(defaultDownloadPath);
        if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath))
            defaultDownloadPath = "./";
    }
    return defaultDownloadPath;
}

Window::Window(QWidget* parent) : QWidget(parent), downloadDir(defaultDownloadPath())
{
    setStyleSheet("background-color: rgba(249, 255, 250, 1);");
    setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + QString("/Qt/resources/torrent_logo.png")));
    setWindowTitle("Torrent Client Qt");
    setMinimumSize(650, 350);
    resize(650, 350);

    initialScreen();
}

void Window::openTorrent()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Torrent File", "", "Torrent files (*.torrent)");
    SPDLOG_INFO("User picked torrent file: {}", filePath.toStdString());
    torrentPath = filePath.toStdString();
    if (!filePath.isEmpty())
        torrentPathText->setText(QString("Torrent: ") + QString(torrentPath.c_str()));
    else
        torrentPathText->setText("");
}

void Window::selectDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    SPDLOG_INFO("User picked download directory: {}", dirPath.toStdString());
    if (!dirPath.isEmpty())
    {
        downloadDir = dirPath.toStdString();
        if (*downloadDir.crbegin() != '/')
            downloadDir.push_back('/');
    }
    else
        downloadDir = defaultDownloadPath();
    downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
}

static QTextEdit* newTextObject()
{
    QTextEdit* res;

    res = new QTextEdit;
    res->setReadOnly(true);
    res->setStyleSheet("border: none; background-color: transparent;");
    res->setMaximumWidth(450);
    // res->setMinimumWidth(400);
    return res;
}

void Window::setCustomTextLines()
{
    torrentPathText = newTextObject();
    torrentPathText->setMinimumWidth(300);

    downloadDirText = newTextObject();
    downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
}

QPushButton* Window::newButtonObject(const char* buttonName, const QString& colour)
{
    QPushButton* res;

    res = new QPushButton(buttonName, this);
    res->setFixedSize(QSize(300, 40));
    res->setStyleSheet("background-color: " + colour + ";color: black;");
    return res;
}

void Window::setCustomButtons()
{
    buttonTorrentPath   = newButtonObject("Select Torrent file", "rgba(255, 190, 248, 1)");
    buttonDownloadDir   = newButtonObject("Select directory to download", "rgba(255, 252, 172, 1)");
    buttonStartDownload = newButtonObject("Start", "rgba(255, 183, 183, 1)");

    buttonStartDownload->setIcon(QIcon(QCoreApplication::applicationDirPath() + QString("/Qt/resources/rocket.png")));
    buttonStartDownload->setIconSize(QSize(30, 30));

    QObject::connect(buttonTorrentPath, SIGNAL(clicked()), this, SLOT(openTorrent()));
    QObject::connect(buttonDownloadDir, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QObject::connect(buttonStartDownload, SIGNAL(clicked()), this, SLOT(startDownload()));
}

void Window::setCustomLayout()
{
    layout = new QVBoxLayout(this);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    layout->addWidget(buttonTorrentPath, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(torrentPathText, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addWidget(buttonDownloadDir, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(downloadDirText, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(buttonStartDownload, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    layout->addWidget(cbLogs, 0, Qt::AlignmentFlag::AlignCenter);
    setLayout(layout);
}

void Window::downloadFile()
{
    try
    {
        TorrentClient tc(torrentPath.c_str(), downloadDir.c_str(), false);
        fileSize = tc.getFileSize();
        fileName = tc.getFileName();
        tc.run();
    }
    catch (const std::runtime_error& e)
    {
        isError = true;
        QMessageBox::critical(this, "Error", e.what());
    }
}

void Window::showCriticalMessageBox(const QString& message)
{
    QMessageBox::critical(this, "Error", message);
}

QProgressBar* Window::newProgressBarObject(int val)
{
    QProgressBar* res = new QProgressBar(this);
    res->setFixedHeight(70);
    res->setValue(val);
    return res;
}

static void formatLayoutDownload(QVBoxLayout* layout, QTextEdit* fileText, QProgressBar* progressBar,
                                 QPushButton* returnBack, bool beforeDownload)
{
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    layout->addSpacerItem(new QSpacerItem(0, 120, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(fileText, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addWidget(progressBar);

    if (beforeDownload)
    {
        layout->addSpacerItem(new QSpacerItem(0, 200, QSizePolicy::Minimum, QSizePolicy::Expanding));
        layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    }
    else
    {
        layout->addSpacerItem(new QSpacerItem(0, 125, QSizePolicy::Minimum, QSizePolicy::Expanding));
        layout->addWidget(returnBack, 0, Qt::AlignmentFlag::AlignCenter);
        layout->addSpacerItem(new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Fixed));
    }
}

void Window::displayDownloadStatus()
{
    std::filesystem::path filePath(downloadDir + fileName);
    std::uintmax_t currentFileSize = 0;
    double percent                 = 0;

    layout = new QVBoxLayout(this);
    setLayout(layout);

    QTextEdit* fileText = newTextObject();
    fileText->setText(QString(fileName.c_str()));

    QProgressBar* progressBar = newProgressBarObject(0);

    formatLayoutDownload(layout, fileText, progressBar, nullptr, true);
    QCoreApplication::processEvents();

    while (static_cast<long long>(currentFileSize) != fileSize && !isError)
    {
        try
        {
            currentFileSize = std::filesystem::file_size(filePath);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        percent = static_cast<double>(currentFileSize) / fileSize;
        progressBar->setValue(percent * 100);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    clearLayout();
    layout = new QVBoxLayout(this);

    fileText = newTextObject();
    fileText->setText(QString(fileName.c_str()));

    progressBar = newProgressBarObject(percent * 100);

    QPushButton* returnBack = newButtonObject("Return to main menu", "rgba(236, 245, 39, 1)");

    formatLayoutDownload(layout, fileText, progressBar, returnBack, false);

    QObject::connect(returnBack, SIGNAL(clicked()), this, SLOT(returnBack()));
    setLayout(layout);
}

void Window::startDownload()
{
    if (torrentPath != "")
    {
        std::thread thread(&Window::downloadFile, this);
        thread.detach();

        while (fileName == "" && !isError)  // wait until we recieve fileName from another thread
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (!isError)
        {
            clearLayout();
            displayDownloadStatus();
        }
        else
            torrentPathText->setText("");

        // reset torrent data
        torrentPath = "";
        fileName    = "";
        isError     = false;
    }
    else
        QMessageBox::critical(this, "Error", "Provide a torrent file!");
}

void Window::clearLayout()
{
    QLayoutItem* item;
    while ((item = layout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }
    delete layout;
}

void Window::setCheckBox()
{
    cbLogs = new QCheckBox("Enable logs (./logs/logs.txt)", this);
    if (isLogsEnanabled)
        cbLogs->setCheckState(Qt::Checked);
    else
        cbLogs->setCheckState(Qt::Unchecked);
    QObject::connect(cbLogs, &QCheckBox::stateChanged, this, &Window::enableDisableLogs);  // Not signal/slot?
}

void Window::enableDisableLogs()
{
    if (!isLogsSetupDone)
    {
        const std::string filePath((QCoreApplication::applicationDirPath() + QString("/logs/logs.txt")).toStdString());
        std::filesystem::resize_file(filePath, 0);

        spdlog::set_level(spdlog::level::info);
        auto logger = spdlog::basic_logger_mt("logger", "logs/logs.txt");
        logger->flush_on(spdlog::level::info);
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%-5l] [%-5t] [%s/%!]\t%v");
        SPDLOG_INFO("Start of the logging");
        isLogsSetupDone = true;
        isLogsEnanabled = true;
    }
    else
    {
        if (isLogsEnanabled)
        {
            SPDLOG_INFO("Logs disabled");
            spdlog::set_level(spdlog::level::off);
            isLogsEnanabled = false;
        }
        else
        {
            spdlog::set_level(spdlog::level::info);
            SPDLOG_INFO("Logs enabled");
            isLogsEnanabled = true;
        }
    }
}

void Window::initialScreen()
{
    setCheckBox();
    setCustomTextLines();
    setCustomButtons();
    setCustomLayout();
}

void Window::returnBack()
{
    clearLayout();
    initialScreen();
}
