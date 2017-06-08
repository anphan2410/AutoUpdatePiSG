#include <QCoreApplication>
#include <QTime>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "anqtdebug.h"
#include <iostream>
#include <QtMessageHandler>

#ifdef Q_OS_WIN
#include <windows.h>
#define _TildeDirectory "D:/home/pi"
#else
#include <time.h>
#define _TildeDirectory "/home/pi"
#define _LinuxCommandBash "/bin/bash"
#endif

#define _DefaultAutoUpdatePiSGFolderPath _TildeDirectory "/AutoUpdatePiSG"
#define _DefaultConfigFilePath _DefaultAutoUpdatePiSGFolderPath "/AutoUpdatePiSG.conf"
#define _DefaultConfigFileLink "http://tamduongs.com:80/iot/AutoUpdatePiSG/AutoUpdatePiSG.conf"
#define _DefaultScriptSha256FilePath _DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256"
#define _DefaultScriptFilePath _DefaultAutoUpdatePiSGFolderPath "/AutoUpdatePiSG.sh"
#define _DefaultProgSha256FilePath _DefaultAutoUpdatePiSGFolderPath "/.ProgSha256"
#define _DefaultTmpProgFolderPath _DefaultAutoUpdatePiSGFolderPath "/Flipper1"
#define _DefaultTmpProgFilePath _DefaultTmpProgFolderPath "/FlipperDemo"
#define _DefaultProgFilePath _TildeDirectory "/Flipper1/FlipperDemo"
#define _DefaultPollingRate 8
#define _DefaultCheckPoint QTime::fromString("14:30:00","hh:mm:ss")
#define _DefaultTimesToTryDownloadingConfigFile 11
#define _DefaultTimeOutInSecondForADownloadOfConfigFile 47
#define _DefaultTimesToTryDownloadingScriptFile 7
#define _DefaultTimeOutInSecondForADownloadOfScriptFile 77
#define _DefaultTimeOutInSecondForScriptFileExecution 777
#define _DefaultTimesToTryDownloadingProgFile 7
#define _DefaultTimeOutInSecondForADownloadOfProgFile 777

anDebugCode(
#define _DefaultLastQProcessStandardOutputFilePath _DefaultAutoUpdatePiSGFolderPath "/LastQProcessStandardOutputCapture.info"
#define _DefaultLastUpdateCycleStandardOutputFilePath _DefaultAutoUpdatePiSGFolderPath "/LastUpdateCycleStandardOutputCapture.info"
)

void GoSleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(ms);
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

anDebugCode(

QString anqMsgCapture;

void anqMsgHandler(QtMsgType, const QMessageLogContext &, const QString & msg)
{
    anqMsgCapture += msg + "\n";
    std::cout << msg.toStdString() << std::endl;
}

void anqMsgCaptureToFile(const QString &ADestinationFilePath, QIODevice::OpenModeFlag WriteIntoFileMode = QIODevice::Truncate)
{
    QFile LastUpdateCycleStandardOutputCapture(ADestinationFilePath);
    if (LastUpdateCycleStandardOutputCapture.open(QIODevice::WriteOnly | WriteIntoFileMode)) {
        QTextStream OutputMessagesIntoFile(&LastUpdateCycleStandardOutputCapture);
        OutputMessagesIntoFile << anqMsgCapture << endl;
    }
    LastUpdateCycleStandardOutputCapture.close();
}

void qDebugAWholeFile(const QString &ASourceFilePath)
{
    QFile readFile(ASourceFilePath);
    if (readFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ReadTextOut(&readFile);
        qDebug() << ReadTextOut.readAll();
    }
    readFile.close();
}
)

