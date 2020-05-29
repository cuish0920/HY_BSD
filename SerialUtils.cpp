#include "SerialUtils.h"
#include <stdio.h>
#include <process.h>

namespace ASYNCSERIAL {
	const UINT SLEEP_TIME_INTERVAL = 5;
	AsyncSerial::AsyncSerial():m_hCom(INVALID_HANDLE_VALUE)
		, m_IsOpen(false), m_Thread(INVALID_HANDLE_VALUE), m_IsRead(false)
	{
		memset(&m_ovWait, 0, sizeof(m_ovWait));
		memset(&m_ovWrite, 0, sizeof(m_ovWrite));
		memset(&m_ovRead, 0, sizeof(m_ovRead));

	}

	AsyncSerial::~AsyncSerial()
	{
	}
	bool AsyncSerial::Isopen() {
		return m_IsOpen;
	}
	bool AsyncSerial::InitCOM(int Port,int buad) {
		if (!openSerialPort(Port)) {
			return false;
		}
		SetupComm(m_hCom, 4096, 4096);//���÷��ͽ��ջ���
		
		DCB dcb;
		GetCommState(m_hCom, &dcb);
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = buad;
		dcb.Parity = 0;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;

		SetCommState(m_hCom, &dcb);//���ô���
		
		PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

		COMMTIMEOUTS ct;
		ct.ReadIntervalTimeout = MAXDWORD;//��ȡ����ʱ����Ϊ��WaitCommEvent�ȴ�����
		ct.ReadTotalTimeoutConstant = 0;  //
		ct.ReadTotalTimeoutMultiplier = 0;//

		ct.WriteTotalTimeoutMultiplier = 500;
		ct.WriteTotalTimeoutConstant = 5000;

		SetCommTimeouts(m_hCom, &ct);


		//�����¼�����
		m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
		m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
		m_ovWait.hEvent = CreateEvent(NULL, false, false, NULL);

		SetCommMask(m_hCom, EV_ERR | EV_RXCHAR);//���ý����¼�
		//������ȡ�߳�
		//m_Thread = (HANDLE)_beginthreadex(NULL, 0, &AsyncSerial::OnRecv, this, 0, NULL);
		m_IsOpen = true;
		return true;
	}
	
	bool AsyncSerial::openSerialPort(int portNo) {

		char szPort[50];
		sprintf_s(szPort, "\\\\.\\COM%d", portNo);
		m_hCom = CreateFileA(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,//�����첽��ʶ
			NULL);
		if (INVALID_HANDLE_VALUE == m_hCom)
		{
			return false;
		}
		return true;
	}
	
	bool AsyncSerial::OpenListenThread(listenCallback m_Callback) {
		/** �����Ƿ��Ѿ��� */
		if (!m_IsOpen) {
			return false;
		}
		/** ����߳��Ƿ��Ѿ������� */
		if (m_Thread != INVALID_HANDLE_VALUE)
		{
			/** �߳��Ѿ����� */
			return false;
		}
		m_Thread = (HANDLE)_beginthreadex(NULL, 0, &AsyncSerial::OnRecv, this, 0, NULL);
		if (!m_Thread)
		{
			return false;
		}
		/** �����̵߳����ȼ�,������ͨ�߳� */
		if (!SetThreadPriority(m_Thread, THREAD_PRIORITY_HIGHEST))
		{
			return false;
		}


		this->m_Callback = m_Callback;
		m_IsRead = true;
		return true;
	}
	bool AsyncSerial::CloseListenTread()
	{
		
		/** ����߳��Ƿ��Ѿ������� */
		if (m_Thread == INVALID_HANDLE_VALUE)
		{
			/** �߳��Ѿ����� */
			return false;
		}
		CloseListenTread();
		m_IsRead = false;
		/** �ȴ��߳��˳� */
		WaitForSingleObject(m_Thread, 5000);//�ȴ��߳̽���
		/** ���߳̾����Ч */
		CloseHandle(m_Thread);
		m_Thread = INVALID_HANDLE_VALUE;
		m_Callback = NULL;
	}
	void AsyncSerial::UninitCOM()
	{
		m_IsOpen = false;
		if (INVALID_HANDLE_VALUE != m_hCom)
		{
			CloseHandle(m_hCom);
			m_hCom = INVALID_HANDLE_VALUE;
		}
		if (NULL != m_ovRead.hEvent)
		{
			CloseHandle(m_ovRead.hEvent);
			m_ovRead.hEvent = NULL;
		}
		if (NULL != m_ovWrite.hEvent)
		{
			CloseHandle(m_ovWrite.hEvent);
			m_ovWrite.hEvent = NULL;
		}
		if (NULL != m_ovWait.hEvent)
		{
			CloseHandle(m_ovWait.hEvent);
			m_ovWait.hEvent = NULL;
		}
		
	}
	int AsyncSerial::ComWriteHex(const char * buf) {
		BOOL rtn = FALSE;
		DWORD WriteSize = 0;
		PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
		m_ovWait.Offset = 0;
		//�޸�Ϊʮ����������
		//�ո�ȥ��
		BYTE *inputData =  hexStr2Bytes((char*)buf);
		if (inputData == NULL) {
			return FALSE;
		}
		rtn = WriteFile(m_hCom, inputData, strlen(buf)/2, &WriteSize, &m_ovWrite);
		delete[] inputData;
		inputData = NULL;
		//rtn = WriteFile(m_hCom, buf, len, &WriteSize, &m_ovWrite);
		int len = 0;
		if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//��̨��ȡ
		{
			//�ȴ�����д�����
			if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
			{
				return false;
			}
		}

		len = WriteSize;
		return rtn != FALSE;
	}

