#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    timerecv=new QTimer();
    connect(&serial,SIGNAL(readyRead()),this,SLOT(on_recv_timer()));
    connect(timerecv, SIGNAL(timeout()), this, SLOT(recv_data()));
    timesend= new QTimer();
    timesend->setInterval(ui->pollingTime->text().toInt());
    connect(timesend,SIGNAL(timeout()),this, SLOT(send_polling()));
    SendIndex=0;
    RecvIndex=0;
    ui->ZoneButton->setEnabled(false);
    ui->tempButton->setEnabled(false);
    ui->elecOpenButton->setEnabled(false);
    ui->elecStopButton->setEnabled(false);
    ui->baudrateBox->setCurrentIndex(5);
    //test();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_searchButton_clicked()                                  //搜索可用端口
{
    ui->portNameBox->clear();

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->portNameBox->addItem(info.portName());
    }
}

void Widget::on_FileButton_clicked()                                    //选择保存记录的文件夹路径
{
    SaveDirPath = QFileDialog::getExistingDirectory(
                   this, "选择记录文件夹",
                    "/");

        if (SaveDirPath.isEmpty())
        {
            return;
        }
        else
        {
            qDebug() << "srcDirPath=" << SaveDirPath;
            SaveDirPath += "/";
        }
}

void Widget::on_openButton_clicked()                                        //串口打开与关闭
{
    if(ui->openButton->text()==QString("打开串口")){
        serial.setPortName(ui->portNameBox->currentText());         //串口名
        serial.setBaudRate(ui->baudrateBox->currentText().toInt()); //波特率
        serial.setDataBits(QSerialPort::Data8);                     //数据位
        serial.setParity(QSerialPort::NoParity);                    //奇偶校验
        serial.setStopBits(QSerialPort::OneStop);                   //停止位
        serial.setFlowControl(QSerialPort::NoFlowControl);          //设置流控制
        if(!serial.open(QIODevice::ReadWrite))                      //打开串口
        {
            QMessageBox::about(NULL, "提示", "无法打开串口！");
            return;
        }
        else{

            ui->openButton->setText(QString("关闭串口"));
            timesend->start();//开启轮询
            if(timesend->isActive()){
                qDebug()<<"polling start!("<<ui->pollingTime->text().toInt()<<"ms)";
            }
            ui->ZoneButton->setEnabled(true);
            ui->tempButton->setEnabled(true);
            ui->elecOpenButton->setEnabled(true);
            ui->elecStopButton->setEnabled(true);
        }
    }
    else{
        serial.close();
        SendIndex=0;
        RecvIndex=0;
        ui->openButton->setText(QString("打开串口"));
        timesend->stop();//关闭轮询
        qDebug()<<"polling stop!(";
        ui->ZoneButton->setEnabled(false);
        ui->tempButton->setEnabled(false);
        ui->elecOpenButton->setEnabled(false);
        ui->elecStopButton->setEnabled(false);
    }
}

void Widget::on_recv_timer()//接收数据定时器
{
    timerecv->start(100);
}
void Widget::test()
{
    QString str=QString("EB90111213141516178C1920212223242526273C");//EB90111213141516178C
    QString header=QString("EB90");
    str=str.toUpper();
    int ipos=str.indexOf(header);
    if(ipos==0){
        QStringList recvs= str.split(header);
        for(int i=1;i<recvs.size();++i){
            QString bag=recvs.at(i);
            if(bag.size()==36){
                display_Polling(bag);
                qDebug()<<"recv polling data:"<<bag;
            }
            else if(bag.size()==16){
                display_Temp(bag);
                qDebug()<<"recv temperature data:"<<bag;
            }
        }
        qDebug()<<"recv:"<<str;
    }
}
void Widget::recv_data()                        //接收数据
{
    timerecv->stop();
    QByteArray recv_buffer = serial.readAll();
    QString header=QString("EB9011");

    if(!recv_buffer.isEmpty()){
        QString str=recv_buffer.toHex().data();
        str=str.toUpper();
        int ipos=str.indexOf(header);
        if(ipos==0){
            QStringList recvs= str.split(header);
            for(int i=1;i<recvs.size();++i){
                QString bag=recvs.at(i);

                if(bag.size()==36){
                    display_Polling(bag);
                    qDebug()<<"recv polling data:"<<bag;
                }
                else if(bag.size()==16){
                    display_Temp(bag);
                    qDebug()<<"recv temperature data:"<<bag;
                }
                else{
                    qDebug()<<"recv error data:"<<bag;
                }
            }
            qDebug()<<"recv:"<<str;
        }
    }
    recv_buffer.clear();
}