int main(int argc, char *argv[])
{
    anDebugCode(qInstallMessageHandler(anqMsgHandler);)
    QCoreApplication a(argc, argv);
    QTimer quitThisQtApp;
    quitThisQtApp.setInterval(7000);
    quitThisQtApp.setSingleShot(true);
    QObject::connect(&quitThisQtApp, &QTimer::timeout, [&a](){a.quit();});
    anqDebug("***********************************************************************");
    anqDebug("############## START AUTO UPDATE PI BY ASCENX SAIGON ##################");
    GoSleep(7000);//This Line Is Not Redundant But For Crucial Purposes
#ifndef Q_OS_WIN
    QProcess * proc = new QProcess();
    proc->setProcessChannelMode(QProcess::MergedChannels);
#endif
    anqDebug("=> Load Default Settings !");
    QString ConfigLink = "";
    quint8 PollingRate = _DefaultPollingRate;
    QTime CheckPoint = _DefaultCheckPoint;
    QString ScriptSha256 = "";
    QString ScriptLink = "";
    QString ProgSha256 = "";
    QString ProgLink = "";
    quint8 TimesToTryDownloadingConfigFile = _DefaultTimesToTryDownloadingConfigFile;
    int TimeOutInMilisecondForADownloadOfConfigFile = _DefaultTimeOutInSecondForADownloadOfConfigFile*1000;
    quint8 TimesToTryDownloadingScriptFile = _DefaultTimesToTryDownloadingScriptFile;
    int TimeOutInMilisecondForADownloadOfScriptFile = _DefaultTimeOutInSecondForADownloadOfScriptFile*1000;
    int TimeOutInMilisecondForScriptFileExecution = _DefaultTimeOutInSecondForScriptFileExecution*1000;
    quint8 TimesToTryDownloadingProgFile = _DefaultTimesToTryDownloadingProgFile;
    int TimeOutInMilisecondForADownloadOfProgFile = _DefaultTimeOutInSecondForADownloadOfProgFile*1000;
    if (QFile::exists(_DefaultConfigFilePath))
    {
        anqDebug("=> Found A Config File AutoUpdatePiSG.conf !");
        anqDebug("=> Try Fetching A Config File Link ...");
        QFile aConfigFile(_DefaultConfigFilePath);
        if (aConfigFile.open(QIODevice::ReadOnly))
        {
            anqDebug("=> Try Completed !");
            QTextStream readFile(&aConfigFile);
            while (!readFile.atEnd())
            {
                QStringList parsedParamsInOneLine = readFile.readLine().split('=');
                if (parsedParamsInOneLine.contains("NextConfigLink"))
                {
                    ConfigLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("NextConfigLink")+1).trimmed();
                    anqDebug("=> Successfully Read The Config Link !");
                }
            }
            anqDebug("   " _VarView(ConfigLink));
            if (ConfigLink.isEmpty())
            {
                anqDebug("   -> Invalid -> Load Default !");
                ConfigLink = _DefaultConfigFileLink;
                anqDebug("   " _VarView(ConfigLink));
            }
            aConfigFile.close();
        }
        anDebugCode(
        else
        {
            anqDebug("=> Try Failed !");
            anqDebug("=> Failed To Read The Config File !");
        }
        )
    }
    else
    {
        ConfigLink = _DefaultConfigFileLink;
    }
    quint8 count0 = 0;
    while (true)
    {
        //Reinitialize Variables
        ScriptLink = "";
        ScriptSha256 = "";
        ProgLink = "";
        ProgSha256 = "";
        //<Start Timing Here If Needed>
        anqDebug("=======================================================================");
        anqDebug("=====================START A NEW UPDATE CYCLE =========================");
        anqDebug("=> Check The Current Settings ...");
        anqDebug("   " _VarView(ConfigLink));
        anqDebug("   " _VarView(PollingRate));
        anqDebug("   " _VarView(CheckPoint.toString()));
        anqDebug("   " _VarView(ScriptSha256));
        anqDebug("   " _VarView(ScriptLink));
        anqDebug("   " _VarView(ProgSha256));
        anqDebug("   " _VarView(ProgLink));
        anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
        anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForScriptFileExecution));
        anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
        //Ping www.google.com 7 times to refresh connections
