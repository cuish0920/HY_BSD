#pragma once
#ifndef BSD_UTILS
#define BSD_UTILS
#include <Windows.h>
#include "__USB20BSDevOpSCexp.h"
#pragma comment(lib,"__USB20BSDevOpSCexp.lib")
#include <string>
#include <iostream>
#include "CJsonObject.h"
#define  DEV_MAX_PIXELS_H     4096       //Ԥ�����ڴ棺ˮƽ�������
#define  DEV_MAX_PIXELS_V     70           //Ԥ�����ڴ棺��ֱ�������

#define  MAX_PEAKS_NUM      64              //ÿ���������peaks����


#define MAX_BS_DEV_USB_DEVICES_ON_ONE_PC             4                     //����һ̨PC���֧�ֵ�BS DEV USB�豸
#define MAX_BS_DEV_COM_DEVICES_ON_ONE_PC             1                     //����һ̨PC���֧�ֵ�BS DEV COM�豸

#define MAX_BS_DEV_FILE_DEVICES_ON_ONE_PC            1                     //����һ̨PC���֧�ֵ�BS DEV FILE�豸��FILE�豸ָ���Ǵ����ļ����ݣ��Ѵ����ļ����������һ���豸��

//һ�й������ݵĶ��壬�������ݴ�����  ԭʼ���� --> ���� -->��ֵ
struct structOneLineSpectralData
{
	//�ɼ�����ԭʼ���ݶ���
	long xfer;                                 // �������ݳ���

	int imgH, imgV;                        // ��Ч���� H / V
	//BYTE inData[DEV_MAX_PIXELS_H*DEV_MAX_PIXELS_V * 2];         //ԭʼ���� 
	BYTE *inData;         //ԭʼ���� 
	//double 	pDblPwr[DEV_MAX_PIXELS_H];       //ÿ�����ض�Ӧ��������COUNT������InData�����õ�
	double 	*pDblPwr;
	WORD ADC12bits_S;																				  //������ͬ���ɼ����ģ�S TEMP ADCֵ
	WORD ADC12bits_C;																				  //������ͬ���ɼ����ģ�C TEMP ADCֵ
	DWORD timeStamp;																				    //������ͬ���ɼ����ģ�time Stamp
	DWORD syncCode;																				        //������ͬ���ɼ����ģ�syncCode

	//�����Ĺ������ݶ��� �����Შ��������������
	double  pdblWL[DEV_MAX_PIXELS_H];																//ÿ�����ض�Ӧ�Ĳ�����H����
	double pdblCorrectPwr[DEV_MAX_PIXELS_H];													//���������ݣ�����

	//Ѱ��
	int pwPeakCount;																								//Ѱ��������
	double pdblPeakPiXPos[MAX_PEAKS_NUM];
	double pdblPeakWL[MAX_PEAKS_NUM];															  // ��ֵ����
	double pdblPeakPwr[MAX_PEAKS_NUM];															   //��ֵ����

};

//Dev ����ģʽ������
struct  structBSDevWorkMode
{
	WORD md0_LineBinning_or_Area;			//FPGA0X03[0] LineBinning_or_Area            -  0 : Line scan/Binning       1 : Area scan			  //�ò�������Area scan or Line Scan ģʽ�����оƬû��Areaģʽ�����������ú�ֵ���� Line scan
	WORD md1_SoftTrig_or_ExternalTrig;		//FPGA0X03[1] SoftTrig_or_ExternalTrig       -  0 : Soft Triger             1 : External Triger      //�����������ⴥ����Ӳ������  �������ڲ��������
	WORD md2_Snapshot_or_Continuous;		//FPGA0X03[2] Snapshot_or_Continuous         -  0 : Snapshot                1 : continuous           //��������ģʽ�£�Snapshot����������
	WORD md3_LevelTrig_or_EdgeTrig;			//FPGA0X03[3] LevelTrig_or_EdgeTrig          -  0 : Level Triger            1 : Edge Triger          //�����ⴥ��ģʽ�£����õ�ƽ��������ش���
	WORD md4_PositiveTrig_or_NegtiveTrig;	//FPGA0X03[4] PositiveTrig_or_NegtiveTrig    -  0 : Positive Triger         1 : Negtive Triger       //�����ⴥ��ģʽ�£��������ԣ��ߵ�ƽ�������ǵ͵�ƽ����
	WORD md5_MechShutterMode;				//FPGA0X03[5] MechShutterMode                -  0 : Shutter by Exposure     1 : shutter by hand      //�������ʹ���ⲿ��е���ţ���е���ŵĹ�����ʽ����е�������ع⶯���е�������ֹ��趨
	WORD md6_MechShutterState;				//FPGA0X03[6] MechShutterState               -  0 : Shutter Close           1 : shutter Open         //�������ʹ���ⲿ��е���ţ��һ�е���Ź���ģʽ����Ϊ by hand����е���ŵ�״̬
	WORD md7_TerminateExposureNow;			//FPGA0X03[7] TerminateExposureNow           -  0 : Work as normal          1 : Terminate Exposure   //����������ģʽ�£��������ʱ��������Ƿ����Ͻ����ع�ʱ�䡣�ò���Ϊ������ʽ���ã����趨�ò���Ϊ1���豸��ֹͣ�����ع⣬�����ع�������豸Ӳ�����Զ��Ѹò�������
};

