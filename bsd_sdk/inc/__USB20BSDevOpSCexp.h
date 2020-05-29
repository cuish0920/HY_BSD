

#ifndef _BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ
#define _BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ



//����һЩdll�����ĺ�ؼ���
#define DLL_PORT_TYPE __declspec(dllexport)
//#define DLL_PORT_FUNCTION_TYPE __stdcall //��vb�п��Զ���
#define DLL_PORT_FUNCTION_TYPE //��labview�ж���Ϊc����������stdcall������������ʹ��


/////////////////////////////////////////////////////////////////////////////////
//ROCKY@20180217

//dev type ���� : ��Ʒ�ͺű�ʶ

//HAMAMATSU ����ϵ�� : ֧�� S10420-1006/1106 binning mode/area mode //ͬʱ֧����ɨ����ɨ
#define S10420_1006_SPEC_DEV 0x4006               //S10420-1006 Product ID
#define S10420_1106_SPEC_DEV 0x4106               //S10420-1106 Product ID

//TCD1205D  ��֥�ͳɱ�
#define TCD1205_SPEC_DEV 0xC05D               //TCD1205D Product ID


//TCD1304   ��֥�ͳɱ�                       
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
//1. ��̬���ӿ��ʼ������

//��ʼ��Object : ��һ�����õĺ���
DLL_PORT_TYPE void* DLL_PORT_FUNCTION_TYPE  DllPortCreateObject(HWND m_hwnd);
										//DBT_DEVICEARRIVAL  : message will be sent to the HWND window when hardware is connected to PC
										//DBT_DEVICEREMOVECOMPLETE  : message will be sent to the HWND window when hardware is disconnected from PC


//delete Object : ���һ�����õĺ������˳���ʱ�����
DLL_PORT_TYPE void DLL_PORT_FUNCTION_TYPE   DllPortDeleteObject(void*arg);



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//2. �豸 ������� ��ʼ������


//��ȡUSB�豸������
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSGetDevCount(void*arg);					//Get the number of cypress USB Devices connected to PC



//��USB�豸
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSOpenDev(void*arg,int iDev);						//Open a certain Cypress USB device, and according to what kind of device(USB20BS or not) is opened, do the initial work
																//open device -> initial control end point pointer -> initial bulk in end point pointer -> get eeprom value
																//iDev:
																	//is the Index of certain devices connected to PC
																//retrun value: 
																	//0 -- Failed; 
																	//1 -- sucessfully opened a USB device, but it is not USB20BS device
																	//2 -- sucessfully opened a USB20BS device

//�ر�USB�豸
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSCloseDev(void*arg);						//Close a certain Cypress USB device, and according to what kind of device(USB20BS or not) is opened, do the initial work
																//open device -> initial control end point pointer -> initial bulk in end point pointer -> get eeprom value
																//iDev:
																	//is the Index of certain devices connected to PC
																//retrun value: 
																	//0 -- Failed; 
																	//1 -- sucessfully opened a USB device, but it is not USB20BS device
																	//2 -- sucessfully opened a USB20BS device


//�����豸��EEPROM����Ĭ������
DLL_PORT_TYPE	int	 DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSTellDevLoadDefaultSettingsFromEEprom(void*arg);