#ifdef Q_OS_WIN
        qDebug() << "This Program Is Only For Testing Purpose On Windows";
        qDebug() << "Please MANUALLY Check The Internet Connection";
        system("pause");
#else
        anqDebug("=> Try Checking The Internet Connection ...");
        anDebugCode(
        proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
        proc->start("ping -c 7 www.google.com");
        proc->waitForFinished(TimeOutInMilisecondForADownloadOfConfigFile*7);
        anDebugCode(
        proc->setStandardOutputFile(QProcess::nullDevice());
        anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
        qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
        anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
        if (proc->state() == QProcess::Running)
        {
            anqDebug("=> Try Timed Out !");
            anqDebug("=> Failed To Connect To The Internet !");
        }
        else
        {
            anqDebug("=> Try Completed !");
            anqDebug("=> Successfully Verify The Internet Connection !");
        }
        )
        proc->close();
#endif
        anqDebug("=> Start Downloading A Config File !");
        anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
        do
        {
            QFile::remove(_DefaultConfigFilePath);
            if ((++count0) > TimesToTryDownloadingConfigFile)
            {
                if (ConfigLink != _DefaultConfigFileLink)
                {
                    anqDebug("=> Failed To Download The Config File With The Given Config Link !");
                    anqDebug("   " _VarView(ConfigLink));
                    anqDebug("=> Load The Default Config Link !");
                    ConfigLink = _DefaultConfigFileLink;
                    anqDebug("   " _VarView(ConfigLink));
                    anqDebug("=> Restart Downloading A Config File !");
                    anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
                    anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
                    count0 = 0;
                }
                else
                {
                    anqDebug("=> Failed To Download The Config File !");
                    break;
                }
            }
#ifdef Q_OS_WIN
            qDebug() << "This Program Is Only For Testing Purpose On Windows";
            qDebug() << "Please MANUALLY Download AutoUpdatePiSG.conf";
            qDebug() << "Then Place It Into The Following Path:";
            qDebug() << _DefaultAutoUpdatePiSGFolderPath;
            system("pause");
#else
            anqDebug("=> Try Downloading The Config File ...");
            anDebugCode(
            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
            proc->start("wget -O " _DefaultConfigFilePath " \"" + ConfigLink + "\"");
            proc->waitForFinished(TimeOutInMilisecondForADownloadOfConfigFile);
            anDebugCode(
            proc->setStandardOutputFile(QProcess::nullDevice());
                    anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                    qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                    anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
            if (proc->state() == QProcess::Running)
            {
                anqDebug("=> Try Timed Out !");
            }
            else
            {
                anqDebug("=> Try Completed !");
            }
            )
            proc->close();
#endif
            if (QFile(_DefaultConfigFilePath).size() < 47)
            {
                QFile::remove(_DefaultConfigFilePath);
            }
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
                    if (parsedParamsInOneLine.contains("NextConfigLink"))
                    {
                        ConfigLink = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("NextConfigLink")+1).trimmed();
                        anqDebug("   " _VarView(ConfigLink));
                        if (ConfigLink.isEmpty())
                        {
                            ConfigLink = _DefaultConfigFileLink;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(ConfigLink));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("PollingRate"))
                    {
                        PollingRate = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("PollingRate")+1).trimmed().toInt();
                        anqDebug("   " _VarView(PollingRate));
                        if (!PollingRate)
                        {
                            PollingRate = _DefaultPollingRate;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(PollingRate));
                        }                        
                    }
                    else if (parsedParamsInOneLine.contains("TimePoint"))
                    {
                        CheckPoint = QTime::fromString(parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimePoint")+1).trimmed(),"hh:mm:ss");
                        anqDebug("   " _VarView(CheckPoint.toString("hh:mm:ss")));
                        if (CheckPoint.isNull() || !CheckPoint.isValid())
                        {
                            CheckPoint = _DefaultCheckPoint;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(CheckPoint.toString("hh:mm:ss")));
                        }                        
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
                        if (!TimesToTryDownloadingConfigFile)
                        {
                            TimesToTryDownloadingConfigFile = _DefaultTimesToTryDownloadingConfigFile;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimesToTryDownloadingConfigFile));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimesToTryDownloadingScriptFile"))
                    {
                        TimesToTryDownloadingScriptFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimesToTryDownloadingScriptFile")+1).trimmed().toInt();
                        anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
                        if (!TimesToTryDownloadingScriptFile)
                        {
                            TimesToTryDownloadingScriptFile = _DefaultTimesToTryDownloadingScriptFile;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimesToTryDownloadingProgFile"))
                    {
                        TimesToTryDownloadingProgFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimesToTryDownloadingProgFile")+1).trimmed().toInt();
                        anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
                        if (!TimesToTryDownloadingProgFile)
                        {
                            TimesToTryDownloadingProgFile = _DefaultTimesToTryDownloadingProgFile;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfConfigFile"))
                    {
                        TimeOutInMilisecondForADownloadOfConfigFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfConfigFile")+1).trimmed().toInt()*1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
                        if (TimeOutInMilisecondForADownloadOfConfigFile<7000)
                        {
                            TimeOutInMilisecondForADownloadOfConfigFile = _DefaultTimeOutInSecondForADownloadOfConfigFile*1000;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfConfigFile));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfScriptFile"))
                    {
                        TimeOutInMilisecondForADownloadOfScriptFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfScriptFile")+1).trimmed().toInt()*1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
                        if (TimeOutInMilisecondForADownloadOfScriptFile<11000)
                        {
                            TimeOutInMilisecondForADownloadOfScriptFile = _DefaultTimeOutInSecondForADownloadOfScriptFile*1000;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForScriptFileExecution"))
                    {
                        TimeOutInMilisecondForScriptFileExecution = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForScriptFileExecution")+1).trimmed().toInt()*1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForScriptFileExecution));
                        if (TimeOutInMilisecondForScriptFileExecution<27000)
                        {
                            TimeOutInMilisecondForScriptFileExecution = _DefaultTimeOutInSecondForScriptFileExecution*1000;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimeOutInMilisecondForScriptFileExecution));
                        }
                    }
                    else if (parsedParamsInOneLine.contains("TimeOutInSecondForADownloadOfProgFile"))
                    {
                        TimeOutInMilisecondForADownloadOfProgFile = parsedParamsInOneLine.at(parsedParamsInOneLine.indexOf("TimeOutInSecondForADownloadOfProgFile")+1).trimmed().toInt()*1000;
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
                        if (TimeOutInMilisecondForADownloadOfProgFile<24000)
                        {
                            TimeOutInMilisecondForADownloadOfProgFile = _DefaultTimeOutInSecondForADownloadOfProgFile*1000;
                            anqDebug("   -> Invalid -> Load Default !");
                            anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
                        }
                    }
               }
               configFile.close();
               anqDebug("=> Finish Parsing The Config File !");
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
                QString fetchedScriptSha256 = "";
                if (IsThereFileScriptSha256)
                {
                    anqDebug("=> Found A .ScriptSha256 File !");
                    QFile ScriptSha256File(_DefaultScriptSha256FilePath);
                    if (ScriptSha256File.open(QIODevice::ReadOnly))
                    {
                       QTextStream readFile(&ScriptSha256File);
                       while (!readFile.atEnd())
                       {
                            anqDebug("=> Fetched .ScriptSha256 File ...");
                            fetchedScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug("   " _VarView(fetchedScriptSha256));
                            anqDebug("   parsed " _VarView(ScriptSha256));
                       }
                       ScriptSha256File.close();
                    }
                    tmpCondition = (fetchedScriptSha256 != ScriptSha256);
                    anDebugWrap(tmpCondition,
                                anqDebug("=> Not Matched ScriptSha256 between the old script and the new one !");
                                );
                    anDebugWrap(!tmpCondition,
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
                        if (++count2>4)
                        {
                            anqDebug("=> Not Matched Sha256 Of The New Script !");
                            break;
                        }                        
                        anqDebug("=> Start Downloading A New Script File !");
                        anqDebug("   " _VarView(TimesToTryDownloadingScriptFile));
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfScriptFile));
                        do
                        {
                            QFile::remove(_DefaultScriptFilePath);
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
                            anDebugCode(
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                            proc->start("wget -O " _DefaultScriptFilePath " \"" + ScriptLink + "\"");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfScriptFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                                    anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                    qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                                    anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                            }
                            )
                            proc->close();
#endif
                            if (QFile(_DefaultScriptFilePath).size() < 47)
                            {
                                QFile::remove(_DefaultScriptFilePath);
                            }
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
                                anDebugCode(
                                            anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                            qDebugAWholeFile(_DefaultScriptSha256FilePath);
                                            anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                                if (proc->state() == QProcess::Running)
                                {
                                    anqDebug("=> Try Timed Out !");
                                }
                                else
                                    anqDebug("=> Try Completed !");
                                )
                                proc->close();
#endif
                            } while(!QFile::exists(_DefaultScriptSha256FilePath));
                            if (count1<=4)
                            {
                                anqDebug("=> Successfully Calculate Sha256 Of The New Script !");
                                count1 = 0;
                                QFile ScriptSha256File(_DefaultScriptSha256FilePath);
                                if (ScriptSha256File.open(QIODevice::ReadOnly))
                                {
                                   QTextStream readFile(&ScriptSha256File);
                                   while (!readFile.atEnd())
                                   {
                                       anqDebug("=> Fetched The .ScriptSha256 File ...");
                                       fetchedScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                       anqDebug("   " _VarView(fetchedScriptSha256));
                                       anqDebug("   parsed " _VarView(ScriptSha256));
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
                    } while (fetchedScriptSha256 != ScriptSha256);
                    if (tmpCondition && (count2<=4))
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
                        anDebugCode(
                        proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                        proc->start(_LinuxCommandBash " " _DefaultScriptFilePath);
                        proc->waitForFinished(TimeOutInMilisecondForScriptFileExecution);
                        anDebugCode(
                        proc->setStandardOutputFile(QProcess::nullDevice());
                                anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                                anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                        if (proc->state() == QProcess::Running)
                        {
                            anqDebug("=> Try Timed Out For " << TimeOutInMilisecondForScriptFileExecution << " milisecond !";);
                        }
                        else
                        {
                            anqDebug("=> Try Completed !");
                        }
                        )
                        proc->close();
#endif
                    }
                }
            }
            anDebugCode(
            else
            {
                anqDebug("=> Not Valid ScriptSha256 !");
            })
            if (ProgSha256.size() == 64)
            {
                anqDebug("=> Valid ProgSha256 !");
                bool tmpCondition = false;
                bool IsThereFileProgSha256 = QFile::exists(_DefaultProgSha256FilePath);
                QString fetchedProgSha256 = "";
                if (IsThereFileProgSha256)
                {
                    anqDebug("=> Found A .ProgSha256 File !");
                    QFile ProgSha256File(_DefaultProgSha256FilePath);
                    if (ProgSha256File.open(QIODevice::ReadOnly))
                    {
                        QTextStream readFile(&ProgSha256File);
                        while (!readFile.atEnd())
                        {
                            anqDebug("=> Fetched .ProgSha256 File ...");
                            fetchedProgSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug("   " _VarView(fetchedProgSha256));
                            anqDebug("   parsed " _VarView(ProgSha256));
                        }
                        ProgSha256File.close();
                    }
                    tmpCondition = (fetchedProgSha256 != ProgSha256);
                    anDebugWrap(tmpCondition,
                                anqDebug("=> Not Matched ProgSha256 between the old program and the new one !");
                            );
                    anDebugWrap(!tmpCondition,
                                anqDebug("=> Matched ProgSha256 --> No New Program");
                            );
                }
                anDebugWrap(!IsThereFileProgSha256,
                            anqDebug("=> Not Found Any .ProgSha256 File !");
                        );
                if (tmpCondition || !IsThereFileProgSha256)
                {
                    quint8 count1 = 0;
                    quint8 count2 = 0;
                    tmpCondition = true;
                    do
                    {
                        if (++count2>4)
                        {
                            anqDebug("=> Not Matched Sha256 Of The New Program !");
                            anqDebug("=> FAILED TO UPDATE The New Program !");
                            break;
                        }
                        anqDebug("=> Start Downloading A New Program File !");
                        anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
                        do
                        {
                            QFile::remove(_DefaultTmpProgFilePath);
                            if (++count1>TimesToTryDownloadingProgFile)
                            {
                                break;
                            }
#ifdef Q_OS_WIN
                            qDebug() << "This Program Is Only For Testing Purpose On Windows";
                            qDebug() << "Please MANUALLY Download Program FlipperDemo";
                            qDebug() << "Then Place The File Into The Following Path:";
                            qDebug() << _DefaultTmpProgFolderPath;
                            system("pause");
#else
                            anqDebug("=> Try Downloading A Program File ...");
                            anDebugCode(
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                            proc->start("wget -O " _DefaultTmpProgFilePath " \"" + ProgLink + "\"");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                                    anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                    qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                                    anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                            }
                            )
                            proc->close();
#endif
                            if (QFile(_DefaultTmpProgFilePath).size() < 47)
                            {
                                QFile::remove(_DefaultTmpProgFilePath);
                            }
                        } while (!QFile::exists(_DefaultTmpProgFilePath));
                        if (count1<=TimesToTryDownloadingProgFile)
                        {
                            anqDebug("=> Successfully Download The Program File !");
                            count1 = 0;
                            do
                            {
                                if (++count1>4)
                                {
                                    break;
                                }
#ifdef Q_OS_WIN
                                qDebug() << "This Program Is Only For Testing Purpose On Windows";
                                qDebug() << "Please MANUALLY Write Sha256Sum Of FlipperDemo Into .ProgSha256";
                                qDebug() << "Then Place File .ProgSha256 Into The Following Path:";
                                qDebug() << _DefaultAutoUpdatePiSGFolderPath;
                                system("pause");
#else
                                anqDebug("=> Try Calculating Sha256 Of The New Prog File ...");
                                proc->setStandardOutputFile(_DefaultProgSha256FilePath);
                                proc->start("sha256sum " _DefaultTmpProgFilePath);
                                proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                                proc->setStandardOutputFile(QProcess::nullDevice());
                                anDebugCode(
                                            anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                            qDebugAWholeFile(_DefaultProgSha256FilePath);
                                            anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                                if (proc->state() == QProcess::Running)
                                {
                                    anqDebug("=> Try Timed Out !");
                                }
                                else
                                    anqDebug("=> Try Completed !");
                                )
                                proc->close();
#endif
                            } while(!QFile::exists(_DefaultProgSha256FilePath));
                            if (count1<=4)
                            {
                                anqDebug("=> Successfully Calculate Sha256 Of The New Program !");
                                count1 = 0;
                                QFile ProgSha256File(_DefaultProgSha256FilePath);
                                if (ProgSha256File.open(QIODevice::ReadOnly))
                                {
                                    QTextStream readFile(&ProgSha256File);
                                    while (!readFile.atEnd())
                                    {
                                        anqDebug("=> Fetched The .ProgSha256 File ...");
                                        fetchedProgSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                        anqDebug("   " _VarView(fetchedProgSha256));
                                        anqDebug("   parsed " _VarView(ProgSha256));
                                    }
                                    ProgSha256File.close();
                                }
                            }
                            else
                            {
                                anqDebug("=> Failed To Calculate Sha256 Of The New Program !");
                                tmpCondition = false;
                                break;
                            }
                        }
                        else
                        {
                            anqDebug("=> Failed To Download The New Program !");
                            tmpCondition = false;
                            break;
                        }
                    } while (fetchedProgSha256 != ProgSha256);
                    if (tmpCondition && (count2<=4))
                    {
                        anqDebug("=> Matched Sha256 Of The Downloaded Program !");
                        count2 = 0;
                        QFile::remove(_DefaultProgFilePath);
                        if (QFile::copy(_DefaultTmpProgFilePath,_DefaultProgFilePath))
                        {
                            anqDebug("=> Successfully UPDATE The Program !");
#ifdef Q_OS_WIN
                            qDebug() << "This Program Is Only For Testing Purpose On Windows";
                            qDebug() << "This Step Can Not Be Imitated !";
                            qDebug() << "The System Should Be Rebooted And Autostarted With Several Programs!";
                            system("pause");
#else
                            anqDebug("=> Try Setting Executable Attribute ...");
                            anDebugCode(
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                            //Set File Attribute To Executable
                            proc->start("chmod +777 " _DefaultProgFilePath);
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfScriptFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                                    anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                    qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                                    anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                                anqDebug("=> Failed To Set Executable Attribute Of The Program File !");
                                anqDebug("=> ERROR OCCURRED !");
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                                anqDebug("=> Successfully Set Executable Attribute Of The Program File !");
                            }
                            )
                            proc->close();
                            //Reboot
                            anqDebug("=> Try Rebooting ...");
                            anDebugCode(
                            anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath);                            
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);
                            anqMsgCapture.clear();
                            )
                            proc->start("reboot");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                                    anqDebug("-------v-------v-------v-------v-------v-------v-------v-------v-------");
                                    qDebugAWholeFile(_DefaultLastQProcessStandardOutputFilePath);
                                    anqDebug("-------^-------^-------^-------^-------^-------^-------^-------^-------");
                            )
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                                anqDebug("=> ERROR OCCURRED !");
                                anDebugCode(anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath, QIODevice::Append);)
                                proc->close();
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                                anqDebug("=> The System Is About To Reboot !");
                                anDebugCode(anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath, QIODevice::Append);
                                anqMsgCapture.clear();
                                )
                                proc->close();
                                //quit this qt app within 7 seconds
                                quitThisQtApp.start();
                                return a.exec();
                            }
