#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class QPushButton;
class Window : public QWidget
{
    Q_OBJECT
   private:
    QPushButton *m_buttonTorrentPath;
    QPushButton *m_buttonDownloadDir;
    QPushButton *m_buttonStartDownload;

    std::string torrentPath;
    std::string downloadDir;
    std::string fileName;
    long long fileSize;
    bool isError = false;

    QTextEdit *m_torrentPathText;
    QTextEdit *m_downloadDirText;
    QVBoxLayout *layout;

    QPushButton *newButtonObject(const char *buttonName, const QString &colour);
    QProgressBar *newProgressBarObject(int val);

    void initialScreen();
    void setCustomTextLines();
    void setCustomButtons();
    void setCustomLayout();
    void clearLayout();

    void downloadFile();

    void displayDownloadStatus();

   private slots:
    void openTorrent();
    void selectDirectory();
    void startDownload();
    void returnBack();

   public:
    explicit Window(QWidget *parent = 0);
};

#endif  // WINDOW_HPP