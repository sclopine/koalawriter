/**KoalaWriter*******************************************************************************

  Version   [1.0]
  FileName  [editor.cpp]
  Author    [Mazzarelli Alessio - mazzarelli.alessio@gmail.com]
  License   [GPLv3]

********************************************************************************************/

#include "editor.h"

Editor::Editor(int startX, int startY, int width, int height, QColor fontColor, int fontSize, QFont fontFamily,
               qreal gopacity, bool gclickMuted, bool gmusicMuted)
{
    clickObject = new Phonon::MediaObject(this);
    clickObject->setCurrentSource(currentClick);
    clickOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(clickObject, clickOutput);
    clickMuted = gclickMuted;
    musicObject = new Phonon::MediaObject(this);
    musicObject->setCurrentSource(currentMusic);
    musicOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(musicObject, musicOutput);
    connect(musicObject, SIGNAL(aboutToFinish()), this, SLOT(replayMusic(void)));
    musicMuted = gmusicMuted;
    timer = new QTimer(this);
    timer->start(60000*3); //autosave every 3 min
    currentFile = "";
    connect(timer, SIGNAL(timeout()), this, SLOT(autoSave()));
    saved = true;
    rect = QRect(startX,startY,width,height); //Default size of border
    opacity = gopacity;
    text = new QTextEdit(this);
    text->setGeometry(rect.x()+1,rect.y()+1,rect.width()-1,rect.height()-1); //Draw text edit 1 pixel smaller than border
    text->viewport()->setAutoFillBackground(false); //make trsparent background but not text
    text->setFrameShape(QFrame::NoFrame); //hide frame
/***************************************************************
    text->setStyleSheet(
            "QScrollBar:vertical {"
                  "border: 0px;"
                  "background: transparent;"
                  "width: 15px;"
                  "margin: 22px 0 22px 0;"
              "}"
              "QScrollBar::handle:vertical {"
                  "border: 1px solid" + currentTextColor.name() + ";"
                  "background: transparent;"
                  "min-height: 20px;"
              "}"
              "QScrollBar::add-line:vertical {"
                  "border: 0px;"
                  "background: transparent;"
                  "height: 20px;"
                  "subcontrol-position: bottom;"
                  "subcontrol-origin: margin;"
              "}"

              "QScrollBar::sub-line:vertical {"
                  "border: 0px;"
                  "background: transparent;"
                  "height: 20px;"
                  "subcontrol-position: top;"
                  "subcontrol-origin: margin;"
              "}"

              "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
                  "border: 2px solid" + currentTextColor.name() + ";"
                  "width: 3px;"
                  "height: 3px;"
                  "background:" + currentTextColor.name() + ";"
              "}"

              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                  "background: none;"
              "}"

              "QScrollBar:horizontal {"
                  "border: 0px;"
                  "background: transparent;"
                  "height: 15px;"
                  "margin: 0px 22px 0 22px;"
              "}"

              "QScrollBar::handle:horizontal {"
                  "border: 1px solid" + currentTextColor.name() + ";"
                  "background: transparent;"
                  "min-width: 20px;"
              "}"

              "QScrollBar::add-line:horizontal {"
                  "border: 0px;"
                  "background: transparent;"
                  "width: 20px;"
                  "subcontrol-position: right;"
                  "subcontrol-origin: margin;"
              "}"

              "QScrollBar::sub-line:horizontal {"
                  "border: 0px;"
                  "background: transparent;"
                  "width: 20px;"
                  "subcontrol-position: left;"
                  "subcontrol-origin: margin;"
              "}"

              "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
                  "border: 2px solid" + currentTextColor.name() + ";"
                  "width: 3px;"
                  "height: 3px;"
                  "background:" + currentTextColor.name() + ";"
              "}"

              "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
                   "background: none;"
              "}"
    );
*****************************************************************/
    text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //To make KoalaWriter behave like OmmWriter
    cursor = QCursor();
    this->setMouseTracking(true); //Permit mouse tracking also if no button is pressed

    //Define all handle 6x6 square center in the rect angle and in half side
    handleTL = QRect(QPoint(rect.topLeft().x()-3,rect.topLeft().y()-3),QSize(6,6));
    handleT = QRect(QPoint(rect.center().x()-3,rect.top()-3),QSize(6,6));
    handleTR = QRect(QPoint(rect.topRight().x()-3,rect.topRight().y()-3),QSize(6,6));
    handleL = QRect(QPoint(rect.left()-3,rect.center().y()-3),QSize(6,6));
    handleR = QRect(QPoint(rect.right()-3,rect.center().y()-3),QSize(6,6));
    handleBL = QRect(QPoint(rect.bottomLeft().x()-3,rect.bottomLeft().y()-3),QSize(6,6));
    handleB = QRect(QPoint(rect.center().x()-3,rect.bottom()-3),QSize(6,6));
    handleBR = QRect(QPoint(rect.bottomRight().x()-3,rect.bottomRight().y()-3),QSize(6,6));
    resizingTL = false;
    resizingTR = false;
    resizingBL = false;
    resizingBR = false;
    resizingT = false;
    resizingB = false;
    resizingL = false;
    resizingR = false;
    move = false;


    //Offset for spacing elements
    xOffset = 35;
    yOffset = 35;

    // Opacity slider
    slider = new QSlider(Qt::Horizontal, this);
    slider->setStyleSheet(
            "QSlider::groove:horizontal {"
                 "border: 2px solid white;"
                 "height: 6px;"
                 "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #424242, stop:1 black);"
                 "margin: 3px 0;"
                 "border-radius: 3px;"
             "}"

             "QSlider::handle:horizontal {"
                 "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #424242, stop:1 black);"
                 "border: 2px solid white;"
                 "width: 18px;"
                 "margin: -2px 0;"
                 "border-radius: 3px;"
             "}"
    );
    slider->setGeometry(rect.center().x()-60, rect.bottom()+10, 120, 20);
    slider->setRange(0, 10);
    slider->setValue(int(opacity*10));
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(setBackgroundOpacity(int)));

    //Draw all option button
    textB = new QPushButton(QIcon(":/buttons/Icons/text.png"),"", this);
    textB->setIconSize(QSize(32,32));
    textB->setGeometry(rect.right()+xOffset,startY,32,32);
    textB->setMouseTracking(true);

    fontComboBox = new QFontComboBox(this);
    fontComboBox->hide();
    fontComboBox->setStyleSheet(
            "QComboBox {"
                "border: 1px solid black;"
                "border-radius: 3px;"
             "}"

             "QComboBox::drop-down {"
                "width: 32px;"
                "border-top-right-radius: 3px;"
                "border-bottom-right-radius: 3px;"
             "}"

             "QComboBox::down-arrow {"
                "image: url(:/buttons/Icons/down_arrow.png);"
                "width: 32;"
                "height: 32;"
             "}"
    );
    fontComboBox->setGeometry(rect.right()+2*xOffset,startY,180,32);

    textColorB = new QPushButton(QIcon(":/buttons/Icons/color.png"),"", this);
    textColorB->hide();
    textColorB->setIconSize(QSize(32,32));
    textColorB->setGeometry(rect.right()+2*xOffset+183,startY,32,32);

    textBDimension = new QPushButton(QIcon(":/buttons/Icons/text_dimension.png"),"", this);
    textBDimension->setIconSize(QSize(32,32));
    textBDimension->setGeometry(rect.right()+xOffset,startY+yOffset,32,32);
    textBDimension->setMouseTracking(true);

    textLessB = new QPushButton(QIcon(":/buttons/Icons/text_less.png"),"", this);
    textLessB->hide();
    textLessB->setIconSize(QSize(32,32));
    textLessB->setGeometry(rect.right()+2*xOffset,startY+yOffset,32,32);

    textDefaultB = new QPushButton(QIcon(":/buttons/Icons/text_default.png"),"", this);
    textDefaultB->hide();
    textDefaultB->setIconSize(QSize(32,32));
    textDefaultB->setGeometry(rect.right()+3*xOffset,startY+yOffset,32,32);

    textMoreB = new QPushButton(QIcon(":/buttons/Icons/text_more.png"),"", this);
    textMoreB->hide();
    textMoreB->setIconSize(QSize(32,32));
    textMoreB->setGeometry(rect.right()+4*xOffset,startY+yOffset,32,32);

    backgroundB = new QPushButton(QIcon(":/buttons/Icons/background.png"),"", this);
    backgroundB->setIconSize(QSize(32,32));
    backgroundB->setGeometry(rect.right()+xOffset,startY+2*yOffset,32,32);
    backgroundB->setMouseTracking(true);

    backgroundPreviousB = new QPushButton(QIcon(":/buttons/Icons/sx.png"),"", this);
    backgroundPreviousB->hide();
    backgroundPreviousB->setIconSize(QSize(32,32));
    backgroundPreviousB->setGeometry(rect.right()+2*xOffset,startY+2*yOffset,32,32);

    backgroundNextB = new QPushButton(QIcon(":/buttons/Icons/dx.png"),"", this);
    backgroundNextB->hide();
    backgroundNextB->setIconSize(QSize(32,32));
    backgroundNextB->setGeometry(rect.right()+3*xOffset,startY+2*yOffset,32,32);

    backgroundColorB = new QPushButton(QIcon(":/buttons/Icons/color.png"),"", this);
    backgroundColorB->hide();
    backgroundColorB->setIconSize(QSize(32,32));
    backgroundColorB->setGeometry(rect.right()+4*xOffset,startY+2*yOffset,32,32);

    musicB = new QPushButton(QIcon(":/buttons/Icons/music.png"),"", this);
    musicB->setIconSize(QSize(32,32));
    musicB->setGeometry(rect.right()+xOffset,startY+3*yOffset,32,32);
    musicB->setMouseTracking(true);

    musicPreviousB = new QPushButton(QIcon(":/buttons/Icons/sx.png"),"", this);
    musicPreviousB->hide();
    musicPreviousB->setIconSize(QSize(32,32));
    musicPreviousB->setGeometry(rect.right()+2*xOffset,startY+3*yOffset,32,32);

    musicNextB = new QPushButton(QIcon(":/buttons/Icons/dx.png"),"", this);
    musicNextB->hide();
    musicNextB->setIconSize(QSize(32,32));
    musicNextB->setGeometry(rect.right()+3*xOffset,startY+3*yOffset,32,32);

    if(musicMuted)
        musicMuteB = new QPushButton(QIcon(":/buttons/Icons/mute_red.png"),"", this);
    else
        musicMuteB = new QPushButton(QIcon(":/buttons/Icons/mute.png"),"", this);
    musicMuteB->hide();
    musicMuteB->setIconSize(QSize(32,32));
    musicMuteB->setGeometry(rect.right()+4*xOffset,startY+3*yOffset,32,32);

    keyboardB = new QPushButton(QIcon(":/buttons/Icons/keyboard.png"),"", this);
    keyboardB->setIconSize(QSize(32,32));
    keyboardB->setGeometry(rect.right()+xOffset,startY+4*yOffset,32,32);
    keyboardB->setMouseTracking(true);

    keyboardPreviousB = new QPushButton(QIcon(":/buttons/Icons/sx.png"),"", this);
    keyboardPreviousB->hide();
    keyboardPreviousB->setIconSize(QSize(32,32));
    keyboardPreviousB->setGeometry(rect.right()+2*xOffset,startY+4*yOffset,32,32);

    keyboardNextB = new QPushButton(QIcon(":/buttons/Icons/dx.png"),"", this);
    keyboardNextB->hide();
    keyboardNextB->setIconSize(QSize(32,32));
    keyboardNextB->setGeometry(rect.right()+3*xOffset,startY+4*yOffset,32,32);

    if(clickMuted)
        keyboardMuteB = new QPushButton(QIcon(":/buttons/Icons/mute_red.png"),"", this);
    else
        keyboardMuteB = new QPushButton(QIcon(":/buttons/Icons/mute.png"),"", this);
    keyboardMuteB->hide();
    keyboardMuteB->setIconSize(QSize(32,32));
    keyboardMuteB->setGeometry(rect.right()+4*xOffset,startY+4*yOffset,32,32);

    fileB = new QPushButton(QIcon(":/buttons/Icons/file.png"),"", this);
    fileB->setIconSize(QSize(32,32));
    fileB->setGeometry(rect.right()+xOffset,startY+5*yOffset,32,32);
    fileB->setMouseTracking(true);

    loadB = new QPushButton(QIcon(":/buttons/Icons/load.png"),"", this);
    loadB->hide();
    loadB->setIconSize(QSize(32,32));
    loadB->setGeometry(rect.right()+2*xOffset,startY+5*yOffset,32,32);
    loadB->setMouseTracking(true);

    saveB = new QPushButton(QIcon(":/buttons/Icons/save.png"),"", this);
    saveB->hide();
    saveB->setIconSize(QSize(32,32));
    saveB->setGeometry(rect.right()+3*xOffset,startY+5*yOffset,32,32);

    saveAsB = new QPushButton(QIcon(":/buttons/Icons/save_as.png"),"", this);
    saveAsB->hide();
    saveAsB->setIconSize(QSize(32,32));
    saveAsB->setGeometry(rect.right()+4*xOffset,startY+5*yOffset,32,32);

    colorDialog = new QColorDialog();
    defaultTextDimension = 14;
    currentTextDimension = fontSize;
    currentFont = fontFamily;

    currentTextColor = fontColor;
    this->updateTextStyle();

    //Hidden interface
    hide = false;

    //Signals
    connect(text->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    connect(fontComboBox, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(changeTextFont(const QFont&)));
    connect(textColorB,SIGNAL(clicked()),this,SLOT(showColorDialog(void)));
    connect(textLessB,SIGNAL(clicked()),this,SLOT(lessText(void)));
    connect(textDefaultB,SIGNAL(clicked()),this,SLOT(defaultText(void)));
    connect(textMoreB,SIGNAL(clicked()),this,SLOT(moreText(void)));
    connect(loadB,SIGNAL(clicked()),this,SLOT(open(void)));
    connect(saveB,SIGNAL(clicked()),this,SLOT(save(void)));
    connect(saveAsB,SIGNAL(clicked()),this,SLOT(saveAs(void)));
}

