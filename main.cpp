#include <QCoreApplication>
#include <QTime>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "anqtdebug.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <iostream>
#define _TildeDirectory "D:/home/pi"
#else
#include <time.h>
#define _TildeDirectory "/home/pi"
#endif

#define _DefaultAutoUpdatePiSGFolderPath _TildeDirectory "/AutoUpdatePiSG"
#define _DefaultConfigFilePath _DefaultAutoUpdatePiSGFolderPath "/AutoUpdatePiSG.conf"
#define _DefaultConfigFileLink "http://tamduongs.com:82/iot/AutoUpdatePiSG.conf"
#define _DefaultScriptSha256FilePath _DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256"
#define _DefaultScriptFilePath _DefaultAutoUpdatePiSGFolderPath "/AutoUpdatePiSG.sh"
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
    quint8 PollingRate = 16;//_DefaultPollingRate;
    QTime CheckPoint = QTime::fromString("16:36:00","hh:mm:ss");//_DefaultCheckPoint;
    QString ScriptSha256 = "";
    QString ScriptLink = "";
    QString ProgSha256 = "";
    QString ProgLink = "";
    quint8 count0 = 0;
    while (true)
    {        
        //<Start Timing Here If Needed>
        QFile::remove(_DefaultConfigFilePath);
        do
        {
            if (++count0>7)
            {
                break;
            }
#ifdef Q_OS_WIN
            qDebug() << "This Program Is Only For Testing Purpose On Windows";
            qDebug() << "Please MANUALLY Download AutoUpdatePiSG.conf";
            qDebug() << "Then Place It Into The Following Path:";
            qDebug() << _DefaultAutoUpdatePiSGFolderPath;
            system("pause");
#else
            proc->execute("wget -P " _DefaultAutoUpdatePiSGFolderPath "\"" _DefaultConfigFileLink "\"");
#endif
        } while (!QFile::exists(_DefaultConfigFilePath));
        if (count0<=7)
        {
            count0 = 0;
            QFile configFile(_DefaultConfigFilePath);
            if (configFile.open(QIODevice::ReadOnly))
            {
               QTextStream scanconfigFile(&configFile);
               QStringList parsedParamsInOneLine;
               while (!scanconfigFile.atEnd())
               {
                    parsedParamsInOneLine = scanconfigFile.readLine().split('=');
                    if (parsedParamsInOneLine.contains("PollingRate"))
                    {
                        PollingRate = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("PollingRate")+1).trimmed().toInt();
                        if (!PollingRate)
                        {
                            PollingRate = _DefaultPollingRate;
                        }
                        anqDebug(_VarView(PollingRate));
                    }
                    else if (parsedParamsInOneLine.contains("TimePoint"))
                    {
                        CheckPoint = QTime::fromString(parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimePoint")+1).trimmed(),"hh:mm:ss");
                        if (CheckPoint.isNull() || !CheckPoint.isValid())
                        {
                            CheckPoint = _DefaultCheckPoint;
                        }
                        anqDebug(_VarView(CheckPoint.toString("hh:mm:ss")));
                    }
                    else if (parsedParamsInOneLine.contains("ScriptSha256"))
                    {
                        ScriptSha256 = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ScriptSha256")+1).trimmed();
                        anqDebug(_VarView(ScriptSha256));
                    }
                    else if (parsedParamsInOneLine.contains("ScriptLink"))
                    {
                        ScriptLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ScriptLink")+1).trimmed();
                        anqDebug(_VarView(ScriptLink));
                    }
                    else if (parsedParamsInOneLine.contains("ProgSha256"))
                    {
                        ProgSha256 = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ProgSha256")+1).trimmed();
                        anqDebug(_VarView(ProgSha256));
                    }
                    else if (parsedParamsInOneLine.contains("ProgLink"))
                    {
                        ProgLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ProgLink")+1).trimmed();
                        anqDebug(_VarView(ProgLink));
                    }
               }
               configFile.close();
            }

            if (ScriptSha256.size() == 64)
            {
                bool IsNewScriptFileNeeded = false;
                bool IsThereFileScriptSha256 = QFile::exists(_DefaultScriptSha256FilePath);
                QString currentScriptSha256 = "";
                if (IsThereFileScriptSha256)
                {
                    QFile ScriptSha256File(_DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256");
                    if (ScriptSha256File.open(QIODevice::ReadOnly))
                    {
                       QTextStream readFile(&ScriptSha256File);
                       while (!readFile.atEnd())
                       {
                            currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                       }
                       ScriptSha256File.close();
                    }
                    IsNewScriptFileNeeded = (currentScriptSha256 != ScriptSha256);
                }
                if (IsNewScriptFileNeeded || !IsThereFileScriptSha256)
                {
                    quint8 count1 = 0;
                    quint8 count2 = 0;
                    do
                    {
                        if (++count2>3)
                        {
                            break;
                        }
                        QFile::remove(_DefaultScriptSha256FilePath);
                        do
                        {
                            if (++count1>3)
                            {
                                break;
                            }
#ifdef Q_OS_WIN
                            qDebug() << "This Program Is Only For Testing Purpose On Windows";
                            qDebug() << "Please MANUALLY Place File .ScriptSha256 Into The Following Path:";
                            qDebug() << _DefaultAutoUpdatePiSGFolderPath;
                            system("pause");
#else
                            proc->execute("wget -P " _DefaultAutoUpdatePiSGFolderPath "\"" ScriptLink "\"");
#endif
                        } while (!QFile::exists(_DefaultScriptSha256FilePath));
                        if (count1<=3)
                        {
                            count1 = 0;

                        }
                    } while (currentScriptSha256 != ScriptSha256);
                    if (count2<=3)
                    {
                        count2 = 0;
                    }
                }
            }
        }
        //<Stop Timing Here If Needed>
        int BreakInterval = 86400000/PollingRate;
        int TimePoint = CheckPoint.msecsSinceStartOfDay() - BreakInterval;
        int SleepTime = 0;
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
