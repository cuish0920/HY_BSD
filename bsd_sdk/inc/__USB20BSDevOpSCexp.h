

#ifndef _BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ
#define _BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ



//定义一些dll导出的宏关键字
#define DLL_PORT_TYPE __declspec(dllexport)
//#define DLL_PORT_FUNCTION_TYPE __stdcall //在vb中可以定义
#define DLL_PORT_FUNCTION_TYPE //在labview中定义为c函数，而非stdcall函数可以正常使用


/////////////////////////////////////////////////////////////////////////////////
//ROCKY@20180217

//dev type 定义 : 产品型号标识

//HAMAMATSU 背照系列 : 支持 S10420-1006/1106 binning mode/area mode //同时支持面扫和线扫
#define S10420_1006_SPEC_DEV 0x4006               //S10420-1006 Product ID
#define S10420_1106_SPEC_DEV 0x4106               //S10420-1106 Product ID

//TCD1205D  东芝低成本
#define TCD1205_SPEC_DEV 0xC05D               //TCD1205D Product ID


//TCD1304   东芝低成本                       
#define TCD1304_SPEC_DEV  0x1304              //TCD1304  Product ID


//rocky@20181209
//NIR
#define SUI_256_SPEC_DEV 0x0256               //SU256LSB Product ID in EEPROM
#define SUI_512_SPEC_DEV 0x0512               //SU512LDB Product ID in EEPROM

//rocky@20181209
//NIR
#define HAMAMATSU_256_SPEC_DEV 0xA256  //HAMAMATSU 256 (1CH) Product ID in EEPROM
#define HAMAMATSU_512_SPEC_DEV 0xA512  //HAMAMATSU 512 (2CH) Product ID in EEPROM




/////////////////////////////////////////////////////////////////////////////////


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//1. 动态链接库初始化函数

//初始化Object : 第一个调用的函数
DLL_PORT_TYPE void* DLL_PORT_FUNCTION_TYPE  DllPortCreateObject(HWND m_hwnd);
										//DBT_DEVICEARRIVAL  : message will be sent to the HWND window when hardware is connected to PC
										//DBT_DEVICEREMOVECOMPLETE  : message will be sent to the HWND window when hardware is disconnected from PC


//delete Object : 最后一个调用的函数，退出的时候调用
DLL_PORT_TYPE void DLL_PORT_FUNCTION_TYPE   DllPortDeleteObject(void*arg);



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//2. 设备 操作句柄 初始化函数


//获取USB设备的数量
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSGetDevCount(void*arg);					//Get the number of cypress USB Devices connected to PC



//打开USB设备
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSOpenDev(void*arg,int iDev);						//Open a certain Cypress USB device, and according to what kind of device(USB20BS or not) is opened, do the initial work
																//open device -> initial control end point pointer -> initial bulk in end point pointer -> get eeprom value
																//iDev:
																	//is the Index of certain devices connected to PC
																//retrun value: 
																	//0 -- Failed; 
																	//1 -- sucessfully opened a USB device, but it is not USB20BS device
																	//2 -- sucessfully opened a USB20BS device

//关闭USB设备
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSCloseDev(void*arg);						//Close a certain Cypress USB device, and according to what kind of device(USB20BS or not) is opened, do the initial work
																//open device -> initial control end point pointer -> initial bulk in end point pointer -> get eeprom value
																//iDev:
																	//is the Index of certain devices connected to PC
																//retrun value: 
																	//0 -- Failed; 
																	//1 -- sucessfully opened a USB device, but it is not USB20BS device
																	//2 -- sucessfully opened a USB20BS device


//命令设备从EEPROM加载默认设置
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSTellDevLoadDefaultSettingsFromEEprom(void*arg);



//获取打开设备的内部配置，打开的设备内部有存储器，存储着之前的配置
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSGetDevInfo(	void*arg,
																			LPSTR pStrDevdsr,  //19 char max
																			LPSTR pStrSn,      //9 char                                                   //设备序列号
																			int *imgX,int *imgY,
																			DWORD *it_us,
																			int *PGA,int *OFF,
																			WORD *LineBinning_or_Area,
																			WORD *SoftTrig_or_ExternalTrig,
																			WORD *Snapshot_or_Continuous,
																			WORD *LevelTrig_or_EdgeTrig,
																			WORD *PositiveTrig_or_NegtiveTrig,
																			WORD *MechShutterMode,
																			WORD *MechShutterState,
																			WORD *TerminateExposureNow);