Editor::~Editor()
{
    delete text;
    delete textB;
    delete fontComboBox;
    delete textColorB;
    delete textBDimension;
    delete textLessB;
    delete textDefaultB;
    delete textMoreB;
    delete backgroundB;
    delete backgroundPreviousB;
    delete backgroundNextB;
    delete backgroundColorB;
    delete musicB;
    delete musicPreviousB;
    delete musicNextB;
    delete musicMuteB;
    delete keyboardB;
    delete keyboardPreviousB;
    delete keyboardNextB;
    delete keyboardMuteB;
    delete fileB;
    delete loadB;
    delete saveB;
    delete saveAsB;
    delete colorDialog;
    delete timer;
    delete slider;
    delete clickObject;
    delete clickOutput;
    delete musicObject;
    delete musicOutput;
}

void Editor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(hide == false)
    {
        painter.setPen(Qt::DashLine);
        painter.drawRect(rect);
        painter.setPen(Qt::SolidLine);//Reset default line-style so only rect is affected
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setOpacity(opacity);
        painter.fillRect(rect,QBrush(Qt::white,Qt::SolidPattern));
        painter.setOpacity(1); //Reset default opacity so only background is affected by opacity change
        painter.drawRect(handleTL);
        painter.drawRect(handleT);
        painter.drawRect(handleTR);
        painter.drawRect(handleL);
        painter.drawRect(handleR);
        painter.drawRect(handleBL);
        painter.drawRect(handleB);
        painter.drawRect(handleBR);
        hidden(hide);
    }else if(hide == true)
    {
        painter.setOpacity(opacity);
        painter.fillRect(rect,QBrush(Qt::white,Qt::SolidPattern));
        painter.setOpacity(1); //Reset default opacity so only background is affected by opacity change
        hidden(hide);
    }
}

