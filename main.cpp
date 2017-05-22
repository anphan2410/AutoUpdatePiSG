#include <QCoreApplication>
#include <QTime>
#include <QProcess>
#include <QFile>
#include <QDir>
#include "andebug.h"

#ifdef Q_OS_WIN
#include <windows.h>
#define _TildeDirectory "D:/home/pi"
#else
#include <time.h>
#define _TildeDirectory "/home/pi"
#endif

#define _DefaultPollingRate 1
#define _DefaultCheckPoint QTime::fromString("14:30:00","hh:mm:ss")

void GoSleep(int ms)
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
    QProcess * proc = new QProcess();
    quint8 count0 = 0;
    quint8 PollingRate = 16;//_DefaultPollingRate;
    QTime CheckPoint = QTime::fromString("16:36:00","hh:mm:ss");//_DefaultCheckPoint;
    while (true)
    {
        //Reinitializing Variable
        count0 = 0;
        //<Start Timing Here If Needed>
        QFile::remove(_TildeDirectory "/AutoUpdatePiSG/AutoUpdatePiSG.conf");
        do
        {
            if (++count0>7)
            {
                break;
            }
#ifdef Q_OS_WIN
            anDebug("Please MANUALLY Download AutoUpdatePiSG.conf And Place It Into " _TildeDirectory "/AutoUpdatePiSG/ !!!");
#else
            proc->execute("wget -P " _TildeDirectory "\"http://www.orimi.com/pdf-test.pdf\"");
#endif
        } while (!QFile::exists(_TildeDirectory "/AutoUpdatePiSG/AutoUpdatePiSG.conf"));
        if (count0<=7)
        {
            count0 = 0;

        }
        //<Stop Timing Here If Needed>
        int BreakInterval = 86400000/PollingRate;
        int TimePoint = CheckPoint.msecsSinceStartOfDay() - BreakInterval;
        int SleepTime;
        do
        {
            TimePoint += BreakInterval;
            SleepTime = TimePoint -QTime::currentTime().msecsSinceStartOfDay();
        } while (SleepTime < 0);
        anDebug("Sleep Until" << QTime::fromMSecsSinceStartOfDay(TimePoint).toString("hh:mm:ss"));
        anqDebug(_VarView(SleepTime) + " milisecond");
        GoSleep(SleepTime);
    }
    return a.exec();
}