void Widget::send_polling()                     //发送轮询指令
{
    QString str;
    str="EB90F000F0";
    QByteArray SendBuffer=QByteArray::fromHex(str.toLatin1());// str->hex
    serial.write(SendBuffer);
    display_SendEdit(str);
    write_SendNote(str);
    qDebug() << "send:" << SendBuffer.toHex().toUpper();

}

void Widget::on_elecOpenButton_clicked()        //开启放电指令
{
    QString str=QString("EB9011AABB");
    QByteArray SendBuffer=QByteArray::fromHex(str.toLatin1());// str->hex
    serial.write(SendBuffer);
    display_SendEdit(str);
    write_SendNote(str);
    qDebug() << "send:"<< SendBuffer.toHex().toUpper();
}

void Widget::on_elecStopButton_clicked()        //关闭放电指令
{
    QString str=QString("EB90115566");
    QByteArray SendBuffer=QByteArray::fromHex(str.toLatin1());// str->hex
    serial.write(SendBuffer);
    display_SendEdit(str);
    write_SendNote(str);
    qDebug() << "send:"<< SendBuffer.toHex().toUpper();
}

void Widget::on_tempButton_clicked()            //采集温度指令
{
    QString str=QString("EB90E100E1");

    QByteArray SendBuffer=QByteArray::fromHex(str.toLatin1());// str->hex
    serial.write(SendBuffer);
    display_SendEdit(str);
    write_SendNote(str);
    qDebug() << "send:" << SendBuffer.toHex().toUpper();
}

void Widget::on_ZoneButton_clicked()            //加热带控制指令
{
    QString str=QString("EB90B300");

    unsigned int pa=0;
    unsigned int pb=0;
    unsigned int pts[8];
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
    int cc=(179+pa*16+pb)%256;
    str+= QString("%1").arg(cc,2,16,QLatin1Char('0'));
    str=str.toUpper();
    QByteArray SendBuffer=QByteArray::fromHex(str.toLatin1());// str->hex
    serial.write(SendBuffer);
    display_SendEdit(str);
    write_SendNote(str);
    qDebug() << "send:" << SendBuffer.toHex().toUpper();
}

void Widget::display_Temp(QString bag)          //展示遥测参数
{
    int nums[8]={0};
    int bins[8]={0};
    int cc=0;
    for(int i=1;i<7;++i){
        nums[i]=hex2dec(bag.mid(i*2,2));
        cc+=nums[i];
    }
    cc%=256;
    nums[7]=hex2dec(bag.mid(14,2));
    QString a=QString("1");
    if(cc==nums[7]){
         QString a2=hex2bin(bag.mid(2,2));
        for(int i=0;i<8;++i){
            bins[i]=a2[i]==a?1:0;
        }
         ui->Zone_D7->setChecked(bins[0]);
         ui->Zone_D4->setChecked(bins[3]);
         ui->Zone_D3->setChecked(bins[4]);
         ui->Zone_D2->setChecked(bins[5]);
         ui->Zone_D1->setChecked(bins[6]);
         ui->Zone_D0->setChecked(bins[7]);
         ui->TemEdit_1->display(nums[2]);
         ui->TemEdit_2->display(nums[3]);
         ui->TemEdit_3->display(nums[4]);
         ui->TemEdit_4->display(nums[5]);
         ui->TemEdit_5->display(nums[6]);
         display_RecvEdit(bag);
    }
    else{
        qDebug()<<cc<<"!="<<nums[7]<<",recv error!";
        display_RecvEdit(QString("recv error:")+bag);
    }
}
void Widget::display_Polling(QString bag)       //展示轮询参数
{
    int nums[18]={0};
    int bins[8]={0};
    int cc=0;
    for(int i=1;i<17;++i){
        nums[i]=hex2dec(bag.mid(i*2,2));
        cc+=nums[i];
    }
    cc%=256;
    nums[17]=hex2dec(bag.mid(34,2));
    QString a=QString("1");
    if(cc==nums[17]){
        qDebug()<<",recv right!";
        QString a13=hex2bin(bag.mid(26,2));
        qDebug()<<a13<<","<<nums[13];
       for(int i=0;i<8;++i){
           bins[i]=a13[i]==a?1:0;
       }
       ui->Polling1->display(nums[1]);
       ui->Polling2->display(nums[2]);
       ui->Polling3->display(nums[3]);
       ui->Polling4->display(nums[4]);

       ui->PollingV1->display(nums[5]);
       ui->PollingV2->display(nums[6]);
       ui->PollingV3->display(nums[7]);
       ui->PollingV4->display(nums[8]);
       ui->PollingV5->display(nums[9]);
       ui->PollingV6->display(nums[10]);
       ui->PollingV7->display(nums[11]);
       ui->PollingVV->display(nums[12]);

       ui->PollingS1->display(bins[0]);
       ui->PollingS2->display(bins[1]);
       ui->PollingS3->display(bins[2]);
       ui->PollingS4->display(bins[3]);
       ui->PollingS5->display(bins[4]);
       ui->PollingS6->display(bins[5]);
       ui->PollingS7->display(bins[6]);
       ui->PollingS8->display(bins[7]);

       ui->PollingL1->display(nums[14]);
       ui->PollingL2->display(nums[15]);
       ui->PollingL3->display(nums[16]);
       display_RecvEdit(bag);
    }
    else{
        qDebug()<<cc<<"!="<<nums[17]<<",recv error!";
        display_RecvEdit(QString("recv error:")+bag);
    }
}

