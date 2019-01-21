#include "layout.h"

void Layout::setStartLayout(QLineEdit* pass, QTextEdit* m_ptxtInfo, QPushButton* pcmd) {
    layout->addWidget(new QLabel("<center><H1>Client</H1></center>"));
    layout->addWidget(info);
    layout->addWidget(pass);
    layout->addWidget(m_ptxtInfo);
    layout->addWidget(pcmd);
}

void Layout::removeStartLayout(QLineEdit* pass, QTextEdit* m_ptxtInfo, QPushButton* pcmd) {
    layout->removeWidget(info);
    layout->removeWidget(pass);
    layout->removeWidget(m_ptxtInfo);
    layout->removeWidget(pcmd);
}

QListView* Layout::setListLayout(QList<QVariant>* names, QString path,
                                 QPushButton* _crFolder, QPushButton* _crFile) {
    listView = new QListView;
    QStandardItemModel* model = new QStandardItemModel(names->count(), 1);

    listView->setMinimumHeight(300);
    listView->setMinimumWidth(500);

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setViewMode(QListView::IconMode);

//    QStringList* stringList = new QStringList;
    int i = 0;
    for (QList<QVariant>::iterator it = names->begin(); it < names->end(); it++) {
        QModelIndex index = model->index(i, 0);
        QMap<QString, QVariant> item = (*it).toMap();
        QString str = item.firstKey();
        model->setData(index, str, Qt::DisplayRole);
//        qDebug() << item.first().toString();
        if (item.first().toString() == "folder") {
            QDir dir;
//            qDebug() <<dir.currentPath() + "/pic/folder.png";
            model->setData(index, QIcon(dir.currentPath() + "/pic/folder.png"), Qt::DecorationRole);
        } else {
            QDir dir;
            model->setData(index, QIcon(dir.currentPath() + "/pic/file.png"), Qt::DecorationRole);
        }
//        QListWidgetItem* lstItem = new QListWidgetItem(item, )
//        stringList->append(item.firstKey());
        i++;
    }
//    model->setStringList(*stringList);
    listView->setModel(model);
    info->clear();
    info = new QLabel("<center>" + path + "</center>");
    layout->addWidget(info);
    layout->addWidget(listView);
    crFolder = _crFolder;
    connect(crFolder, SIGNAL(clicked()), SLOT(slotInputDialog()));
    crFile = _crFile;
    connect(crFile, SIGNAL(clicked()), SLOT(slotInputDialog()));
    layout->addWidget(crFolder);
    layout->addWidget(crFile);
    return listView;
}

void Layout::removeListLayout() {
    layout->removeWidget(info);
    layout->removeWidget(listView);
    layout->removeWidget(crFolder);
    layout->removeWidget(crFile);
}


