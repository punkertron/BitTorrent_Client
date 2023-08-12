#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QTextEdit>
#include <QWidget>

class QPushButton;
class MainWindow : public QWidget
{
    Q_OBJECT
   private:
    QPushButton *m_buttonTorrentPath;
    QPushButton *m_buttonDownloadDir;
    QPushButton *m_buttonStartDownload;

    std::string torrentPath;
    std::string downloadDir;

    QTextEdit *m_torrentPathText;
    QTextEdit *m_downloadDirText;

    void setCustomTextLines();
    void setCustomButtons();
    void setCustomLayout();

   private slots:
    void openTorrent();
    void selectDirectory();
    void startDownload();

   public:
    explicit MainWindow(QWidget *parent = 0);
};

#endif  // WINDOW_HPP