	bool AsyncSerial::ComWrite(LPBYTE buf, int &len) {
		BOOL rtn = FALSE;
		DWORD WriteSize = 0;
		PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
		m_ovWait.Offset = 0;
		rtn = WriteFile(m_hCom, buf, len, &WriteSize, &m_ovWrite);
		len = 0;
		if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//��̨��ȡ
		{
			//�ȴ�����д�����
			if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
			{
				return false;
			}
		}

		len = WriteSize;
		return rtn != FALSE;
	}
	/*���ջص�����*/
	unsigned int __stdcall AsyncSerial::OnRecv(void* LPParam) 
	{
		/** �õ������ָ�� */
		AsyncSerial *obj = reinterpret_cast<AsyncSerial*>(LPParam);
		DWORD WaitEvent = 0, Bytes = 0;
		BOOL Status = FALSE;
		//BYTE ReadBuf[4096];
		DWORD Error;
		COMSTAT cs = { 0 };

		while (obj->m_IsRead) {
			WaitEvent = 0;
			obj->m_ovWait.Offset = 0;
			Status = WaitCommEvent(obj->m_hCom, &WaitEvent, &obj->m_ovWait);
			//BYTE ReadBuf[4096];
			//WaitCommEventҲ��һ���첽���������Ҫ�ȴ�
			if (FALSE == Status && GetLastError() == ERROR_IO_PENDING)//
			{
				//����������������̻߳�ͣ�ڴˣ����hCom�رջ���������False
				Status = GetOverlappedResult(obj->m_hCom, &obj->m_ovWait, &Bytes, TRUE);
			}
			ClearCommError(obj->m_hCom, &Error, &cs);
			if (TRUE == Status //�ȴ��¼��ɹ�
				&& WaitEvent&EV_RXCHAR//�����������ݵ���
				&& cs.cbInQue > 0)//������
			{
				BYTE *ReadBuf = new BYTE[cs.cbInQue];
				if (ReadBuf == NULL) {
					continue;
				}
				Bytes = 0;
				obj->m_ovRead.Offset = 0;
				memset(ReadBuf, 0, cs.cbInQue);
				//printf("%d\r\n", cs.cbInQue);
				//�����Ѿ����ﻺ������ReadFile���ᵱ���첽�������������ȡ������True
				Status = ReadFile(obj->m_hCom, ReadBuf, cs.cbInQue, &Bytes, &obj->m_ovRead);
				if (Status != FALSE)
				{
					/*printf("Bytes:%d", Bytes);
					ReadBuf[Bytes] = '\0';
					printf("%s", ReadBuf);*/
					/*for (int index = 0; index < cs.cbInQue; index++) {
						printf("%c", ReadBuf[index]);
					}*/
					if (obj->m_Callback != NULL) {
						obj->m_Callback(ReadBuf, Bytes);
					}
					//std::cout << "Read:" << ReadBuf << "   Len:" << Bytes << std::endl;
				}
				else {
					PurgeComm(obj->m_hCom, PURGE_RXCLEAR | PURGE_RXABORT);
				}
				
				delete []ReadBuf;
				ReadBuf = NULL;
			}
			else
			{
				
				Sleep(SLEEP_TIME_INTERVAL);
				continue;
			}
		}
		return 0;
	}
	
	BYTE* AsyncSerial::hexStr2Bytes(std::string src)
	{
		char *strEnd;
		int m = 0;
		int len = src.length() / 2;
		BYTE* ret = new BYTE[len];
		if (NULL == ret) {
			return NULL;
		}
		for (int i = 0; i < len; i++)
		{
			m = i * 2;
			std::string subs = src.substr(m, 2);
			ret[i] = strtol(subs.c_str(), &strEnd, 16);
		}
		return ret;
	}
	
	void AsyncSerial::Bytes2HexStr(BYTE *src, int srcLen, BYTE *des)
	{
		BYTE *res;
		int i = 0;

		res = des;
		while (srcLen > 0)
		{
			sprintf((char*)(res + i * 2), "%02X", *(src + i));
			i++;
			srcLen--;
		}
	}
	
};

namespace SYNCSERIAL {

	SyncSerial::SyncSerial():m_hCom(INVALID_HANDLE_VALUE)
		, m_IsOpen(false), m_IsRead(false)
	{
		InitializeCriticalSection(&m_cs);
	}

