#ifndef DIRHANDLER_H
#define DIRHANDLER_H

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QFileInfo>
#include <QTextStream>

class DirHandler
{
private:
    QDir* dir;
public:
    DirHandler();
    bool init();
    bool makeDir(const QString dirName);
    bool makeFile(const QString fileName);
//    bool chDir(const QString dirName);
    bool rm(const QString name);
    QString getDir() {
        int pos = dir->currentPath().indexOf("root");
        return dir->currentPath().mid(pos);
    }
    QString goover(QString name) {
        QString goover;
        if (name == "..") {
            int pos = dir->currentPath().lastIndexOf("/");
            QString path = dir->currentPath().left(pos);
            qDebug() << path;
            if (path.contains("Nastya'sMagic/root")) {
                dir->setCurrent(path);
                return path;
            } else {
                return dir->currentPath();
            }
        } else {
            QFileInfo file (dir->currentPath() + '/' + name);
            if (!file.exists()) {
                return NULL;
            }
            if (file.isDir()) {
                dir->setCurrent(dir->currentPath() + '/' + name);
                return dir->currentPath();
            }
        }
        return NULL;
    }
    QString getType(QString name) {
        QFileInfo file (dir->currentPath() + '/' + name);
        if (!file.exists()) {
            return NULL;
        }
        if (file.isDir()) { return "folder"; }
        if (file.isFile()){ return "file"; }
        return NULL;
    }
    QString getFileContent(QString name) {
        QFile file(dir->currentPath() + "/" + name);
        file.open(QFile::ReadOnly);
        QString content = file.readAll();
        file.close();
        return content;
    }
    void saveFile(QString name, QString content) {
        QFile file(dir->currentPath() + "/" + name);
        file.open(QFile::WriteOnly);
        QByteArray arr;
        arr.insert(0, content);
        file.write(arr);
        file.close();
    }
    QList<QVariant> getSubDirs();
};

#endif // DIRHANDLER_H
