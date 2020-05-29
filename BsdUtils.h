#pragma once
#ifndef BSD_UTILS
#define BSD_UTILS
#include <Windows.h>
#include "__USB20BSDevOpSCexp.h"
#pragma comment(lib,"__USB20BSDevOpSCexp.lib")
#include <string>
#include <iostream>
#include "CJsonObject.h"
#define  DEV_MAX_PIXELS_H     4096       //预申请内存：水平最大像素
#define  DEV_MAX_PIXELS_V     70           //预申请内存：垂直最大像素

#define  MAX_PEAKS_NUM      64              //每个光谱最大peaks数量


#define MAX_BS_DEV_USB_DEVICES_ON_ONE_PC             4                     //定义一台PC最多支持的BS DEV USB设备
#define MAX_BS_DEV_COM_DEVICES_ON_ONE_PC             1                     //定义一台PC最多支持的BS DEV COM设备

#define MAX_BS_DEV_FILE_DEVICES_ON_ONE_PC            1                     //定义一台PC最多支持的BS DEV FILE设备（FILE设备指的是存盘文件数据，把存盘文件数据虚拟成一个设备）

//一行光谱数据的定义，包含数据处理结果  原始数据 --> 光谱 -->峰值
struct structOneLineSpectralData
{
	//采集到的原始数据定义
	long xfer;                                 // 传输数据长度

	int imgH, imgV;                        // 有效像素 H / V
	//BYTE inData[DEV_MAX_PIXELS_H*DEV_MAX_PIXELS_V * 2];         //原始数据 
	BYTE *inData;         //原始数据 
	//double 	pDblPwr[DEV_MAX_PIXELS_H];       //每个像素对应的能量（COUNT），由InData解析得到
	double 	*pDblPwr;
	WORD ADC12bits_S;																				  //随数据同步采集到的：S TEMP ADC值
	WORD ADC12bits_C;																				  //随数据同步采集到的：C TEMP ADC值
	DWORD timeStamp;																				    //随数据同步采集到的：time Stamp
	DWORD syncCode;																				        //随数据同步采集到的：syncCode

	//处理后的光谱数据定义 （横轴波长，纵轴能量）
	double  pdblWL[DEV_MAX_PIXELS_H];																//每个像素对应的波长，H方向
	double pdblCorrectPwr[DEV_MAX_PIXELS_H];													//处理后的数据，能量

	//寻峰
	int pwPeakCount;																								//寻到峰数量
	double pdblPeakPiXPos[MAX_PEAKS_NUM];
	double pdblPeakWL[MAX_PEAKS_NUM];															  // 峰值波长
	double pdblPeakPwr[MAX_PEAKS_NUM];															   //峰值能量

};

//Dev 工作模式描述符
struct  structBSDevWorkMode
{
	WORD md0_LineBinning_or_Area;			//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //该参数设置Area scan or Line Scan 模式，如果芯片没有Area模式，则无论设置何值都是 Line scan
	WORD md1_SoftTrig_or_ExternalTrig;		//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //设置软触发或外触发（硬触发）  软触发即内部软件触发
	WORD md2_Snapshot_or_Continuous;		//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //设置软触发模式下，Snapshot或连续软触发
	WORD md3_LevelTrig_or_EdgeTrig;			//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //设置外触发模式下，采用电平触发或边沿触发
	WORD md4_PositiveTrig_or_NegtiveTrig;	//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //设置外触发模式下，触发极性，高电平触发还是低电平触发
	WORD md5_MechShutterMode;				//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //设置如果使用外部机械快门，机械快门的工作方式，机械开门随曝光动或机械快门由手工设定
	WORD md6_MechShutterState;				//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //设置如果使用外部机械快门，且机械快门工作模式设置为 by hand，机械快门的状态
	WORD md7_TerminateExposureNow;			//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //设置在软触发模式下，如果积分时间过长，是否马上结束曝光时间。该参数为非锁定式调用，即设定该参数为1后，设备会停止超长曝光，但是曝光结束后设备硬件会自动把该参数回零
};

