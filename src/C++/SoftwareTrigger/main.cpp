/************************************************************************/
// *******本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，
// 在运行之前，请使用相机客户端修改相机IP地址的网段与主机的网段一致*********    
//********This demo is a simple demonstration of the use of the SDK. There is no additional code to modify the camera IP.
// Before running, please use the camera client to modify the network segment of the camera IP address to be consistent with that of the host.*********
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

bool isGrabbingFlag = false;

static bool setSoftTriggerConf(IAcquisitionControlPtr sptrAcquisitionCtl)
{
	// 设置触发源为软触发
	// set trigger source to software trigger
	CEnumNode enumNode = sptrAcquisitionCtl->triggerSource();
	bool bRet = enumNode.setValueBySymbol("Software");
	if (bRet != true)
	{
		printf("set trigger source fail.\n");
		return false;
	}

	// 设置触发器
	// set trigger
	enumNode = sptrAcquisitionCtl->triggerSelector();
	bRet = enumNode.setValueBySymbol("FrameStart");
	if (bRet != true)
	{
		printf("set trigger selector fail.\n");
		return false;
	}

	// 设置触发模式
	// set trigger mode
	enumNode = sptrAcquisitionCtl->triggerMode();
	bRet = enumNode.setValueBySymbol("On");
	if (bRet != true)
	{
		printf("set trigger mode fail.\n");
		return false;
	}	
	return true;
}

void onGetFrame(const CFrame &pFrame)
{
	// 标准输出换行
	// standard output line feed
	printf("\r\n");

	// 判断帧的有效性
	// judge the validity of frame
	bool isValid = pFrame.valid();
	if (!isValid)
	{
		printf("frame is invalid!\n");
		return;
	}
	else
	{
		uint64_t blockId = pFrame.getBlockId();
		printf("blockId = %d.\n", blockId);
		isGrabbingFlag = false;
	}

	return;
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
			inputIndex -= 1;// 显示的Index是从1开始 english: input index starts from 1 
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

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址）
	// print camera info (index,Type,vendor name, model,serial number,DeviceUserID,IP Address)
	displayDeviceInfo(vCameraPtrList);
	int cameraIndex = selectDevice(vCameraPtrList.size());
	cameraSptr = vCameraPtrList[cameraIndex];

	// 连接设备
	// connect device
	if (!cameraSptr->connect())
	{
		printf("connect camera fail.\n");
		return 0;
	}


	// 创建AcquisitionControl对象
	// create acquisitioncontrol object
	IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);
	if (NULL == sptrAcquisitionControl.get())
	{
		printf("create AcquisitionControl object fail.\n");
		// 实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}

	// 设置软触发配置
	// Set software trigger configuration
	bRet = setSoftTriggerConf(sptrAcquisitionControl);
	if (!bRet)
	{
		printf("set soft trigger config fail.\n");
		// 实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}

	// 创建流对象
	// create stream object
	IStreamSourcePtr streamPtr = systemObj.createStreamSource(cameraSptr);
	if (NULL == streamPtr.get())
	{
		printf("create stream obj  fail.\r\n");
		// 实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}

	// 注册回调函数
	// register callback fuction
	bRet = streamPtr->attachGrabbing(onGetFrame);
	if(!bRet)
	{
		printf("attach Grabbing fail.\n");
		//实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}

	// 开始拉流
	// start grabbing
	bool isStartGrabbingSuccess = streamPtr->startGrabbing();
	if (!isStartGrabbingSuccess)
	{
		printf("StartGrabbing  fail.\n");
		isGrabbingFlag = false;
		//实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}
	else
	{
		isGrabbingFlag = true;
	}

	// 执行一次软触发
	// execute a soft trigger
	CCmdNode cmdNode = sptrAcquisitionControl->triggerSoftware();
	bRet = cmdNode.execute();
	if (!bRet)
	{
		printf("Software Trigger is fail.\n");
		// 实际应用中应及时释放相关资源，如diconnect相机等，不宜直接return
		// in practical application, relevant resources should be released in time, such as diconnect camera, etc. it is not suitable to return directly
		return 0;
	}	
	
	// 等待完成一次软触发
	// wait for one soft trigger to complete
	while(isGrabbingFlag)
	{
		CThread::sleep(50);
	}

	// 注销回调函数
	// unregister callback fuction
	bRet = streamPtr->detachGrabbing(onGetFrame);
	if(!bRet)
	{
		printf("detachGrabbing  fail.\n");
	}

	// 停止相机拉流
	// stop camera grabbing
	bRet = streamPtr->stopGrabbing();
	if(!bRet)
	{
		printf("stopGrabbing  fail.\n");
	}
	
	// 断开设备
	// disconnect device
	if (!cameraSptr->disConnect())
	{
		printf("disConnect camera fail.\n");
		return 0;
	}

    printf("disConnect successfully thread ID :%d\n", CThread::getCurrentThreadID());

    return 1;
}
