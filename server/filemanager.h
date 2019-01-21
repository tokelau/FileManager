#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QFileInfo>
#include <QTextStream>

class FileManager
{
private:
    QDir* dir;
public:
    FileManager() {
        dir = new QDir;
    }
    bool init() {
        if (makeDir("/Nastya'sMagic/root")) {
            dir->setCurrent(dir->currentPath() + "/Nastya'sMagic/root");
            return true;
        }
        return false;
    }
    bool makeDir(const QString dirName) {
        if (dir->exists(dirName)) {
            return false;
        }
        if (dir->mkpath(dir->currentPath() + "/" + dirName)) {
            return true;
        }
        return false;
    }
    bool makeFile(const QString fileName) {
        QFile file(dir->currentPath() + "/" + fileName);
        if (file.exists()) {
            return false;
        }
        if (file.open(QIODevice::ReadWrite)) {
            file.close();
            return true;
        }
        return false;
    }
    bool remove(const QString name) {
        QFileInfo path(dir->currentPath(), name);
        if (path.isFile()) {
            dir->remove(name);
            return true;
        } else if (path.isDir()) {
            QString str = dir->currentPath();
            dir->setCurrent(dir->currentPath() + "/" + name);
            dir->removeRecursively();
            dir->setCurrent(str);
            dir->rmdir(name);
            return true;
        }
        return false;
    }
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
        if (!file.exists()) { return NULL; }
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
    bool saveFile(QString name, QString content) {
        QFile file(dir->currentPath() + "/" + name);
        if(file.open(QFile::WriteOnly)) {
            QByteArray arr;
            arr.insert(0, content);
            file.write(arr);
            file.close();
            return true;
        }
        return false;
    }
    QList<QVariant> getSubDirs() {
        dir->refresh();
        QList<QFileInfo> info = dir->entryInfoList();

        QList<QVariant> subs;
        info.pop_front();
        for(QList<QFileInfo>::iterator i = info.begin(); i < info.end(); i++) {
            QMap<QString, QVariant> sub;
            QString type = (*i).isFile() ? "file" : "folder";
            if ((*i).exists()) {
                sub.insert((*i).fileName(), type);
                subs.append(sub);
            }
        }
        return subs;
    }
    ~FileManager() {
        delete dir;
    }
 };

#endif // FILEMANAGER_H
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QWidget>

class FileManager : public QWidget
{
    Q_OBJECT

public:
    FileManager(QWidget *parent = 0);
    ~FileManager();
};

#endif // FILEMANAGER_H
