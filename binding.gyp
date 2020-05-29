{
  "targets": [
    {
	  "target_name": "factoryInterface",
      "sources": [ "factoryInterface.cc","objectFactory.cc","CJsonObject.cpp","SerialUtils.cpp",
	  "cJSON.cpp","EncodeUtils.cpp","OpencvUtils.cpp","LogUtils.cpp","BsdUtils.cpp"],
      "include_dirs":[
         "<!(node -e \"require('nan')\")",
		 "C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC/14.16.27023/atlmfc/include",
		 "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/um",
		 "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/shared",
		 "E:/opencv/opencv_sdk/include",
		 "F:/ELECTRON/BSDev9.0.0/HY_BSD/log4api/include",
		 "F:/ELECTRON/BSDev9.0.0/HY_BSD/bsd_sdk/inc"
      ],
      "libraries":[],
      "link_settings":{
        "libraries":["-l C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC/14.16.27023/atlmfc/lib/x64/nafxcw.lib",
		"C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC/14.16.27023/atlmfc/lib/x64/atls.lib",
		"E:/opencv/opencv_sdk/x64/vc14/lib/opencv_world347.lib",
		"F:/ELECTRON/BSDev9.0.0/HY_BSD/log4api/log4cpp.lib",
		"F:/ELECTRON/BSDev9.0.0/HY_BSD/bsd_sdk/x64/res/lib/__USB20BSDevOpSCexp.lib",
		
		]
      },
      #"cflags!": [ "-fno-exceptions" ],
      #"cflags": [ "-std=c++11" ],
      #"cflags_cc!": [ "-fno-exceptions" ]
    }
  ]
}