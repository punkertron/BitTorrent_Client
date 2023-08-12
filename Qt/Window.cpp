#include "Window.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QPushButton>
#include <QSize>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "TorrentClient.hpp"

// #include <iostream>

#include <filesystem>

static const std::string defaultDownloadPath()
{
    char *homePath = getenv("HOME");
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

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), downloadDir(defaultDownloadPath())
{
    setStyleSheet("background-color: rgba(231, 255, 236, 1);");
    setWindowIcon(QIcon(QString(getenv("PWD")) + QString("/Qt/resources/torrent_logo.png")));
    setWindowTitle("Torrent Client Qt");
    setMinimumSize(350, 150);
    resize(650, 300);

    setCustomTextLines();
    setCustomButtons();
    setCustomLayout();
}

void MainWindow::openTorrent()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Torrent File", "", "Torrent files (*.torrent)");
    torrentPath      = filePath.toStdString();
    m_torrentPathText->setText(QString("Torrent: ") + QString(torrentPath.c_str()));
    // std::cerr << "Torrent file: " << torrentPath << std::endl;
}

void MainWindow::selectDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    downloadDir     = dirPath.toStdString();
    if (*downloadDir.crbegin() != '/')
        downloadDir.push_back('/');
    m_downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
    // std::cerr << "Dowload dir: " << downloadDir << std::endl;
}

void MainWindow::setCustomTextLines()
{
    m_torrentPathText = new QTextEdit;
    m_torrentPathText->setReadOnly(true);
    m_torrentPathText->setStyleSheet("border: none; background-color: transparent;");
    m_torrentPathText->setMinimumWidth(300);

    m_downloadDirText = new QTextEdit;
    m_downloadDirText->setReadOnly(true);
    m_downloadDirText->setStyleSheet("border: none; background-color: transparent;");
    m_downloadDirText->setText(QString("Path: ") + QString(downloadDir.c_str()));
    m_torrentPathText->setMinimumWidth(300);
}

void MainWindow::setCustomButtons()
{
    m_buttonTorrentPath   = new QPushButton("Select Torrent file", this);
    m_buttonDownloadDir   = new QPushButton("Select directory to download", this);
    m_buttonStartDownload = new QPushButton("Start", this);

    m_buttonTorrentPath->setFixedSize(QSize(300, 40));
    m_buttonDownloadDir->setFixedSize(QSize(300, 40));
    m_buttonStartDownload->setFixedSize(QSize(300, 40));

    m_buttonTorrentPath->setStyleSheet("background-color: rgba(255, 190, 248, 1); color: black;");
    m_buttonDownloadDir->setStyleSheet("background-color: rgba(255, 252, 172, 1); color: black;");
    m_buttonStartDownload->setStyleSheet("background-color: rgba(255, 183, 183, 1); color: black;");

    m_buttonStartDownload->setIcon(QIcon(QString(getenv("PWD")) + QString("/Qt/resources/rocket.png")));
    m_buttonStartDownload->setIconSize(QSize(30, 30));

    QObject::connect(m_buttonTorrentPath, SIGNAL(clicked()), this, SLOT(openTorrent()));
    QObject::connect(m_buttonDownloadDir, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QObject::connect(m_buttonStartDownload, SIGNAL(clicked()), this, SLOT(startDownload()));
}

void MainWindow::setCustomLayout()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
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

void MainWindow::startDownload()
{
    if (torrentPath != "")
    {
        TorrentClient tc(torrentPath.c_str(), downloadDir.c_str(), false);
        tc.run();
    }
}
