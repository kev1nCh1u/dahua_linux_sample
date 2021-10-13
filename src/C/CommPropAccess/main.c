/// \file
/// \~chinese
/// \brief 通用属性访问接口使用范例
/// \example main.c
/// \~english
/// \brief common property access interface sample
/// \example main.c

// **********************************************************************
// 本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，在运行之前，请使
// 用相机客户端修改相机IP地址的网段与主机的网段一致。
// This Demo shows how to use GenICam API(C) to write a simple program.
// Please make sure that the camera and PC are in the same subnet before running the demo.
// If not, you can use camera client software to modify the IP address of camera to the same subnet with PC.

// 本程序演示了发现设备，连接设备，修改相机曝光时间、图像宽度、ReverseX、DeviceUserID、TriggerSource等属性，断开连接操作
// This program shows how to discover and connect device, set camera params such as ExposureTime, Image Width, ReverseX, DeviceUserID,
// TriggerSource etc. and disconnect device
// ***********************************************************************
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GenICam/CAPI/SDK.h"

static int32_t GENICAM_connect(GENICAM_Camera *pGetCamera)
{
	int32_t isConnectSuccess;

	isConnectSuccess = pGetCamera->connect(pGetCamera, accessPermissionControl);

	if( isConnectSuccess != 0)
	{
		printf("connect camera failed.\n");
		return -1;
	}
	
	return 0;
}

static int32_t modifyCameraExposureTime(GENICAM_Camera *pGetCamera)
{
	int32_t isDoubleNodeSuccess;
	double exposureTimeValue;
	GENICAM_DoubleNode *pDoubleNode = NULL;
	GENICAM_DoubleNodeInfo doubleNodeInfo = {0};

	doubleNodeInfo.pCamera = pGetCamera;
	memcpy(doubleNodeInfo.attrName, "ExposureTime", sizeof("ExposureTime"));

	isDoubleNodeSuccess = GENICAM_createDoubleNode(&doubleNodeInfo, &pDoubleNode);
	if (isDoubleNodeSuccess != 0)
	{
		printf("GENICAM_createDoubleNode fail.\n");
		return -1;
	}

	exposureTimeValue = 0.0;
	isDoubleNodeSuccess = pDoubleNode->getValue(pDoubleNode, &exposureTimeValue);
	if (0 != isDoubleNodeSuccess)
	{
		printf("get ExposureTime fail.\n");
		// 注意：需要释放pDoubleNode内部对象内存
		// Attention: should release pDoubleNode internal object before return
		pDoubleNode->release(pDoubleNode);
		return -1;
	}
	else
	{
		printf("before change ,ExposureTime is %f\n",exposureTimeValue);
	}	

	isDoubleNodeSuccess = pDoubleNode->setValue(pDoubleNode, (exposureTimeValue - 2));
	if (0 != isDoubleNodeSuccess)
	{
		printf("set ExposureTime fail.\n");
		// 注意：需要释放pDoubleNode内部对象内存
		// Attention: should release pDoubleNode internal object before return
		pDoubleNode->release(pDoubleNode);
		return -1;
	}

	isDoubleNodeSuccess = pDoubleNode->getValue(pDoubleNode, &exposureTimeValue);
	if (0 != isDoubleNodeSuccess)
	{
		printf("get ExposureTime fail.\n");
		// 注意：需要释放pDoubleNode内部对象内存
		// Attention: should release pDoubleNode internal object before return
		pDoubleNode->release(pDoubleNode);
		return -1;
	}
	else
	{
		printf("after change ,ExposureTime is %f\n",exposureTimeValue);
		// 注意：需要释放pDoubleNode内部对象内存
		// Attention: should release pDoubleNode internal object at the end of use
		pDoubleNode->release(pDoubleNode);
	}

	return 0;
}

