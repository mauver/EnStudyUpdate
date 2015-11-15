#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

#include "webmanager.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

private slots:
    void on_notify_http_finished(QString dstPath);
    void on_notify_http_progress(QString dstPath, qint64 readBytes, qint64 totalBytes);

private:
    Ui::UpdateDialog *ui;

    QWebManager *manager;

    enum requestStatus {
        none = 0x00,
        ver,
        notify,
        down_list,
        update,
    };

    QString currentVersion;
    QString latestVersion;
    QString lastUpdateFile;
    requestStatus currentRequest;

    bool isInstalled;

    bool checkInstall();
};

#endif // UPDATEDIALOG_H
