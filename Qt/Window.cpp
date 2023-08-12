#include "Window.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSize>
#include <QSpacerItem>
#include <QWindow>
#include <filesystem>
#include <thread>

#include "TorrentClient.hpp"

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
    setMinimumSize(650, 300);
    resize(650, 300);

    initialScreen();
}

void Window::openTorrent()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Torrent File", "", "Torrent files (*.torrent)");
    if (!filePath.isEmpty())
    {
        torrentPath = filePath.toStdString();
        m_torrentPathText->setText(QString("Torrent: ") + QString(torrentPath.c_str()));
    }
}

void Window::selectDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dirPath.isEmpty())
    {
        downloadDir = dirPath.toStdString();
        if (*downloadDir.crbegin() != '/')
            downloadDir.push_back('/');
    }
    else
        downloadDir = "./";
    m_downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
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
    m_torrentPathText = newTextObject();

    m_downloadDirText = newTextObject();
    m_downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
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
    m_buttonTorrentPath   = newButtonObject("Select Torrent file", "rgba(255, 190, 248, 1)");
    m_buttonDownloadDir   = newButtonObject("Select directory to download", "rgba(255, 252, 172, 1)");
    m_buttonStartDownload = newButtonObject("Start", "rgba(255, 183, 183, 1)");

    m_buttonStartDownload->setIcon(QIcon(QCoreApplication::applicationDirPath() + QString("/Qt/resources/rocket.png")));
    m_buttonStartDownload->setIconSize(QSize(30, 30));

    QObject::connect(m_buttonTorrentPath, SIGNAL(clicked()), this, SLOT(openTorrent()));
    QObject::connect(m_buttonDownloadDir, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QObject::connect(m_buttonStartDownload, SIGNAL(clicked()), this, SLOT(startDownload()));
}

void Window::setCustomLayout()
{
    layout = new QVBoxLayout(this);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    layout->addWidget(m_buttonTorrentPath, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(m_torrentPathText, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addWidget(m_buttonDownloadDir, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(m_downloadDirText, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(m_buttonStartDownload, 0, Qt::AlignmentFlag::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    setLayout(layout);
}

void Window::downloadFile()
{
    TorrentClient tc(torrentPath.c_str(), downloadDir.c_str(), false);
    fileSize = tc.getFileSize();
    fileName = tc.getFileName();
    tc.run();
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

    while (fileSize != static_cast<long long>(currentFileSize))
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
        QCoreApplication::processEvents();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    clearLayout();
    layout = new QVBoxLayout(this);

    fileText = newTextObject();
    fileText->setText(QString(fileName.c_str()));

    progressBar = newProgressBarObject(100);

    QPushButton* returnBack = newButtonObject("Return to main menu", "rgba(236, 245, 39, 1)");

    formatLayoutDownload(layout, fileText, progressBar, returnBack, false);

    QObject::connect(returnBack, SIGNAL(clicked()), this, SLOT(returnBack()));
    setLayout(layout);
    torrentPath = "";
}

void Window::startDownload()
{
    if (torrentPath != "")
    {
        std::thread thread(&Window::downloadFile, this);
        thread.detach();

        while (fileName == "")  // wait until we recieve fileName from another thread
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        clearLayout();
        displayDownloadStatus();
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

void Window::initialScreen()
{
    setCustomTextLines();
    setCustomButtons();
    setCustomLayout();
}

void Window::returnBack()
{
    clearLayout();
    initialScreen();
}
