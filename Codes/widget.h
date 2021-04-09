#ifndef WIDGET_H
#define WIDGET_H

#include <header.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //处理接收指令
    void display_Temp(QString bag);         //展示遥测参数
    void display_Polling(QString bag);      //展示轮询参数
    void display_SendEdit(QString bag);     //更新发送区
    void display_RecvEdit(QString bag);     //更新接收区
    void write_SendNote(QString bag);                  //更新发送日志
    void write_RecvNote(QString bag);                  //更新接收日志
    void test();
    int hex2dec(QString  str);
    QString hex2bin(QString  str);
public slots:

private slots:

    void on_searchButton_clicked();         //搜索可用端口
    void on_FileButton_clicked();           //选择保存记录的文件夹路径
    void on_openButton_clicked();           //串口打开与关闭
    void on_recv_timer();                   //接收数据定时器
    void recv_data();                       //接收数据
    //发送指令
    void send_polling();                    //发送轮询指令
    void on_elecOpenButton_clicked();       //开启放电指令
    void on_elecStopButton_clicked();       //关闭放电指令
    void on_tempButton_clicked();           //采集温度指令
    void on_ZoneButton_clicked();           //加热带控制指令


    void on_clearRecvButton_clicked();      //清空接收区

    void on_clearSendButton_clicked();      //清空发送区

    void on_clearAllButton_clicked();       //全部清空

private:
    Ui::Widget *ui;         //UI界面
    QSerialPort serial;     //使用端口
    QTimer *timesend;       //轮询计时器
    QTimer *timerecv;       //接收计时器
    int RecvIndex;          //接收计数
    int SendIndex;          //发送计数
    QString SaveDirPath;    //文件夹路径名
};
#endif // WIDGET_H