//��ȡ���豸���ڲ����ã��򿪵��豸�ڲ��д洢�����洢��֮ǰ������
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE  	DllPortUSB20BSGetDevInfo(	void*arg,
																			LPSTR pStrDevdsr,  //19 char max
																			LPSTR pStrSn,      //9 char                                                   //�豸���к�
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
//���øú�����ȡ���в���:
//����ֵ  =  devtype
//devType  �豸���Ͷ���� �ļ���ͷ��غ궨�壬���嶨������
		// #define S10420_1006_SPEC_DEV 0x4006               //S10420-1006 Product ID
		// #define S10420_1106_SPEC_DEV 0x4106               //S10420-1106 Product ID
		// #define TCD1205_SPEC_DEV     0xC05D               //TCD1205D Product ID
		// #define TCD1304_SPEC_DEV     0x1304               //TCD1304  Product ID

//pStrDevdsr �豸������ָ��, ���19 char
//pStrSn     ���к�ָ��, 9 char
//*imX       ����ֱ���
//*imY       ����ֱ���
// *it_us =   ����ʱ�� (��λ us)
// *OFF   =   offset_reg  ȡֵ��Χ�� -255 ~ +255 �� ��Ӧ offset ��ѹ = 1.1765mV * offset_reg. ��ӦADC offset��ֵ = 19.2 * offset_reg
// *PGA   =   Gain_reg   ȡֵ��Χ�� 0~63 �� ��Ӧgain ���� = 6.0/(1.0+(5.0*float(63-m_vgagain_reg)/63.0))
//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //�ò�������Area scan or Line Scan ģʽ�����оƬû��Areaģʽ�����������ú�ֵ���� Line scan
//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //�����������ⴥ����Ӳ������  �������ڲ��������
//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //��������ģʽ�£�Snapshot����������
//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //�����ⴥ��ģʽ�£����õ�ƽ��������ش���
//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //�����ⴥ��ģʽ�£��������ԣ��ߵ�ƽ�������ǵ͵�ƽ����
//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //�������ʹ���ⲿ��е���ţ���е���ŵĹ�����ʽ����е�������ع⶯���е�������ֹ��趨
//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //�������ʹ���ⲿ��е���ţ��һ�е���Ź���ģʽ����Ϊ by hand����е���ŵ�״̬
//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //����������ģʽ�£��������ʱ��������Ƿ����Ͻ����ع�ʱ�䡣�ò���Ϊ������ʽ���ã����趨�ò���Ϊ1���豸��ֹͣ�����ع⣬�����ع�������豸Ӳ�����Զ��Ѹò�������


																	
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//3.�豸���ú���





//rocky@2018 iSaveOrNOT = 0 Do not save, iSaveOrNOT =0xFF save//if do NOT call this function, Do not save
//E2PROM �豸���� ���̿��أ���������ô˺����򿪿��أ�Ĭ�ϲ��洢���ò�����E2prom�У����ô˺����򿪿��غ󣬸������ÿ��Ա�����E2PROM
//ע�⣺ �豸ʵ��ʹ�ù����У�����д��E2prom���мȶ����ʵ������ݶ�ʧ��E2PROMʧЧ����˽��鲻Ҫ���������ݱ��濪��
DLL_PORT_TYPE void  DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSSetE2promSaveOrNOT(void*arg,BYTE iSaveOrNOT);             
		



// rocky@2018
// ���ù���ģʽ
//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //�ò�������Area scan or Line Scan ģʽ�����оƬû��Areaģʽ�����������ú�ֵ���� Line scan
//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //�����������ⴥ����Ӳ������  �������ڲ��������
//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //��������ģʽ�£�Snapshot����������
//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //�����ⴥ��ģʽ�£����õ�ƽ��������ش���
//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //�����ⴥ��ģʽ�£��������ԣ��ߵ�ƽ�������ǵ͵�ƽ����
//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //�������ʹ���ⲿ��е���ţ���е���ŵĹ�����ʽ����е�������ع⶯���е�������ֹ��趨
//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //�������ʹ���ⲿ��е���ţ��һ�е���Ź���ģʽ����Ϊ by hand����е���ŵ�״̬
//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //����������ģʽ�£��������ʱ��������Ƿ����Ͻ����ع�ʱ�䡣�ò���Ϊ������ʽ���ã����趨�ò���Ϊ1���豸��ֹͣ�����ع⣬�����ع�������豸Ӳ�����Զ��Ѹò�������
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


//it_us �����Ļ���ʱ�䣬��λ us����СΪ32
//����ֵʵ�ʵĻ���ʱ�䣬��λ : us      //ʵ�ʻ���ʱ�� Ϊ 16us ��������
DLL_PORT_TYPE	DWORD DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSSetIntegrationTime(void*arg,DWORD it_us);					//set the sensor's Integration Time
																										    


//offset_reg ȡֵ��Χ�� -255 ~ +255 �� ��Ӧ offset ��ѹ = 1.1765mV * offset_reg. ��ӦADC offset��ֵ = 19.2 * offset_reg
//Gain_reg   ȡֵ��Χ�� 0~63 �� ��Ӧgain ���� = 6.0/(1.0+(5.0*float(63-m_vgagain_reg)/63.0))
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE   DllPortUSB20BSsetAFE(void*arg,int PGA,int OFF);   //Set PGA(R/G) and OFFSET(R/G) registers in AFE









//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//4.���ݻ�ȡ����


//���ݵ�ǰ��оƬ���� tcd1304 / 1205 / 1006/1106 �� Area/Binning �趨��ȷ��һ�� Snapshot �����ݳ���
//����ֵ��ÿ��snapshot �������ݵĳ��ȣ��ڵ���snapshot֮ǰ��Ҫ���øú�����ȡ m_xfer size
DLL_PORT_TYPE	int  DLL_PORT_FUNCTION_TYPE DllPortUSB20BSGetDataLengthPerSnapShot(void*arg);


//Data request functions    SnapShot ��ʽ��ȡһ������
	//inData  : �洢��ȡ�����ݵ�ָ��
	//m_xfer  : ����ȡ���ݵĳ���

	//iTimeOut : ��ʱ��������,��λms�������ʱ���ش���

	//*pDblPwr: ���� - ���������Ĺ������� double��
    //imgH    : ���� - sensor ��ˮƽ������
	//imgV    : ���� - sensor �Ĵ�ֱ������

	//*ADC12bits_S    : ���� - ���ع�ͬ���ɼ����� STEMP ADC ֵ
	//*ADC12bits_C    : ���� - ���ع�ͬ���ɼ����� CTEMP ADC ֵ
	//*timeStamp      : ���� - ʱ���
    //*syncCode       : ���� - ͬ���룬(����)
	
//����ֵ��0-ʧ��    ����-�ɹ�
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
//5.IO����
//IO Functions
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE DllPortUSB20BS_FPGA_SET_GPIO(void*arg,BYTE io8);	//ͨ��FPGA0X03/fpga0x80�Ĵ����趨FPGA��8��IO

//����MCU�� A,B,C,D,E PORT��I/O���򣬶�ȡ/д�� MCU��A,B,C,D,E PORT
//�ú������������ã��谴�ղο��������
DLL_PORT_TYPE	BYTE DLL_PORT_FUNCTION_TYPE  DllPortUSB20BS_MCU_READ_PORT(void*arg,BYTE MCU_PORT_NAME);						//��ȡMCU PORT INPUTֵ �� MCU_PORT_NAME�����壺 0x0A -- PORTA  0x0B -- PORTB  0x0C -- PORTC  0x0D -- PORTD  0x0E -- PORTE
DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE  DllPortUSB20BS_MCU_WRITE_PORT(void*arg,BYTE MCU_PORT_NAME,BYTE D8);				//д��MCU PORT OUTPUTֵ �� MCU_PORT_NAME�����壺 0x0A -- PORTA  0x0B -- PORTB  0x0C -- PORTC  0x0D -- PORTD  0x0E -- PORTE
//D8����д���8bits IO��ֵ


DLL_PORT_TYPE	int DLL_PORT_FUNCTION_TYPE DllPortUSB20BS_SET_PWM(void*arg,WORD cnt_cycle,WORD cnt_width);													//����PWM���ں�ռ�ձ�





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//6.EEPROM / SPI FLASH ROM����

//Get 64 Bytes from EEprom, ���Ÿ��û�
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromGet64Bytes(void*arg,WORD addr,LPBYTE pData); //Get 64 bytes from EEProm through EP0

//Manufacturer's reserved call, Write 8 Bytes to EEprom, this is a function only for Provider of USB20BS device. Must NOT open to customer.
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromWirte8Bytes_ManufactureFunc(void*arg,WORD addr,LPBYTE pData); //write 8 bytes to EEProm through EP0


//Manufacturer's reserved call, Write 64 Bytes to EEprom, this is a function only for Provider of USB20BS device. Must NOT open to customer.
DLL_PORT_TYPE int DLL_PORT_FUNCTION_TYPE DllPortUSB20BSEEPromWirte64Bytes_ManufactureFunc(void*arg,WORD addr,LPBYTE pData); //write 64 bytes to EEProm through EP0


	


//------------------------------------export functin declaration over------------------------------------

#endif //_BS_DEV_OP_ATTACH_ALL_DLL_USB_02039402348923748HJ