//rocky@2018
//��������������������������������������������������������������������������������������
//����ṹ�壬�����ˣ�һ̨BS DEV��ȫ�������APP�����Ӽ��������
//�ò����ŵ�˵������APP����Χ�Ƹ����ݽṹ��չ���ģ�APP�Ĺ��ܾ��ǲ�������ʾ�ýṹ���ڵ����� 
//��������������������������������������������������������������������������������������
struct  structBSDevAppDeviceDsr
{
	//App��������
	char _app_1devPortType;											 // �豸�ӿ����ͣ� 'U' - USB     'C' - COM    ��   other wise ��Ч
	int  _app_2nPort;														//��APP�ˣ��豸Ϊ�ڼ����˿� ( USB / COM )

	double _app_3startWL;												//�豸��ʼ����
	double _app_4endWL;													//�豸��������
	//double  _app_5pdblWL[DEV_MAX_PIXELS_H];								//ÿ�����ض�Ӧ�Ĳ�����H����
	double  *_app_5pdblWL;								//ÿ�����ض�Ӧ�Ĳ�����H����

	//dev���������� ( BS DEV�豸�ɼ�����: 1�� back ground  1�е�ǰ�ɼ���������
	//structOneLineSpectralData _devdata_1BackGround;			//BackGround����
	structOneLineSpectralData _devdata_2CurrentSpecData;    //Current Spec����
	int _devdata_3currentSpecData_copy_to_n_StaticOverLay;	//��Current Spec���ݿ������ڼ���OverLay��



	//Dev��������   _dev ��ͷ�Ķ���Ӳ����������Ϣ
	int  _dev_0devtype;                            //�豸���� dev type (sensor pid)���궨����ļ���ͷ
	char _dev_1devdsr[19];                      //�豸�������ִ�����_dev_0devtype��������õ�
	char _dev_1SN[9];						   	   //�豸���кţ���Ӳ���������洢��Ӳ���� EEProm ��
	int  _dev_2Pixels_H;						   //�豸����Ԫ��  H
	int  _dev_2Pixels_V;						   //�豸����Ԫ��  V
	structBSDevWorkMode  _dev_3DevMode;			//�豸���� ģʽ ��8��������
	DWORD _dev_4IntTime_us;						//����ʱ�䣬 ��λ: us
	int  _dev_5gain_reg;			//�豸�ڲ�sensor���Ҳ�ͨ��  VGA Gain �趨ֵ
	int  _dev_6offset_reg;				//�豸�ڲ�sensor���Ҳ�ͨ��  Offset   �趨ֵ
//	int  _dev_7left_ch_vga_gain_reg;			//�豸�ڲ�sensor�����ͨ��  VGA Gain �趨ֵ
//	int  _dev_8left_ch_offset_reg;				//�豸�ڲ�sensor�����ͨ��  Offset   �趨ֵ


};

struct  FrameStruct {
	/*�û�����Ӧ���ͷţ��������ͷŽ���ϵͳ����ͷ�*/
	double * wavedata;//�궨��������
	double * framedata;//��ǰ֡DNֵ
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
	*��ָ�����кŵ��豸
	*deviceNo
	*/
	bool openDevice(int deviceNo);

	/*
	*�ر��豸
	*/
	bool closeDevice();
	/*
	*��ȡ�豸�����Ϣ
	*/
	int getDevInfo(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*���ù���ģʽ
	*/
	int setWorkMode(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*����xxx
	*/
	int setAFE(int PGA= -220, int OFF=63);
	/*
	*��ȡ�궨����
	*ʹ�ú���Ҫ�ֶ��ͷ�m_usb_bsdevDsr._app_5pdblW
	*/
	int getWaveLength(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	/*
	*��ȡ��������
	*
	*/
	int getData(structBSDevAppDeviceDsr &m_usb_bsdevDsr);
	int shutDown();
	//���һ�����ݲɼ�
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

