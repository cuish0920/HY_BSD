{
 'variables': {
    'opencv_dir%': 'E:/opencv/opencv_sdk',
	'msvc_dir':'C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC',
	'Windows_Kits_dir':'C:/Program Files (x86)/Windows Kits',
	'log4api_dir':'F:/ELECTRON/BSDev9.0.0/HY_BSD/log4api',
	'bsd_sdk_dir':'F:/ELECTRON/BSDev9.0.0/HY_BSD/bsd_sdk'
  },
  "targets": [
    {
	  "target_name": "factoryInterface",
      "sources": [ "factoryInterface.cc","objectFactory.cc","CJsonObject.cpp","SerialUtils.cpp",
	  "cJSON.cpp","EncodeUtils.cpp","OpencvUtils.cpp","LogUtils.cpp","BsdUtils.cpp"],
      "include_dirs":[
         "<!(node -e \"require('nan')\")",
		 "<(msvc_dir)/14.16.27023/atlmfc/include",
		 "<(Windows_Kits_dir)/10/Include/10.0.17763.0/um",
		 "<(Windows_Kits_dir)/10/Include/10.0.17763.0/shared",
		 "<(opencv_dir)/include",
		 "<(log4api_dir)/include",
		 "<(bsd_sdk_dir)/inc"
      ],
      "libraries":[],
      "link_settings":{
        "libraries":["-l<(msvc_dir)/14.16.27023/atlmfc/lib/x64/nafxcw.lib",
		"-l<(msvc_dir)/14.16.27023/atlmfc/lib/x64/atls.lib",
		"-l<(opencv_dir)/x64/vc14/lib/opencv_world347.lib",
		"-l<(log4api_dir)/log4cpp.lib",
		"-l<(bsd_sdk_dir)/x64/res/lib/__USB20BSDevOpSCexp.lib",
		
		]
      },
      #"cflags!": [ "-fno-exceptions" ],
      #"cflags": [ "-std=c++11" ],
      #"cflags_cc!": [ "-fno-exceptions" ]
    }
  ]
}