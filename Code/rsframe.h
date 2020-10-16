#ifndef RSFRAME_H
#define RSFRAME_H

struct OrderFrame{      //中心机命令帧
    unsigned char indexs;       //指令次数,中心机自己保留使用,不传输
    unsigned char Head[2];      //帧头
    unsigned char Cmd;          //指令类型
    unsigned char Parameter;    //指令参数
    unsigned char CheckCode[2]; //校验码
};
struct RecvFrame{       //下位机应答帧
    unsigned char indexs;       //应答次数,保留使用
    unsigned char Head[2];      //帧头
    unsigned char Cmd;          //指令类型
    unsigned char *Parameter;   //指令参数
    unsigned char CheckCode[2]; //校验码
};
struct Temperatures{    //定义温度采集数据
    unsigned char nums;         //采温点个数，保留使用
    unsigned char zones;        //加热带状态
    unsigned char *tempes;      //采温点数据,都取A/D转换数值高8位
};
struct Pollings{        //定义轮询遥测数据
    unsigned char num_volts;    //蓄电池单体个数，保留使用
    unsigned char parent_vlot;  //母线电压,所有电压都取A/D转换数值高8位
    unsigned char array_vlot;   //太阳阵输入电压
    unsigned char vlot_ratio;   //升压占空比
    unsigned char *volts;       //单体电压数据
    unsigned char volume;       //电池容量
    unsigned char work_state;       //工作状态
    unsigned char indexs;       //指令计数
    unsigned char last_cmd;     //最后一条指令
    unsigned char last_state;   //最后一条指令状态
};

#endif // RSFRAME_H