void Editor::mousePressEvent(QMouseEvent *event)
{

    //Resize
    if(handleTL.contains(event->pos()))
        resizingTL = true;
    if(handleTR.contains(event->pos()))
        resizingTR = true;
    if(handleBL.contains(event->pos()))
        resizingBL = true;
    if(handleBR.contains(event->pos()))
        resizingBR = true;
    if(handleT.contains(event->pos()))
        resizingT = true;
    if(handleB.contains(event->pos()))
        resizingB = true;
    if(handleL.contains(event->pos()))
        resizingL = true;
    if(handleR.contains(event->pos()))
        resizingR = true;
    //Move
    if(rect.contains(event->pos()) && !rect.contains(event->pos(),true)) //If mouse is on the frame
    {
        origin = event->pos();
        move = true;
    }
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    resizingTL = false;
    resizingTR = false;
    resizingBL = false;
    resizingBR = false;
    resizingT = false;
    resizingB = false;
    resizingL = false;
    resizingR = false;
    move = false;
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{

    hide = false;

    //Resizing Top Left
    if(resizingTL == true)
    {
        rect.setTopLeft(event->pos());
        handleTL.moveCenter(event->pos());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Top Right
    if(resizingTR == true)
    {
        rect.setTopRight(event->pos());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(event->pos());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Bottom Left
    if(resizingBL == true)
    {
        rect.setBottomLeft(event->pos());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(event->pos());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Bottom Right
    if(resizingBR == true)
    {
        rect.setBottomRight(event->pos());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(event->pos());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Top
    if(resizingT == true)
    {
        rect.setTop(event->pos().y());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), event->pos().y()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Bottom
    if(resizingB == true)
    {
        rect.setBottom(event->pos().y());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2), event->pos().y()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Left
    if(resizingL == true)
    {
        rect.setLeft(event->pos().x());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(event->pos().x(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Resizing Right
    if(resizingR == true)
    {
        rect.setRight(event->pos().x());
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleR.moveCenter(QPoint(event->pos().x(), rect.bottomRight().y()-(rect.height()/2)));
    }

    //Move
    if(move == true)
    {
        rect.translate(event->pos()-origin);
        handleTL.moveCenter(rect.topLeft());
        handleTR.moveCenter(rect.topRight());
        handleBL.moveCenter(rect.bottomLeft());
        handleBR.moveCenter(rect.bottomRight());
        handleT.moveCenter(QPoint(rect.topRight().x()-(rect.width()/2), rect.top()));
        handleB.moveCenter(QPoint(rect.bottomRight().x()-(rect.width()/2),rect.bottom()));
        handleL.moveCenter(QPoint(rect.left(), rect.bottomRight().y()-(rect.height()/2)));
        handleR.moveCenter(QPoint(rect.right(), rect.bottomRight().y()-(rect.height()/2)));
        origin=event->pos();

    }

    //Draw cursor
    if(handleBR.contains(event->pos()) || handleTL.contains(event->pos())) //Bottom Rigth or Top Left
    {
        cursor.setShape(Qt::SizeFDiagCursor);
    }else if(handleTR.contains(event->pos()) || handleBL.contains(event->pos())) //Top Rigth or Bottom Left
    {
        cursor.setShape(Qt::SizeBDiagCursor);
    }else if(handleT.contains(event->pos()) || handleB.contains(event->pos())) //Top or Bottom
    {
        cursor.setShape(Qt::SizeVerCursor);
    }else if(handleL.contains(event->pos()) || handleR.contains(event->pos())) //Rigth or Left
    {
        cursor.setShape(Qt::SizeHorCursor);
    }else if(rect.contains(event->pos()) && !rect.contains(event->pos(),true)) //Move
    {
        cursor.setShape(Qt::SizeAllCursor);
    }else{
        cursor.setShape(Qt::ArrowCursor); //Default
    }
    setCursor(cursor);

    text->setGeometry(rect.x()+1,rect.y()+1,rect.width()-1,rect.height()-1); //Update textEdit following border

    //Update position of options buttons
    textB->move(rect.right()+xOffset,rect.top());
    fontComboBox->move(rect.right()+2*xOffset,rect.top());
    textColorB->move(rect.right()+2*xOffset+183,rect.top());


    textBDimension->move(rect.right()+xOffset,textB->y()+yOffset);
    textLessB->move(rect.right()+2*xOffset,textB->y()+yOffset);
    textDefaultB->move(rect.right()+3*xOffset,textB->y()+yOffset);
    textMoreB->move(rect.right()+4*xOffset,textB->y()+yOffset);


    backgroundB->move(rect.right()+xOffset,textBDimension->y()+yOffset);
    backgroundPreviousB->move(rect.right()+2*xOffset,textBDimension->y()+yOffset);
    backgroundNextB->move(rect.right()+3*xOffset,textBDimension->y()+yOffset);
    backgroundColorB->move(rect.right()+4*xOffset,textBDimension->y()+yOffset);


    musicB->move(rect.right()+xOffset,backgroundB->y()+yOffset);
    musicPreviousB->move(rect.right()+2*xOffset,backgroundB->y()+yOffset);
    musicNextB->move(rect.right()+3*xOffset,backgroundB->y()+yOffset);
    musicMuteB->move(rect.right()+4*xOffset,backgroundB->y()+yOffset);


    keyboardB->move(rect.right()+xOffset,musicB->y()+yOffset);
    keyboardPreviousB->move(rect.right()+2*xOffset,musicB->y()+yOffset);
    keyboardNextB->move(rect.right()+3*xOffset,musicB->y()+yOffset);
    keyboardMuteB->move(rect.right()+4*xOffset,musicB->y()+yOffset);


    fileB->move(rect.right()+xOffset,keyboardB->y()+yOffset);
    loadB->move(rect.right()+2*xOffset,keyboardB->y()+yOffset);
    saveB->move(rect.right()+3*xOffset,keyboardB->y()+yOffset);
    saveAsB->move(rect.right()+4*xOffset,keyboardB->y()+yOffset);


    //show advanced buttons
    if(textB->underMouse())
    {
        fontComboBox->show();
        textColorB->show();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
    }
    if(textBDimension->underMouse())
    {
        fontComboBox->hide();
        textColorB->hide();
        textLessB->show();
        textDefaultB->show();
        textMoreB->show();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
    }
    if(backgroundB->underMouse())
    {
        fontComboBox->hide();
        textColorB->hide();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->show();
        backgroundNextB->show();
        backgroundColorB->show();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
    }
    if(musicB->underMouse())
    {
        fontComboBox->hide();
        textColorB->hide();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->show();
        musicNextB->show();
        musicMuteB->show();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
    }
    if(keyboardB->underMouse())
    {
        fontComboBox->hide();
        textColorB->hide();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->show();
        keyboardNextB->show();
        keyboardMuteB->show();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
    }
    if(fileB->underMouse())
    {
        fontComboBox->hide();
        textColorB->hide();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->show();
        saveB->show();
        saveAsB->show();
    }

    //Opacity slider
    slider->move(rect.center().x()-60, rect.bottom()+10);
    //redraw interface
    update();
}

void Editor::setBackgroundOpacity(int o)
{
    opacity = (qreal)o/10;
    update();
}

void Editor::hidden(bool hide)
{
    if(hide == true)
    {
        textB->hide();
        textBDimension->hide();
        backgroundB->hide();
        musicB->hide();
        keyboardB->hide();
        fileB->hide();
        fontComboBox->hide();
        textColorB->hide();
        textLessB->hide();
        textDefaultB->hide();
        textMoreB->hide();
        backgroundPreviousB->hide();
        backgroundNextB->hide();
        backgroundColorB->hide();
        musicPreviousB->hide();
        musicNextB->hide();
        musicMuteB->hide();
        keyboardPreviousB->hide();
        keyboardNextB->hide();
        keyboardMuteB->hide();
        loadB->hide();
        saveB->hide();
        saveAsB->hide();
        slider->hide();
        setCursor(QCursor(Qt::BlankCursor));

    }else if(hide == false)
    {
        textB->show();
        textBDimension->show();
        backgroundB->show();
        musicB->show();
        keyboardB->show();
        fileB->show();
        slider->show();
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void Editor::documentWasModified()
{
    if(!clickMuted)
    {
        clickObject->stop();
        clickObject->play();
    }
    hide = true;
    saved = false;
    update(); //redraw interface
}

void Editor::changeTextFont(const QFont& font)
{


    currentFont = font;
    this->updateTextStyle();
}

void Editor::showColorDialog(void)
{
    currentTextColor = colorDialog->getColor();
    this->updateTextStyle();
}

void Editor::lessText(void)
{
    currentTextDimension--;
    this->updateTextStyle();
}

void Editor::moreText(void)
{
    currentTextDimension =  currentTextDimension+2;
    this->updateTextStyle();
}

void Editor::defaultText(void)
{
    currentTextDimension = defaultTextDimension;
    this->updateTextStyle();
}

void Editor::updateTextStyle(void)
{
    text->setStyleSheet("QTextEdit {"
                        "font-family: \""+ currentFont.toString() +"\";"
                        "font-size: "+ QString::number(currentTextDimension) +"px;"
                        "color:"+ currentTextColor.name() +";"
                    "}"
/********************************************************************
                    "QScrollBar:vertical {"
                          "border: 0px;"
                          "background: transparent;"
                          "width: 15px;"
                          "margin: 22px 0 22px 0;"
                      "}"
                      "QScrollBar::handle:vertical {"
                          "border: 1px solid" + currentTextColor.name() + ";"
                          "background: transparent;"
                          "min-height: 20px;"
                      "}"
                      "QScrollBar::add-line:vertical {"
                          "border: 0px;"
                          "background: transparent;"
                          "height: 20px;"
                          "subcontrol-position: bottom;"
                          "subcontrol-origin: margin;"
                      "}"

                      "QScrollBar::sub-line:vertical {"
                          "border: 0px;"
                          "background: transparent;"
                          "height: 20px;"
                          "subcontrol-position: top;"
                          "subcontrol-origin: margin;"
                      "}"

                      "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
                          "border: 2px solid" + currentTextColor.name() + ";"
                          "width: 3px;"
                          "height: 3px;"
                          "background:" + currentTextColor.name() + ";"
                      "}"

                      "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                          "background: none;"
                      "}"

                      "QScrollBar:horizontal {"
                          "border: 0px;"
                          "background: transparent;"
                          "height: 15px;"
                          "margin: 0px 22px 0 22px;"
                      "}"

                      "QScrollBar::handle:horizontal {"
                          "border: 1px solid" + currentTextColor.name() + ";"
                          "background: transparent;"
                          "min-width: 20px;"
                      "}"

                      "QScrollBar::add-line:horizontal {"
                          "border: 0px;"
                          "background: transparent;"
                          "width: 20px;"
                          "subcontrol-position: right;"
                          "subcontrol-origin: margin;"
                      "}"

                      "QScrollBar::sub-line:horizontal {"
                          "border: 0px;"
                          "background: transparent;"
                          "width: 20px;"
                          "subcontrol-position: left;"
                          "subcontrol-origin: margin;"
                      "}"

                      "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
                          "border: 2px solid" + currentTextColor.name() + ";"
                          "width: 3px;"
                          "height: 3px;"
                          "background:" + currentTextColor.name() + ";"
                      "}"

                      "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
                           "background: none;"
                      "}"
*******************************************************************/
    );
}

void Editor::open(void){

    if(!saved)
    {
        //<thanks trolltech> :)
        QMessageBox msgBox;
        msgBox.setText(tr("The document has been modified."));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch(ret){
           case QMessageBox::Save:
               this->save();
               break;
           case QMessageBox::Cancel:
               break;
           default:
               break;
        }
        //</thanks>
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), QDir::homePath(), tr("Text Files (*.txt)"));

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream textStream(&file);
    text->setPlainText(textStream.readAll());
    currentFile = fileName;
    text->document()->setModified(false);
}

void Editor::save(void)
{
    QString fileName;
    if(currentFile.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this,
            tr("Save File"), QDir::homePath() + "/document.txt", tr("Text Files (*.txt)"));
        currentFile = fileName;
    }

    QFile file(currentFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        currentFile = "";
        return;
    }


    QTextStream textStream(&file);
    textStream << text->toPlainText();
    saved = true;
    text->document()->setModified(false);
}

void Editor::saveAs(void)
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"), QDir::homePath() + "/document.txt", tr("Text Files (*.txt)"));

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        currentFile = "";
        return;
    }


    QTextStream textStream(&file);
    textStream << text->toPlainText();
    currentFile = fileName;
    saved = true;
    text->document()->setModified(false);
}

void Editor::autoSave(void)
{
    if(!saved && !currentFile.isEmpty())
        this->save();
}

void Editor::playMusic(void)
{
    if(!musicMuted)
    {
        musicObject->stop();
        musicObject->play();
    }
}

void Editor::replayMusic(void)
{
    musicObject->setCurrentSource(currentMusic);
    musicObject->play();
}