void Widget::display_SendEdit(QString bag)                 //更新发送区
{
    QString send = ui->sendTextEdit->toPlainText();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString  cur_time = current_date_time.toString("hh:mm:ss");
    send += (cur_time+QString(":")+bag+QString("\n"));
    ui->sendTextEdit->clear();
    ui->sendTextEdit->append(send);
    SendIndex++;
    ui->SendNumber->display(SendIndex);

}
void Widget::display_RecvEdit(QString bag)                 //更新接收区
{
    QString recv = ui->recvTextEdit->toPlainText();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString  cur_time = current_date_time.toString("hh:mm:ss ");
    recv += (cur_time+QString(" : ")+bag+QString("\n"));
    ui->recvTextEdit->clear();
    ui->recvTextEdit->append(recv);
    RecvIndex++;
    ui->RecvNumber->display(RecvIndex);

}

void Widget::write_SendNote(QString bag)                 //更新发送日志
{
    QString  filepath = QString("/SendNote.csv");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString  cur_time = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    QFile file(SaveDirPath+filepath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append| QIODevice::Text)){
            QMessageBox::about(NULL, "失败", "无法打开或创建文件");
            return;
    }
    QString s1=cur_time+QString(",")+bag+QString("\n");
    QByteArray str = s1.toUtf8();
    file.write(str,str.length());
    file.close();
}

void Widget::write_RecvNote(QString bag)                 //更新接收日志
{
    QString  filepath = QString("/RecvNote.csv");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString  cur_time = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    QFile file(SaveDirPath+filepath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append| QIODevice::Text)){
            QMessageBox::about(NULL, "失败", "无法打开或创建文件");
            return;
    }
    QString s1=cur_time+QString(",")+bag+QString("\n");
    QByteArray str = s1.toUtf8();
    file.write(str,str.length());
    file.close();
}

void Widget::on_clearRecvButton_clicked()       //清空接收区
{
    ui->recvTextEdit->clear();
    RecvIndex=0;
    ui->RecvNumber->display(RecvIndex);
}

void Widget::on_clearSendButton_clicked()       //清空发送区
{
    ui->sendTextEdit->clear();
    SendIndex=0;
    ui->SendNumber->display(SendIndex);
}

void Widget::on_clearAllButton_clicked()        //全部清空
{
    on_clearRecvButton_clicked();
    on_clearSendButton_clicked();
}

int Widget::hex2dec(QString  str){              // hex->dec 10(HEX)->16 (DEC)
    int num=str.toInt(NULL, 16);
    return num;
}
QString Widget::hex2bin(QString  str){          // hex->bin 11(HEX)-> 0001 0001(BIN)
    int num=str.toInt(NULL, 16);
    QString a=QString::number(num,2);
    int siz=8-a.size();
    for(int i=0;i<siz;++i){
        a=QString("0")+a;
    }
    return a;
}
