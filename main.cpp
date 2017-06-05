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
#define _DefaultTimesToTryDownloadingConfigFile 7
#define _DefaultTimeOutInSecondForADownloadOfConfigFile 47
#define _DefaultTimesToTryDownloadingScriptFile 4
#define _DefaultTimeOutInSecondForADownloadOfScriptFile 60
#define _DefaultTimeOutInSecondForScriptFileExecution 600
#define _DefaultTimesToTryDownloadingProgFile 4
#define _DefaultTimeOutInSecondForADownloadOfProgFile 240

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
    quint8 TimesToTryDownloadingConfigFile = _DefaultTimesToTryDownloadingConfigFile;
    int TimeOutInMilisecondForADownloadOfConfigFile = _DefaultTimeOutInSecondForADownloadOfConfigFile*1000;
    quint8 TimesToTryDownloadingScriptFile = _DefaultTimesToTryDownloadingScriptFile;
    int TimeOutInMilisecondForADownloadOfScriptFile = _DefaultTimeOutInSecondForADownloadOfScriptFile*1000;
    int TimeOutInMilisecondForScriptFileExecution = _DefaultTimeOutInSecondForScriptFileExecution*1000;
    quint8 TimesToTryDownloadingProgFile = _DefaultTimesToTryDownloadingProgFile;
    int TimeOutInMilisecondForADownloadOfProgFile = _DefaultTimeOutInSecondForADownloadOfProgFile*1000;
    QString ScriptSha256 = "";
    QString ScriptLink = "";
    QString ProgSha256 = "";
    QString ProgLink = "";
    quint8 count0 = 0;
    while (true)
    {        
        anqDebug("=======================================================================");
        anqDebug("=====================START A NEW UPDATE CYCLE =========================");
        //<Start Timing Here If Needed>
        QFile::remove(_DefaultConfigFilePath);
        anqDebug("=> Start Downloading A Config File !");
        anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
        do
        {
            if (++count0 > TimesToTryDownloadingConfigFile)
            {
                anqDebug("=> Failed To Download The Config File !");
                break;
            }
#ifdef Q_OS_WIN
            qDebug() << "This Program Is Only For Testing Purpose On Windows";
            qDebug() << "Please MANUALLY Download AutoUpdatePiSG.conf";
            qDebug() << "Then Place It Into The Following Path:";
            qDebug() << _DefaultAutoUpdatePiSGFolderPath;
            system("pause");
#else
            anqDebug("=> Try Downloading The Config File ...");
            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" _DefaultConfigFileLink "\"");
            proc->waitForFinished(TimeOutInMilisecondForADownloadOfConfigFile);
            anDebugCode(if (proc->state() == QProcess::Running)
            {
                anqDebug("=> Try Timed Out !");
            }
            else
                anqDebug("=> Try Completed !");)
            proc->close();
#endif
        } while (!QFile::exists(_DefaultConfigFilePath));
        if (count0<=TimesToTryDownloadingConfigFile)
        {
            anqDebug("=> Successfully Download The Config File !");
            count0 = 0;
            QFile configFile(_DefaultConfigFilePath);
            if (configFile.open(QIODevice::ReadOnly))
            {
               anqDebug("=> Successfully Read The Config File !");
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
                        anqDebug("   " _VarView(PollingRate));
                    }
                    else if (parsedParamsInOneLine.contains("TimePoint"))
                    {
                        CheckPoint = QTime::fromString(parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimePoint")+1).trimmed(),"hh:mm:ss");
                        if (CheckPoint.isNull() || !CheckPoint.isValid())
                        {
                            CheckPoint = _DefaultCheckPoint;
                        }
                        anqDebug("   " _VarView(CheckPoint.toString("hh:mm:ss")));
                    }
                    else if (parsedParamsInOneLine.contains("ScriptSha256"))
                    {
                        ScriptSha256 = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ScriptSha256")+1).trimmed();
                        anqDebug("   " _VarView(ScriptSha256));
                    }
                    else if (parsedParamsInOneLine.contains("ScriptLink"))
                    {
                        ScriptLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ScriptLink")+1).trimmed();
                        anqDebug("   " _VarView(ScriptLink));
                    }
                    else if (parsedParamsInOneLine.contains("ProgSha256"))
                    {
                        ProgSha256 = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ProgSha256")+1).trimmed();
                        anqDebug("   " _VarView(ProgSha256));
                    }
                    else if (parsedParamsInOneLine.contains("ProgLink"))
                    {
                        ProgLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("ProgLink")+1).trimmed();
                        anqDebug("   " _VarView(ProgLink));
                    }
                    else if (parsedParamsInOneLine.contains("TimesToTryDownloadingConfigFile"))
                    {
                        TimesToTryDownloadingConfigFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimesToTryDownloadingConfigFile")+1).trimmed().toInt();
                        anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
                    }
                    else if (parsedParamsInOneLine.contains("TimesToTryDownloadingScriptFile"))
                    {
                        TimesToTryDownloadingScriptFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimesToTryDownloadingScriptFile")+1).trimmed().toInt();
                        anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
                    }
                    else if (parsedParamsInOneLine.contains("TimesToTryDownloadingProgFile"))
                    {
                        TimesToTryDownloadingProgFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimesToTryDownloadingProgFile")+1).trimmed().toInt();
                        anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfConfigFile"))
                    {
                        TimeOutInMilisecondForADownloadOfConfigFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfConfigFile")+1).trimmed().toInt();
                        if (TimeOutInMilisecondForADownloadOfConfigFile<7)
                        {
                            TimeOutInMilisecondForADownloadOfConfigFile = _DefaultTimeOutInSecondForADownloadOfConfigFile;
                        }
                        TimeOutInMilisecondForADownloadOfConfigFile *= 1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfScriptFile"))
                    {
                        TimeOutInMilisecondForADownloadOfScriptFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfScriptFile")+1).trimmed().toInt();
                        if (TimeOutInMilisecondForADownloadOfScriptFile<11)
                        {
                            TimeOutInMilisecondForADownloadOfScriptFile = _DefaultTimeOutInSecondForADownloadOfScriptFile;
                        }
                        TimeOutInMilisecondForADownloadOfScriptFile *= 1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForScriptFileExecution"))
                    {
                        TimeOutInMilisecondForScriptFileExecution = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForScriptFileExecution")+1).trimmed().toInt();
                        if (TimeOutInMilisecondForScriptFileExecution<27)
                        {
                            TimeOutInMilisecondForScriptFileExecution = _DefaultTimeOutInSecondForScriptFileExecution;
                        }
                        TimeOutInMilisecondForScriptFileExecution *= 1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForScriptFileExecution));
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfProgFile"))
                    {
                        TimeOutInMilisecondForADownloadOfProgFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfProgFile")+1).trimmed().toInt();
                        if (TimeOutInMilisecondForADownloadOfProgFile<24)
                        {
                            TimeOutInMilisecondForADownloadOfProgFile = _DefaultTimeOutInSecondForADownloadOfProgFile;
                        }
                        TimeOutInMilisecondForADownloadOfProgFile *= 1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
                    }
               }
               configFile.close();
               anqDebug("=> Successfully Parsed The Config File !");
            }
            anDebugCode(
            else
            {
                anqDebug("=> Failed To Read The Config File !");
                anqDebug("=> Failed To Update Neccessary Variables !");
            })
            if (ScriptSha256.size() == 64)
            {
                anqDebug("=> Valid ScriptSha256 !");
                bool tmpCondition = false;
                bool IsThereFileScriptSha256 = QFile::exists(_DefaultScriptSha256FilePath);
                QString currentScriptSha256 = "";
                if (IsThereFileScriptSha256)
                {
                    anqDebug("=> Found A .ScriptSha256 File !");
                    QFile ScriptSha256File(_DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256");
                    if (ScriptSha256File.open(QIODevice::ReadOnly))
                    {
                       QTextStream readFile(&ScriptSha256File);
                       while (!readFile.atEnd())
                       {
                            anqDebug("=> Fetched .ScriptSha256 File ...");
                            currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug("   " _VarView(currentScriptSha256));
                            anqDebug("new" _VarView(ScriptSha256));
                       }
                       ScriptSha256File.close();
                    }
                    tmpCondition = (currentScriptSha256 != ScriptSha256);
                    anDebugWrap(tmpCondition,
                                anqDebug("=> Not Matched ScriptSha256 between the old script and the new one !");
                                );
                    anDebugWrap(tmpCondition,
                                anqDebug("=> Matched ScriptSha256 --> No New Script");
                                );
                }
                anDebugWrap(!IsThereFileScriptSha256,
                                anqDebug("=> Not Found Any .ScriptSha256 File !");
                                );
                if (tmpCondition || !IsThereFileScriptSha256)
                {
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
                        anqDebug("=> Start Downloading A New Script File !");
                        anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
                        do
                        {
                            if (++count1>TimesToTryDownloadingScriptFile)
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
                            anqDebug("=> Try Downloading A Script File ...");
                            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" + ScriptLink + "\"");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfScriptFile);
                            anDebugCode(if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                                anqDebug("=> Try Completed !");)
                            proc->close();
#endif
                        } while (!QFile::exists(_DefaultScriptFilePath));
                        if (count1<=TimesToTryDownloadingScriptFile)
                        {
                            anqDebug("=> Successfully Download The Script File !");
                            count1 = 0;
                            do
                            {
                                if (++count1>4)
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
                                proc->waitForFinished(TimeOutInMilisecondForADownloadOfScriptFile);
                                proc->setStandardOutputFile(QProcess::nullDevice());
                                anDebugCode(if (proc->state() == QProcess::Running)
                                {
                                    anqDebug("=> Try Timed Out !");
                                }
                                else
                                    anqDebug("=> Try Completed !");)
                                proc->close();
#endif
                            } while(!QFile::exists(_DefaultScriptSha256FilePath));
                            if (count1<=4)
                            {
                                anqDebug("=> Successfully Calculate Sha256 Of The New Script !");
                                count1 = 0;
                                QFile ScriptSha256File(_DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256");
                                if (ScriptSha256File.open(QIODevice::ReadOnly))
                                {
                                   QTextStream readFile(&ScriptSha256File);
                                   while (!readFile.atEnd())
                                   {
                                       anqDebug("=> Fetched The .ScriptSha256 File ...");
                                       currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                       anqDebug("   " _VarView(currentScriptSha256));
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
                        proc->start(_LinuxCommandBash " " _DefaultScriptFilePath);
                        proc->waitForFinished(TimeOutInMilisecondForScriptFileExecution);
                        anDebugCode(if (proc->state() == QProcess::Running)
                        {
                            anqDebug("=> Try Timed Out For " << TimeOutInMilisecondForScriptFileExecution << " milisecond !";);
                        }
                        else
                            anqDebug("=> Try Completed !");)
                        proc->close();
#endif
                    }
                }
            }
            else
            {
                anqDebug("=> Not Valid ScriptSha256 !");
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
        anqDebug("   " _VarView(SleepTime) " milisecond");
        GoSleep(SleepTime);
    }
    return a.exec();
}
