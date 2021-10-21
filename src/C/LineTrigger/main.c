/// \file
/// \~chinese
/// \brief 外部触发范例
/// \example main.c
/// \~english
/// \brief Line trigger sample
/// \example main.c

/************************************************************************/
// 本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，在运行之前，请使
// 用相机客户端修改相机IP地址的网段与主机的网段一致。                
// This Demo shows how to use GenICam API(C) to write a simple program.
// Please make sure that the camera and PC are in the same subnet before running the demo.
// If not, you can use camera client software to modify the IP address of camera to the same subnet with PC. 

// 本程序演示了发现设备，连接设备，设置外部触发，断开连接操作
// This program shows how to discover and connect device, set trigger config and disconnect device
/************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GenICam/CAPI/SDK.h"

int32_t isGrabbingFlag=0;

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

static int32_t GENICAM_CreateStreamSource(GENICAM_Camera *pGetCamera, GENICAM_StreamSource **ppStreamSource)
{
	int32_t isCreateStreamSource;
	GENICAM_StreamSourceInfo stStreamSourceInfo;


	stStreamSourceInfo.channelId = 0;
	stStreamSourceInfo.pCamera = pGetCamera;

	isCreateStreamSource = GENICAM_createStreamSource(&stStreamSourceInfo, ppStreamSource);

	if( isCreateStreamSource != 0)
	{
		printf("create stream obj  fail.\r\n");
		return -1;
	}

	return 0;
}

static int32_t GENICAM_startGrabbing(GENICAM_StreamSource *pStreamSource)
{
	int32_t isStartGrabbingSuccess;
	GENICAM_EGrabStrategy eGrabStrategy;

	eGrabStrategy = grabStrartegySequential;
	isStartGrabbingSuccess = pStreamSource->startGrabbing(pStreamSource, 0, eGrabStrategy);

	if( isStartGrabbingSuccess != 0)
	{
		printf("StartGrabbing  fail.\n");
		return -1;
	}

	return 0;
}

static int32_t GENICAM_stopGrabbing(GENICAM_StreamSource *pStreamSource)
{
	int32_t isStopGrabbingSuccess;

	isStopGrabbingSuccess = pStreamSource->stopGrabbing(pStreamSource);
	if( isStopGrabbingSuccess != 0)
	{
		printf("StopGrabbing  fail.\n");
		return -1;
	}

	return 0;
}

static int32_t setLineTriggerConf(GENICAM_AcquisitionControl *pAcquisitionCtrl)
{
	int32_t nRet;
	GENICAM_EnumNode enumNode = {0};
	
	// 设置触发源为外部触发 
	// Set trigger source to Line1 
	enumNode = pAcquisitionCtrl->triggerSource(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "Line1");
	if (nRet != 0)
	{
		printf("set trigger source failed.\n");
		//注意：需要释放enumNode内部对象内存
		//Attention: should release enumNode internal object before return
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	//Attention: should release enumNode internal object at the end of use
	enumNode.release(&enumNode);

	// 设置触发器 
	// Set trigger selector to FrameStart 
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerSelector(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "FrameStart");
	if (nRet != 0)
	{
		printf("set trigger selector failed.\n");
		//注意：需要释放enumNode内部对象内存
		//Attention: should release enumNode internal object before return
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	//Attention: should release enumNode internal object at the end of use
	enumNode.release(&enumNode);

	// 设置触发模式 
	// Set trigger mode to On 
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerMode(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "On");
	if (nRet != 0)
	{
		printf("set trigger mode failed.\n");
		//注意：需要释放enumNode内部对象内存
		//Attention: should release enumNode internal object before return
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	//Attention: should release enumNode internal object at the end of use
	enumNode.release(&enumNode);


	// 设置外触发为上升沿（下降沿为FallingEdge） 
	// Set trigger activation to RisingEdge(FallingEdge in opposite) 
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerActivation(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "RisingEdge");
	if (nRet != 0)
	{
		printf("set trigger activation failed.\n");
		//注意：需要释放enumNode内部对象内存
		//Attention: should release enumNode internal object before return
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	//Attention: should release enumNode internal object at the end of use
	enumNode.release(&enumNode);

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

void onGetFrame(GENICAM_Frame* pFrame)
{
	int32_t ret = -1;
	uint64_t blockId = 0;

	printf("\r\n");

	ret = pFrame->valid(pFrame);
	if(0 == ret)
	{
		blockId = pFrame->getBlockId(pFrame);
		printf("blockId = %d.\r\n",blockId);
		isGrabbingFlag=0;
	}
	else
	{
		printf("Frame is invalid!\n");
	}

	//注意：使用该帧后需要显示释放
	//Attention: should release pFrame object at the end of use
	pFrame->release(pFrame);

	return;
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
			// 输入的序号从1开始
			// Input index starts from 1
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
	GENICAM_StreamSource *pStreamSource = NULL;
	GENICAM_AcquisitionControl *pAcquisitionCtrl = NULL;
	GENICAM_AcquisitionControlInfo acquisitionControlInfo = {0};
	uint32_t cameraCnt = 0;
	int cameraIndex = -1;

	ret = GENICAM_getSystemInstance(&pSystem);
	if (-1 == ret)
	{
		printf("pSystem is null.\r\n");
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
	
	if(cameraCnt < 1)
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
	// Connect to camera
	ret = GENICAM_connect(pCamera);
	if(ret != 0)
	{
		printf("connect camera failed.\n");
		getchar();
		return;
	}
	
	// 创建AcquisitionControl属性节点
	// Create AcquisitionControl property node
	acquisitionControlInfo.pCamera = pCamera;
	ret=GENICAM_createAcquisitionControl(&acquisitionControlInfo, &pAcquisitionCtrl);
	if ((ret != 0) || (NULL == pAcquisitionCtrl))
	{
		printf("Create Acquisition Control Fail.\n");
		getchar();
		return;
	}

	// 设置外部触发配置
	// Set external trigger config
	ret = setLineTriggerConf(pAcquisitionCtrl);
	if (ret != 0)
	{
		printf("set Line trigger config failed.\n");
		//注意：需要释放pAcquisitionCtrl内部对象内存
		//Attention: should release pAcquisitionCtrl internal object before return
		pAcquisitionCtrl->release(pAcquisitionCtrl);
		getchar();
		return;
	}

	//注意：需要释放pAcquisitionCtrl内部对象内存
	//Attention: should release pAcquisitionCtrl internal object at the end of use
	pAcquisitionCtrl->release(pAcquisitionCtrl);

	// 创建流对象
	// Create stream source object
	ret = GENICAM_CreateStreamSource(pCamera, &pStreamSource);
	if((ret != 0) || (NULL == pStreamSource))
	{
		printf("create stream obj  fail.\r\n");
		getchar();
		return;
	}

	// 注册回调函数
	// Register grabbing callback function
	ret = pStreamSource->attachGrabbing(pStreamSource, onGetFrame);
	if (ret != 0)
	{
		printf("attch Grabbing fail!\n");
		//注意：需要释放pStreamSource内部对象内存
		//Attention: should release pStreamSource internal object before return
		pStreamSource->release(pStreamSource);
		getchar();
		return;
	}

	// 开始拉流
	// Start grabbing
	ret = GENICAM_startGrabbing(pStreamSource);
	if(ret != 0)
	{
		printf("StartGrabbing  fail.\n");
		//注意：需要释放pStreamSource内部对象内存
		//Attention: should release pStreamSource internal object before return
		pStreamSource->release(pStreamSource);
		isGrabbingFlag=0;
		getchar();
		return;
	}
	else
	{
		isGrabbingFlag=1;
	}

	while(isGrabbingFlag)
	{
		usleep(50000);
	}

	// 注销回调函数
	// Unregister grabbing callback function
	ret = pStreamSource->detachGrabbing(pStreamSource, onGetFrame);
	if (ret != 0)
	{
		printf("detach Grabbing fail!\n");
	}

	// 停止拉流
	// Stop grabbing
	ret = GENICAM_stopGrabbing(pStreamSource);
	if(ret != 0)
	{
		printf("Stop Grabbing  fail.\n");
	}

	// 注意：需要释放pStreamSource内部对象内存
	// Attention: should release pStreamSource internal object after grabbing
	pStreamSource->release(pStreamSource);

	// 断开设备
	// Disconnect camera
	ret = GENICAM_disconnect(pCamera);
	if(ret != 0)
	{
		printf("disconnect camera failed.\n");
		getchar();
		return;
	}
	
	printf("Press any key to exit.\n");
	getchar();
	return;
}