#endif
                        }
                    }
                }
            }
            anDebugCode(
            else
            {
                anqDebug("=> Not Valid ProgSha256 !");
            })
        }
        //<Stop Timing Here If Needed>
        anqDebug("=> TAKE A BREAK !");
        anqDebug("   " _VarView(CheckPoint.toString()));
        anqDebug("=> Calculate Time To Next Check Point ...");
        int BreakInterval = 86400000/PollingRate;
        int TimePoint = CheckPoint.msecsSinceStartOfDay();
        int SleepTime = 0;
        while (TimePoint > 0)
        {
            TimePoint -= BreakInterval;
        }
        anqDebug("   Original TimePoint=" + QTime::fromMSecsSinceStartOfDay(TimePoint+BreakInterval).toString("hh:mm:ss"));
        anqDebug("   Break Interval=" + QTime::fromMSecsSinceStartOfDay(BreakInterval).toString("hh:mm:ss"));
        do
        {
            TimePoint += BreakInterval;
            SleepTime = TimePoint -QTime::currentTime().msecsSinceStartOfDay();
        } while (SleepTime < 0);
        anqDebug("   " _VarView(SleepTime) " milisecond");
        anDebugCode(
        if (TimePoint <= 86400000)
        {
            anqDebug("=> GET SLEEP UNTIL " << QTime::fromMSecsSinceStartOfDay(TimePoint).toString("hh:mm:ss"));
        }
        else
        {
            anqDebug("=> GET SLEEP UNTIL " << QTime::fromMSecsSinceStartOfDay(TimePoint-86400000).toString("hh:mm:ss"));
        })
        anDebugCode(
        anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath);
        anqMsgCapture.clear();
        )
        GoSleep(SleepTime);
    }
    //quit this qt app within 7 seconds
    quitThisQtApp.start();
    return a.exec();
}
