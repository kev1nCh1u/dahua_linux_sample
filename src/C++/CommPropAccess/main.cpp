/// \file
/// \~chinese
/// \brief 通用属性访问接口使用范例
/// \example main.cpp
/// \~english
/// \brief common property access interface sample
/// \example main.cpp

// *******本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，
// 在运行之前，请使用相机客户端修改相机IP地址的网段与主机的网段一致*********    
//********This demo is a simple demonstration of the use of the SDK. There is no additional code to modify the camera IP.
//Before running, please use the camera client to modify the network segment of the camera IP address to be consistent with that of the host.*********

#ifndef __unix__
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/GigE/GigECamera.h"
#include "GenICam/GigE/GigEInterface.h"
#include "Infra/PrintLog.h"
#include "Memory/SharedPtr.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;


static void modifyCameraExposureTime(ICameraPtr& cameraSptr)
{
	CDoubleNode nodeExposureTime(cameraSptr, "ExposureTime");
	if (!nodeExposureTime.isValid())
	{
		printf(" get ExposureTime node fail.\n");
		return;
	}
	
	double dbValue = 0.0;
	if(false == nodeExposureTime.getValue(dbValue))
	{
		printf("get ExposureTime fail.\n");
		return;
	}
	else
	{
		printf("before change ,ExposureTime is %f\n",dbValue);
	}

	if(false == nodeExposureTime.setValue(dbValue - 2))
	{
		printf("set ExposureTime fail.\n");
		return;
	}
	
	if(false == nodeExposureTime.getValue(dbValue))
	{
		printf("get ExposureTime fail.\n");
		return;
	}
	else
	{
		printf("after change ,ExposureTime is %f\n",dbValue);
	}
}

static void modifyCameraWidth(ICameraPtr& cameraSptr)
{
	CIntNode nodeWidth(cameraSptr, "Width");
	if(false == nodeWidth.isValid())
	{
		printf(" get Width node fail.\n");
		return;
	}
	int64_t nValue = 0;

	if(false == nodeWidth.getValue(nValue))
	{
		printf("get Width fail.\n");
		return;
	}
	else
	{
		printf("before change ,Width is %d\n",nValue);
	}

	if(false == nodeWidth.setValue(nValue-8))
	{
		printf("set Width fail.\n");
		return;
	}

	if(false == nodeWidth.getValue(nValue))
	{
		printf(" get Width fail.\n");
	}
	else
	{
		printf("after change ,Width is %d\n",nValue);
	}
}

static void modifyCameraReverseX(ICameraPtr& cameraSptr)
{
	CBoolNode nodeBool(cameraSptr, "ReverseX");
	if(false == nodeBool.isValid())
	{
		printf("get ReverseX node fail.\n");
		return;
	}

	bool bReverse = false;
	if(false == nodeBool.getValue(bReverse))
	{
		printf("get ReverseX fail.\n");
		return;
	}
	else
	{
		printf("before change ,ReverseX is %d\n",bReverse);
	}

	if(false == nodeBool.setValue(!bReverse))
	{
		printf("set ReverseX fail.\n");
		return;
	}

	if(false == nodeBool.getValue(bReverse))
	{
		printf("get ReverseX fail.\n");
		return;
	}
	else
	{
		printf("after change ,ReverseX is %d\n",bReverse);
	}
}

static void modifyCameraDeviceUserID(ICameraPtr& cameraSptr)
{
	CStringNode nodeString(cameraSptr, "DeviceUserID");
	if(false == nodeString.isValid())
	{
		printf("get DeviceUserID node fail.\n");
		return;
	}
	
	CString strID;
	if(false == nodeString.getValue(strID))
	{
		printf("get DeviceUserID value fail.\n");
		return;
	}
	else
	{
		char cameraUserID[256] = {0};
		memcpy(cameraUserID, strID.c_str(), strID.length());
		printf("before change ,DeviceUserID is %s\n", cameraUserID);
	}

	if(false == nodeString.setValue("camera"))
	{
		printf("set DeviceUserID value fail.\n");
		return;
	}
	
	if(false == nodeString.getValue(strID))
	{
		printf("get DeviceUserID value fail.\n");
		return;
	}
	else
	{
		char cameraUserID[256] = {0};
		memcpy(cameraUserID, strID.c_str(), strID.length());
		printf("after change ,DeviceUserID is %s\n", cameraUserID);
	}
}

static void modifyCameraTriggerSelector(ICameraPtr& cameraSptr)
{
	CEnumNode nodeEnum(cameraSptr, "TriggerSelector");

	if(false == nodeEnum.isValid())
	{
		printf("get TriggerSelector node fail.\n");
		return;
	}

	CString strValue;
	if(false == nodeEnum.getValueSymbol(strValue))
	{
		printf("get TriggerSelector fail.\n");
		return;
	}
	else
	{
		printf("before change ,TriggerSelector is %s\n",strValue.c_str());
	}

	if(false == nodeEnum.setValueBySymbol("FrameStart"))
	{
		printf("set TriggerSelector fail.\n");
		return;
	}

	if(false == nodeEnum.getValueSymbol(strValue))
	{
		printf("get TriggerSelector fail.\n");
		return;
	}
	else
	{
		printf("after change ,TriggerSelector is %s\n",strValue.c_str());
	}
}