//rocky@2018
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
//顶层结构体，描述了，一台BS DEV的全部定义和APP端连接及数据情况
//好不夸张的说：整个APP就是围绕该数据结构体展开的，APP的功能就是操作和显示该结构体内的数据 
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
struct  structBSDevAppDeviceDsr
{
	//App端描述符
	char _app_1devPortType;											 // 设备接口类型： 'U' - USB     'C' - COM    ，   other wise 无效
	int  _app_2nPort;														//在APP端，设备为第几个端口 ( USB / COM )

	double _app_3startWL;												//设备开始波长
	double _app_4endWL;													//设备结束波长
	//double  _app_5pdblWL[DEV_MAX_PIXELS_H];								//每个像素对应的波长，H方向
	double  *_app_5pdblWL;								//每个像素对应的波长，H方向

	//dev数据描述符 ( BS DEV设备采集数据: 1行 back ground  1行当前采集到的数据
	//structOneLineSpectralData _devdata_1BackGround;			//BackGround数据
	structOneLineSpectralData _devdata_2CurrentSpecData;    //Current Spec数据
	int _devdata_3currentSpecData_copy_to_n_StaticOverLay;	//将Current Spec数据拷贝到第几个OverLay中



	//Dev端描述符   _dev 开头的都是硬件读出的信息
	int  _dev_0devtype;                            //设备类型 dev type (sensor pid)，宏定义见文件开头
	char _dev_1devdsr[19];                      //设备描述符字串，由_dev_0devtype解析翻译得到
	char _dev_1SN[9];						   	   //设备序列号，从硬件读出，存储在硬件的 EEProm 中
	int  _dev_2Pixels_H;						   //设备的像元数  H
	int  _dev_2Pixels_V;						   //设备的像元数  V
	structBSDevWorkMode  _dev_3DevMode;			//设备工作 模式 （8个参量）
	DWORD _dev_4IntTime_us;						//积分时间， 单位: us
	int  _dev_5gain_reg;			//设备内部sensor的右侧通道  VGA Gain 设定值
	int  _dev_6offset_reg;				//设备内部sensor的右侧通道  Offset   设定值
//	int  _dev_7left_ch_vga_gain_reg;			//设备内部sensor的左侧通道  VGA Gain 设定值
//	int  _dev_8left_ch_offset_reg;				//设备内部sensor的左侧通道  Offset   设定值


};

struct  FrameStruct {
	/*用户用完应当释放，若忘记释放将由系统完成释放*/
	double * wavedata;//标定波长数据
	double * framedata;//当前帧DN值
	int width;
	int height;
	int size;
	std::string isn;
};

class BsdUtils
{
public:
	BsdUtils();
	~BsdUtils();
	static std::string getDervicesInfo();
public:
	/*
	*打开指定序列号的设备
	*deviceNo
	*/
	bool openDevice(int deviceNo);

	/*
	*关闭设备
	*/
	bool closeDevice();
	/*
	*获取设备相关信息
	*/
	int getDevInfo(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*设置工作模式
	*/
	int setWorkMode(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*设置xxx
	*/
	int setAFE(int PGA= -220, int OFF=63);
	/*
	*读取标定波长
	*使用后需要手动释放m_usb_bsdevDsr._app_5pdblW
	*/
	int getWaveLength(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*读取光谱数据
	*
	*/
	int getData(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	int shutDown();
	//完成一次数据采集
	bool setFlagGetFrame(bool value);
	bool getFlagGetFrame();
	int setIntegrationTime(int it_us);
	FrameStruct m_Frame;
	int e2promSaveOrNOT(bool m_iSaveOrNOT);
private:
	bool IsGetFrame;
	void * m_handle;
};
#endif // !BSD_UTILS

