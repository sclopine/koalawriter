/**KoalaWriter*******************************************************************************

  Version   [1.0]
  FileName  [main.cpp]
  Author    [Mazzarelli Alessio - mazzarelli.alessio@gmail.com]
  License   [GPLv3]

********************************************************************************************/

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("KoalaWriter");
    MainWindow w;

    w.show();
    return a.exec();
}
