#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RecvIndex=0;SendIndex=0;
    PollingFlag=false;
    timesend= new QTimer();
    timesend->setInterval(1000);
    connect(timesend,SIGNAL(timeout()),this, SLOT(Polling()));
    connect(&serial, &QSerialPort::readyRead, this, &MainWindow::serialPort_readyRead);
    ui->sendButton->setEnabled(false);
    ui->baudrateBox->setCurrentIndex(3);

}

MainWindow::~MainWindow()
{
    delete ui;

}
void MainWindow::serialPort_readyRead()                             //接收信息
{
    QByteArray buffer = serial.readAll();
    QString recv = ui->recvTextEdit->toPlainText();
    QString currecv=buffer.toHex().toUpper();
    if(currecv[0]=='E'&&currecv[1]=='B') recv += ("\n"+currecv);
    else recv += currecv;
    ui->recvTextEdit->clear();
    ui->recvTextEdit->append(recv);
}
void MainWindow::DisPlay_Temperatures()//展示温度数据
{

}
void MainWindow::DisPlay_Pollings()//展示轮询数据
{

}
void MainWindow::on_PollingButton_clicked(){
    int value=ui->pollingTime->text().toInt();
    if(value>=100){
        timesend->start(value);
    }
}
void MainWindow::on_OFFPolling_clicked()
{
        timesend->stop();
}
void MainWindow::Polling()                              //轮询命令
{
    QString str;
    str="EB90F000";
    QByteArray data;
    StringToHex(str,data);
    serial.write(data);SendIndex++;
    ui->SendNumber->display(SendIndex);

}
void MainWindow::on_searchButton_clicked()                          //查找串口
{
    ui->portNameBox->clear();

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->portNameBox->addItem(info.portName());
    }
}

void MainWindow::on_openButton_clicked()                            //开关串口
{
    if(ui->openButton->text()==QString("打开串口"))
    {        
        serial.setPortName(ui->portNameBox->currentText());         //串口名
        serial.setBaudRate(ui->baudrateBox->currentText().toInt()); //波特率
        switch(ui->dataBitsBox->currentIndex())                     //数据位数
        {
        case 8: serial.setDataBits(QSerialPort::Data8); break;
        default: break;
        }
        switch(ui->ParityBox->currentIndex())                       //校验
        {
        case 0: serial.setParity(QSerialPort::NoParity); break;
        case 1: serial.setParity(QSerialPort::OddParity);break;
        case 2: serial.setParity(QSerialPort::EvenParity);break;
        default: break;
        }
        switch(ui->stopBitsBox->currentIndex())                     //停止位
        {
        case 1: serial.setStopBits(QSerialPort::OneStop); break;
        case 2: serial.setStopBits(QSerialPort::TwoStop); break;
        default: break;
        }
        serial.setFlowControl(QSerialPort::NoFlowControl);          //设置流控制
        if(!serial.open(QIODevice::ReadWrite))                      //打开串口
        {
            QMessageBox::about(NULL, "提示", "无法打开串口！");
            return;
        }
        ui->portNameBox->setEnabled(false);
        ui->baudrateBox->setEnabled(false);
        ui->dataBitsBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->stopBitsBox->setEnabled(false);
        ui->openButton->setText(QString("关闭串口"));
        ui->sendButton->setEnabled(true);
    }
    else                                                            //关闭串口
    {
        serial.close();
        ui->portNameBox->setEnabled(true);
        ui->baudrateBox->setEnabled(true);
        ui->dataBitsBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->stopBitsBox->setEnabled(true);
        ui->openButton->setText(QString("打开串口"));
        timesend->stop();
        ui->sendButton->setEnabled(false);
    }
}

void MainWindow::on_sendButton_clicked()                            //获取发送区界面上的数据,并转换成HEX格式,发送出去
{
    QByteArray data;
    QString str= ui->sendTextEdit->toPlainText();
    StringToHex(str,data);
    serial.write(data);
    SendIndex++;
    ui->SendNumber->display(SendIndex);
}
void MainWindow::on_clearAllButton_clicked()                        //清空两区
{
    ui->sendTextEdit->clear();
    ui->recvTextEdit->clear();
}

void MainWindow::on_clearRecvButton_clicked()                       //清空接收区
{
      ui->recvTextEdit->clear();
}
void MainWindow::on_clearSendButton_clicked()                       //清空发送区
{
    ui->sendTextEdit->clear();
}
void MainWindow::on_elecOpenButton_clicked()                        //放电命令
{
    if(ui->elecOpenButton->text()==QString("开启放电"))
    {
        QString str;QByteArray data;
        str="EB9011AA";
        StringToHex(str,data);
        serial.write(data);
        SendIndex++;
        ui->SendNumber->display(SendIndex);
        ui->elecOpenButton->setText(QString("禁止放电"));
    }
    else{
        QString str;
        str="EB901155";
        QByteArray data;
        StringToHex(str,data);

        serial.write(data);
        SendIndex++;
        ui->SendNumber->display(SendIndex);
        ui->elecOpenButton->setText(QString("开启放电"));
    }
}
void MainWindow::on_tempButton_clicked()                            //温度采集命令
{
    QString str;
    str="EB90E100";
    QByteArray data;
    StringToHex(str,data);

    serial.write(data);
    SendIndex++;
    ui->SendNumber->display(SendIndex);
}
void MainWindow::on_ZoneButton_clicked()                            //加热带控制命令
{
    unsigned int pa=0;
    unsigned int pb=0;
    unsigned int pts[8];
    QString str;
    str="EB90B300";
    pts[0]=(ui->Zone_0->isChecked())?1:0;
    pts[1]=(ui->Zone_1->isChecked())?1:0;
    pts[2]=(ui->Zone_2->isChecked())?1:0;
    pts[3]=(ui->Zone_3->isChecked())?1:0;
    pts[4]=(ui->Zone_4->isChecked())?1:0;
    pts[5]=(ui->Zone_5->isChecked())?1:0;
    pts[6]=(ui->Zone_6->isChecked())?1:0;
    pts[7]=(ui->Zone_7->isChecked())?1:0;
    for (int i=0;i<4;i++) {
        pa = pts[i]+pa*2;
    }
    for (int i=4;i<8;i++) {
        pb = pts[i]+pb*2;
    }
    str[6]=(pa>=10)?(pa-10+'A'):(pa+'0');
    str[7]=(pb>=10)?(pb-10+'A'):(pb+'0');
    QByteArray data;
    StringToHex(str,data);

    serial.write(data);
    SendIndex++;
    ui->SendNumber->display(SendIndex);
}

void MainWindow::StringToHex(QString str, QByteArray &senddata)     //转换为16进制
{
    int hexdata,lowhexdata,hexdatalen = 0,len = str.length();char lstr,hstr;
    senddata.resize(len/2);
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ') {
            i++; continue;
        }
        i++;
        if(i >= len) break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))  break;
        else  hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

char MainWindow::ConvertHexChar(char ch)                            //转换为16进制
{
    if((ch >= '0') && (ch <= '9'))
         return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
         return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
         return ch-'a'+10;
    else return (-1);
}
