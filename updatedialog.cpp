#include "updatedialog.h"
#include "ui_updatedialog.h"

#include "userdefs.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QProcess>

#include "filemanager.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog),
    manager(new QWebManager()),
    currentVersion(""),
    latestVersion(""),
    isInstalled(false),
    lastUpdateFile("")
{
    ui->setupUi(this);

    connect(manager, SIGNAL(notifyFinished(QString)), this, SLOT(on_notify_http_finished(QString)));
    connect(manager, SIGNAL(notifyProgress(QString,qint64,qint64)), this, SLOT(on_notify_http_progress(QString,qint64, qint64)));

    isInstalled = checkInstall();

    // getting the recent version
    currentRequest = ver;
    manager->request(ENSTUDY_VER, false);

    setWindowFlags(Qt::WindowMinimizeButtonHint);
}

UpdateDialog::~UpdateDialog()
{
    if( manager != NULL )
        delete manager;

    delete ui;
}

bool UpdateDialog::checkInstall(){
    if( !QDir(ENSTUDY_PATH).exists() )
        QDir().mkdir(ENSTUDY_PATH);

    return QFile(ENSTUDY_EXE).exists();
}

void UpdateDialog::on_notify_http_finished(QString dstPath){
    if( currentRequest == ver ){
        latestVersion = manager->getResultString();
        FileManager::iniWrite("version", latestVersion);
        QString notifyURL = QString(ENSTUDY_URL).append("/").append(latestVersion).append(ENSTUDY_NOTIFY);

        this->setWindowTitle(QString("EnStudy Downloading...(v.%1)").arg(latestVersion));

        if( !isInstalled ){
            this->show();
            currentRequest = notify;
            manager->request(notifyURL, false);
        }
        else{
            currentVersion = FileManager::iniRead("version");

            if( latestVersion.contains("404 Not Found") ||
                latestVersion.compare(currentVersion) == 0 ){
                // no internet connection or same version
                // just running EnStudy.exe
                QProcess process;
                process.startDetached(ENSTUDY_EXE);
                qApp->quit();
            }
            else{
                this->show();
                this->setWindowTitle(QString("EnStudy Updating...(v.%1 -> v.%2)").arg(currentVersion, latestVersion));
                // download new version
                currentRequest = notify;
                manager->request(notifyURL, false);
            }
        }
    }
    else if( currentRequest == notify ){
        ui->textEditNotify->setText(manager->getResultString());

        QString verURL = QString(ENSTUDY_URL).append("/").append(latestVersion).append(ENSTUDY_LIST);
        currentRequest = down_list;
        manager->request(verURL, false);
    }
    else if( currentRequest == down_list ){
        QString listRaw = manager->getResultString();
        QStringList list = listRaw.split("\n");

        ui->progressBarUpdate->setMaximum(list.size());
        ui->progressBarUpdate->setValue(0);

        foreach(QString file, list){
            QStringList subDir = file.split("\\\\");
            // folder create
            QString folderPath = ENSTUDY_PATH;
            QString fileUrlPath = "";
            for(int i=0; i<subDir.size()-1; ++i){
                QDir().mkdir(folderPath.append("\\").append(subDir[i]));
                fileUrlPath.append("/").append(subDir[i]);
            }
            // request file download
            currentRequest = update;
            manager->requestPush(QString(ENSTUDY_URL).append("/").append(latestVersion).append(fileUrlPath)
                                 .append("/").append(subDir.last()),
                                 true,
                                 folderPath.append("\\").append(subDir.last()));
            lastUpdateFile = folderPath;
        }
        manager->requestRun();
    }
    else if( currentRequest == update ){
        if( dstPath.compare(lastUpdateFile) == 0){
            QProcess process;
            process.startDetached(ENSTUDY_EXE);
            qApp->quit();
        }
        else{
            ui->progressBarUpdate->setValue(ui->progressBarUpdate->value()+1);
        }
    }
}

void UpdateDialog::on_notify_http_progress(QString dstPath, qint64 readBytes, qint64 totalBytes){
    ui->progressBarUpdate_2->setMaximum(totalBytes);
    ui->progressBarUpdate_2->setValue(readBytes);

    QFileInfo info(dstPath);
    ui->labelUpdate->setText(QString("Downloading (<b>%1</b>) [%2/%3]").arg(info.fileName(),
       QString::number(ui->progressBarUpdate->value()), QString::number(ui->progressBarUpdate->maximum())));
}
