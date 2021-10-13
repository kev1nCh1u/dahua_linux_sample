/// \file
/// \~chinese
/// \brief 拉流处理范例
/// \example grab.c
/// \~english
/// \brief stream grabbing sample
/// \example grab.c

// *******本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，
// 在运行之前，请使用相机客户端修改相机IP地址的网段与主机的网段一致*********    
//********This demo is a simple demonstration of the use of the SDK. There is no additional code to modify the camera IP.
//Before running, please use the camera client to modify the network segment of the camera IP address to be consistent with that of the host.*********

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pthread.h"
#include "GenICam/CAPI/SDK.h"
#include "Media/RecordVideo.h"

#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define CREATE_SUSPENDED    0x00000004
GENICAM_StreamSource *pStreamSource = NULL;
HANDLE hHandle = NULL;//录像句柄
int nThreadFinished = 0;

static int32_t GENICAM_connect(GENICAM_Camera *pGetCamera)
{
	int32_t isConnectSuccess;

	isConnectSuccess = pGetCamera->connect(pGetCamera, accessPermissionControl);

	if( isConnectSuccess != 0)
	{
		printf("connect cameral failed.\n");
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

int32_t CALL_METHOD frameGrabbingProc(void)
{
	int i;
	int32_t ret = -1;
	uint64_t imageCount = 0;
	GENICAM_Frame* pFrame;

	for (i = 0; i < 200; i++)
	{
		if(NULL == pStreamSource)
		{
			return 0;
		}


		ret = pStreamSource->getFrame(pStreamSource, &pFrame, 300);
		if (ret < 0)
		{
			printf("getFrame  fail.\n");
			continue;
		}

		ret = pFrame->valid(pFrame);
		if (ret < 0)
		{
			printf("frame is invalid!\n");

			// 注意：使用该帧后需要显示释放
			// Caution：release the frame after using it	
			pFrame->release(pFrame);

			continue;
		}

		printf("get frame[%llu] successfully!\n", ++imageCount);

		if (hHandle)
        {
            RECORD_SFrameInfo frameInfo;
            frameInfo.data = (const unsigned char*)pFrame->getImage(pFrame);
            frameInfo.size = pFrame->getImageSize(pFrame);
            frameInfo.paddingX = pFrame->getImagePaddingX(pFrame);
            frameInfo.paddingY = pFrame->getImagePaddingY(pFrame);
            frameInfo.pixelformat = pFrame->getImagePixelFormat(pFrame);

            // 录制一帧图像
            // record one frame
            if (RECORD_SUCCESS == inputOneFrame(hHandle, &frameInfo))
            {
                printf("record frame %llu successfully!\n", imageCount);
            }
        }
		
		// 注意：使用该帧后需要显示释放
		// Caution：release the frame after using it	
		pFrame->release(pFrame);
	}
	printf("run over \n");
	nThreadFinished=1;
	return 1;
}


// ********************** 这部分处理与SDK操作相机无关，用于显示设备列表 begin*****************************
//***********BEGIN: These functions are not related to API call and used to display device info***********
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
		// Idx 设备列表的相机索引 最大表示字数：3
		// Camera index in device list, display in 3 characters
		printf("%-3d", cameraIndex + 1);

		// Type 相机的设备类型（GigE，U3V，CL，PCIe
		// Camera type (eg:GigE，U3V，CL，PCIe)
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

		// VendorName 制造商信息 最大表示字数：10
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

		// ModeName 相机的型号信息 最大表示字数：10 
		// Camera model name, display in 10 characters 
		printf(" %-10.10s", pDisplayCamera->getModelName(pDisplayCamera));

		// Serial Number 相机的序列号 最大表示字数：15 
		// Camera serial number, display in 15 characters 
		printf(" %-15.15s", pDisplayCamera->getSerialNumber(pDisplayCamera));

		// deviceUserID 自定义用户ID 最大表示字数：15 
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
			// 防止console显示乱码,UTF8转换成ANSI进行显示
			// Prevent console from displaying garbled code and convert utf8 to ANSI for display
			memcpy(deviceUserIDCat, deviceUserID, sizeof(deviceUserIDCat));
			printf(" %-15.15s", deviceUserIDCat);
		}

		// IPAddress GigE相机时获取IP地址
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
	char* find = NULL;
	
	// 提示用户选择 
	// inform user to select
	printf("\nPlease input the camera index: ");
	while (1)
	{
		// 获取输入内容字符串 
		// get input string*/
		memset(inputStr, 0, sizeof(inputStr));
		fgets(inputStr, sizeof(inputStr), stdin);
		// 清空输入缓存 
		// clear input flush
		fflush(stdin);
		
		// fgets比gets多吃一个换行符号，取出换行符号
		// fgets eats one more line feed symbol than gets, and takes out the line feed symbol
		find = strchr(inputStr, '\n');
		if (find) { *find = '\0'; }
				
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

void main()
{
	int32_t ret;
	int32_t cameraIndex = -1;
	GENICAM_System *pSystem = NULL;
	GENICAM_Camera *pCamera = NULL;
	GENICAM_Camera *pCameraList = NULL;
	uint32_t cameraCnt = 0;

	pthread_t threadID;
	
	// 设置录像参数
    // set record parameters
    RECORD_SRecordParam sRecordParam;
    int64_t nWidth;
    GENICAM_IntNode *pWidthNode = NULL;
    GENICAM_IntNodeInfo widthNodeInfo = { 0 };
    int64_t nHeight;
    GENICAM_IntNode *pHeightNode = NULL;
    GENICAM_IntNodeInfo heightNodeInfo = { 0 };

	// 发现设备
	// discovery device
	ret = GENICAM_getSystemInstance(&pSystem);
	if (-1 == ret)
	{
		printf("pSystem is null.\r\n");
		return ;
	}

	ret = pSystem->discovery(pSystem, &pCameraList, &cameraCnt, typeAll);
	if (-1 == ret)
	{
		printf("discovery device fail.\r\n");
		return ;
	}

	if(cameraCnt < 1)
	{
		printf("no Camera is discovered.\r\n");
		return;
	}
	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址）
	// print camera info (index,Type,vendor name, model,serial number,DeviceUserID,IP Address)
	displayDeviceInfo(pCameraList, cameraCnt);
	// 选择需要连接的相机
	// Select the camera 
	cameraIndex = selectDevice(cameraCnt);
	pCamera = &pCameraList[cameraIndex];
	// 连接设备
	// connect to camera
	ret = GENICAM_connect(pCamera);
	if(ret != 0)
	{
		printf("connect cameral failed.\n");
		return;
	}
	
	
	// 创建流对象
	// create stream source instance
	ret = GENICAM_CreateStreamSource(pCamera, &pStreamSource);
	if(ret != 0)
	{
		printf("create stream obj  fail.\r\n");
		return;
	}

	
	// 拉流
	// start grabbing from camera
	ret = GENICAM_startGrabbing(pStreamSource);
	if(ret != 0)
	{
		printf("StartGrabbing  fail.\n");
		return;
	}
	
	    // 宽和高必须是2的整数倍
    // The width and height must be an integer multiple of 2.
    widthNodeInfo.pCamera = pCamera;
    memcpy(widthNodeInfo.attrName, "Width", sizeof("Width"));
    if (0 != GENICAM_createIntNode(&widthNodeInfo, &pWidthNode))
    {
        printf("GENICAM_createIntNode fail.\n");
        return;
    }
    if (0 == pWidthNode->isValid(pWidthNode) && 0 == pWidthNode->getValue(pWidthNode, &nWidth))
    {
        sRecordParam.width = nWidth;
    }
    else
    {
        printf("get Width fail.\n");
        return;
    }

    heightNodeInfo.pCamera = pCamera;
    memcpy(heightNodeInfo.attrName, "Height", sizeof("Height"));
    if (0 != GENICAM_createIntNode(&heightNodeInfo, &pHeightNode))
    {
        printf("GENICAM_createIntNode fail.\n");
        return;
    }
    if (0 == pHeightNode->isValid(pHeightNode) && 0 == pHeightNode->getValue(pHeightNode, &nHeight))
    {
        sRecordParam.height = nHeight;
    }
    else
    {
        printf("get Height fail.\n");
        return;
    }

    // 帧率(大于0)。请设置成实际取流帧率。
    // frame rate(greater than 0). Please set to the actual streaming frame rate.
    sRecordParam.frameRate = 20;

    //sRecordParam.frameRate = -1;

    //// 等待1000ms使驱动统计的帧率更准确
    //// Wait 1000ms to make the frame rate of drive statistics more accurate
    //CThread::sleep(1000);
    //StreamStatisticsInfo streamStatsInfo;
    //if (streamPtr->getStatisticsInfo(streamStatsInfo))
    //{
    //    if (cameraSptr->getType() == ICamera::typeGige)
    //    {
    //        sRecordParam.frameRate = streamStatsInfo.gigeStatisticsInfo.fps;
    //    }
    //    else if (cameraSptr->getType() == ICamera::typeU3v)
    //    {
    //        sRecordParam.frameRate = streamStatsInfo.u3vStatisticsInfo.fps;
    //    }
    //    else if (cameraSptr->getType() == ICamera::typePCIe)
    //    {
    //        sRecordParam.frameRate = streamStatsInfo.pcieStatisticsInfo.fps;
    //    }
    //}

    //// 如果从驱动获取帧率失败，则读取ResultingFrameRateAbs属性值当作回放帧率.
    //// 但由于网络性能等影响，该值与实际取流帧率可能有差距。
    //// If obtaining the frame rate from the driver fails, then read the ResultingFrameRateAbs property value as the playback frame rate.
    //// However, due to the impact of network performance, etc., this value may differ from the actual streaming frame rate.
    //if (sRecordParam.frameRate <= 0)
    //{
    //    double fFrameRate;
    //    CDoubleNode nodeFrameRate = CDoubleNode(cameraSptr, "ResultingFrameRateAbs");
    //    if (nodeFrameRate.isValid() && nodeFrameRate.getValue(fFrameRate))
    //    {
    //        sRecordParam.frameRate = fFrameRate;
    //    }
    //    else
    //    {
    //        printf("get ResultingFrameRateAbs fail.\n");
    //        return;
    //    }
    //}

    // 视频质量(1-100)。参数越大，视频越清晰，但体积也越大。
    // video quality(1-100). The larger the parameter, the clearer the video, but the larger the volume.
    sRecordParam.quality = 30;

    // 视频格式
    // video format
    sRecordParam.recordFmtType = RECORD_VIDEO_FMT_AVI;

    // 如果是绝对路径，例如"D:/Test/Record.avi"，请确保路径确实存在。
    // if it is a full path like "D:/Test/Record.avi", please make sure that the path does exist.
    sRecordParam.recordFilePath = "Record.avi";

    // 打开录像句柄
    // open the record hHandle
    if (RECORD_SUCCESS == openRecord(&sRecordParam, &hHandle))
    {
        // create frame grabbing thread
		if(pthread_create(&threadID, 0, frameGrabbingProc, NULL) != 0)
		{
			printf("Failed to create getFrame thread!\n");
			return;
		}

        // 等待拉流线程结束
		// wait until the grabbing thread ends
        while(nThreadFinished == 0)
		{
			usleep(10000);
		}

        // 关闭录像句柄
        // close the record hHandle
        closeRecord(hHandle);
    }
    else
    {
        printf("openRecord fail.\n");
    }
	
	
	// stop grabbing from camera
	GENICAM_stopGrabbing(pStreamSource);

	//close camera
	GENICAM_disconnect(pCamera);

	// close stream
	pStreamSource->release(pStreamSource);
	
	return;
}