static void modifyCameraTriggerMode(ICameraPtr& cameraSptr)
{
	CEnumNode nodeEnum(cameraSptr, "TriggerMode");

	if(false == nodeEnum.isValid())
	{
		printf("get TriggerMode node fail.\n");
		return;
	}

	CString strValue;
	if(false == nodeEnum.getValueSymbol(strValue))
	{
		printf("get TriggerMode fail.\n");
		return;
	}
	else
	{
		printf("before change ,TriggerMode is %s\n",strValue.c_str());
	}

	if(false == nodeEnum.setValueBySymbol("On"))
	{
		printf("set TriggerMode fail.\n");
		return;
	}

	if(false == nodeEnum.getValueSymbol(strValue))
	{
		printf("get TriggerMode fail.\n");
		return;
	}
	else
	{
		printf("after change ,TriggerMode is %s\n",strValue.c_str());
	}
}

static void executeTriggerSoftware(ICameraPtr& cameraSptr)
{
	CCmdNode nodeCmd(cameraSptr, "TriggerSoftware");
	if(false == nodeCmd.isValid())
	{
		printf("get TriggerSoftware node fail.\n");
		return;
	}

	if(false == nodeCmd.execute())
	{
		printf("execute fail.\n");
	}
	else
	{
		printf("execute TriggerSoftware success.\n");
	}
}

// ********************** 这部分处理与SDK操作相机无关，用于显示设备列表 begin*****************************
//***********BEGIN: These functions are not related to API call and used to display device info***********
static void displayDeviceInfo(TVector<ICameraPtr>& vCameraPtrList)
{
	ICameraPtr cameraSptr;
	// 打印Title行
	// Print title line
	printf("\nIdx Type Vendor     Model      S/N             DeviceUserID    IP Address    \n");
	printf("------------------------------------------------------------------------------\n");
	for (int cameraIndex = 0; cameraIndex < vCameraPtrList.size(); cameraIndex++)
	{
		cameraSptr = vCameraPtrList[cameraIndex];
		// Idx 设备列表的相机索引 最大表示字数：3
		// Camera index in device list, display in 3 characters
		printf("%-3d", cameraIndex + 1);

		// Type 相机的设备类型（GigE，U3V，CL，PCIe
		// Camera type (eg:GigE，U3V，CL，PCIe)
		switch (cameraSptr->getType())
		{
		case ICamera::typeGige:
			printf(" GigE");
			break;
		case ICamera::typeU3v:
			printf(" U3V ");
			break;
		case ICamera::typeCL:
			printf(" CL  ");
			break;
		case ICamera::typePCIe:
			printf(" PCIe");
			break;
		default:
			printf("     ");
			break;
		}

		// VendorName 制造商信息 最大表示字数：10
		// Camera vendor name, display in 10 characters
		const char* vendorName = cameraSptr->getVendorName();
		char vendorNameCat[11];
		if (strlen(vendorName) > 10)
		{
			strncpy(vendorNameCat, vendorName, 7);
			vendorNameCat[7] = '\0';
			strcat(vendorNameCat, "...");
			printf(" %-10.10s", vendorNameCat);
		}
		else
		{
			printf(" %-10.10s", vendorName);
		}

		// ModeName 相机的型号信息 最大表示字数：10 
		// Camera model name, display in 10 characters 
		printf(" %-10.10s", cameraSptr->getModelName());

		// Serial Number 相机的序列号 最大表示字数：15 
		// Camera serial number, display in 15 characters 
		printf(" %-15.15s", cameraSptr->getSerialNumber());

		// deviceUserID 自定义用户ID 最大表示字数：15 
		// Camera user id, display in 15 characters 
		const char* deviceUserID = cameraSptr->getName();
		char deviceUserIDCat[16];
		if (strlen(deviceUserID) > 15)
		{
			strncpy(deviceUserIDCat, deviceUserID, 12);
			deviceUserIDCat[12] = '\0';
			strcat(deviceUserIDCat, "...");
			printf(" %-15.15s", deviceUserIDCat);
		}
		else
		{
			memcpy(deviceUserIDCat, deviceUserID, sizeof(deviceUserIDCat));
			printf(" %-15.15s", deviceUserIDCat);
		}

		// IPAddress GigE相机时获取IP地址 
		// IP address of GigE camera 
		IGigECameraPtr gigeCameraPtr = IGigECamera::getInstance(cameraSptr);
		if (NULL != gigeCameraPtr.get())
		{
			CString ip = gigeCameraPtr->getIpAddress();
			printf(" %s", ip.c_str());
		}
		printf("\n");

	}
}

