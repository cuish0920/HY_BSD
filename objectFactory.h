#ifndef MYOBJECT_H
#define MYOBJECT_H
#include <node.h>


#include <node_object_wrap.h>
//////////////////字符编码///////////////////
#include "EncodeUtils.h"
//////////////////字符编码///////////////////


//////////////////引入json///////////////////
#include "CJsonObject.h"
//////////////////引入json///////////////////

//////////////////引入opencv///////////////////
#include "OpencvUtils.h"
//////////////////引入opencv///////////////////

/////////////////////////////////////
#include <afxdisp.h>   
/////////////////////////////////////

//////////////////串口支持///////////////////
#include "SerialUtils.h"
//////////////////串口支持///////////////////

//////////////////光谱仪相机支持///////////////////
#include "BsdUtils.h"

//////////////////光谱仪相机支持///////////////////

//////////////////光谱仪相机支持///////////////////
#include "LogUtils.h"
//////////////////光谱仪相机支持///////////////////

//////////////////线程安全支持///////////////////
#include "threadsafe_queue.h"
//////////////////线程安全支持///////////////////


#include <uv.h>
#include <map> 

#define CAMERY_SIZE_R 100
#define CAMERY_RENDER 6
#define RENDER_MAX_FPS 20
#define RENDER_LINE_WIDTH 640
struct AsyncData
{
	std::wstring type;  // 类型
	std::wstring message;  // 消息
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
//	BsdUtils * m_bsd;//光谱仪器的句柄
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
	
		/*****************日志回调函数******************************/
		static AsyncData g_commonasyncData;//
		static v8::Persistent<v8::Function> g_logCallback;
		static void setLogCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void log(v8::Isolate* isolate, const wchar_t * const message);


	private:
		/*公共异步回调函数*/
		static uv_async_t g_common_async;
		static void eventCommonHandle(uv_async_t* handle);
		static v8::Persistent<v8::Function> g_commonCallback;
		/*公共异步回调函数*/
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
		//获取图像数据
		static UINT WINAPI capture(void *arg);
		static UINT WINAPI captureview(void *arg);

		static uv_async_t g_render_async;
		static void eventCallbackRender(uv_async_t* handle);
		//记录虎启的线程数
		static int thread_cout;
	private:
		
		
		

	private:
		
	private:
		static CRITICAL_SECTION   m_cs_0;
	private:
		//记录打开的设备
		static std::map<std::string, BsdUtils * > m_bsdEntrty;
		//暗背景数据
		static std::map<std::string, cv::Mat> m_bsdBack;
		//记录回调函数
		static v8::Persistent<v8::Function>Callback;
		static threadsafe_queue<specData> specDataV;
	};

}  // namespace demo

#endif