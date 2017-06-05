#include <QCoreApplication>
#include <QTime>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
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
#define _DefaultConfigFileLink "http://tamduongs.com:82/iot/AutoUpdatePiSG.conf"
#define _DefaultScriptSha256FilePath _DefaultAutoUpdatePiSGFolderPath "/.ScriptSha256"
#define _DefaultScriptFilePath _DefaultAutoUpdatePiSGFolderPath "/AutoUpdatePiSG.sh"
#define _DefaultProgSha256FilePath _DefaultAutoUpdatePiSGFolderPath "/.ProgSha256"
#define _DefaultTmpProgFolderPath _DefaultAutoUpdatePiSGFolderPath "/Flipper1"
#define _DefaultTmpProgFilePath _DefaultTmpProgFolderPath "/FlipperDemo"
#define _DefaultProgFilePath _TildeDirectory "/Flipper1/FlipperDemo"
#define _DefaultPollingRate 2
#define _DefaultCheckPoint QTime::fromString("14:30:00","hh:mm:ss")
#define _DefaultTimesToTryDownloadingConfigFile 11
#define _DefaultTimeOutInSecondForADownloadOfConfigFile 47
#define _DefaultTimesToTryDownloadingScriptFile 7
#define _DefaultTimeOutInSecondForADownloadOfScriptFile 60
#define _DefaultTimeOutInSecondForScriptFileExecution 600
#define _DefaultTimesToTryDownloadingProgFile 7
#define _DefaultTimeOutInSecondForADownloadOfProgFile 240

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

void anqMsgCaptureToFile(const QString &ADestinationFilePath)
{
    QFile LastUpdateCycleStandardOutputCapture(ADestinationFilePath);
    if (LastUpdateCycleStandardOutputCapture.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream OutputMessagesIntoFile(&LastUpdateCycleStandardOutputCapture);
        OutputMessagesIntoFile << anqMsgCapture << endl;
    }
    LastUpdateCycleStandardOutputCapture.close();
}

void anqMsgCaptureFromFile(const QString &ASourceFilePath)
{
    QFile readFile(ASourceFilePath);
    if (readFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ReadTextOut(&readFile);
        anqMsgCapture += ReadTextOut.readAll();
    }
    ReadFile.close();
}
)

int main(int argc, char *argv[])
{
    anDebugCode(qInstallMessageHandler(anqMsgHandler);)
    QCoreApplication a(argc, argv);
#ifndef Q_OS_WIN
    QProcess * proc = new QProcess();
    proc->setProcessChannelMode(QProcess::MergedChannels);
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
        anDebugCode(anqMsgCapture.clear();)
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
            anDebugCode(
            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" _DefaultConfigFileLink "\"");
            proc->waitForFinished(TimeOutInMilisecondForADownloadOfConfigFile);
            anDebugCode(
            proc->setStandardOutputFile(QProcess::nullDevice());
            anqMsgCaptureFromFile(_DefaultLastQProcessStandardOutputFilePath);
            if (proc->state() == QProcess::Running)
            {
                anqDebug("=> Try Timed Out !");
            }
            else
            {
                anqDebug("=> Try Completed !");
                anqDebug("---------------------------------------------------------------------");
                proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                anqDebug("---------------------------------------------------------------------");
            })
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
                    QFile ScriptSha256File(_DefaultScriptSha256FilePath);
                    if (ScriptSha256File.open(QIODevice::ReadOnly))
                    {
                       QTextStream readFile(&ScriptSha256File);
                       while (!readFile.atEnd())
                       {
                            anqDebug("=> Fetched .ScriptSha256 File ...");
                            currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug("   " _VarView(currentScriptSha256));
                            anqDebug("   parsed " _VarView(ScriptSha256));
                       }
                       ScriptSha256File.close();
                    }
                    tmpCondition = (currentScriptSha256 != ScriptSha256);
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
                            anDebugCode(
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                            proc->start("wget -P " _DefaultAutoUpdatePiSGFolderPath " \"" + ScriptLink + "\"");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfScriptFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                            anqMsgCaptureFromFile(_DefaultLastQProcessStandardOutputFilePath);
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                                anqDebug("---------------------------------------------------------------------");
                                proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                                anqDebug("---------------------------------------------------------------------");
                            })
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
                                anDebugCode(
                                anqMsgCaptureFromFile(_DefaultScriptSha256FilePath);
                                if (proc->state() == QProcess::Running)
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
                                QFile ScriptSha256File(_DefaultScriptSha256FilePath);
                                if (ScriptSha256File.open(QIODevice::ReadOnly))
                                {
                                   QTextStream readFile(&ScriptSha256File);
                                   while (!readFile.atEnd())
                                   {
                                       anqDebug("=> Fetched The .ScriptSha256 File ...");
                                       currentScriptSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                       anqDebug("   " _VarView(currentScriptSha256));
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
                    } while (currentScriptSha256 != ScriptSha256);
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
                        anqMsgCaptureFromFile(_DefaultLastQProcessStandardOutputFilePath);
                        if (proc->state() == QProcess::Running)
                        {
                            anqDebug("=> Try Timed Out For " << TimeOutInMilisecondForScriptFileExecution << " milisecond !";);
                        }
                        else
                        {
                            anqDebug("=> Try Completed !");
                            anqDebug("=> Script Execution Standard Output Captured !");
                            anqDebug("---------------------------------------------------------------------");
                            proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                            anqDebug("---------------------------------------------------------------------");
                        })
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
                QString currentProgSha256 = "";
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
                            currentProgSha256 = readFile.readLine().trimmed().split(' ').at(0);
                            anqDebug("   " _VarView(currentProgSha256));
                            anqDebug("   parsed " _VarView(ProgSha256));
                        }
                        ProgSha256File.close();
                    }
                    tmpCondition = (currentProgSha256 != ProgSha256);
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
                            break;
                        }
                        QFile::remove(_DefaultTmpProgFilePath);
                        anqDebug("=> Start Downloading A New Program File !");
                        anqDebug("   " _VarView(TimesToTryDownloadingProgFile));
                        anqDebug("   " _VarView(TimeOutInMilisecondForADownloadOfProgFile));
                        do
                        {
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
                            proc->start("wget -P " _DefaultTmpProgFolderPath " \"" + ProgLink + "\"");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                            anqMsgCaptureFromFile(_DefaultLastQProcessStandardOutputFilePath);
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                            }
                            else
                            {
                                anqDebug("=> Try Completed !");
                                anqDebug("---------------------------------------------------------------------");
                                proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                                anqDebug("---------------------------------------------------------------------");
                            })
                            proc->close();
