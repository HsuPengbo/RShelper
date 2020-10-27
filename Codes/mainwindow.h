#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSystemTrayIcon>
#include <QRadioButton>
#include <QTimer>
#include <QMessageBox>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
private slots:
    void serialPort_readyRead();//接收数据
    void DisPlay_Temperatures();//展示温度数据
    void DisPlay_Pollings();//展示轮询数据
    void on_sendButton_clicked();//发送数据
    void on_PollingButton_clicked();//开启轮询
    void on_OFFPolling_clicked();//关闭轮询
    void Polling();
    void on_searchButton_clicked();//搜索串口
    void on_openButton_clicked();//打开串口
    void on_elecOpenButton_clicked();//开启放电
    void on_tempButton_clicked();//温度采集
    void on_ZoneButton_clicked();//加热带控制命令
    void on_clearAllButton_clicked();//清空两区
    void on_clearRecvButton_clicked();//清空接收区
    void on_clearSendButton_clicked();//清空发送区
private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QTimer *timesend;
    int RecvIndex;
    int SendIndex;
    bool PollingFlag;
};

#endif // MAINWINDOW_H