static int32_t modifyCameraWidth(GENICAM_Camera *pGetCamera)
{
	int32_t isIntNodeSuccess;
	int64_t widthValue;
	GENICAM_IntNode *pIntNode = NULL;
	GENICAM_IntNodeInfo intNodeInfo = {0};

	intNodeInfo.pCamera = pGetCamera;
	memcpy(intNodeInfo.attrName, "Width", sizeof("Width"));

	isIntNodeSuccess = GENICAM_createIntNode(&intNodeInfo, &pIntNode);
	if (0 != isIntNodeSuccess)
	{
		printf("GENICAM_createIntNode fail.\n");
		return -1;
	}

	widthValue = 0;
	isIntNodeSuccess = pIntNode->getValue(pIntNode, &widthValue);
	if (0 != isIntNodeSuccess != 0)
	{
		printf("get Width fail.\n");
		// 注意：需要释放pIntNode内部对象内存
		// Attention: should release pIntNode internal object before return
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("before change ,Width is %d\n", widthValue);
	}	

	isIntNodeSuccess = pIntNode->setValue(pIntNode, (widthValue - 8));
	if (0 != isIntNodeSuccess)
	{
		printf("set Width fail.\n");
		// 注意：需要释放pIntNode内部对象内存
		// Attention: should release pIntNode internal object before return
		pIntNode->release(pIntNode);
		return -1;
	}

	isIntNodeSuccess = pIntNode->getValue(pIntNode, &widthValue);
	if (0 != isIntNodeSuccess)
	{
		printf("get Width fail.\n");
		// 注意：需要释放pIntNode内部对象内存
		// Attention: should release pIntNode internal object before return
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("after change ,Width is %d\n",widthValue);
		// 注意：需要释放pIntNode内部对象内存
		// Attention: should release pIntNode internal object at the end of use
		pIntNode->release(pIntNode);
	}	

	return 0;
}

static int32_t modifyCameraReverseX(GENICAM_Camera *pGetCamera)
{
	int32_t isBoolNodeSuccess;
	uint32_t reverseXValue;
	GENICAM_BoolNode *pBoolNode = NULL;
	GENICAM_BoolNodeInfo boolNodeInfo = {0};

	boolNodeInfo.pCamera = pGetCamera;
	memcpy(boolNodeInfo.attrName, "ReverseX", sizeof("ReverseX"));

	isBoolNodeSuccess = GENICAM_createBoolNode(&boolNodeInfo, &pBoolNode);
	if (0 != isBoolNodeSuccess)
	{
		printf("GENICAM_createBoolNode fail.\n");
		return -1;
	}

	reverseXValue = 0;
	isBoolNodeSuccess = pBoolNode->getValue(pBoolNode, &reverseXValue);
	if (0 != isBoolNodeSuccess)
	{
		printf("get ReverseX fail.\n");
		// 注意：需要释放pBoolNode内部对象内存
		// Attention: should release pBoolNode internal object before return
		pBoolNode->release(pBoolNode);
		return -1;
	}
	else
	{
		printf("before change ,ReverseX is %u\n", reverseXValue);
	}	

	isBoolNodeSuccess = pBoolNode->setValue(pBoolNode, 1);
	if (0 != isBoolNodeSuccess)
	{
		printf("set ReverseX fail.\n");
		// 注意：需要释放pBoolNode内部对象内存
		// Attention: should release pBoolNode internal object before return
		pBoolNode->release(pBoolNode);
		return -1;
	}

	isBoolNodeSuccess = pBoolNode->getValue(pBoolNode, &reverseXValue);
	if (0 != isBoolNodeSuccess)
	{
		printf("get ReverseX fail.\n");
		// 注意：需要释放pBoolNode内部对象内存
		// Attention: should release pBoolNode internal object before return
		pBoolNode->release(pBoolNode);
		return -1;
	}
	else
	{
		printf("after change ,ReverseX is %u\n", reverseXValue);
		// 注意：需要释放pBoolNode内部对象内存
		// Attention: should release pBoolNode internal object at the end of use
		pBoolNode->release(pBoolNode);
	}	

	return 0;
}