//调用该函数获取下列参数:
//返回值  =  devtype
//devType  设备类型定义见 文件开头相关宏定义，具体定义如下
		// #define S10420_1006_SPEC_DEV 0x4006               //S10420-1006 Product ID
		// #define S10420_1106_SPEC_DEV 0x4106               //S10420-1106 Product ID
		// #define TCD1205_SPEC_DEV     0xC05D               //TCD1205D Product ID
		// #define TCD1304_SPEC_DEV     0x1304               //TCD1304  Product ID

//pStrDevdsr 设备描述符指针, 最多19 char
//pStrSn     序列号指针, 9 char
//*imX       横向分辨率
//*imY       纵向分辨率
// *it_us =   积分时间 (单位 us)
// *OFF   =   offset_reg  取值范围： -255 ~ +255 ， 对应 offset 电压 = 1.1765mV * offset_reg. 对应ADC offset数值 = 19.2 * offset_reg
// *PGA   =   Gain_reg   取值范围： 0~63 ， 对应gain 倍数 = 6.0/(1.0+(5.0*float(63-m_vgagain_reg)/63.0))
//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //该参数设置Area scan or Line Scan 模式，如果芯片没有Area模式，则无论设置何值都是 Line scan
//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //设置软触发或外触发（硬触发）  软触发即内部软件触发
//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //设置软触发模式下，Snapshot或连续软触发
//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //设置外触发模式下，采用电平触发或边沿触发
//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //设置外触发模式下，触发极性，高电平触发还是低电平触发
//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //设置如果使用外部机械快门，机械快门的工作方式，机械开门随曝光动或机械快门由手工设定
//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //设置如果使用外部机械快门，且机械快门工作模式设置为 by hand，机械快门的状态
//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //设置在软触发模式下，如果积分时间过长，是否马上结束曝光时间。该参数为非锁定式调用，即设定该参数为1后，设备会停止超长曝光，但是曝光结束后设备硬件会自动把该参数回零


																	
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//3.设备设置函数





//rocky@2018 iSaveOrNOT = 0 Do not save, iSaveOrNOT =0xFF save//if do NOT call this function, Do not save
//E2PROM 设备设置 存盘开关，如果不调用此函数打开开关，默认不存储设置参数到E2prom中，调用此函数打开开关后，各类设置可以被存入E2PROM
//注意： 设备实际使用过程中，经常写入E2prom会有既定几率导致数据丢失或E2PROM失效，因此建议不要经常打开数据保存开关
DLL_PORT_TYPE void  DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSSetE2promSaveOrNOT(void*arg,BYTE iSaveOrNOT);             
		



// rocky@2018
// 设置工作模式
//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //该参数设置Area scan or Line Scan 模式，如果芯片没有Area模式，则无论设置何值都是 Line scan
//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //设置软触发或外触发（硬触发）  软触发即内部软件触发
//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //设置软触发模式下，Snapshot或连续软触发
//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //设置外触发模式下，采用电平触发或边沿触发
//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //设置外触发模式下，触发极性，高电平触发还是低电平触发
//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //设置如果使用外部机械快门，机械快门的工作方式，机械开门随曝光动或机械快门由手工设定
//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //设置如果使用外部机械快门，且机械快门工作模式设置为 by hand，机械快门的状态
//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //设置在软触发模式下，如果积分时间过长，是否马上结束曝光时间。该参数为非锁定式调用，即设定该参数为1后，设备会停止超长曝光，但是曝光结束后设备硬件会自动把该参数回零
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSSetWorkMode(
												        void*arg,
														WORD LineBinning_or_Area,
														WORD SoftTrig_or_ExternalTrig,
														WORD Snapshot_or_Continuous,
														WORD LevelTrig_or_EdgeTrig,
														WORD PositiveTrig_or_NegtiveTrig,
														WORD MechShutterMode,
														WORD MechShutterState,
														WORD TerminateExposureNow
														);


//it_us 期望的积分时间，单位 us，最小为32
//返回值实际的积分时间，单位 : us      //实际积分时间 为 16us 的整数倍
DLL_PORT_TYPE	DWORD DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSSetIntegrationTime(void*arg,DWORD it_us);					//set the sensor's Integration Time
																										    