#endif
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
                                qDebug() << _DefaultTmpProgFolderPath;
                                system("pause");
#else
                                anqDebug("=> Try Calculating Sha256 Of The New Prog File ...");
                                proc->setStandardOutputFile(_DefaultProgSha256FilePath);
                                proc->start("sha256sum " _DefaultTmpProgFilePath);
                                proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                                proc->setStandardOutputFile(QProcess::nullDevice());
                                anDebugCode(
                                anqMsgCaptureFromFile(_DefaultProgSha256FilePath);
                                if (proc->state() == QProcess::Running)
                                {
                                    anqDebug("=> Try Timed Out !");
                                }
                                else
                                    anqDebug("=> Try Completed !");)
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
                                        currentProgSha256 = readFile.readLine().trimmed().split(' ').at(0);
                                        anqDebug("   " _VarView(currentProgSha256));
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
                    } while (currentProgSha256 != ProgSha256);
                    if (tmpCondition && (count2<=4))
                    {
                        anqDebug("=> Matched Sha256 Of The Downloaded Program !");
                        count2 = 0;
                        QFile::remove(_DefaultProgFilePath);
                        if (QFile::copy(_DefaultTmpProgFilePath,_DefaultProgFilePath))
                        {
                            anqDebug("=> Successfully Update The Program !");
                            anqDebug("=> Start Rebooting The System !");
#ifdef Q_OS_WIN
                            qDebug() << "This Program Is Only For Testing Purpose On Windows";
                            qDebug() << "This Step Can Not Be Imitated !";
                            qDebug() << "The System Should Be Rebooted And Autostarted With Several Programs!";
                            system("pause");
#else
                            anqDebug("=> Try Rebooting ...");
                            anDebugCode(
                            anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath);
                            proc->setStandardOutputFile(_DefaultLastQProcessStandardOutputFilePath);)
                            //Set File Attribute To Executable
                            proc->start("chmod +777 " _DefaultProgFilePath);
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                            anDebugCode(
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Failed To Set Executable Attribute Of The Program File !");
                            }
                            else
                                anqDebug("=> Successfully Set Executable Attribute Of The Program File !");
                            anqDebug("---------------------------------------------------------------------");
                            proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                            anqDebug("---------------------------------------------------------------------");)
                            proc->close();
                            //Reboot
                            proc->start("reboot");
                            proc->waitForFinished(TimeOutInMilisecondForADownloadOfProgFile);
                            anDebugCode(
                            proc->setStandardOutputFile(QProcess::nullDevice());
                            anqMsgCaptureFromFile(_DefaultLastQProcessStandardOutputFilePath);
                            if (proc->state() == QProcess::Running)
                            {
                                anqDebug("=> Try Timed Out !");
                                anqDebug("=> Error Occurred ! The System Is Not Rebooted !");
                            }
                            else
                                anqDebug("=> This Line Should Be Never Printed Out !");
                            anqDebug("---------------------------------------------------------------------");
                            proc->execute("cat " _DefaultLastQProcessStandardOutputFilePath);
                            anqDebug("---------------------------------------------------------------------");)
                            proc->close();
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
        anqDebug("=> Calculate Time To Next Check Point ...");
        int BreakInterval = 86400000/PollingRate;
        int TimePoint = CheckPoint.msecsSinceStartOfDay() - BreakInterval;
        int SleepTime = 0;
        do
        {
            TimePoint += BreakInterval;
            SleepTime = TimePoint -QTime::currentTime().msecsSinceStartOfDay();
        } while (SleepTime < 0);
        anqDebug("   " _VarView(SleepTime) " milisecond");
        anqDebug("=> GET SLEEP UNTIL " << QTime::fromMSecsSinceStartOfDay(TimePoint).toString("hh:mm:ss"));
        GoSleep(SleepTime);
        anDebugCode(anqMsgCaptureToFile(_DefaultLastUpdateCycleStandardOutputFilePath);)
    }
    return a.exec();
}