static int32_t modifyCameraDeviceUserID(GENICAM_Camera *pGetCamera)
{
	int32_t isStringNodeSuccess;
	char szDeviceUserID[256] = {0};
	uint32_t maxCnt;
	GENICAM_StringNode *pStringNode = NULL;
	GENICAM_StringNodeInfo stringNodeInfo = {0};

	stringNodeInfo.pCamera = pGetCamera;
	memcpy(stringNodeInfo.attrName, "DeviceUserID", sizeof("DeviceUserID"));

	isStringNodeSuccess = GENICAM_createStringNode(&stringNodeInfo, &pStringNode);
	if (0 != isStringNodeSuccess)
	{
		printf("GENICAM_createStringNode fail.\n");
		return -1;
	}

	memset(szDeviceUserID, 0, 256);
	maxCnt = 256;
	isStringNodeSuccess = pStringNode->getValue(pStringNode, szDeviceUserID, &maxCnt);
	if (0 != isStringNodeSuccess)
	{
		printf("get DeviceUserID fail.\n");
		// 注意：需要释放pStringNode内部对象内存
		// Attention: should release pStringNode internal object before return
		pStringNode->release(pStringNode);
		return -1;
	}
	else
	{
		printf("before change ,DeviceUserID is %s\n",szDeviceUserID);
	}	

	isStringNodeSuccess = pStringNode->setValue(pStringNode, "camera");
	if (0 != isStringNodeSuccess)
	{
		printf("set DeviceUserID fail.\n");
		// 注意：需要释放pStringNode内部对象内存
		// Attention: should release pStringNode internal object before return
		pStringNode->release(pStringNode);
		return -1;
	}

	memset(szDeviceUserID, 0, 256);
	maxCnt = 256;
	isStringNodeSuccess = pStringNode->getValue(pStringNode, szDeviceUserID, &maxCnt);
	if (0 != isStringNodeSuccess)
	{
		printf("get DeviceUserID fail.\n");
		// 注意：需要释放pStringNode内部对象内存
		// Attention: should release pStringNode internal object before return
		pStringNode->release(pStringNode);
		return -1;
	}
	else
	{
		printf("after change ,DeviceUserID is %s\n",szDeviceUserID);
		// 注意：需要释放pStringNode内部对象内存
		// Attention: should release pStringNode internal object at the end of use
		pStringNode->release(pStringNode);
	}	

	return 0;
}

