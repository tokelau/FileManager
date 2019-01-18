#include "client.h"

Client::Client(const QString& strHost, quint16 nPort, QWidget* pwt/* = 0*/) :
    QWidget (pwt), m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotCheckAuth()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    
    m_ptxtInfo = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    m_ptxtInfo->setReadOnly(true);
    pass = new QLineEdit;
    
    pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
//    connect(m_ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));
    connect(pass, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));

    menu = new QMenu;
    menu->addAction("Удалить");
    menu->addAction ("Перейти");
    connect(menu, SIGNAL(triggered(QAction*)), SLOT(slotActivated(QAction*)));

    //layout setup
    layout = new QVBoxLayout;
    layoutHandler = new Layout(layout);
    layoutHandler->setStartLayout(pass, m_ptxtInfo, pcmd);

    setLayout(layout);

    itemSelectionModel = new QItemSelectionModel;
    listView = new QListView;
    model = new QStringListModel;
}

void Client::slotReadyRead() {
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_11);
    for(;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        in >> mFromServer;
//        qDebug() << mFromServer;

        if (!mFromServer["subfolders"].toList().isEmpty()) {
            QList<QVariant>* names = new QList<QVariant>;
            names->append(mFromServer["subfolders"].toList());
            layoutHandler->removeListLayout();
            QPushButton* crFolder = new QPushButton("&Добавить папку");
            connect(crFolder, SIGNAL(clicked()), SLOT(slotCreateFolder()));
            QPushButton* crFile = new QPushButton("&Добавить файл");
            connect(crFile, SIGNAL(clicked()), SLOT(slotCreateFile()));
            listView = layoutHandler->setListLayout(names, mFromServer["path"].toString(), crFolder, crFile);
        }
        QString title = mFromServer["title"].toString();
        if (!title.isEmpty()) {
//            qDebug() << title;
            QPushButton* send = new QPushButton("&Сохранить");
            connect(send, SIGNAL(clicked()), SLOT(slotSaveContent()));
//            connect(close, SIGNAL(clicked()), SLOT(slotClose(w)));
            layoutHandler->setFileLayout(title, mFromServer["filecontent"].toString(), send);
//            mToServer.clear();
        }

        m_ptxtInfo->append(mFromServer["time"].toString() + " " + mFromServer["message"].toString());
        m_nNextBlockSize = 0;
    }
}

void Client::slotError(QAbstractSocket::SocketError err) {
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                        "The host was not found." :
                                        err == QAbstractSocket::RemoteHostClosedError ?
                                            "The remote host is closed." :
                                            err == QAbstractSocket::ConnectionRefusedError ?
                                                "The connection refused" :
                                                QString(m_pTcpSocket->errorString()));

    m_ptxtInfo->append(strError);
}

void Client::slotSendToServer() {
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);

    mToServer.insert("auth", pass->text());
    mToServer.insert("message", m_ptxtInput->text());
    mToServer.insert("time", QTime::currentTime());

    out << quint16(0) << mToServer;

//    qDebug() << mToServer;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
    mToServer.clear();
}

void Client::slotCheckAuth() {
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_11);
    for(;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        in >> mFromServer;

        if (mFromServer["auth"].toInt() == -2) {
            m_ptxtInfo->append("Wrong password.");
        } else if (mFromServer["auth"].toInt() == 1) {
            disconnect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(slotCheckAuth()));
            connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

            layoutHandler->removeStartLayout(pass, m_ptxtInfo, pcmd);

            names = new QList<QVariant>;
            names->append(mFromServer["subfolders"].toList());
            QPushButton* crFolder = new QPushButton("&Добавить папку");
            connect(crFolder, SIGNAL(clicked()), SLOT(slotCreateFolder()));
            QPushButton* crFile = new QPushButton("&Добавить файл");
            connect(crFile, SIGNAL(clicked()), SLOT(slotCreateFile()));
            listView = layoutHandler->setListLayout(names, mFromServer["path"].toString(), crFolder, crFile);
            layoutHandler->setStyle(this);
        }

        pass->clear();
        m_ptxtInfo->append(mFromServer["time"].toString() + " " + mFromServer["message"].toString());
        m_nNextBlockSize = 0;
    }
}

void Client::slotConnected() {
    m_ptxtInfo->append("Received the connected() signal.");
}
