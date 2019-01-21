#include "server.h"

Server::Server(quint16 nPort, QString pass, QWidget* pwt /*= 0*/) : QWidget (pwt), m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer;
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:"
                              + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    this->pass = pass;

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

    fm = new FileManager;
    if (fm->init()) {
        m_ptxt->append("Server: Root directory successfully created or refreshed.");
    } else {
        m_ptxt->append("Server: Wrong. Can't create root directory. Check admin rights.");
    }
}

void Server::slotNewConnection() {
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

//    mToClient.insert("info", "Server Response: Connected!");
    sendToClient(pClientSocket);
}

void Server::slotReadClient() {
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);

    in.setVersion(QDataStream::Qt_5_11);
    for(;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if(pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        in >> mFromClient;


        if (mFromClient["auth"].toString() != pass) {
            mToClient.insert("auth", -2);
        } else {
//            qDebug() << mFromClient["auth"].toString();
            mToClient.insert("auth", 1);
            mToClient.insert("subfolders", fm->getSubDirs());
            mToClient.insert("path", fm->getDir());
        }

        QString strMessage;
        QString message;

        if (!mFromClient["auth"].toString().isEmpty()) {
            strMessage = mFromClient["time"].toString() + " Recieved password: " + mFromClient["auth"].toString();
        }
        if (!mFromClient["message"].toString().isEmpty()) {
            strMessage = mFromClient["time"].toString() + " Recieved password: " + mFromClient["message"].toString();
        }
        if (!mFromClient["action"].toString().isEmpty()) {
            if (mFromClient["action"].toString() == "remove") {
                if (fm->remove(mFromClient["target"].toString())) {
                    mToClient.insert("subfolders", fm->getSubDirs());
                    mToClient.insert("path", fm->getDir());
                    message = "Successfully removed.";
                }
                message = "Successfully removed.";
            }
            if (mFromClient["action"].toString() == "goover") {
                QString target = mFromClient["target"].toString();
                QString type = fm->getType(target);
                if (type == NULL) {
                    message = "Path does not exists.";
//                    mToClient.insert("message", "Path does not exists.");
                }
                if (type == "folder") {
                    fm->goover(mFromClient["target"].toString());
                    mToClient.insert("subfolders", fm->getSubDirs());
                    mToClient.insert("path", fm->getDir());
                }
                if (type == "file") {
                    mToClient.insert("title", target);
                    mToClient.insert("filecontent", fm->getFileContent(target));
                }
            }
            if (mFromClient["action"].toString() == "save") {
                if (fm->saveFile(mFromClient["title"].toString(), mFromClient["filecontent"].toString())) {
                    message = "Successfully saved.";
                }
//                mToClient.insert("message", "Successfully saved.");
                message = "Failed saving";
            }
            if (mFromClient["action"].toString() == "create") {
                if (!mFromClient["folder"].toString().isEmpty()) {
                    bool res = fm->makeDir(mFromClient["folder"].toString());
                    QString message = res ? "Successfully create folder." : "Problem: "
                                                                            "The folder already exists or another.";
                    mToClient.insert("path", fm->getDir());
                }
                if (!mFromClient["file"].toString().isEmpty()) {

                    bool res = fm->makeFile(mFromClient["file"].toString() + ".txt");
                    QString message = res ? "Successfully create file." : "Problem: "
                                                                            "The file already exists or another.";
                    qDebug() << message;
                }
                mToClient.insert("subfolders", fm->getSubDirs());
            }
            strMessage = mFromClient["time"].toString() + " Recieved action: " + mFromClient["action"].toString();
        }
        m_ptxt->append(strMessage);
        mToClient.insert("message", message);

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket);
        mToClient.clear();
//        sendToClient(pClientSocket, "Server response: Welcome!");
    }
}

void Server::sendToClient(QTcpSocket* pSocket) {
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);

    mToClient.insert("time", QTime::currentTime());

    out << quint16(0) << mToClient;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
