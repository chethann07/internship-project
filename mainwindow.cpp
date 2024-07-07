#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TCP_Server = new QTcpServer();
    if(TCP_Server->listen(QHostAddress::LocalHost,8080))
    {
        connect(TCP_Server,SIGNAL(newConnection()),this,SLOT(newConnection()));
        QMessageBox::information(this,"Simple chat app!","Server Started!");
    }
    else
    {
        QMessageBox::information(this,"Simple chat app!","Server Start Fail!");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnection()
{
    while (TCP_Server->hasPendingConnections()) {
        Add_New_Client_Connection(TCP_Server->nextPendingConnection());
    }
}

void MainWindow::Read_Data_from_Socket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
    QByteArray Message_From_Server = socket->readAll();
    QString Message = "Client: " + QString::number(socket->socketDescriptor())+ "::" +QString::fromStdString(Message_From_Server.toStdString());
    ui->textEdit_Client_Message->append(Message);
}

void MainWindow::Add_New_Client_Connection(QTcpSocket *socket)
{
    Connection_List.append(socket);
    connect(socket,SIGNAL(readyRead()),this,SLOT(Read_Data_from_Socket()));
    ui->comboBox_Client_List->addItem(QString::number(socket->socketDescriptor()));
    QString Client = "Client: " + QString::number(socket->socketDescriptor()) + "Connected With Server.";
    ui->textEdit_Client_Message->append(Client);
}


void MainWindow::on_pushButton_Send_clicked()
{
    QString Message_For_Client = ui->lineEdit_MessageForClient->text();
    QString Receiver = ui->comboBox_Client_List->currentText();
    if(ui->comboBox_SendMessage->currentText() == "All")
    {
        foreach(QTcpSocket *socket,Connection_List)
        {
            socket->write(Message_For_Client.toStdString().c_str());
        }
    }
    else
    {
        foreach(QTcpSocket *socket,Connection_List)
        {
            if(socket->socketDescriptor() == Receiver.toLongLong())
            {
                socket->write(Message_For_Client.toStdString().c_str());
            }
        }
    }
}