//offset_reg 取值范围： -255 ~ +255 ， 对应 offset 电压 = 1.1765mV * offset_reg. 对应ADC offset数值 = 19.2 * offset_reg
//Gain_reg   取值范围： 0~63 ， 对应gain 倍数 = 6.0/(1.0+(5.0*float(63-m_vgagain_reg)/63.0))
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSsetAFE(void*arg,int PGA,int OFF);   //Set PGA(R/G) and OFFSET(R/G) registers in AFE









//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//4.数据获取函数


//根据当前的芯片类型 tcd1304 / 1205 / 1006/1106 和 Area/Binning 设定，确定一次 Snapshot 的数据长度
//返回值，每次snapshot 返回数据的长度，在调用snapshot之前需要调用该函数获取 m_xfer size
DLL_PORT_TYPE	int  DLL_PORT_FUNCTION_TYPE DllPortUSB20BSGetDataLengthPerSnapShot(void*arg);


//Data request functions    SnapShot 方式获取一行数据
	//inData  : 存储获取的数据的指针
	//m_xfer  : 待获取数据的长度

	//iTimeOut : 超时返回设置,单位ms，如果超时返回错误

	//*pDblPwr: 返回 - 解析出来的光谱数据 double型
    //imgH    : 输入 - sensor 的水平像素数
	//imgV    : 输入 - sensor 的垂直像素数

	//*ADC12bits_S    : 返回 - 和曝光同步采集到的 STEMP ADC 值
	//*ADC12bits_C    : 返回 - 和曝光同步采集到的 CTEMP ADC 值
	//*timeStamp      : 返回 - 时间戳
    //*syncCode       : 返回 - 同步码，(保留)
	
//返回值：0-失败    非零-成功
DLL_PORT_TYPE	int  DLL_PORT_FUNCTION_TYPE DllPortUSB20BSSnapShotGetData(void*arg,
																		  LPBYTE inData,
																		  long m_xfer,

																		  int iTimeOut,

																		  double *pDblPwr,int imgH,int imgV,
																		  WORD *ADC12bits_S,
																		  WORD *ADC12bits_C,
																		  DWORD *timeStamp,
																		  DWORD *syncCode);
DLL_PORT_TYPE	int  DLL_PORT_FUNCTION_TYPE DllPortUSB20BSSnapShotShutdown(void*arg);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//5.IO函数
//IO Functions
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE DllPortUSB20BS_FPGA_SET_GPIO(void*arg,BYTE io8);	//通过FPGA0X03/fpga0x80寄存器设定FPGA内8个IO

//设置MCU的 A,B,C,D,E PORT的I/O方向，读取/写入 MCU的A,B,C,D,E PORT
//该函数不能随便调用，需按照参考代码调用
DLL_PORT_TYPE	BYTE DLL_PORT_FUNCTION_TYPE  DllPortUSB20BS_MCU_READ_PORT(void*arg,BYTE MCU_PORT_NAME);						//读取MCU PORT INPUT值 ， MCU_PORT_NAME代表含义： 0x0A -- PORTA  0x0B -- PORTB  0x0C -- PORTC  0x0D -- PORTD  0x0E -- PORTE
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE  DllPortUSB20BS_MCU_WRITE_PORT(void*arg,BYTE MCU_PORT_NAME,BYTE D8);				//写入MCU PORT OUTPUT值 ， MCU_PORT_NAME代表含义： 0x0A -- PORTA  0x0B -- PORTB  0x0C -- PORTC  0x0D -- PORTD  0x0E -- PORTE
//D8代表写入的8bits IO数值


DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE DllPortUSB20BS_SET_PWM(void*arg,WORD cnt_cycle,WORD cnt_width);													//设置PWM周期和占空比





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//6.EEPROM / SPI FLASH ROM函数

//Get 64 Bytes from EEprom, 开放给用户
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromGet64Bytes(void*arg,WORD addr,LPBYTE pData); //Get 64 bytes from EEProm through EP0

//Manufacturer's reserved call, Write 8 Bytes to EEprom, this is a function only for Provider of USB20BS device. Must NOT open to customer.
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromWirte8Bytes_ManufactureFunc(void*arg,WORD addr,LPBYTE pData); //write 8 bytes to EEProm through EP0


//Manufacturer's reserved call, Write 64 Bytes to EEprom, this is a function only for Provider of USB20BS device. Must NOT open to customer.
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromWirte64Bytes_ManufactureFunc(void*arg,WORD addr,LPBYTE pData); //write 64 bytes to EEProm through EP0


	


//------------------------------------export functin declaration over------------------------------------

#endif //_BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