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
#define _LinuxCommandBash "/bin/bash"
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
#ifndef Q_OS_WIN
    QProcess * proc = new QProcess();
#endif
    quint8 PollingRate = 16;//_DefaultPollingRate;
    QTime CheckPoint = QTime::fromString("16:36:00","hh:mm:ss");//_DefaultCheckPoint;
    QString ScriptSha256 = "";
    QString ScriptLink = "";
    QString ProgSha256 = "";
    QString ProgLink = "";
    quint8 count0 = 0;
    while (true)
    {        
        anqDebug("===================================================================");
        anqDebug("=====================START A NEW UPDATE CYCLE =====================");
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
            anqDebug("=> Try Downloading Config File ...");
            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" _DefaultConfigFileLink "\"");
            proc->waitForFinished(60000);//timeout 1 minute
            if (proc->state() == QProcess::Running)
            {
                anqDebug("=> Try Timed Out !");
            }
            else
                anqDebug("=> Try Completed !");
            proc->close();
#endif
        } while (!QFile::exists(_DefaultConfigFilePath));
        if (count0<=7)
        {
            anqDebug("=> Successful Download Config File !");
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
                anqDebug("=> Valid ScriptSha256 !");
                bool tmpCondition = false;
                bool IsThereFileScriptSha256 = QFile::exists(_DefaultScriptSha256FilePath);
                QString currentScriptSha256 = "";
                if (IsThereFileScriptSha256)
                {
                    anqDebug("=> Found .ScriptSha256 File !");
                    QFile ScriptSha256File(_DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256");
                    if (ScriptSha256File.open(QIODevice::ReadOnly))
                    {
                       QTextStream readFile(&ScriptSha256File);
                       while (!readFile.atEnd())
                       {
                            anqDebug("=> Fetched .ScriptSha256 File ...");
                            currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug(_VarView(currentScriptSha256));
                            anqDebug("new" _VarView(ScriptSha256));
                       }
                       ScriptSha256File.close();
                    }
                    tmpCondition = (currentScriptSha256 != ScriptSha256);
                    anDebugWrap(tmpCondition,
                                anqDebug("=> Not Matched ScriptSha256 between the old script and the new one !");
                                );
                }
                anDebugWrap(!IsThereFileScriptSha256,
                                anqDebug("=> Not Found .ScriptSha256 File !");
                                );
                if (tmpCondition || !IsThereFileScriptSha256)
                {
                    anqDebug("=> Start Downloading New Script File !");
                    quint8 count1 = 0;
                    quint8 count2 = 0;
                    tmpCondition = true;
                    do
                    {
                        if (++count2>3)
                        {
                            anqDebug("=> Not Matched Sha256 Of The New Script !");
                            break;
                        }
                        QFile::remove(_DefaultScriptFilePath);
                        do
                        {
                            if (++count1>3)
                            {
                                break;
                            }
#ifdef Q_OS_WIN
                            qDebug() << "This Program Is Only For Testing Purpose On Windows";
                            qDebug() << "Please MANUALLY Download Script AutoUpdatePiSG.sh";
                            qDebug() << "Then Place Into The Following Path:";
                            qDebug() << _DefaultAutoUpdatePiSGFolderPath;
                            system("pause");
#else
                            anqDebug("=> Try Downloading Script File ...");
                            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" + ScriptLink + "\"");
                            proc->waitForFinished(1800000);//timeout 30 minutes
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                                anqDebug("=> Try Completed !");
                            proc->close();
#endif
                        } while (!QFile::exists(_DefaultScriptFilePath));
                        if (count1<=3)
                        {
                            anqDebug("=> Successful Download Script File !");
                            count1 = 0;
                            do
                            {
                                if (++count1>3)
                                {
                                    break;
                                }
#ifdef Q_OS_WIN
                                qDebug() << "This Program Is Only For Testing Purpose On Windows";
                                qDebug() << "Please MANUALLY Write Sha256Sum Of AutoUpdatePiSG.sh Into .ScriptSha256";
                                qDebug() << "Then Place File .ScriptSha256 Into The Following Path:";
                                qDebug() << _DefaultAutoUpdatePiSGFolderPath;
                                system("pause");
#else
                                anqDebug("=> Try Calculating Sha256 Of The New Script File ...");
                                proc->setStandardOutputFile(_DefaultScriptSha256FilePath);
                                proc->start("sha256sum " _DefaultScriptFilePath);
                                proc->waitForFinished(300000);//timeout 5 minutes
                                proc->setStandardOutputFile(QProcess::nullDevice());
                                if (proc->state() == QProcess::Running)
                                {
                                    anqDebug("=> Try Timed Out !");
                                }
                                else
                                    anqDebug("=> Try Completed !");
                                proc->close();
#endif
                            } while(!QFile::exists(_DefaultScriptSha256FilePath));
                            if (count1<=3)
                            {
                                anqDebug("=> Successful Calculate Sha256 Of The New Script !");
                                count1 = 0;
                                QFile ScriptSha256File(_DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256");
                                if (ScriptSha256File.open(QIODevice::ReadOnly))
                                {
                                   QTextStream readFile(&ScriptSha256File);
                                   while (!readFile.atEnd())
                                   {
                                       anqDebug("=> Fetched .ScriptSha256 File ...");
                                       currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                       anqDebug(_VarView(currentScriptSha256));
                                       anqDebug("new" _VarView(ScriptSha256));
                                   }
                                   ScriptSha256File.close();
                                }
                            }
                            else
                            {
                                anqDebug("=> Failed To Calculate Sha256 Of The New Script !");
                                tmpCondition = false;
                                break;
                            }
                        }
                        else
                        {
                            anqDebug("=> Failed To Download The New Script !");
                            tmpCondition = false;
                            break;
                        }
                    } while (currentScriptSha256 != ScriptSha256);
                    if (tmpCondition && (count2<=3))
                    {
                        anqDebug("=> Matched Sha256 Of The Downloaded Script !");
                        count2 = 0;
#ifdef Q_OS_WIN
                        qDebug() << "This Program Is Only For Testing Purpose On Windows";
                        qDebug() << "Please MANUALLY Create A Result Equivalently As Follows,";
                        qDebug() << "Execute " _DefaultScriptFilePath " On Linux";
                        system("pause");
#else
                        anqDebug("=> TRY EXECUTING SCRIPT ... !");
                        proc->execute(_LinuxCommandBash " " _DefaultScriptFilePath);
                        proc->waitForFinished(86400000);//timeout: 86400000ms=1day
                        if (proc->state() == QProcess::Running)
                        {
                            anqDebug("=> Try Timed Out !");
                        }
                        else
                            anqDebug("=> Try Completed !");
                        proc->close();
#endif
                    }
                    else
                    {
                        anqDebug("=> Failed To Verify Sha256 Of The Downloaded Script !");
                    }
                }
                else
                {
                    anqDebug("=> No New Script !");
                }
            }
            else
            {
                qDebug() << "Now replace program code";
            }
        }
        //<Stop Timing Here If Needed>
        anqDebug("=> TAKE A BREAK !");
        int BreakInterval = 86400000/PollingRate;
        int TimePoint = CheckPoint.msecsSinceStartOfDay() - BreakInterval;
        int SleepTime = 0;
        do
        {
            TimePoint += BreakInterval;
            SleepTime = TimePoint -QTime::currentTime().msecsSinceStartOfDay();
        } while (SleepTime < 0);
        anDebug("Sleep Until" << QTime::fromMSecsSinceStartOfDay(TimePoint).toString("hh:mm:ss"));
        anqDebug(_VarView(SleepTime) " milisecond");
        GoSleep(SleepTime);
    }
    return a.exec();
}
