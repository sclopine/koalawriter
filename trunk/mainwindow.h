/**KoalaWriter*******************************************************************************

  Version   [1.0]
  FileName  [mainwindow.h]
  Author    [Mazzarelli Alessio - mazzarelli.alessio@gmail.com]
  License   [GPLv3]

********************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>
#include <QDir>
#include <QDesktopWidget>
#include <QSettings>
#include "editor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Editor *editor;
    void lookForBackGround(void);
    void lookForClick(void);
    void lookForMusic(void);
    void setBackground(int pos);
    void setClick(int pos);
    void setMusic(int pos);
    void searchBackground(void);
    void searchClick(void);
    void searchMusic(void);
    QDir backgroundDir;
    QDir clickDir;
    QDir musicDir;
    QStringList *backgroundList;
    QStringList *clickList;
    QStringList *musicList;
    int currentBackground;
    int currentClick;
    int currentMusic;
    QColorDialog *colorDialog;
    void writeSettings(void);
    void readSettings(void);
    int startX;
    int startY;
    int width;
    int height;
    QColor fontColor;
    int fontSize;
    QFont fontFamily;
    qreal opacity;
    QString background;
    QColor backgroundColor;
    QString click;
    QString music;
    bool clickMute;
    bool musicMute;

public slots:
    void nextBackground(void);
    void previousBackground(void);
    void colorBackground(void);
    void nextClick(void);
    void previousClick(void);
    void muteClick(void);
    void nextMusic(void);
    void previousMusic(void);
    void muteMusic(void);
};

#endif // MAINWINDOW_H
