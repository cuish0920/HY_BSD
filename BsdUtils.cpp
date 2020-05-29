#include "BsdUtils.h"
//////////////////引入opencv///////////////////
#include "OpencvUtils.h"
//////////////////引入opencv///////////////////
#define random(a,b) (rand()%(b-a)+a)
BsdUtils::BsdUtils():m_handle(NULL), IsGetFrame(false)
{
	m_Frame.wavedata = NULL;
	m_Frame.framedata = NULL;
}

BsdUtils::~BsdUtils() 
{
	if (m_Frame.wavedata != NULL)
	{
		delete[] m_Frame.wavedata;
		m_Frame.wavedata = NULL;
	}
	if (m_Frame.framedata != NULL)
	{
		delete[] m_Frame.framedata;
		m_Frame.framedata = NULL;
	}
}

/*
*以json字符串形式返回
*获得设备列表中光谱仪信息
*index
*des
*dev
*/
std::string BsdUtils::getDervicesInfo()
{
	void * m_handle = DllPortCreateObject(NULL);
	int devcount = DllPortUSB20BSGetDevCount(m_handle);
	structBSDevAppDeviceDsr m_usb_bsdevDsr;			//所有连接到到PC的 USB BS DEV 的设定
	int result = -1;
	neb::CJsonObject deviceInfo;
	deviceInfo.AddEmptySubArray("result");
	for (int index = 0; index < devcount; index++) {

		result = DllPortUSB20BSOpenDev(m_handle, index);
		//std::cout << "DllPortUSB20BSOpenDev  " << result << std::endl;

		//DllPortUSB20BSGetDevInfo(
		//LPSTR pStrDevdsr,  //19 char max
		//LPSTR pStrSn,      //9 char                                                   //设备序列号
		//int *imgX, int *imgY,
		//DWORD *it_us,
		//int *PGA, int *OFF,
		//WORD *LineBinning_or_Area,
		//WORD *SoftTrig_or_ExternalTrig,
		//WORD *Snapshot_or_Continuous,
		//WORD *LevelTrig_or_EdgeTrig,
		//WORD *PositiveTrig_or_NegtiveTrig,
		//WORD *MechShutterMode,
		//WORD *MechShutterState,
		//WORD *TerminateExposureNow
		//)


		result = DllPortUSB20BSGetDevInfo(
			m_handle,
			m_usb_bsdevDsr._dev_1devdsr,
			m_usb_bsdevDsr._dev_1SN,
			&m_usb_bsdevDsr._dev_2Pixels_H, &m_usb_bsdevDsr._dev_2Pixels_V,
			&m_usb_bsdevDsr._dev_4IntTime_us,
			&m_usb_bsdevDsr._dev_5gain_reg, &m_usb_bsdevDsr._dev_6offset_reg,
			&m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area,
			&m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig,
			&m_usb_bsdevDsr._dev_3DevMode.md2_Snapshot_or_Continuous,
			&m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig,
			&m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig,
			&m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode,
			&m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState,
			&m_usb_bsdevDsr._dev_3DevMode.md7_TerminateExposureNow
		);
		neb::CJsonObject deviceItem;
		deviceItem.Add("index", index);
		deviceItem.Add("DES", m_usb_bsdevDsr._dev_1devdsr);
		deviceItem.Add("ISN", m_usb_bsdevDsr._dev_1SN);
		
		deviceInfo["result"].Add(deviceItem);
		std::cout <<"index:"<<index<< "DEV:" << m_usb_bsdevDsr._dev_1devdsr << " ISN:" << m_usb_bsdevDsr._dev_1SN << " ImgH:" << m_usb_bsdevDsr._dev_2Pixels_H << " ImgV:" << m_usb_bsdevDsr._dev_2Pixels_V << std::endl;
	}
	DllPortDeleteObject(m_handle);

	return deviceInfo.ToString();
}

/*
*打开指定序列号的设备
*deviceNo :序列号
*/
bool BsdUtils::openDevice(int deviceNo)
{
	if (m_handle != NULL) {
		return false;
	}
	m_handle = DllPortCreateObject(NULL);
	return DllPortUSB20BSOpenDev(m_handle, deviceNo);
}

/*
*关闭设备
*
*/
bool BsdUtils::closeDevice() 
{
	if (m_handle != NULL) {
		DllPortUSB20BSCloseDev(m_handle);
		DllPortDeleteObject(m_handle);
		return true;
	}
	
	return false;
}