	SyncSerial::~SyncSerial()
	{
		DeleteCriticalSection(&m_cs);
	}
	bool SyncSerial::openSerialPort(int portNo) {

		char szPort[50];
		sprintf_s(szPort, "\\\\.\\COM%d", portNo);
		m_hCom = CreateFileA(szPort,
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			NULL,
			OPEN_EXISTING,
			0/*ͬ����ʽ�򿪴���*/,
			NULL);
		
		if (INVALID_HANDLE_VALUE == m_hCom)
		{
			return false;
		}
		return true;
	}


	bool SyncSerial::InitCOM(int Port, int buad) {
		if (!openSerialPort(Port)) {
			return false;
		}
		SetupComm(m_hCom, 4096, 4096);//���÷��ͽ��ջ���

		
		DCB dcb;

		GetCommState(m_hCom, &dcb);//���ô���
		//BuildCommDCBA(szDCBparam, &dcb);

		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = buad;
		dcb.Parity = 0;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		//dcb.fRtsControl = RTS_CONTROL_ENABLE;
		SetCommState(m_hCom, &dcb);

		PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);//��ջ���

		COMMTIMEOUTS ct;
		//���ö�ȡ��ʱʱ�䣬��ReadFlie��ȴ�ʱ��
		ct.ReadIntervalTimeout = 0;
		ct.ReadTotalTimeoutConstant = 100;
		ct.ReadTotalTimeoutMultiplier = 200;

		ct.WriteTotalTimeoutMultiplier = 500;
		ct.WriteTotalTimeoutConstant = 5000;

		SetCommTimeouts(m_hCom, &ct);//���ó�ʱ
		
		m_IsOpen = true;
		return true;
	}
	void SyncSerial::UninitCOM()
	{
		m_IsOpen = false;
		if (INVALID_HANDLE_VALUE != m_hCom)
		{
			CloseHandle(m_hCom);
			m_hCom = INVALID_HANDLE_VALUE;
		}
	}


	bool SyncSerial::ComRead(LPBYTE buf, int &len) {
		DWORD ReadSize = 0;
		BOOL rtn = FALSE;

		//���ö�ȡ1���ֽ����ݣ��������������ݵ���ʱ����������أ�����ֱ����ʱ
		rtn = ReadFile(m_hCom, buf, 1, &ReadSize, NULL);

		//����ǳ�ʱrtn=true����ReadSize=0����������ݵ�����ȡһ���ֽ�ReadSize=1
		if (rtn == TRUE && 1 == ReadSize)
		{
			DWORD Error;
			COMSTAT cs = { 0 };
			int ReadLen = 0;
			//��ѯʣ������ֽ�δ��ȡ���洢��cs.cbInQue��
			ClearCommError(m_hCom, &Error, &cs);
			ReadLen = (cs.cbInQue > len) ? len : cs.cbInQue;
			if (ReadLen > 0)
			{
				//����֮ǰ�ȴ�ʱ�Զ�ȡһ���ֽڣ�����buf+1
				rtn = ReadFile(m_hCom, buf + 1, ReadLen, &ReadSize, NULL);
				len = 0;
				if (rtn)
				{
					len = ReadLen + 1;
				}
			}
		}
		PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
		return (rtn != FALSE)&& ReadSize;
	}

	bool SyncSerial::ComWrite(LPBYTE buf, int &len) {
		PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
		BOOL rtn = FALSE;
		DWORD WriteSize = 0;
		rtn = WriteFile(m_hCom, buf, len, &WriteSize, NULL);
		len = WriteSize;
		return rtn != FALSE;
	}

	bool SyncSerial::ComWriteHex(const char* buf) {
		PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
		BOOL rtn = FALSE;
		DWORD WriteSize = 0;
		BYTE *inputData = hexStr2Bytes((char*)buf);
		if (inputData == NULL) {
			return FALSE;
		}
		rtn = WriteFile(m_hCom, inputData, strlen(buf)/2, &WriteSize, NULL);
		delete[] inputData;
		inputData = NULL;
		//int len = WriteSize;
		return rtn != FALSE;
	}
	void SyncSerial::Lock() 
	{
		EnterCriticalSection(&m_cs);
	}
	void SyncSerial::Unlock() 
	{
		LeaveCriticalSection(&m_cs);
	}

	BYTE* SyncSerial::hexStr2Bytes(std::string src)
	{
		char *strEnd;
		int m = 0;
		int len = src.length() / 2;
		BYTE* ret = new BYTE[len];
		if (NULL == ret) {
			return NULL;
		}
		for (int i = 0; i < len; i++)
		{
			m = i * 2;
			std::string subs = src.substr(m, 2);
			ret[i] = strtol(subs.c_str(), &strEnd, 16);
		}
		return ret;
	}

	void SyncSerial::Bytes2HexStr(BYTE *src, int srcLen, BYTE *des)
	{
		BYTE *res;
		int i = 0;

		res = des;
		while (srcLen > 0)
		{
			sprintf((char*)(res + i * 2), "%02X", *(src + i));
			i++;
			srcLen--;
		}
	}

};
