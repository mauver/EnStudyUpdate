#include "filemanager.h"

#include <QSettings>
#include "userdefs.h"

namespace FileManager{
    const static QString rootPath = ENSTUDY_PATH;
    const static QString settingPath = "settings.ini";
    static QSettings settings(QString(rootPath).append("\\").append(settingPath), QSettings::IniFormat);

    void iniWrite(QString key, QString value){
        settings.setValue(key, value);
        settings.sync();
    }

    QString iniRead(QString key){
        return settings.value(key).toString();
    }
}
