#ifndef MYOBJECT_H
#define MYOBJECT_H
#include <node.h>


#include <node_object_wrap.h>
//////////////////�ַ�����///////////////////
#include "EncodeUtils.h"
//////////////////�ַ�����///////////////////


//////////////////����json///////////////////
#include "CJsonObject.h"
//////////////////����json///////////////////

//////////////////����opencv///////////////////
#include "OpencvUtils.h"
//////////////////����opencv///////////////////

/////////////////////////////////////
#include <afxdisp.h>   
/////////////////////////////////////

//////////////////����֧��///////////////////
#include "SerialUtils.h"
//////////////////����֧��///////////////////

//////////////////���������֧��///////////////////
#include "BsdUtils.h"

//////////////////���������֧��///////////////////

//////////////////���������֧��///////////////////
#include "LogUtils.h"
//////////////////���������֧��///////////////////

//////////////////�̰߳�ȫ֧��///////////////////
#include "threadsafe_queue.h"
//////////////////�̰߳�ȫ֧��///////////////////


#include <uv.h>
#include <map> 

#define CAMERY_SIZE_R 100
#define CAMERY_RENDER 6
#define RENDER_MAX_FPS 20
#define RENDER_LINE_WIDTH 640
struct AsyncData
{
	std::wstring type;  // ����
	std::wstring message;  // ��Ϣ
	int value;
	neb::CJsonObject data;
};
typedef struct  specData {
	uint8_t * imagedata;
	uint32_t bufSize;
	std::string isn;
	int Width;
	int Height;
};

struct UVElement {
	uv_work_t work;              //libuv
	v8::Persistent<v8::Function> callback;    //javascript callback  <heap>
	neb::CJsonObject data;
	std::string param0;
	std::string param1;
	std::string param2;
	std::string _func;
};

//struct ThreadData {
//	BsdUtils * m_bsd;//���������ľ��
//	uv_async_t g_common_async;
//};

namespace demo {

	class MyObject : public node::ObjectWrap 
	{
	public:
		static void Init(v8::Isolate* isolate);
		static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

	private:
		explicit MyObject();
		~MyObject();

		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void callBack(const v8::FunctionCallbackInfo<v8::Value>& args);
		static v8::Persistent<v8::Function> constructor;

		static double get_wall_time();
	
		/*****************��־�ص�����******************************/
		static AsyncData g_commonasyncData;//
		static v8::Persistent<v8::Function> g_logCallback;
		static void setLogCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void log(v8::Isolate* isolate, const wchar_t * const message);


	private:
		/*�����첽�ص�����*/
		static uv_async_t g_common_async;
		static void eventCommonHandle(uv_async_t* handle);
		static v8::Persistent<v8::Function> g_commonCallback;
		/*�����첽�ص�����*/
		static void startCommonWork(uv_work_t * work);
		static void CommonWorkCompleted(uv_work_t * work, int status);
	private:
		static void getDervicesInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
	private:
		static void openCamery(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void closeCamery(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getOpenedCamery(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getParamByIsn(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void setParamByIsn(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void startView(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void stopview(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getCameryOpenStatus(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getCameryAQStatus(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void exeRemoveBack(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void saveData(const v8::FunctionCallbackInfo<v8::Value>& args);
		
	private:
		static void _funopen_equp_Work(UVElement * element);
		static void _funstopView_Work(UVElement * element);
		static void _funremoveBack_Work(UVElement * element);
		static void _funSaveData_Work(UVElement * element);
	private:
		static bool g_quit;
		static bool is_save;
		//��ȡͼ������
		static UINT WINAPI capture(void *arg);
		static UINT WINAPI captureview(void *arg);

		static uv_async_t g_render_async;
		static void eventCallbackRender(uv_async_t* handle);
		//��¼�������߳���
		static int thread_cout;
	private:
		
		
		

	private:
		
	private:
		static CRITICAL_SECTION   m_cs_0;
	private:
		//��¼�򿪵��豸
		static std::map<std::string, BsdUtils * > m_bsdEntrty;
		//����������
		static std::map<std::string, cv::Mat> m_bsdBack;
		//��¼�ص�����
		static v8::Persistent<v8::Function>Callback;
		static threadsafe_queue<specData> specDataV;
	};

}  // namespace demo

#endif