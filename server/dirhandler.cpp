#include "dirhandler.h"

DirHandler::DirHandler()
{
//    currentPath = QDir::homePath() + "/Nastya'sMagic/root";
    dir = new QDir;
}

bool DirHandler::init() {
    if (makeDir("/Nastya'sMagic/root")) {
        dir->setCurrent(dir->currentPath() + "/Nastya'sMagic/root");
        return true;
    }
    return false;
}

bool DirHandler::makeDir(const QString dirName) {
    if (dir->exists(dirName)) {
//        qDebug() << "folder exists";
        return false;
    }
    if (dir->mkpath(dir->currentPath() + "/" + dirName)) {
//        dir->setCurrent(dir->currentPath() + "/" + dirName);
        return true;
    } else {
        return false;
    }
}

bool DirHandler::makeFile(const QString fileName) {
    QFile file(dir->currentPath() + "/" + fileName);
    if (file.exists()) {
        return false;
    }
    if (file.open(QIODevice::ReadWrite)) {
        return true;
    } else {
        return false;
    }
}

bool DirHandler::rm(const QString name) {
    //если это папка
    QFileInfo path(dir->currentPath(), name);
    if (path.isFile()) {
        dir->remove(name);
//        qDebug() <<"file";
    } else if (path.isDir()) {
//        dir->setPath(dir->currentPath() + "/" + name);
        QString str = dir->currentPath();
        dir->setCurrent(dir->currentPath() + "/" + name);
        dir->removeRecursively();
//        dir->cdUp();
//        qDebug() << dir->currentPath();
//        qDebug() << dir->path();
        dir->setCurrent(str);
        dir->rmdir(name);
        return true;
    }
    return false;
}

QList<QVariant> DirHandler::getSubDirs() {
    dir->refresh();
    QList<QFileInfo> info = dir->entryInfoList();
    QList<QVariant> subs;
    info.pop_front();
//    info.pop_front();
    for(QList<QFileInfo>::iterator i = info.begin(); i < info.end(); i++) {
        QMap<QString, QVariant> sub;
        QString type = (*i).isFile() ? "file" : "folder";
        if ((*i).exists()) {
            sub.insert((*i).fileName(), type);
            subs.append(sub);
        }

    }
//    qDebug() << subs;
    return subs;
}
