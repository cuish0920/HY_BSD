#include "objectFactory.h"
namespace demo {

	using v8::Context;
	using v8::Function;
	using v8::FunctionCallbackInfo;
	using v8::FunctionTemplate;
	using v8::Isolate;
	using v8::Local;
	using v8::NewStringType;
	using v8::Number;
	using v8::Object;
	using v8::Persistent;
	using v8::String;
	using v8::Value;
	using v8::Exception;


	Persistent<Function> MyObject::constructor;
	v8::Persistent<v8::Function> MyObject::g_logCallback;

	/*�����첽�ص�����*/
	uv_async_t MyObject::g_common_async = { 0 };
	v8::Persistent<v8::Function> MyObject::g_commonCallback;
	AsyncData MyObject::g_commonasyncData;
	
	threadsafe_queue<specData> MyObject::specDataV;
	CRITICAL_SECTION   MyObject::m_cs_0;
	std::map<std::string, BsdUtils * >MyObject::m_bsdEntrty;
	std::map<std::string, cv::Mat> MyObject::m_bsdBack;
	bool MyObject::g_quit= true;
	bool MyObject::is_save = false;
	int MyObject::thread_cout=0;
	uv_async_t MyObject::g_render_async;
	//��¼�ص�����
	v8::Persistent<v8::Function> MyObject::Callback;
	MyObject::MyObject() {

		InitializeCriticalSection(&m_cs_0);//<gps������ʾ
		try {
			LogUtils::init_log("");
			LogUtils::debugger->debug("��־��ʹ���ɹ�");
		}
		catch (...) {

		}
		//�������Թ���
		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);
	}

	MyObject::~MyObject() {
		
		try {
			DeleteCriticalSection(&m_cs_0);//<gps����
			LogUtils::debugger->debug("�ͷ��ڴ�");
		
			

			LogUtils::debugger->debug("�������� shutdown");
			LogUtils::shutdown();
		}
		catch (...) {

		}

	}

	double MyObject::get_wall_time()
	{
		#ifdef _WIN32
			return static_cast<double>(clock()) / static_cast<double>(CLOCKS_PER_SEC);
		#else
			struct timeval time;
			if (gettimeofday(&time, NULL))
			{
				return 0;
			}
			return (double)time.tv_sec + (double)time.tv_usec*0.000001;
		#endif
	}

	void MyObject::Init(Isolate* isolate) {
		
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "MyObject", NewStringType::kNormal).ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);



		NODE_SET_PROTOTYPE_METHOD(tpl, "setLogCallback", setLogCallback);

		NODE_SET_PROTOTYPE_METHOD(tpl, "callBack", callBack);
		//
		NODE_SET_PROTOTYPE_METHOD(tpl, "getDervicesInfo", getDervicesInfo);
		NODE_SET_PROTOTYPE_METHOD(tpl, "openCamery", openCamery);
		NODE_SET_PROTOTYPE_METHOD(tpl, "closeCamery", closeCamery);

		NODE_SET_PROTOTYPE_METHOD(tpl, "startview", startView);
		NODE_SET_PROTOTYPE_METHOD(tpl, "stopview", stopview);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getCameryOpenStatus", getCameryOpenStatus);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getCameryAQStatus", getCameryAQStatus);
		NODE_SET_PROTOTYPE_METHOD(tpl, "exeRemoveBack", exeRemoveBack);
		NODE_SET_PROTOTYPE_METHOD(tpl, "saveData", saveData);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getOpenedCamery", getOpenedCamery);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getParamByIsn", getParamByIsn);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setParamByIsn", setParamByIsn);
		
		
		Local<Context> context = isolate->GetCurrentContext();
		constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());

		uv_async_init(uv_default_loop(), &g_render_async, eventCallbackRender);
		uv_async_init(uv_default_loop(), &g_common_async, eventCommonHandle);//
	}

	void MyObject::New(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		Local<Context> context = isolate->GetCurrentContext();

		if (args.IsConstructCall()) {
			// ���캯��һ�����ã�`new MyObject(...)`
			double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue(context).FromMaybe(0);
			MyObject* obj = new MyObject();
			obj->Wrap(args.This());
			args.GetReturnValue().Set(args.This());
		}
		else {
			// ����ͨ���� `MyObject(...)` һ�����ã�תΪ������á�
			const int argc = 1;
			Local<Value> argv[argc] = { args[0] };
			Local<Function> cons = Local<Function>::New(isolate, constructor);
			Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
			args.GetReturnValue().Set(instance);
		}
	}

	void MyObject::NewInstance(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		const unsigned argc = 1;
		Local<Value> argv[argc] = { args[0] };
		Local<Function> cons = Local<Function>::New(isolate, constructor);
		Local<Context> context = isolate->GetCurrentContext();
		Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();

		args.GetReturnValue().Set(instance);
	}

	void MyObject::callBack(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (args.Length() < 1) {
			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "invalid parameter[func]";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}

	

		// �����һ���������Ǻ��������׳�����
		if (!args[0]->IsFunction())
		{
			v8::MaybeLocal<String> message = v8::String::NewFromUtf8(isolate, EncodeUtils::toUtf8String(L"The first argument must be a fun!").c_str());

			isolate->ThrowException(v8::Exception::TypeError(message.ToLocalChecked()));
			return;
		}
		//v8::String::Utf8Value param_type(args[0]->ToString());
		//std::string isn = std::string(*param_type);//���ϵͳ�������
		//isn = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(isn));
		//LogUtils::debugger->debug("callback %s", data_d.c_str());
		//�����߳���������ݲɼ�������ͨ���ص�������ʾ
	
		
		Callback.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
		
		// ��ȡ�ص���������������ȫ�ֱ���
		//Local<Function> frameDataCallback = v8::Local<v8::Function>::Cast(args[1]);

	}

	
	void MyObject::setLogCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();

		// �����һ���������Ǻ��������׳�����
		if (!args[0]->IsFunction())
		{
			v8::MaybeLocal<String> message = v8::String::NewFromUtf8(isolate, EncodeUtils::toUtf8String(L"The first argument must be a function!").c_str());
			isolate->ThrowException(v8::Exception::TypeError(
				message.ToLocalChecked()));
			return;
		}

		// ��ȡ�ص���������������ȫ�ֱ���
		auto logCallback = v8::Local<v8::Function>::Cast(args[0]);
		g_logCallback.Reset(isolate, logCallback);

		log(isolate, L"Set log callback success!");
	}
	
	void MyObject::log(v8::Isolate* isolate, const wchar_t * const message)
	{
		std::string messageUtf8 = EncodeUtils::toUtf8String(message);
		v8::MaybeLocal<v8::String> argv = v8::String::NewFromUtf8(isolate, messageUtf8.c_str());
		v8::Local<v8::Value> value = argv.ToLocalChecked();
		v8::Local<v8::Function> logCallback = v8::Local<v8::Function>::New(isolate, g_logCallback);
		logCallback->Call(isolate->GetCurrentContext(), v8::Null(isolate), 1, &value);
	}

	/*
	*** UVElement  * element = new UVElement();
	*** element->work.data = element;
	*** element->param0 = data_d;
	*** element->callback.Reset(isolate, Local<Function>::Cast(args[1]));//������ص�����
	*** element->_func = "_funcName";
	*** ������Ȼص�����
	*** uv_queue_work(uv_default_loop(), &element->work, startCommonWork, CommonWorkCompleted);
	*/
	void MyObject::startCommonWork(uv_work_t * work) {
		UVElement * element = (UVElement*)work->data;

		try {
			if (element->_func.compare("open_equp") == 0) {
				//ִ�й��������ļ����βü�
				_funopen_equp_Work(element);
			}
			else if (element->_func.compare("stop_view") == 0) {
				_funstopView_Work(element);
			}
			else if (element->_func.compare("remove_back") == 0) {
				_funremoveBack_Work(element);
			}
			else if (element->_func.compare("save_data") == 0) {
				_funSaveData_Work(element);
			}
			//else if (element->_func.compare("spe2reflect") == 0) {
			//	_funSpe2Reflect_Work(element);
			//}
			//else if (element->_func.compare("ndvi") == 0) {

			//	_funNdvi_Work(element);
			//}


		}
		catch (...) {
			LogUtils::debugger->error("����������[%s]����", element->_func.c_str());
		}

	}

	void  MyObject::CommonWorkCompleted(uv_work_t * work, int status) {
		Isolate * isolate = Isolate::GetCurrent();
		UVElement * element = (UVElement*)work->data;
		const unsigned argc = 1;

		v8::Local<v8::Value> argv[1] = {
			//�ص����ĺ�������
			String::NewFromUtf8(isolate,EncodeUtils::toUtf8String(EncodeUtils::StringToWString(element->data.ToString()).c_str()).c_str(),NewStringType::kNormal).ToLocalChecked()
		};
		//Local<Value> argv[argc] = { String::NewFromUtf8(isolate,element->param1.c_str()).ToLocalChecked() };
		Local<Function>::New(isolate, element->callback)->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), argc, argv);
		element->callback.Reset();
		delete element;
	}

	/**************************************************
	***@author:cui_sh
	***@brief:������
	***@time:��ȡ�豸�б��е����
	***@version:
	**************************************************/
	void MyObject::getDervicesInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();

		std::string imperx_info = BsdUtils::getDervicesInfo();
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, imperx_info.c_str(), NewStringType::kNormal).ToLocalChecked());
	}
	/**************************************************
	***@author:cui_sh
	***@brief:�����ص���������
	***@time:
	***@version:
	**************************************************/
	void MyObject::eventCommonHandle(uv_async_t* handle)
	{
		Isolate*  isolate = v8::Isolate::GetCurrent();
		AsyncData *asyncData = reinterpret_cast<AsyncData*>(handle->data);
		v8::Local<v8::Value> argv[1] = {
			//�ص����ĺ�������
			String::NewFromUtf8(isolate,EncodeUtils::toUtf8String(EncodeUtils::StringToWString(asyncData->data.ToString()).c_str()).c_str(),NewStringType::kNormal).ToLocalChecked()
		};
		v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, g_commonCallback);
		cb->Call(isolate->GetCurrentContext(), v8::Null(isolate), 1, argv);
	}

	/**************************************************
	***@author:cui_sh
	***@brief:�򿪹�������
	***@time:
	***@version:
	**************************************************/
	void MyObject::openCamery(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (args.Length() < 3) {
			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "invalid parameter[str,func,func]";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		//v8::MaybeLocal<String> message = v8::String::NewFromUtf8(isolate, EncodeUtils::toUtf8String(L"The first argument must be a fun!").c_str());
		

		
		v8::String::Utf8Value param_type(isolate, args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
		//v8::String::Utf8Value param_type();
		std::string data_d = std::string(*param_type);//���ϵͳ�������
		data_d = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(data_d));
		//LogUtils::debugger->debug("param_type %s", data_d.c_str());
		UVElement  * element = new UVElement();
		element->work.data = element;
		element->param0 = data_d;
		//element->param1 = data_d;
		element->callback.Reset(isolate, Local<Function>::Cast(args[2]));
		element->_func = "open_equp";//�򿪹�����

		auto logCallback = v8::Local<v8::Function>::Cast(args[1]);
		g_commonCallback.Reset(isolate, logCallback);

		uv_queue_work(uv_default_loop(), &element->work, startCommonWork, CommonWorkCompleted);
	}


	void MyObject::closeCamery(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (!g_quit)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					EncodeUtils::toUtf8String(L"error! Data acquisition").c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		if (is_save)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					EncodeUtils::toUtf8String(L"error! Data rec").c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		try
		{
			LogUtils::debugger->debug("closeCamery[1]");
			
			Sleep(200);
			//LogUtils::debugger->debug("closeCamery[1]");
			/*��Ҫ��У�飬�豸�򿪲���ִ��*/
			if (!m_bsdEntrty.empty())
			{
				//LogUtils::debugger->debug("closeCamery[2]");
				std::map<std::string, BsdUtils*>::iterator iter;
				for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++) {
					iter->second->closeDevice();
					delete iter->second;
					iter->second = NULL;
					//std::cout << iter->first << "-" << iter->second << std::endl;
				}
				m_bsdEntrty.clear();
				
				std::map<std::string, cv::Mat>::iterator iterback;
				for (iterback = m_bsdBack.begin(); iterback != m_bsdBack.end(); iterback++) {
					iterback->second.release();
				}
				m_bsdBack.clear();

				//LogUtils::debugger->debug("closeCamery[4]");
				std::string msg = "success";
				msg = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(msg));
				args.GetReturnValue().Set(String::NewFromUtf8(isolate, msg.c_str(), NewStringType::kNormal).ToLocalChecked());
			}
			else
			{
				std::string error = "�豸�Ѿ��ر�";
				error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
				isolate->ThrowException(Exception::TypeError(
					String::NewFromUtf8(isolate,
						error.c_str(),
						NewStringType::kNormal).ToLocalChecked()));
				return;
			}
		}
		catch (...)
		{
			LogUtils::error->error("error[closeCamery]");
		}
		


	}


	/**************************************************
	***@author:cui_sh
	***@brief:�򿪹������� ���δ�
	***@time:
	***@version:
	**************************************************/
	void MyObject::_funopen_equp_Work(UVElement * element)
	{
		try
		{
			neb::CJsonObject param(element->param0);
			neb::CJsonObject out_param;
			//LogUtils::debugger->debug("%s",element->param0.c_str());
			int elementSize = param["result"].GetArraySize();//���Ҫ�򿪵��豸������
			//�������ڼ�¼���������Ķ���
			std::string isn;
			std::string des;
			std::map<std::string, BsdUtils*>::iterator iter;
			for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++) {
				iter->second->closeDevice();
				delete iter->second;
				//std::cout << iter->first << "-" << iter->second << std::endl;
			}
			m_bsdEntrty.clear();


			std::map<std::string, cv::Mat>::iterator iterback;
			for (iterback = m_bsdBack.begin(); iterback != m_bsdBack.end(); iterback++) {
				iterback->second.release();
			}
			m_bsdBack.clear();


			for (int index = 0; index < elementSize; index++)
			{
				BsdUtils * m_bsd = new BsdUtils();
				param["result"][index].Get("ISN", isn);
				param["result"][index].Get("DES", des);
				if (!m_bsd->openDevice(index))
				{
					delete m_bsd;
					m_bsd = NULL;

					LogUtils::debugger->debug("���豸ʧ�� %s", isn);
					return;
				}

				neb::CJsonObject callbackData;
				callbackData.Add("ISN", isn);
				callbackData.Add("DES", des);
				callbackData.Add("total", elementSize);
				callbackData.Add("current", index + 1);
				structBSDevAppDeviceDsr m_usb_bsdevDsr;
				m_bsd->getDevInfo(m_usb_bsdevDsr);
				m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area = 0;
				m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig = 0;
				m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig = 0;
				m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig = 0;
				m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode = 0;
				m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState = 0;
				//m_bsd->setWorkMode(m_usb_bsdevDsr);
				//m_bsd->setAFE();
				//��ȡ�궨����
				m_bsd->getWaveLength(m_usb_bsdevDsr);
				//��Ҫһ�������õ�ǰ֡����


				callbackData.AddEmptySubArray("xalis");
				LogUtils::debugger->debug(isn);
				cv::Mat image(1, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1, m_usb_bsdevDsr._app_5pdblWL);
				//std::cout << image << std::endl;
				cv::resize(image, image, cv::Size(RENDER_LINE_WIDTH, 1));
				for (int l = 0; l < image.cols; l++)
				{

					callbackData["xalis"].Add((int)image.at<double>(0, l));
				}
				image.release();
				//��������������


				//for (int l = 0; l < m_usb_bsdevDsr._dev_2Pixels_H; l++)
				//{
				//	
				//	//callbackData["xalis"].Add((int)m_usb_bsdevDsr._app_5pdblWL[l]);
				//	printf("%d %f\r\n", l, m_usb_bsdevDsr._app_5pdblWL[l]);
				//}
				callbackData.Add("percentage_value", (int)(index + 1) / elementSize * 100.0);

				delete[] m_usb_bsdevDsr._app_5pdblWL;
				m_usb_bsdevDsr._app_5pdblWL = NULL;
				m_bsdEntrty.insert(std::pair<std::string, BsdUtils*>(isn, m_bsd));

				//׼����������Ŷ���

				//cv::Mat zero = cv::Mat::zeros(m_usb_bsdevDsr._dev_2Pixels_V, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1);
				//zero.copyTo(iterback->second);

				m_bsdBack.insert(std::pair<std::string, cv::Mat>(isn, cv::Mat::zeros(m_usb_bsdevDsr._dev_2Pixels_V, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1)));

				out_param.Add("ISN", isn);
				out_param.Add("DES", des);

				//callbackData.Add("xails", );
				g_commonasyncData.type = L"open_equp";
				g_commonasyncData.data = callbackData;
				g_common_async.data = &g_commonasyncData;
				uv_async_send(&g_common_async);

			}
			element->data = out_param;
		}
		catch (...)
		{
			printf("������\r\n");
		}
		
	}
	void MyObject::stopview(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate*  isolate = v8::Isolate::GetCurrent();
		if (is_save)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					EncodeUtils::toUtf8String(L"error! Data rec").c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		//û�б���ʱ��Ԥ����
		if (g_quit)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					EncodeUtils::toUtf8String(L"error!").c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		
		try
		{
			//LogUtils::debugger->debug("stopview");
			//�߳��еȴ��˳����
			UVElement  * element = new UVElement();
			element->work.data = element;
			element->callback.Reset(isolate, Local<Function>::Cast(args[0]));
			element->_func = "stop_view";//�򿪹�����
			uv_queue_work(uv_default_loop(), &element->work, startCommonWork, CommonWorkCompleted);


			
			std::string msg = "success";
			msg = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(msg));
			args.GetReturnValue().Set(String::NewFromUtf8(isolate, msg.c_str(), NewStringType::kNormal).ToLocalChecked());
		}
		catch (...)
		{
			LogUtils::error->error("error[stopview]");
		}

	}
	/**************************************************
	***@author:cui_sh
	***@brief:�򿪹������� ���δ�
	***@time:
	***@version:
	**************************************************/
	void MyObject::_funstopView_Work(UVElement * element)
	{
		neb::CJsonObject out;
	

		//
		std::map<std::string, BsdUtils*>::iterator iter;
		for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++)
		{
			iter->second->shutDown();
		
			
		}
		g_quit = true;
		while (thread_cout)
		{
			LeaveCriticalSection(&m_cs_0);
			//std::cout << "wait exit " << thread_cout <<std::endl;
			Sleep(200);
		}
		//��ԭ����
		
		//��ն����д���ʾ������
		while (!specDataV.empty())
		{
			specData m_data;
			if (specDataV.try_pop(m_data))
			{
				delete[] m_data.imagedata;
				m_data.imagedata = NULL;
			}
			Sleep(1);
		}
		out.Add("msg","success");
		element->data = out;
	}
	
	void MyObject::startView(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate*  isolate = v8::Isolate::GetCurrent();
		if (m_bsdEntrty.empty())
		{
			std::string error = "Device is not open";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}

		if (!g_quit)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					EncodeUtils::toUtf8String(L"error! stream is open").c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		g_quit = false;

		std::map<std::string, BsdUtils*>::iterator iter;
		thread_cout = 0;
		for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++) 
		{
			//����v8�첽��Ⱦ����
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, captureview, NULL, 0, NULL));

			//�������ݻ�ȡ�߳�
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, capture, iter->second, 0, NULL));
			thread_cout++;
		}
		//����
		args.GetReturnValue().Set(String::NewFromUtf8(isolate,"success", NewStringType::kNormal).ToLocalChecked());
	}

	UINT WINAPI MyObject::capture(void *arg)
	{
		BsdUtils *obj = reinterpret_cast<BsdUtils*>(arg);
		//��Ҫ֪����ȡ��̨�豸������
		structBSDevAppDeviceDsr m_usb_bsdevDsr;
		obj->getDevInfo(m_usb_bsdevDsr);
		/*Isolate*  isolate = v8::Isolate::GetCurrent();
		std::map<std::string, v8::Local<v8::Function>>::iterator iter = m_callbackFun.find(m_usb_bsdevDsr._dev_1SN);
		v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, iter->second);
		*/
		std::map<std::string, cv::Mat>::iterator iterback = m_bsdBack.find(m_usb_bsdevDsr._dev_1SN);
		//bool trigger_t = false;
		double time_stamp = get_wall_time();
		int index = 0;
		while (true)
		{
			EnterCriticalSection(&m_cs_0);
			if (0 != obj->getData(m_usb_bsdevDsr))
			{
				cv::Mat image(1, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr);
				////ԭʼ���ݿ������ڱ���
				//cv::Mat image(1, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1, cv::Scalar(rand() % 5000));
				cv::absdiff(image, iterback->second, image);
				cv::resize(image, image, cv::Size(RENDER_LINE_WIDTH, 1));
				//LogUtils::debugger->debug("--start--%s", m_usb_bsdevDsr._dev_1SN);
				//for (int l = 0; l < m_usb_bsdevDsr._dev_2Pixels_H; l++) 
				//{
				//	LogUtils::debugger->debug("%f ", m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr[l]);
				//	//printf("%d %f\r\n", l, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr[l]);
				//}
				//LogUtils::debugger->debug("--end--%s", m_usb_bsdevDsr._dev_1SN);
				specData v_d;
				v_d.bufSize = image.cols * image.elemSize();
				v_d.Height = image.rows;
				v_d.Width = image.cols;
				v_d.imagedata = new byte[v_d.bufSize];
				v_d.isn = m_usb_bsdevDsr._dev_1SN;
				memcpy(v_d.imagedata, image.data, v_d.bufSize);
				specDataV.push(v_d);
				//std::cout << image << std::endl;

				/*specData v_d;
				v_d.bufSize = m_usb_bsdevDsr._dev_2Pixels_H*4;
				v_d.Height = m_usb_bsdevDsr._dev_2Pixels_V;
				v_d.Width = m_usb_bsdevDsr._dev_2Pixels_H;
				v_d.imagedata = new byte[v_d.bufSize];
				v_d.isn = m_usb_bsdevDsr._dev_1SN;
				memcpy(v_d.imagedata, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr, v_d.bufSize);
				specDataV.push(v_d);*/

				image.release();

				/*specData v_d;
				v_d.bufSize = m_usb_bsdevDsr._dev_2Pixels_H;
				v_d.Height = 1;
				v_d.Width = m_usb_bsdevDsr._dev_2Pixels_H;
				v_d.imagedata = new uint8_t[v_d.bufSize];
				v_d.isn = m_usb_bsdevDsr._dev_1SN;
				memcpy(v_d.imagedata, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr, v_d.bufSize);
				specDataV.push(v_d);*/
				
				//�����ݷ�����л����� 
				
				/*double _time = get_wall_time();
				printf("%.02f fps\r\n", 1.f / (_time - time_stamp));
				time_stamp = _time;*/
				//delete[] v_d.imagedata;
				//std::cout << "���ݻص�_dev_2Pixels_H" << m_usb_bsdevDsr._dev_2Pixels_H << std::endl;
				//std::cout << "���ݻص�_dev_2Pixels_H" << m_usb_bsdevDsr._dev_2Pixels_V << std::endl;
				//std::cout <<"���ݻص�" <<m_usb_bsdevDsr._dev_1SN <<std::endl;
			}
			/*else
			{
				std::cout <<"���ݻص�������" <<m_usb_bsdevDsr._dev_1SN <<std::endl;

			}*/
			
			delete[]m_usb_bsdevDsr._devdata_2CurrentSpecData.inData;
			delete[]m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr;
			m_usb_bsdevDsr._devdata_2CurrentSpecData.inData = NULL;
			m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr = NULL;
			if (g_quit)
			{
				
				LeaveCriticalSection(&m_cs_0);
				break;
			}
			LeaveCriticalSection(&m_cs_0);
			Sleep(1);
		}

		/*if (trigger_t)
		{
			m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig = 1;
			obj->setWorkMode(m_usb_bsdevDsr);
			trigger_t = false;
		}*/
		thread_cout--;
		//std::cout << "�˳�" << m_usb_bsdevDsr._dev_1SN << std::endl;
		return 0;
	}


	void MyObject::eventCallbackRender(uv_async_t* handle)
	{
		try
		{
			Isolate*  isolate = v8::Isolate::GetCurrent();
			//AsyncData *asyncData = reinterpret_cast<AsyncData*>(handle->data);
			//��ISN�ص� ��ʾ 
			if (!specDataV.empty())
			{


				specData m_data;
				if (specDataV.try_pop(m_data))
				{
					//if (m_data.isn == "01891809")
					{
						v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, Callback);

						//v8::Local<v8::SharedArrayBuffer>  buffer = v8::SharedArrayBuffer::New(isolate, m_data.imagedata, m_data.bufSize);
						//v8::Local<v8::Uint16Array> buffdata = v8::Uint16Array::New(buffer, 0, m_data.bufSize);

						v8::Local<v8::SharedArrayBuffer> spbuffer = v8::SharedArrayBuffer::New(isolate, m_data.imagedata, m_data.bufSize);
						v8::Local<v8::Float64Array> spbuffdata = v8::Float64Array::New(spbuffer, 0, m_data.bufSize / 8);
						

						v8::Local<v8::Value> argv[2] = { String::NewFromUtf8(isolate,m_data.isn.c_str(),NewStringType::kNormal).ToLocalChecked(),spbuffdata };
						cb->Call(isolate->GetCurrentContext(), v8::Null(isolate), 2, argv);

						delete [] m_data.imagedata;
						m_data.imagedata = NULL;
					}
					

					//std::cout << "���ݻص���ʾ ISN:" << m_data.isn << std::endl;
					//���ûص����������Ⱦ
				}
			}
		}
		catch (...)
		{
			std::cout << "���ݻص���ʾ ������" << std::endl;
		}
		
	}

	UINT WINAPI MyObject::captureview(void *arg)
	{
		try
		{
			while (!g_quit)
			{
				if (!specDataV.empty() && specDataV.size() > CAMERY_SIZE_R)
				{
					LogUtils::debugger->error("�ڴ�ռ�ù��� �ͷ�%d", CAMERY_SIZE_R);
					for (int index = 0; index < CAMERY_SIZE_R; index++)
					{
						specData m_data;
						if (specDataV.try_pop(m_data)) 
						{
							delete[] m_data.imagedata;
							m_data.imagedata = NULL;
						}
						

					}

				}
				//if ((!specDataV.empty()) && specDataV.size() > CAMERY_RENDER)
				//{
				//	/*for (int index = 0; index < CAMERY_RENDER/2; index++)
				//	{
				//		specData m_data;
				//		if (specDataV.try_pop(m_data))
				//		{
				//			delete[] m_data.imagedata;
				//			m_data.imagedata = NULL;
				//		}
				//	}*/
				//	//��Ⱦ
				//	uv_async_send(&g_render_async);
				//}
				uv_async_send(&g_render_async);
				Sleep(1);
			}
		}
		catch (...)
		{

		}
		

		return 0;
	}

	/**************************************************
	***@author:cui_sh
	***@brief:���ص�ǰ��Ƶ��״̬
	***@time:
	***@version:
	**************************************************/
	void MyObject::getCameryOpenStatus(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (m_bsdEntrty.empty())
		{
			args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
			return;
		}
		args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
		
	}
	/**************************************************
	***@author:cui_sh
	***@brief:���ص�ǰ��Ƶ��״̬
	***@time:
	***@version:
	**************************************************/
	void MyObject::getCameryAQStatus(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, !g_quit));
	}
	/**************************************************
	***@author:cui_sh
	***@brief:�۳�����������
	***@time:
	***@version:
	**************************************************/
	void MyObject::exeRemoveBack(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (args.Length() < 3) {
			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "invalid parameter[str,func,func]";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		v8::String::Utf8Value param_type(isolate, args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
		std::string data_d = std::string(*param_type);//���ϵͳ�������
		data_d = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(data_d));

		UVElement  * element = new UVElement();
		element->work.data = element;
		element->param0 = data_d;
		element->callback.Reset(isolate, Local<Function>::Cast(args[2]));
		element->_func = "remove_back";//�򿪹�����

		auto logCallback = v8::Local<v8::Function>::Cast(args[1]);
		g_commonCallback.Reset(isolate, logCallback);

		uv_queue_work(uv_default_loop(), &element->work, startCommonWork, CommonWorkCompleted);
	}

	/**************************************************
	***@author:cui_sh
	***@brief:���ݻص�
	***@time:
	***@version:
	**************************************************/
	void MyObject::saveData(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (args.Length() < 3) {
			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "invalid parameter[str,func,func]";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}
		if (m_bsdEntrty.empty())
		{
			std::string error = "unknow exception!";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
		}

		v8::String::Utf8Value param_type(isolate, args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
		//v8::String::Utf8Value param_type(args[0]->ToString());
		std::string data_d = std::string(*param_type);//���ϵͳ�������
		data_d = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(data_d));

		UVElement  * element = new UVElement();
		element->work.data = element;
		element->param0 = data_d;
		element->callback.Reset(isolate, Local<Function>::Cast(args[2]));
		element->_func = "save_data";//��������

		auto logCallback = v8::Local<v8::Function>::Cast(args[1]);
		g_commonCallback.Reset(isolate, logCallback);

		uv_queue_work(uv_default_loop(), &element->work, startCommonWork, CommonWorkCompleted);
	}

	/**************************************************
	***@author:cui_sh
	***@brief:���ݴ���
	***@time:
	***@version:
	**************************************************/
	void MyObject::_funSaveData_Work(UVElement * element)
	{
		std::ofstream write_frame_data;
		//int error = 0;
		try
		{
			//���ݴ洢 /��ָ��Ŀ¼�� �Ե�ǰ����isn�洢
			neb::CJsonObject out;
			char buf[512];
			std::string data_d = element->param0;
			CTime t = CTime::GetCurrentTime();

			neb::CJsonObject callbackData;

			int current = 0;
			double percentage_value = 0;
			std::map<std::string, BsdUtils*>::iterator iter;
			for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++)
			{
				//EnterCriticalSection(&m_cs_0);
				iter->second->setFlagGetFrame(true);//���ô洢��ʶ
				//LeaveCriticalSection(&m_cs_0);

				std::cout << iter->first << " ���ô����ʶ" << std::endl;
				percentage_value = current + 1;
				percentage_value = percentage_value / m_bsdEntrty.size() *100.0;
				callbackData.Clear();
				callbackData.Add("percentage_value", (int)percentage_value);
				callbackData.Add("current", current + 1);
				callbackData.Add("total", m_bsdEntrty.size());
				callbackData.Add("ISN", iter->first);
				callbackData.Add("DES", "���ô����ʶ");

				g_commonasyncData.type = L"save_data";
				g_commonasyncData.data = callbackData;
				g_common_async.data = &g_commonasyncData;

				uv_async_send(&g_common_async);
				current++;
			}

			//�ȴ����ݴ��洢���
			int time_start = get_wall_time();
			bool time_out = false;
			//current = 0;
			for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++)
			{
				time_out = false;
				//iter->second->setFlagGetFrame(true);//���ô洢��ʶ
				std::cout << iter->first << " �ȴ�����" << std::endl;
				while (iter->second->getFlagGetFrame())
				{
					//if(iter->second->getFlagGetFrame())
					if ((get_wall_time() - time_start) >= 2)
					{
						time_out = true;
						break;
					}
					Sleep(100);
				}

				//error = 1;
				std::cout << iter->first << " ��ʼ����" << std::endl;

				FrameStruct m_Frame = iter->second->m_Frame;
				if (data_d.compare("") == 0 || data_d.length() == 0)
				{
					sprintf(buf, "./data/%04d%02d%02d%02d%02d%02d/%04d%02d%02d%02d%02d%02d_isn%s.txt", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), m_Frame.isn.c_str());
				}
				else
				{
					sprintf(buf, "%s/%04d%02d%02d%02d%02d%02d/%04d%02d%02d%02d%02d%02d_isn%s.txt", data_d.c_str(), t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), m_Frame.isn.c_str());
				}
				//error = 2;
				EncodeUtils::createDirectory(buf);
				//write_frame_data.clear();
				write_frame_data.open(buf, std::fstream::out);
				//volatile
				std::map<std::string, cv::Mat>::iterator iterback = m_bsdBack.find(iter->first);
				//cv::Mat zero = cv::Mat::zeros(1, m_Frame.height, CV_64FC1);
				write_frame_data << "#ISN:��������("<< iter->first<<")" << std::endl;
				write_frame_data << "#width:" << m_Frame.width << std::endl;
				write_frame_data << "#height:" << m_Frame.height << std::endl;
				write_frame_data << "#�궨����:�Ӷ�Ӧ�����ж����Ĳ�����Ϣ" << std::endl;
				write_frame_data << "# DNֵ:��ǰ֡����DNֵ" << std::endl;
				write_frame_data << "# ������DNֵ:��δ���а������۳������Ϊ��" << std::endl;
				write_frame_data << "[�궨���� DNֵ ������DNֵ]" << std::endl;
				//std::cout<< zero << std::endl;
				//��ʼ���ݴ���
				if (!time_out)
				{
					//error = 4;
					//������ȡ����   //�������۳�
					for (int index = 0; index < m_Frame.height; index++)
					{
						sprintf(buf,"%f %f %f\n",m_Frame.wavedata[index], m_Frame.framedata[index], iterback->second.at<double>(0, index));
						write_frame_data << buf;
						//printf("%f ", zero.at<double>(0, index));
						//���ݴ洢
						//write_frame_data << m_Frame.wavedata[index] << " ";  //ָ�����,����ַ���,�հ״���''���
						//write_frame_data << m_Frame.framedata[index] << std::endl;
					}
					//printf("\r\n");
					//error =5;
				}
				else
				{
					//error = 6;
					//�������ݳ�ʱ
					write_frame_data << m_Frame.isn << ":�������ݶ�ȡ��ʱ" << std::endl;
				}
				//zero.release();
				write_frame_data.flush();
				write_frame_data.close();
				//error = 7;
				percentage_value = current + 1;
				percentage_value = percentage_value / m_bsdEntrty.size() * 100;
				callbackData.Clear();
				callbackData.Add("percentage_value", (int)percentage_value);
				callbackData.Add("current", current + 1);
				callbackData.Add("total", m_bsdEntrty.size());
				callbackData.Add("ISN", iter->first);
				callbackData.Add("DES", "���ݴ���");

				g_commonasyncData.type = L"save_data";
				g_commonasyncData.data = callbackData;
				g_common_async.data = &g_commonasyncData;

				uv_async_send(&g_common_async);

				current++;
				//error = 8;
			}
			out.Add("msg", "success");
			element->data = out;
		}
		catch (...)
		{
			if (write_frame_data.is_open())
			{
				write_frame_data.flush();
				write_frame_data.close();
			}
			printf("������\r\n");
		}
		
	}


	/**************************************************
	***@author:cui_sh
	***@brief:�۳�������
	***@time:
	***@version:
	**************************************************/
	void MyObject::_funremoveBack_Work(UVElement * element)
	{
		neb::CJsonObject inputData(element->param0);
		//printf("_funremoveBack_Work %s", element->param0.c_str());
		int IsRemoveBackground;
		int RemoveBackgroundNumber;
		inputData.Get("IsRemoveBackground", IsRemoveBackground);
		inputData.Get("RemoveBackgroundNumber", RemoveBackgroundNumber);
		
		//printf("_funremoveBack_Work %s %d %d \r\n", element->param0.c_str(), IsRemoveBackground, RemoveBackgroundNumber);
		/*callbackData.Add("ISN", isn);
		callbackData.Add("DES", des);
		callbackData.Add("total", elementSize);
		callbackData.Add("current", index + 1);*/

		//callbackData.Add("xails", );
		
	
	

		neb::CJsonObject out;
		if (IsRemoveBackground == 1)
		{
			//��ɰ������۳�����
			std::map<std::string, BsdUtils*>::iterator iter;
			int current = 0;
			for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++)
			{
				neb::CJsonObject callbackData;

				structBSDevAppDeviceDsr m_usb_bsdevDsr;
				iter->second->getDevInfo(m_usb_bsdevDsr);
				
				std::map<std::string, cv::Mat>::iterator iterback = m_bsdBack.find(m_usb_bsdevDsr._dev_1SN);
				for (int index = 0; index < RemoveBackgroundNumber; index++)
				{
					//ѭ��ָ���Ĵ�����ȡ����������
					EnterCriticalSection(&m_cs_0);
					if (0 != iter->second->getData(m_usb_bsdevDsr))
					{
						cv::Mat image(m_usb_bsdevDsr._dev_2Pixels_V, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1, m_usb_bsdevDsr._devdata_2CurrentSpecData.pDblPwr);
						
						if (index == 0)
						{
							cv::add(image, iterback->second, iterback->second);
						}
						else
						{
							cv::add(image, iterback->second, iterback->second);
							cv::Mat divMat(image.rows, image.cols, CV_64FC1, cv::Scalar(2));
							cv::divide(iterback->second, divMat, iterback->second);
							divMat.release();

						}
						image.release();
					}
					double percentage_value = RemoveBackgroundNumber;
					percentage_value = (index + 1) / percentage_value * 100.0;
					
					callbackData.Add("percentage_value", (int)percentage_value);
					callbackData.Add("current", current + 1);
					callbackData.Add("total", m_bsdEntrty.size());
					callbackData.Add("ISN", m_usb_bsdevDsr._dev_1SN);
					callbackData.Add("DES", m_usb_bsdevDsr._dev_1devdsr);
					g_commonasyncData.type = L"remove_back";
					g_commonasyncData.data = callbackData;
					g_common_async.data = &g_commonasyncData;

					uv_async_send(&g_common_async);
					LeaveCriticalSection(&m_cs_0);
				}
				current++;
			}
		}
		else
		{
			std::map<std::string, BsdUtils*>::iterator iter;
			for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++)
			{
				structBSDevAppDeviceDsr m_usb_bsdevDsr;
				iter->second->getDevInfo(m_usb_bsdevDsr);
				std::map<std::string, cv::Mat>::iterator iterback = m_bsdBack.find(m_usb_bsdevDsr._dev_1SN);
				iterback->second = cv::Mat::zeros(m_usb_bsdevDsr._dev_2Pixels_V, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1);
				/*cv::Mat zero = cv::Mat::zeros(m_usb_bsdevDsr._dev_2Pixels_V, m_usb_bsdevDsr._dev_2Pixels_H, CV_64FC1);
				zero.copyTo(iterback->second);*/
			}
			

		}


		out.Add("msg", "success");
		element->data = out;
	}

	/**************************************************
	***@author:cui_sh
	***@brief:��ù����ǲ��������ݵ�ǰ��isnֵ
	***@time:
	***@version:
	**************************************************/
	void MyObject::getParamByIsn(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();

		if (args.Length() < 1) {
			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "invalid parameter[isn]";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;
		}

		if (m_bsdEntrty.empty())
		{

			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "exception unconnected!";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;

		}
		v8::String::Utf8Value param_type(isolate, args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
		//v8::String::Utf8Value param_type(args[0]->ToString());
		std::string isn = std::string(*param_type);//���ϵͳ�������
		isn = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(isn));



		neb::CJsonObject opt;
		//opt.AddEmptySubArray("result");
		std::map<std::string, BsdUtils*>::iterator iter = m_bsdEntrty.find(isn);
		//for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++) 
		//{
		structBSDevAppDeviceDsr m_usb_bsdevDsr;
		iter->second->getDevInfo(m_usb_bsdevDsr);

		/*m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area = 0;
		m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig = 0;
		m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig = 0;
		m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig = 0;
		m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode = 0;
		m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState = 0;*/
	
		opt.Add("isn", iter->first.c_str());
		opt.Add("scanMode", m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area);
		opt.Add("trigerSource", m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig);
		opt.Add("trigerType", m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig);
		opt.Add("trigerAction", m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig);
		opt.Add("shutterMode", m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode);
		opt.Add("shutterState", m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState);
		opt.Add("exposureTimeRaw",(int)m_usb_bsdevDsr._dev_4IntTime_us);
		opt.Add("gain_reg", (int)m_usb_bsdevDsr._dev_5gain_reg);
		opt.Add("offset_reg", (int)m_usb_bsdevDsr._dev_6offset_reg);

		//neb::CJsonObject item;
		//item.Add("isn", iter->first.c_str());
		//opt["result"].Add(item);
		//}
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, opt.ToString().c_str(), NewStringType::kNormal).ToLocalChecked());
	}

	/**************************************************
	***@author:cui_sh
	***@brief:���ص�ǰ�Ѿ��򿪵Ĺ�������
	***@time:
	***@version:
	**************************************************/
	void MyObject::getOpenedCamery(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (m_bsdEntrty.empty())
		{

			// �׳�һ�����󲢴��ص� JavaScript��
			std::string error = "exception unconnected!";
			error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate,
					error.c_str(),
					NewStringType::kNormal).ToLocalChecked()));
			return;

		}
		neb::CJsonObject opt;
		opt.AddEmptySubArray("result");


		std::map<std::string, BsdUtils*>::iterator iter;
		for (iter = m_bsdEntrty.begin(); iter != m_bsdEntrty.end(); iter++) {
			neb::CJsonObject item;
			item.Add("isn", iter->first.c_str());
			opt["result"].Add(item);
		}
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, opt.ToString().c_str(), NewStringType::kNormal).ToLocalChecked());
	}

	/**************************************************
	***@author:cui_sh
	***@brief:���ò���ͨ��isn
	***@time:2020-05-19
	***@version:
	**************************************************/
	void MyObject::setParamByIsn(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		//����isn ���������  ���������ֵ ���д������
		Isolate* isolate = args.GetIsolate();
		try
		{
			if (args.Length() < 3) {
				// �׳�һ�����󲢴��ص� JavaScript��
				std::string error = "invalid parameter[str,str,str]";
				error = EncodeUtils::UnicodeToUtf8(EncodeUtils::StringToWString(error));
				isolate->ThrowException(Exception::TypeError(
					String::NewFromUtf8(isolate,
						error.c_str(),
						NewStringType::kNormal).ToLocalChecked()));
				return;
			}

			v8::String::Utf8Value param_typeisn(isolate, args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());

			std::string data_isn = std::string(*param_typeisn);//���ϵͳ�������
			data_isn = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(data_isn));

			v8::String::Utf8Value param_typename(isolate, args[1]->ToString(isolate->GetCurrentContext()).ToLocalChecked());

			std::string data_name = std::string(*param_typename);//���ϵͳ�������
			data_name = EncodeUtils::WStringToString(EncodeUtils::Utf8ToUnicode(data_name));
			
			int value = args[2].As<Number>()->Value();
			//LogUtils::debugger->debug("isn��%s data_name:%s value:%d", data_isn.c_str(), data_name.c_str(), value);
			//���в�������
			std::map<std::string, BsdUtils*>::iterator iter = m_bsdEntrty.find(data_isn);
			structBSDevAppDeviceDsr m_usb_bsdevDsr;
			iter->second->getDevInfo(m_usb_bsdevDsr);

			
			
			neb::CJsonObject opt;
			opt.Clear();
			int value_result = 0;
			if (data_name.compare("scanMode") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md0_LineBinning_or_Area = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("trigerSource") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md1_SoftTrig_or_ExternalTrig = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("trigerType") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md3_LevelTrig_or_EdgeTrig = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);

			}
			else if (data_name.compare("trigerAction") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md4_PositiveTrig_or_NegtiveTrig = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("shutterMode") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md5_MechShutterMode = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("shutterState") == 0)
			{
				m_usb_bsdevDsr._dev_3DevMode.md6_MechShutterState = value;
				value_result = iter->second->setWorkMode(m_usb_bsdevDsr);
				opt.Add("value", value_result);

			}
			else if (data_name.compare("exposureTimeRaw") == 0)
			{
				m_usb_bsdevDsr._dev_4IntTime_us = value;
				value_result = iter->second->setIntegrationTime(value);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("gain_reg") == 0)
			{
				m_usb_bsdevDsr._dev_5gain_reg = value;
				value_result = iter->second->setAFE(value, m_usb_bsdevDsr._dev_6offset_reg);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("offset_reg") == 0)
			{
				m_usb_bsdevDsr._dev_6offset_reg = value;
				value_result = iter->second->setAFE(m_usb_bsdevDsr._dev_5gain_reg, value);
				opt.Add("value", value_result);
			}
			else if (data_name.compare("eepstatus") == 0)
			{
				//printf("e2promSaveOrNOT %d\r\n", value);
				value_result = iter->second->e2promSaveOrNOT((bool)value);
				opt.Add("value", value_result);
			}
			opt.Add("msg","success");
			args.GetReturnValue().Set(String::NewFromUtf8(isolate, opt.ToString().c_str(), NewStringType::kNormal).ToLocalChecked());

			//printf("%s %s %d\r\n", data_isn.c_str(), data_name.c_str(), value);
		}catch(...){
			LogUtils::debugger->debug("���������ô���");
		}
		
	}

}  // namespace demo