static char* trim(char* pStr)
{
	char* pDst = pStr;
	char* pTemStr = NULL;
	int ret = -1;

	if (pDst != NULL)
	{
		pTemStr = pDst + strlen(pStr) - 1;
		// 除去字符串首部空格
		// remove the first space of the string
		while ( *pDst ==' ')
		{
			pDst++;
		}
		// 除去字符串尾部空格
		// remove trailing space from string
		while ((pTemStr > pDst) &&(*pTemStr == ' '))
		{
			*pTemStr-- = '\0';
		}
	}
	return pDst;
}

// 函数功能：判断pInpuStr字符串每个字符是否都为数字。 
// function: judge whether each character of pinpustr string is a number. 
// 该函数与SDK接口操作相机无关 
// this function is independent of the SDK interface operation camera 
static int isInputValid(char* pInpuStr)
{
	char numChar;
	char* pStr = pInpuStr;
	while (*pStr != '\0')
	{
		numChar = *pStr;
		if ((numChar > '9') || (numChar < '0'))
		{
			return -1;
		}
		pStr++;
	}
	return 0;
}

// 函数功能：从displayDeviceInfo显示的相机列表选择需要操作的相机对象。 
// function: select the camera object to be operated from the list of cameras displayed in displaydeviceinfo
// 该函数与SDK接口操作相机无关 
// this function is independent of the SDK interface operation camera 
static int selectDevice(int cameraCnt)
{
	char inputStr[256] = {0};
	char* pTrimStr;
	int inputIndex = -1;
	int ret = -1;
	// 提示用户选择 
	// inform user to select
	printf("\nPlease input the camera index: ");
	while (1)
	{
		// 获取输入内容字符串 
		// get input string*/
		memset(inputStr, 0, sizeof(inputStr));
		scanf("%s", inputStr);
		// 清空输入缓存 
		// clear input flush
		fflush(stdin);

		// 除去字符串首尾空格 
		// remove starting and trailing spaces from string
		pTrimStr = trim(inputStr);
		// 判断输入字符串是否为数字
		// judge whether the input string is a number
		ret = isInputValid(pTrimStr);
		if (ret == 0)
		{
			// 输入的字符串转换成为数字 
			// the input string is converted to a number
			inputIndex = atoi(pTrimStr);
			// 判断用户选择合法性 */
			// judge the validity of user selection
			inputIndex -= 1;//显示的Index是从1开始 english: input index starts from 1 
			if ((inputIndex >= 0) && (inputIndex < cameraCnt))
			{
				break;
			}
		}

		printf("Input invalid! Please input the camera index: ");
	}
	return inputIndex;
}
// ********************** 这部分处理与SDK操作相机无关，用于显示设备列表 end*****************************
// *** This part of the processing is independent of the SDK operation camera and is used to display the device list**

int main()
{
	ICameraPtr cameraSptr;

	// 发现设备
	// discovery device
	CSystem &systemObj = CSystem::getInstance();
	TVector<ICameraPtr> vCameraPtrList;
	bool bRet = systemObj.discovery(vCameraPtrList);
	if (!bRet)
	{
		printf("discovery device fail.\n");
		return 0;
	}

	if (0 == vCameraPtrList.size())
	{
		printf("no camera device find.\n");
		return 0;
	}

	// print camera info (index,Type,vendor name, model,serial number,DeviceUserID,IP Address)
	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址）
	displayDeviceInfo(vCameraPtrList);
	int cameraIndex = selectDevice(vCameraPtrList.size());
	cameraSptr = vCameraPtrList[cameraIndex];

	//连接设备
	//connect device
	if (!cameraSptr->connect())
	{
		printf("connect camera fail.\n");
		return 0;
	}

	// 修改相机曝光时间，通用double型属性访问实例
	// set camera's ExposureTime, an example of double property access
	modifyCameraExposureTime(cameraSptr);

	// 修改相机像素宽度，通用int型属性访问实例
	// set camera's Width, an example of int property access
	
	modifyCameraWidth(cameraSptr);

	//修改相机ReverseX，通用bool型属性访问实例
	// set camera's ReverseX, an example of bool property access
	modifyCameraReverseX(cameraSptr);

	// 修改相机DeviceUserID，通用string型属性访问实例
	// set camera's DeviceUserID, an example of string property access
	modifyCameraDeviceUserID(cameraSptr);

	// 修改相机TriggerSelector，通用enum型属性访问实例
	// set camera's TriggerSelector, an example of enum property access
	modifyCameraTriggerSelector(cameraSptr);

	// 修改相机TriggerMode，通用enum型属性访问实例
	// set camera's TriggerMode, an example of enum property access

	modifyCameraTriggerMode(cameraSptr);

	// 修改相机TriggerSoftware，通用command型属性访问实例
	// execute camera's TriggerSoftware, an example of command property access

	executeTriggerSoftware(cameraSptr);

	
	// 断开设备
	// disconnect device
	if (!cameraSptr->disConnect())
	{
		printf("disConnect camera fail.\n");
		return 0;
	}
	getchar();
	printf("disConnect successfully thread ID :%d\n", CThread::getCurrentThreadID());
	return 1;
}
