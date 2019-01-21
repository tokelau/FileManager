#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QTextEdit>
#include <QLabel>

class FileEditor : public QWidget
{
private:
    QTextEdit* te;
    QString title;
protected:
    void closeEvent(QCloseEvent* ce) {
        emit close();
    }
public:
    FileEditor(QString name, QString content, QPushButton* send, QWidget* widget = 0) : QWidget(widget) {
        QVBoxLayout* l = new QVBoxLayout;
        l->addWidget(new QLabel("<H1>" + name + "</H1>"));
        te = new QTextEdit;
        te->setText(content);
        l->addWidget(te);
        te->setMinimumHeight(480);
        te->setMinimumWidth(600);
        te->moveCursor(QTextCursor::End);
        title = name;

        l->addWidget(send);
//        l->addWidget(close);
        setLayout(l);
        show();
        qDebug() << "start.";
    }
};

#endif // FILEEDITOR_H