static int32_t modifyCameraTriggerSelector(GENICAM_Camera *pGetCamera)
{
	int32_t isEnumNodeSuccess;
	char triggerSelectorValue[256] = {0};
	uint32_t maxCnt;
	GENICAM_EnumNode *pEnumNode = NULL;
	GENICAM_EnumNodeInfo enumNodeInfo = {0};

	enumNodeInfo.pCamera = pGetCamera;
	memcpy(enumNodeInfo.attrName, "TriggerSelector", sizeof("TriggerSelector"));

	isEnumNodeSuccess = GENICAM_createEnumNode(&enumNodeInfo, &pEnumNode);
	if( isEnumNodeSuccess != 0)
	{
		printf("GENICAM_createEnumNode fail.\n");
		return -1;
	}

	memset(triggerSelectorValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggerSelectorValue, &maxCnt);
	if (0 != isEnumNodeSuccess)
	{
		printf("get TriggerSelector fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("before change ,TriggerSelector is %s\n",triggerSelectorValue);
	}	

	isEnumNodeSuccess = pEnumNode->setValueBySymbol(pEnumNode, "FrameStart");
	if (0 != isEnumNodeSuccess)
	{
		printf("set TriggerSelector fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}

	memset(triggerSelectorValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggerSelectorValue, &maxCnt);
	if (0 != isEnumNodeSuccess)
	{
		printf("get TriggerSelector fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("after change ,TriggerSelector is %s\n",triggerSelectorValue);
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object at the end of use
		pEnumNode->release(pEnumNode);
	}	

	return 0;
}

static int32_t modifyCameraTriggerMode(GENICAM_Camera *pGetCamera)
{
	int32_t isEnumNodeSuccess;
	char triggerModeValue[256] = {0};
	uint32_t maxCnt;
	GENICAM_EnumNode *pEnumNode = NULL;
	GENICAM_EnumNodeInfo enumNodeInfo = {0};

	enumNodeInfo.pCamera = pGetCamera;
	memcpy(enumNodeInfo.attrName, "TriggerMode", sizeof("TriggerMode"));

	isEnumNodeSuccess = GENICAM_createEnumNode(&enumNodeInfo, &pEnumNode);
	if (0 != isEnumNodeSuccess)
	{
		printf("GENICAM_createEnumNode fail.\n");
		return -1;
	}

	memset(triggerModeValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggerModeValue, &maxCnt);
	if (0 != isEnumNodeSuccess)
	{
		printf("get TriggerMode fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("before change ,TriggerMode is %s\n",triggerModeValue);
	}	

	isEnumNodeSuccess = pEnumNode->setValueBySymbol(pEnumNode, "On");
	if (0 != isEnumNodeSuccess)
	{
		printf("set TriggerMode fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}

	memset(triggerModeValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggerModeValue, &maxCnt);
	if( isEnumNodeSuccess != 0)
	{
		printf("get TriggerMode fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("after change ,TriggerMode is %s\n", triggerModeValue);
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object at the end of use
		pEnumNode->release(pEnumNode);
	}	

	return 0;
}

static int32_t modifyCameraTriggerSource(GENICAM_Camera *pGetCamera)
{
	int32_t isEnumNodeSuccess;
	char triggeSourceValue[256] = {0};
	uint32_t maxCnt;
	GENICAM_EnumNode *pEnumNode = NULL;
	GENICAM_EnumNodeInfo enumNodeInfo = {0};

	enumNodeInfo.pCamera = pGetCamera;
	memcpy(enumNodeInfo.attrName, "TriggerSource", sizeof("TriggerSource"));

	isEnumNodeSuccess = GENICAM_createEnumNode(&enumNodeInfo, &pEnumNode);
	if (0 != isEnumNodeSuccess)
	{
		printf("GENICAM_createEnumNode fail.\n");
		return -1;
	}

	memset(triggeSourceValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggeSourceValue, &maxCnt);
	if (0 != isEnumNodeSuccess)
	{
		printf("get TriggerSource fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("before change ,TriggerSource is %s\n",triggeSourceValue);
	}	

	isEnumNodeSuccess = pEnumNode->setValueBySymbol(pEnumNode, "Software");
	if( isEnumNodeSuccess != 0)
	{
		printf("set TriggerSource fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}

	memset(triggeSourceValue, 0, 256);
	maxCnt = 256;
	isEnumNodeSuccess = pEnumNode->getValueSymbol(pEnumNode, triggeSourceValue, &maxCnt);
	if (0 != isEnumNodeSuccess)
	{
		printf("get TriggerSource fail.\n");
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object before return
		pEnumNode->release(pEnumNode);
		return -1;
	}
	else
	{
		printf("after change ,TriggerSource is %s\n", triggeSourceValue);
		// 注意：需要释放pEnumNode内部对象内存
		// Attention: should release pEnumNode internal object at the end of use
		pEnumNode->release(pEnumNode);
	}	

	return 0;
}

static int32_t executeTriggerSoftware(GENICAM_Camera *pGetCamera)
{
	int32_t isCmdNodeSuccess;
	GENICAM_CmdNode *pCmdNode = NULL;
	GENICAM_CmdNodeInfo cmdNodeInfo = {0};

	cmdNodeInfo.pCamera = pGetCamera;
	memcpy(cmdNodeInfo.attrName, "TriggerSoftware", sizeof("TriggerSoftware"));

	isCmdNodeSuccess = GENICAM_createCmdNode(&cmdNodeInfo, &pCmdNode);
	if (0 != isCmdNodeSuccess)
	{
		printf("GENICAM_createCmdNode fail.\n");
		return -1;
	}

	isCmdNodeSuccess = pCmdNode->execute(pCmdNode);
	if (0 != isCmdNodeSuccess)
	{
		printf("execute fail.\n");
		// 注意：需要释放pCmdNode内部对象内存
		// Attention: should release pCmdNode internal object before return
		pCmdNode->release(pCmdNode);
		return -1;
	}
	else
	{
		printf("execute TriggerSoftware success.\n");
		// 注意：需要释放pCmdNode内部对象内存
		// Attention: should release pCmdNode internal object at the end of use
		pCmdNode->release(pCmdNode);
	}	

	return 0;
}

static int32_t GENICAM_disconnect(GENICAM_Camera *pGetCamera)
{
	int32_t isDisconnectSuccess;

	isDisconnectSuccess = pGetCamera->disConnect(pGetCamera);
	if( isDisconnectSuccess != 0)
	{
		printf("disconnect fail.\n");
		return -1;
	}
	
	return 0;
}

// ***********开始： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********BEGIN: These functions are not related to API call and used to display device info***********
static void displayDeviceInfo(GENICAM_Camera *pCameraList, int cameraCnt)
{
	GENICAM_Camera *pDisplayCamera = NULL;
	GENICAM_GigECameraInfo GigECameraInfo;
	GENICAM_GigECamera* pGigECamera = NULL;
	int cameraIndex;
	int keyType;
	const char *vendorName = NULL;
	char vendorNameCat[11];
	const char* deviceUserID = NULL;
	char deviceUserIDCat[16];
	const char* ipAddress = NULL;

	int ret = 0;

	// 打印Title行 
	// Print title line 
	printf("\nIdx Type Vendor     Model      S/N             DeviceUserID    IP Address    \n");
	printf("------------------------------------------------------------------------------\n");

	for (cameraIndex = 0; cameraIndex < cameraCnt; cameraIndex++)
	{
		pDisplayCamera = &pCameraList[cameraIndex];
		// 设备列表的相机索引  最大表示字数：3
		// Camera index in device list, display in 3 characters
		printf("%-3d", cameraIndex + 1);

		// 相机的设备类型（GigE，U3V，CL，PCIe）
		// Camera type 
		keyType = pDisplayCamera->getType(pDisplayCamera);
		switch (keyType)
		{
		case typeGige:
			printf(" GigE");
			break;
		case typeUsb3:
			printf(" U3V ");
			break;
		case typeCL:
			printf(" CL  ");
			break;
		case typePCIe:
			printf(" PCIe");
			break;
		default:
			printf("     ");
			break;
		}

		// 制造商信息  最大表示字数：10 
		// Camera vendor name, display in 10 characters 
		vendorName = pDisplayCamera->getVendorName(pDisplayCamera);
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

		// 相机的型号信息 最大表示字数：10 
		// Camera model name, display in 10 characters 
		printf(" %-10.10s", pDisplayCamera->getModelName(pDisplayCamera));

		// 相机的序列号 最大表示字数：15 
		// Camera serial number, display in 15 characters 
		printf(" %-15.15s", pDisplayCamera->getSerialNumber(pDisplayCamera));

		// 自定义用户ID 最大表示字数：15 
		// Camera user id, display in 15 characters 
		deviceUserID = pDisplayCamera->getName(pDisplayCamera);
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

		// GigE相机时获取IP地址 
		// IP address of GigE camera 
		if (keyType == typeGige)
		{
			GigECameraInfo.pCamera = pDisplayCamera;
			ret = GENICAM_createGigECamera(&GigECameraInfo, &pGigECamera);
			if (ret == 0)
			{
				ipAddress = pGigECamera->getIpAddress(pGigECamera);
				if (ipAddress != NULL)
				{
					printf(" %s", pGigECamera->getIpAddress(pGigECamera));
				}

			}
		}

		printf("\n");
	}
	return;
}


static char* trim(char* pStr)
{
	char* pDst = pStr;
	char* pTemStr = NULL;
	int ret = -1;

	if (pDst != NULL)
	{
		pTemStr = pDst + strlen(pStr) - 1;
		while ( *pDst ==' ')
		{
			pDst++;
		}
		while ((pTemStr > pDst) &&(*pTemStr == ' '))
		{
			*pTemStr-- = '\0';
		}
	}
	return pDst;
}


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


static int selectDevice(int cameraCnt)
{
	char inputStr[256] = {0};
	char* pTrimStr;
	int inputIndex = -1;
	int ret = -1;

	printf("\nPlease input the camera index: ");
	while (1)
	{
		memset(inputStr, 0, sizeof(inputStr));
		scanf("%s", inputStr);
		fflush(stdin);

		pTrimStr = trim(inputStr);
		ret = isInputValid(pTrimStr);
		if (ret == 0)
		{
			inputIndex = atoi(pTrimStr);
			//  输入的序号从1开始 
			//  Input index starts from 1 
			inputIndex -= 1;
			if ((inputIndex >= 0) && (inputIndex < cameraCnt))
			{
				break;
			}
		}

		printf("Input invalid! Please input the camera index: ");
	}
	return inputIndex;
}
// ***********结束： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********END: These functions are not related to API call and used to display device info***********


void main()
{
	int32_t ret;
	GENICAM_System *pSystem = NULL;
	GENICAM_Camera *pCamera = NULL;
	GENICAM_Camera *pCameraList = NULL;
	uint32_t cameraCnt = 0;
	int cameraIndex = -1;

	ret = GENICAM_getSystemInstance(&pSystem);
	if (-1 == ret)
	{
		printf("GENICAM_getSystemInstance failed.\r\n");
		getchar();
		return ;
	}

	// 发现设备 
	// discover camera 
	ret = pSystem->discovery(pSystem, &pCameraList, &cameraCnt, typeAll);
	if (-1 == ret)
	{
		printf("discovery device fail.\r\n");
		getchar();
		return ;
	}
	
	if (cameraCnt < 1)
	{
		printf("there is no device.\r\n");
		getchar();
		return;
	}

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(pCameraList, cameraCnt);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	cameraIndex = selectDevice(cameraCnt);
	pCamera = &pCameraList[cameraIndex];

	// 连接设备 
	// connect to camera 
	ret = GENICAM_connect(pCamera);
	if (0 != ret)
	{
		printf("connect camera failed.\n");
		getchar();
		return;
	}

	// 修改相机曝光时间，通用double型属性访问实例 
	// set camera's ExposureTime, an example of double value property access 
	modifyCameraExposureTime(pCamera);

	// 修改相机像素宽度，通用int型属性访问实例 
	// set camera's Width, an example of integer value property access 
	modifyCameraWidth(pCamera);

	// 修改相机ReverseX，通用bool型属性访问实例 
	// set camera's ReverseX, an example of boolean value property access 
	modifyCameraReverseX(pCamera);

	// 修改相机DeviceUserID，通用string型属性访问实例 
	// set camera's DeviceUserID, an example of string value property access 
	modifyCameraDeviceUserID(pCamera);

	// 修改相机TriggerSelector，通用enum型属性访问实例 
	// set camera's TriggerSelector, an example of enumeration value property access 
	modifyCameraTriggerSelector(pCamera);

	// 修改相机TriggerMode，通用enum型属性访问实例 
	// set camera's TriggerMode, an example of enumeration value property access 
	modifyCameraTriggerMode(pCamera);

	// 修改相机TriggerSource，通用enum型属性访问实例 
	// set camera's TriggerSource, an example of enumeration value property access 
	modifyCameraTriggerSource(pCamera);

	// 执行相机TriggerSoftware，通用command型属性访问实例 
	// execute camera's TriggerSoftware, an example of command type property access 
	executeTriggerSoftware(pCamera);

	// 断开设备 
	// disconnect camera 
	ret = GENICAM_disconnect(pCamera);
	if (0 != ret)
	{
		printf("disconnect camera failed.\n");
		getchar();
		return;
	}
	
	printf("Press any key to exit.\n");
	getchar();
	return;
}

