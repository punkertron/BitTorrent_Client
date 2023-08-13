#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class QPushButton;
class Window : public QWidget
{
    Q_OBJECT
   private:
    QCheckBox *m_cbLogs;
    QPushButton *m_buttonTorrentPath;
    QPushButton *m_buttonDownloadDir;
    QPushButton *m_buttonStartDownload;

    std::string torrentPath;
    std::string downloadDir;
    std::string fileName;
    long long fileSize;
    bool isError         = false;
    bool isLogsSetupDone = false;
    bool isLogsEnanabled = false;

    QTextEdit *m_torrentPathText;
    QTextEdit *m_downloadDirText;
    QVBoxLayout *layout;

    QPushButton *newButtonObject(const char *buttonName, const QString &colour);
    QProgressBar *newProgressBarObject(int val);

    void initialScreen();

    void setCheckBox();
    void setCustomTextLines();
    void setCustomButtons();
    void setCustomLayout();
    void clearLayout();

    void downloadFile();

    void displayDownloadStatus();

    void showCriticalMessageBox(const QString &message);

   private slots:
    void openTorrent();
    void selectDirectory();
    void startDownload();
    void returnBack();
    void enableDisableLogs();

   public:
    explicit Window(QWidget *parent = 0);
};

#endif  // WINDOW_HPP