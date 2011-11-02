/**KoalaWriter*******************************************************************************

  Version   [1.0]
  FileName  [mainwindow.cpp]
  Author    [Mazzarelli Alessio - mazzarelli.alessio@gmail.com]
  License   [GPLv3]

********************************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow()
{
    this->lookForBackGround();
    this->lookForClick();
    this->lookForMusic();
    this->readSettings();
    editor = new Editor(startX, startY, width, height, fontColor, fontSize, fontFamily, opacity, clickMute, musicMute);
    this->searchBackground();
    this->searchClick();
    this->searchMusic();
    editor->setAutoFillBackground(true);
    this->setCentralWidget(editor);
    connect(editor->backgroundNextB,SIGNAL(clicked()),this,SLOT(nextBackground(void)));
    connect(editor->backgroundPreviousB,SIGNAL(clicked()),this,SLOT(previousBackground(void)));
    connect(editor->backgroundColorB,SIGNAL(clicked()),this,SLOT(colorBackground(void)));
    colorDialog = new QColorDialog();
    connect(editor->keyboardNextB,SIGNAL(clicked()),this,SLOT(nextClick(void)));
    connect(editor->keyboardPreviousB,SIGNAL(clicked()),this,SLOT(previousClick(void)));
    connect(editor->keyboardMuteB,SIGNAL(clicked()),this,SLOT(muteClick(void)));
    connect(editor->musicNextB,SIGNAL(clicked()),this,SLOT(nextMusic(void)));
    connect(editor->musicPreviousB,SIGNAL(clicked()),this,SLOT(previousMusic(void)));
    connect(editor->musicMuteB,SIGNAL(clicked()),this,SLOT(muteMusic(void)));
    this->showFullScreen();
    this->activateWindow();
    this->raise();
}

MainWindow::~MainWindow()
{
    delete editor;
    delete backgroundList;
    delete clickList;
    delete musicList;
    delete colorDialog;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()){
    case Qt::Key_Escape:
        if(!editor->saved)
        {
            //<thanks trolltech> :)
            QMessageBox msgBox;
            msgBox.setText(tr("The document has been modified."));
            msgBox.setInformativeText(tr("Do you want to save your changes?"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch(ret){
               case QMessageBox::Save:
                   editor->save();
                   //break;
               case QMessageBox::Discard:
                    this->close();
                   //qApp->quit();
                   //break;
               case QMessageBox::Cancel:
                   break;
               default:
                   break;
            }
            //</thanks>
        }else{
            this->close();
            //qApp->quit();
        }
        this->writeSettings();
        break;
    case Qt::Key_F:
        if (event->modifiers() & Qt::ControlModifier){
            this->showFullScreen();
            break;
        }
    case Qt::Key_M:
        if (event->modifiers() & Qt::ControlModifier){
            this->showMaximized();//To have a large window once come back from minimization
            this->showMinimized();
            break;
        }
    case Qt::Key_S:
        if (event->modifiers() & Qt::ControlModifier){
            editor->save();
            break;
        }
    default:
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::lookForBackGround(void)
{
    QString backgroundDirName = QApplication::applicationDirPath();
    backgroundDirName += QString("/../Resources/Background");
    backgroundDir = QDir(backgroundDirName);
    //I need only some file types
    backgroundDir.setFilter(QDir::Files);
    QStringList filter;
    filter << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm"
                    << "*.tiff" << "*.xbm" << "*.xpm";
    backgroundDir.setNameFilters(filter);
    //Create a list of all file
    backgroundList = new QStringList(backgroundDir.entryList());
}

void MainWindow::nextBackground(void)
{
    if(currentBackground != backgroundList->length()-1)
    {
        setBackground(++currentBackground);
        background = backgroundList->at(currentBackground);
    }
    else if(currentBackground == backgroundList->length()-1)
    {
        currentBackground = 0;
        setBackground(currentBackground);
        background = backgroundList->at(currentBackground);
    }
}

void MainWindow::previousBackground(void)
{
    if(currentBackground != 0)
    {
        setBackground(--currentBackground);
        background = backgroundList->at(currentBackground);
    }
    else if(currentBackground == 0)
    {
        currentBackground = backgroundList->length()-1;
        setBackground(currentBackground);
        background = backgroundList->at(currentBackground);
    }
}

void MainWindow::colorBackground(void)
{
    QPalette palette;
    backgroundColor = QColor(colorDialog->getColor());
    palette.setBrush(QPalette::Window, QBrush(backgroundColor, Qt::SolidPattern));
    editor->setPalette(palette);
    background = "NONE";
}

void MainWindow::setBackground(int pos)
{
    QPalette palette;
    QImage background = QImage(backgroundDir.filePath(backgroundList->at(pos)));
    //Resize background image if necessary
    QRect resolution = QApplication::desktop()->screenGeometry();
    if(resolution.width() != background.width() || resolution.height() != background.height())
    {
        background = background.scaled(QSize(resolution.width(), resolution.height()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    palette.setBrush(QPalette::Window, QBrush(background));
    editor->setPalette(palette);
}

void MainWindow::lookForClick(void)
{
    QString clickDirName = QApplication::applicationDirPath();
    clickDirName += QString("/../Resources/Click");
    clickDir = QDir(clickDirName);
    clickDir.setFilter(QDir::Files);
    QStringList filter;
    filter << "*.wav" << "*.mp3";
    clickDir.setNameFilters(filter);
    clickList = new QStringList(clickDir.entryList());
}

void MainWindow::setClick(int pos)
{
    editor->currentClick = clickDir.filePath(clickList->at(pos));
    editor->clickObject->setCurrentSource(editor->currentClick);
    currentClick = pos;
    editor->clickObject->stop();
    editor->clickObject->play();
}

void MainWindow::nextClick(void)
{
    if(currentClick != clickList->length()-1)
    {
        setClick(++currentClick);
    }
    else if(currentClick == clickList->length()-1)
    {
        currentClick = 0;
        setClick(currentClick);
    }
}

void MainWindow::previousClick(void)
{
    if(currentClick != 0)
    {
        setClick(--currentClick);
    }
    else if(currentClick == 0)
    {
        currentClick = clickList->length()-1;
        setClick(currentClick);
    }
}

void MainWindow::muteClick(void)
{
    if(editor->clickMuted)
    {
        editor->clickMuted = false;
        editor->keyboardMuteB->setIcon(QIcon(":/buttons/Icons/mute.png"));

    }else if(!editor->clickMuted)
    {
        editor->clickMuted = true;
        editor->keyboardMuteB->setIcon(QIcon(":/buttons/Icons/mute_red.png"));
    }

}


void MainWindow::lookForMusic(void)
{
    QString musicDirName = QApplication::applicationDirPath();
    musicDirName += QString("/../Resources/Music");
    musicDir = QDir(musicDirName);
    musicDir.setFilter(QDir::Files);
    QStringList filter;
    filter << "*.wav" << "*.mp3";
    musicDir.setNameFilters(filter);
    musicList = new QStringList(musicDir.entryList());
}

void MainWindow::setMusic(int pos)
{
    editor->currentMusic = musicDir.filePath(musicList->at(pos));
    editor->musicObject->setCurrentSource(editor->currentMusic);
    currentMusic = pos;
    editor->playMusic();
}

void MainWindow::nextMusic(void)
{
    if(currentMusic != musicList->length()-1)
    {
        setMusic(++currentMusic);
    }
    else if(currentMusic == musicList->length()-1)
    {
        currentMusic = 0;
        setMusic(currentMusic);
    }
}

void MainWindow::previousMusic(void)
{
    if(currentMusic != 0)
    {
        setMusic(--currentMusic);
    }
    else if(currentMusic == 0)
    {
        currentMusic = musicList->length()-1;
        setMusic(currentMusic);
    }
}

void MainWindow::muteMusic(void)
{
    if(editor->musicMuted)
    {
        editor->musicMuted = false;
        editor->musicMuteB->setIcon(QIcon(":/buttons/Icons/mute.png"));
        editor->playMusic();

    }else if(!editor->musicMuted)
    {
        editor->musicObject->stop();
        editor->musicMuted = true;
        editor->musicMuteB->setIcon(QIcon(":/buttons/Icons/mute_red.png"));
    }

}

void MainWindow::searchBackground(void)
{
    if(background != "NONE")
    {
        int b = backgroundList->indexOf(background);
        if(b != -1)
        {
            this->setBackground(b);
            currentBackground = b;
        }else{
            this->setBackground(0);
            currentBackground = 0;
        }
    }else if(background == "NONE")
    {
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(backgroundColor, Qt::SolidPattern));
        editor->setPalette(palette);
        currentBackground = 0;
    }
}

void MainWindow::searchClick(void)
{
    int c = clickList->indexOf(click);
    if(c != -1)
    {
        editor->currentClick = clickDir.filePath(clickList->at(c));
        editor->clickObject->setCurrentSource(editor->currentClick);
        currentClick = c;
    }else{
        editor->currentClick = clickDir.filePath(clickList->at(0));
        editor->clickObject->setCurrentSource(editor->currentClick);
        currentClick = 0;
    }
}

void MainWindow::searchMusic(void)
{
    int m = musicList->indexOf(music);
    if(m != -1)
    {
        editor->currentMusic = musicDir.filePath(musicList->at(m));
        editor->musicObject->setCurrentSource(editor->currentMusic);
        currentMusic = m;

    }else{
        editor->currentMusic = musicDir.filePath(musicList->at(0));
        editor->musicObject->setCurrentSource(editor->currentMusic);
        currentMusic = 0;

    }

    editor->playMusic();
}

void MainWindow::writeSettings(void)
{
    QSettings settings("KoalaWriter", "KoalaWriter");

    settings.beginGroup("Options");
    settings.setValue("startX", editor->rect.topLeft().x());
    settings.setValue("startY", editor->rect.topLeft().y());
    settings.setValue("width", editor->rect.width());
    settings.setValue("height", editor->rect.height());
    settings.setValue("fontColor", editor->currentTextColor);
    settings.setValue("fontSize", editor->currentTextDimension);
    settings.setValue("fontFamily", editor->currentFont.toString());
    settings.setValue("opacity", editor->opacity);
    if(background == "NONE")
        settings.setValue("background", "NONE");
    else if(background != "NONE")
        settings.setValue("background", backgroundList->at(currentBackground));
    settings.setValue("backgroundColor", backgroundColor);
    settings.setValue("click", clickList->at(currentClick));
    settings.setValue("clickMute", editor->clickMuted);
    settings.setValue("music", musicList->at(currentMusic));
    settings.setValue("musicMute", editor->musicMuted);
    settings.endGroup();

}

void MainWindow::readSettings(void)
{
    QSettings settings("KoalaWriter", "KoalaWriter");

    settings.beginGroup("Options");
    QRect resolution = QApplication::desktop()->screenGeometry();
    startX = settings.value("startX", 20).toInt();
    startY = settings.value("startY", 20).toInt();
    width = settings.value("width", resolution.width()/2).toInt();
    height = settings.value("height", resolution.height()-100).toInt();
    fontColor = settings.value("fontColor", Qt::black).value<QColor>();
    fontSize = settings.value("fontSize", 14).toInt();
    fontFamily = settings.value("fontFamily").toString();
    opacity = settings.value("opacity",0.0).toFloat();
    background = settings.value("background", backgroundList->at(0)).toString();
    backgroundColor = settings.value("backgroundColor", Qt::transparent).value<QColor>();
    click = settings.value("click", clickList->at(0)).toString();
    clickMute = settings.value("clickMute", false).toBool();
    music = settings.value("music", musicList->at(0)).toString();
    musicMute = settings.value("musicMute", false).toBool();
    settings.endGroup();
}
