/**KoalaWriter*******************************************************************************

  Version   [1.0]
  FileName  [editor.h]
  Author    [Mazzarelli Alessio - mazzarelli.alessio@gmail.com]
  License   [GPLv3]

********************************************************************************************/

#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTextEdit>
#include <QtGlobal>
#include <QPushButton>
#include <QFontComboBox>
#include <QColorDialog>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDialogButtonBox>
#include <QSlider>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

class Editor : public QWidget
{
    Q_OBJECT
public:
    Editor(int, int, int, int, QColor, int, QFont, qreal, bool, bool);
    ~Editor();
    QRect rect;
    void hidden(bool hide);
    bool hide;
    QTextEdit *text;
    QPushButton *backgroundPreviousB;
    QPushButton *backgroundNextB;
    QPushButton *backgroundColorB;
    bool saved;
    Phonon::MediaObject *clickObject;
    Phonon::AudioOutput *clickOutput;
    QString currentClick;
    QPushButton *keyboardPreviousB;
    QPushButton *keyboardNextB;
    QPushButton *keyboardMuteB;
    bool clickMuted;
    Phonon::MediaObject *musicObject;
    Phonon::AudioOutput *musicOutput;
    QString currentMusic;
    QPushButton *musicPreviousB;
    QPushButton *musicNextB;
    QPushButton *musicMuteB;
    bool musicMuted;
    void playMusic(void);
    int currentTextDimension;
    QFont currentFont;
    QColor currentTextColor;
    qreal opacity;

private:
    QPoint origin;
    QCursor cursor;
    QRect handleTL;
    QRect handleT;
    QRect handleTR;
    QRect handleL;
    QRect handleR;
    QRect handleBL;
    QRect handleB;
    QRect handleBR;
    bool resizingTL;
    bool resizingTR;
    bool resizingBL;
    bool resizingBR;
    bool resizingT;
    bool resizingB;
    bool resizingL;
    bool resizingR;
    bool move;
    int xOffset;
    int yOffset;
    int defaultTextDimension;
    QString currentFile;
    QPushButton *textB;
    QFontComboBox *fontComboBox;
    QPushButton *textColorB;
    QPushButton *textBDimension;
    QPushButton *textLessB;
    QPushButton *textDefaultB;
    QPushButton *textMoreB;
    QPushButton *backgroundB;
    QPushButton *musicB;
    QPushButton *keyboardB;
    QPushButton *fileB;
    QPushButton *loadB;
    QPushButton *saveB;
    QPushButton *saveAsB;
    QColorDialog *colorDialog;
    QTimer *timer;
    QSlider *slider;
    void updateTextStyle(void);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

public slots:
    void save(void);
    void replayMusic(void);

private slots:
    void documentWasModified();
    void changeTextFont(const QFont& font);
    void showColorDialog(void);
    void lessText(void);
    void defaultText(void);
    void moreText(void);
    void open(void);
    void saveAs(void);
    void autoSave(void);
    void setBackgroundOpacity(int);
};

#endif // EDITOR_H
