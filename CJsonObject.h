/*******************************************************************************
* Project:  neb
* @file     CJsonObject.hpp
* @brief    Json
* @author   bwarliao
* @date:    2014-7-16
* @note
* Modify history:
******************************************************************************/
//https://www.cnblogs.com/bwar/p/9191479.html
#ifndef CJSONOBJECT_HPP_
#define CJSONOBJECT_HPP_

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
//#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <string>
#include <map>
#include "cJSON.h"


namespace neb
{

	class CJsonObject
	{
	public:     // method of ordinary json object or json array
		CJsonObject();
		CJsonObject(const std::string& strJson);
		CJsonObject(const CJsonObject* pJsonObject);
		CJsonObject(const CJsonObject& oJsonObject);
		virtual ~CJsonObject();

		CJsonObject& operator=(const CJsonObject& oJsonObject);
		bool operator==(const CJsonObject& oJsonObject) const;
		bool Parse(const std::string& strJson);
		void Clear();
		bool IsEmpty() const;
		bool IsArray() const;
		std::string ToString() const;
		std::string ToFormattedString() const;
		const std::string& GetErrMsg() const
		{
			return(m_strErrMsg);
		}

	public:     // method of ordinary json object
		bool AddEmptySubObject(const std::string& strKey);
		bool AddEmptySubArray(const std::string& strKey);
		CJsonObject& operator[](const std::string& strKey);
		std::string operator()(const std::string& strKey) const;
		bool Get(const std::string& strKey, CJsonObject& oJsonObject) const;
		bool Get(const std::string& strKey, std::string& strValue) const;
		bool Get(const std::string& strKey, int32& iValue) const;
		bool Get(const std::string& strKey, uint32& uiValue) const;
		bool Get(const std::string& strKey, int64& llValue) const;
		bool Get(const std::string& strKey, uint64& ullValue) const;
		bool Get(const std::string& strKey, bool& bValue) const;
		bool Get(const std::string& strKey, float& fValue) const;
		bool Get(const std::string& strKey, double& dValue) const;
		bool Add(const std::string& strKey, const CJsonObject& oJsonObject);
		bool Add(const std::string& strKey, const std::string& strValue);
		bool Add(const std::string& strKey, int32 iValue);
		bool Add(const std::string& strKey, uint32 uiValue);
		bool Add(const std::string& strKey, int64 llValue);
		bool Add(const std::string& strKey, uint64 ullValue);
		bool Add(const std::string& strKey, bool bValue, bool bValueAgain);
		bool Add(const std::string& strKey, float fValue);
		bool Add(const std::string& strKey, double dValue);
		bool Delete(const std::string& strKey);
		bool Replace(const std::string& strKey, const CJsonObject& oJsonObject);
		bool Replace(const std::string& strKey, const std::string& strValue);
		bool Replace(const std::string& strKey, int32 iValue);
		bool Replace(const std::string& strKey, uint32 uiValue);
		bool Replace(const std::string& strKey, int64 llValue);
		bool Replace(const std::string& strKey, uint64 ullValue);
		bool Replace(const std::string& strKey, bool bValue, bool bValueAgain);
		bool Replace(const std::string& strKey, float fValue);
		bool Replace(const std::string& strKey, double dValue);

	public:     // method of json array
		int GetArraySize();
		CJsonObject& operator[](unsigned int uiWhich);
		std::string operator()(unsigned int uiWhich) const;
		bool Get(int iWhich, CJsonObject& oJsonObject) const;
		bool Get(int iWhich, std::string& strValue) const;
		bool Get(int iWhich, int32& iValue) const;
		bool Get(int iWhich, uint32& uiValue) const;
		bool Get(int iWhich, int64& llValue) const;
		bool Get(int iWhich, uint64& ullValue) const;
		bool Get(int iWhich, bool& bValue) const;
		bool Get(int iWhich, float& fValue) const;
		bool Get(int iWhich, double& dValue) const;
		bool Add(const CJsonObject& oJsonObject);
		bool Add(const std::string& strValue);
		bool Add(int32 iValue);
		bool Add(uint32 uiValue);
		bool Add(int64 llValue);
		bool Add(uint64 ullValue);
		bool Add(int iAnywhere, bool bValue);
		bool Add(float fValue);
		bool Add(double dValue);
		bool AddAsFirst(const CJsonObject& oJsonObject);
		bool AddAsFirst(const std::string& strValue);
		bool AddAsFirst(int32 iValue);
		bool AddAsFirst(uint32 uiValue);
		bool AddAsFirst(int64 llValue);
		bool AddAsFirst(uint64 ullValue);
		bool AddAsFirst(int iAnywhere, bool bValue);
		bool AddAsFirst(float fValue);
		bool AddAsFirst(double dValue);
		bool Delete(int iWhich);
		bool Replace(int iWhich, const CJsonObject& oJsonObject);
		bool Replace(int iWhich, const std::string& strValue);
		bool Replace(int iWhich, int32 iValue);
		bool Replace(int iWhich, uint32 uiValue);
		bool Replace(int iWhich, int64 llValue);
		bool Replace(int iWhich, uint64 ullValue);
		bool Replace(int iWhich, bool bValue, bool bValueAgain);
		bool Replace(int iWhich, float fValue);
		bool Replace(int iWhich, double dValue);

	private:
		CJsonObject(cJSON* pJsonData);

