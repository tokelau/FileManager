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
    QStringListModel* model = new QStringListModel;

    listView->setMinimumHeight(300);
    listView->setMinimumWidth(500);

    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList* stringList = new QStringList;
    for (QList<QVariant>::iterator it = names->begin(); it < names->end(); it++) {
        QMap<QString, QVariant> item = (*it).toMap();
        stringList->append(item.firstKey());
    }
    model->setStringList(*stringList);
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