int BsdUtils::getDevInfo(structBSDevAppDeviceDsr &m_usb_bsdevDsr)
{
	/*返回值  =  devtype
	*devType  设备类型定义见 文件开头相关宏定义，具体定义如下
	* #define S10420_1006_SPEC_DEV 0x4006               //S10420-1006 Product ID
	* #define S10420_1106_SPEC_DEV 0x4106               //S10420-1106 Product ID
	* #define TCD1205_SPEC_DEV     0xC05D               //TCD1205D Product ID
	* #define TCD1304_SPEC_DEV     0x1304
	*/
	//structBSDevAppDeviceDsr m_usb_bsdevDsr;			//所有连接到到PC的 USB BS DEV 的设定
	int devtype = DllPortUSB20BSGetDevInfo(
		m_handle,
		m_usb_bsdevDsr._dev_1devdsr,
		m_usb_bsdevDsr._dev_1SN,
		&m_usb_bsdevDsr._dev_2Pixels_H, &m_usb_bsdevDsr._dev_2Pixels_V,
		&m_usb_bsdevDsr._dev_4IntTime_us,
		&m_usb_bsdevDsr._dev_5gain_reg, &m_usb_bsdevDsr._dev_6offset_reg,
		&m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area,
		&m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig,
		&m_usb_bsdevDsr._dev_3DevMode.md2_Snapshot_or_Continuous,
		&m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig,
		&m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig,
		&m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode,
		&m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState,
		&m_usb_bsdevDsr._dev_3DevMode.md7_TerminateExposureNow
	);

	return devtype;
}
int BsdUtils::e2promSaveOrNOT(bool m_iSaveOrNOT)
{
	BYTE iSaveOrNOT = 0x00;
	if (m_iSaveOrNOT)
	{
		iSaveOrNOT = 0xFF;
	}
	
	//BYTE iSaveOrNOT= m_iSaveOrNOT==true
	DllPortUSB20BSSetE2promSaveOrNOT(m_handle, iSaveOrNOT);
	return 1;
}
int BsdUtils::setWorkMode(structBSDevAppDeviceDsr &m_usb_bsdevDsr)
{
	
	return DllPortUSB20BSSetWorkMode(
		m_handle,
		m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area,
		m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig,
		0,
		m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig,
		m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig,
		m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode,
		m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState,
		0
	);
}

int BsdUtils::setAFE(int PGA, int OFF)
{
	return DllPortUSB20BSsetAFE(m_handle, PGA, OFF);
}

int BsdUtils::setIntegrationTime(int it_us)
{
	return DllPortUSB20BSSetIntegrationTime(m_handle, it_us);
}
int BsdUtils::getWaveLength(structBSDevAppDeviceDsr &m_usb_bsdevDsr)
{
	//从 ROM 读取标定文件
	DWORD  *dwdWL = new DWORD[m_usb_bsdevDsr._dev_2Pixels_H];
	BYTE   *bytWL = new BYTE[m_usb_bsdevDsr._dev_2Pixels_H * 4];
	int result = -1;
	float pct = 0;
	for (int l = 0; l < m_usb_bsdevDsr._dev_2Pixels_H * 4 / 64; l++)
	{
		//读取标定文件
		result = DllPortUSB20BSEEPromGet64Bytes(m_handle, 256 + l * 64, bytWL + l * 64);
		pct = (float(l) / float(m_usb_bsdevDsr._dev_2Pixels_H * 4 / 64 - 1)) * float(100.0);
		//printf("%.2f %%\r\n", pct);
	}
	memcpy((BYTE *)dwdWL, bytWL, m_usb_bsdevDsr._dev_2Pixels_H * 4);
	
	
	/*cv::resize(image, image, cv::Size(480, 1));
	std::cout << image.cols << "  " << image.rows << std::endl;*/
	//for (int cols = 0; cols < image.cols; cols++)
	//{
	//	printf("%d %f\r\n", cols, image.at<double>(1, cols));
	//	//callbackData["xalis"].Add((int)image.at<float>(1, cols));
	//}
	//cv::Mat image2(1, m_usb_bsdevDsr._dev_2Pixels_H, CV_8UC3);//创建100*100 无符号8位 3通道矩阵
	
	m_usb_bsdevDsr._app_5pdblWL = new double[DEV_MAX_PIXELS_H];
	if (m_Frame.wavedata == NULL)
	{
		m_Frame.height = m_usb_bsdevDsr._dev_2Pixels_H;
		m_Frame.width = m_usb_bsdevDsr._dev_2Pixels_V;
		m_Frame.isn = m_usb_bsdevDsr._dev_1SN;
		m_Frame.wavedata = new double[m_usb_bsdevDsr._dev_2Pixels_H];
	}
	for (int l = 0; l < m_usb_bsdevDsr._dev_2Pixels_H; l++)
	{
		m_usb_bsdevDsr._app_5pdblWL[l] = (double)dwdWL[l];

		m_usb_bsdevDsr._app_5pdblWL[l] /= 1000.0;
		m_Frame.wavedata[l] = m_usb_bsdevDsr._app_5pdblWL[l];
		//printf("%d %f\r\n",l, m_usb_bsdevDsr._app_5pdblWL[l]);
	}
	
	//memcpy(m_usb_bsdevDsr._devdata_1BackGround.pdblWL, m_usb_bsdevDsr._app_5pdblWL, DEV_MAX_PIXELS_H * sizeof(double));
	//memcpy(m_usb_bsdevDsr._devdata_2CurrentSpecData.pdblWL, m_usb_bsdevDsr._app_5pdblWL, DEV_MAX_PIXELS_H * sizeof(double));
	//system("pause");
	delete[]bytWL;
	delete[]dwdWL;
	dwdWL = NULL;
	bytWL = NULL;
	return result;
}