	private:
		cJSON * m_pJsonData;
		cJSON* m_pExternJsonDataRef;
		std::string m_strErrMsg;
		std::map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
		std::map<std::string, CJsonObject*> m_mapJsonObjectRef;
	};

}

#endif /* CJSONHELPER_HPP_ */
//int main()
//{
//	int iValue;
//	std::string strValue;
//	neb::CJsonObject oJson("{\"refresh_interval\":60,"
//		"\"dynamic_loading\":["
//		"{"
//		"\"so_path\":\"plugins/User.so\", \"load\":false, \"version\":1,"
//		"\"cmd\":["
//		"{\"cmd\":2001, \"class\":\"neb::CmdUserLogin\"},"
//		"{\"cmd\":2003, \"class\":\"neb::CmdUserLogout\"}"
//		"],"
//		"\"module\":["
//		"{\"path\":\"im/user/login\", \"class\":\"neb::ModuleLogin\"},"
//		"{\"path\":\"im/user/logout\", \"class\":\"neb::ModuleLogout\"}"
//		"]"
//		"},"
//		"{"
//		"\"so_path\":\"plugins/ChatMsg.so\", \"load\":false, \"version\":1,"
//		"\"cmd\":["
//		"{\"cmd\":2001, \"class\":\"neb::CmdChat\"}"
//		"],"
//		"\"module\":[]"
//		"}"
//		"]"
//		"}");
//	std::cout << oJson.ToString() << std::endl;
//	std::cout << "-------------------------------------------------------------------" << std::endl;
//	std::cout << oJson["dynamic_loading"][0]["cmd"][1]("class") << std::endl;
//	oJson["dynamic_loading"][0]["cmd"][0].Get("cmd", iValue);
//	std::cout << "iValue = " << iValue << std::endl;
//	oJson["dynamic_loading"][0]["module"][0].Get("path", strValue);
//	std::cout << "strValue = " << strValue << std::endl;
//	std::cout << "-------------------------------------------------------------------" << std::endl;
//	oJson.AddEmptySubObject("depend");
//	oJson["depend"].Add("nebula", "https://github.com/Bwar/Nebula");
//	oJson["depend"].AddEmptySubArray("bootstrap");
//	oJson["depend"]["bootstrap"].Add("BEACON");
//	oJson["depend"]["bootstrap"].Add("LOGIC");
//	oJson["depend"]["bootstrap"].Add("LOGGER");
//	oJson["depend"]["bootstrap"].Add("INTERFACE");
//	oJson["depend"]["bootstrap"].Add("ACCESS");
//	std::cout << oJson.ToString() << std::endl;
//	std::cout << "-------------------------------------------------------------------" << std::endl;
//	std::cout << oJson.ToFormattedString() << std::endl;
//}
//[bwar@nebula demo]$ . / CJsonObjectTest
//{ "refresh_interval":60,"dynamic_loading" : [{"so_path":"plugins/User.so","load" : false,"version" : 1,"cmd" : [{"cmd":2001,"class" : "neb::CmdUserLogin"},{"cmd":2003,"class" : "neb::CmdUserLogout"}],"module" : [{"path":"im/user/login","class" : "neb::ModuleLogin"},{"path":"im/user/logout","class" : "neb::ModuleLogout"}]},{"so_path":"plugins/ChatMsg.so","load" : false,"version" : 1,"cmd" : [{"cmd":2001,"class" : "neb::CmdChat"}],"module" : []}] }
//------------------------------------------------------------------ -
//neb::CmdUserLogout
//iValue = 2001
//strValue = im / user / login
//------------------------------------------------------------------ -
//{"refresh_interval":60, "dynamic_loading" : [{"so_path":"plugins/User.so", "load" : false, "version" : 1, "cmd" : [{"cmd":2001, "class" : "neb::CmdUserLogin"}, { "cmd":2003,"class" : "neb::CmdUserLogout" }], "module" : [{"path":"im/user/login", "class" : "neb::ModuleLogin"}, { "path":"im/user/logout","class" : "neb::ModuleLogout" }]}, { "so_path":"plugins/ChatMsg.so","load" : false,"version" : 1,"cmd" : [{"cmd":2001,"class" : "neb::CmdChat"}],"module" : [] }], "depend" : {"nebula":"https://github.com/Bwar/Nebula", "bootstrap" : ["BEACON", "LOGIC", "LOGGER", "INTERFACE", "ACCESS"]}}
//------------------------------------------------------------------ -
//{
//	"refresh_interval": 60,
//		"dynamic_loading" : [{
//		"so_path":  "plugins/User.so",
//			"load" : false,
//			"version" : 1,
//			"cmd" : [{
//			"cmd":  2001,
//				"class" : "neb::CmdUserLogin"
//		}, {
//			"cmd":  2003,
//			"class" : "neb::CmdUserLogout"
//		}],
//			"module":   [{
//				"path": "im/user/login",
//					"class" : "neb::ModuleLogin"
//			}, {
//				"path": "im/user/logout",
//				"class" : "neb::ModuleLogout"
//			}]
//	}, {
//		"so_path":  "plugins/ChatMsg.so",
//		"load" : false,
//		"version" : 1,
//		"cmd" : [{
//				"cmd":  2001,
//				"class" : "neb::CmdChat"
//			}],
//		"module" : []
//	}],
//		"depend":   {
//					"nebula":   "https://github.com/Bwar/Nebula",
//						"bootstrap" : ["BEACON", "LOGIC", "LOGGER", "INTERFACE", "ACCESS"]
//				}
//}