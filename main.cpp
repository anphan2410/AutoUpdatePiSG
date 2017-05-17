#include <QCoreApplication>
#include <QTime>
#include <QProcess>
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#define TildeDirectory D:/home/pi
#else
#include <time.h>
#define TildeDirectory /home/pi
#endif

void GoSleep(quint64 ms)
{
#ifdef Q_OS_WIN
    Sleep(ms);
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTime qtiming;
    QProcess * proc = new QProcess();
    quint8 count0 = 0;
    qDebug() << QDir().mkpath("D:/abc/ngoc/okay");
    while (true)
    {
        qtiming.start();

        while ((++count0)<=7)
        {

        }
        if (count0 )
        //--Take A Break--
    }
    return a.exec();
}