int BsdUtils::getData(structBSDevAppDeviceDsr &m_usb_bsdevDsr)

{
	m_usb_bsdevDsr._devdata_2CurrentSpecData.xfer = DllPortUSB20BSGetDataLengthPerSnapShot(m_handle);
	m_usb_bsdevDsr._devdata_2CurrentSpecData.imgH = m_usb_bsdevDsr._dev_2Pixels_H;
	m_usb_bsdevDsr._devdata_2CurrentSpecData.imgV = m_usb_bsdevDsr._dev_2Pixels_V;

	int iTimeOut = 0xfffffff;// (m_usb_bsdevDsr._dev_4IntTime_us / 1000) + 3000;
	m_usb_bsdevDsr._devdata_2CurrentSpecData.inData = new BYTE[DEV_MAX_PIXELS_H*DEV_MAX_PIXELS_V * 2];
	m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr = new double[DEV_MAX_PIXELS_H];

	int result = DllPortUSB20BSSnapShotGetData(
		m_handle,
		m_usb_bsdevDsr._devdata_2CurrentSpecData.inData,
		m_usb_bsdevDsr._devdata_2CurrentSpecData.xfer,
		iTimeOut,
		m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr,
		m_usb_bsdevDsr._devdata_2CurrentSpecData.imgH,
		m_usb_bsdevDsr._devdata_2CurrentSpecData.imgV,
		&m_usb_bsdevDsr._devdata_2CurrentSpecData.ADC12bits_C,
		&m_usb_bsdevDsr._devdata_2CurrentSpecData.ADC12bits_S,
		&m_usb_bsdevDsr._devdata_2CurrentSpecData.timeStamp,
		&m_usb_bsdevDsr._devdata_2CurrentSpecData.syncCode);

	if (getFlagGetFrame())
	{
		setFlagGetFrame(false);//不在存储下一帧数据
		//把当前帧数据放入内存中，等待用户读取
		if (m_Frame.framedata == NULL)
		{
			m_Frame.framedata = new double[m_usb_bsdevDsr._dev_2Pixels_H];
		}
		if (m_Frame.framedata != NULL)
		{
			m_Frame.height = m_usb_bsdevDsr._dev_2Pixels_H;
			m_Frame.width = m_usb_bsdevDsr._dev_2Pixels_V;
			memcpy(m_Frame.framedata, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr, m_usb_bsdevDsr._dev_2Pixels_H*8);
			/*for (int index = 0; index < m_usb_bsdevDsr._dev_2Pixels_H; index++)
			{
				printf("%d %f %f\r\n", index, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr[index], m_Frame.framedata[index]);
			}*/

		}

	}
	/*printf("result= %d \r\n", result);
	if (result != 0) 
	{
		for (int l = 0; l < m_usb_bsdevDsr._dev_2Pixels_H; l++)
		{
			if (m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr[l] <= 0)
				return 0;
		}
	}*/
	/*delete[]m_usb_bsdevDsr._devdata_2CurrentSpecData.inData;
	delete[]m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr;

	m_usb_bsdevDsr._devdata_2CurrentSpecData.inData = NULL;
	m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr = NULL;*/
	return result;
}
int BsdUtils::shutDown()
{
	return DllPortUSB20BSSnapShotShutdown(m_handle);
}
bool BsdUtils::setFlagGetFrame(bool value)
{
	IsGetFrame = value;
	return true;
}

bool BsdUtils::getFlagGetFrame()
{
	return IsGetFrame;
}