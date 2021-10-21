#ifndef DAHUA_H
#define DAHUA_H

#include <math.h>
#include <QWidget>
#include <QMessageBox>
#include <QMutex>
#include "GenICam/System.h"
#include "Media/VideoRender.h"
#include "Media/ImageConvert.h"
#include "MessageQue.h"

class CFrameInfo : public Dahua::Memory::CBlock
{
public:
	CFrameInfo()
	{
		m_pImageBuf = NULL;
		m_nBufferSize = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_PixelType = Dahua::GenICam::gvspPixelMono8;
		m_nPaddingX = 0;
		m_nPaddingY = 0;
		m_nTimeStamp = 0;
	}

	~CFrameInfo()
	{
	}

public:
	BYTE		*m_pImageBuf;
	int			m_nBufferSize;
	int			m_nWidth;
	int			m_nHeight;
	Dahua::GenICam::EPixelType	m_PixelType;
	int			m_nPaddingX;
	int			m_nPaddingY;
	uint64_t	m_nTimeStamp;
};

namespace Ui {
class CammerWidget;
}

class CammerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CammerWidget(QWidget *parent = 0);
    ~CammerWidget();

	//枚举触发方式
	enum ETrigType
	{
		trigContinous = 0,	//连续拉流
		trigSoftware = 1,	//软件触发
		trigLine = 2,		//外部触发		
	};

	//回调函数
	void DahuaCallback(const Dahua::GenICam::CFrame & frame);
	//设置曝光
	bool SetExposeTime(double exposureTime);
	//设置增益
	bool SetAdjustPlus(double gainRaw);
	//打开相机
	bool CameraOpen(void);
	//关闭相机
	bool CameraClose(void);
	//开始采集
	bool CameraStart(void);
	//停止采集
	bool CameraStop(void);
	//切换采集方式、触发方式 （连续采集、外部触发、软件触发）
	void CameraChangeTrig(ETrigType trigType = trigContinous);
	//执行一次软触发（该接口为大华添加，用于执行软触发）            
	void ExecuteSoftTrig(void);
	//检测像机数、序列号
	void CameraCheck(void);
	/// \brief 显示一帧图像
	/// \param [in] pRgbFrameBuf 要显示的图像数据
	/// \param [in] nWidth 图像的宽
	/// \param [in] nHeight 图像的高
	/// \param [in] pixelFormat 图像的格式
	/// \retval true 显示失败
	/// \retval false  显示成功 
	bool ShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t pixelFormat);

private:
    Ui::CammerWidget *ui;
	
	Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vCameraPtrList;	// 发现的相机列表
	Dahua::GenICam::ICameraPtr m_pCamera;								// 当前相机，默认为列表中的第一个相机	
	Dahua::GenICam::IStreamSourcePtr m_pStreamSource;					// 流对象

	//CRender                             _render;          // 显示对象	
	Dahua::Infra::CThreadLite           _displayThread;   // 显示线程      
	TMessageQue<CFrameInfo>				_displayFrameQueue;// 显示队列

	Dahua::Infra::CMutex				m_mxTime;
	double								m_dDisplayInterval;         // 显示间隔
	uintmax_t							m_nTimestampFreq;           // 时间戳频率
	uintmax_t							m_nFirstFrameTime;          // 第一帧的时间戳
	uintmax_t							m_nLastFrameTime;           // 上一帧的时间戳

	VR_HANDLE          m_handler;           /* 绘图句柄 */
	VR_OPEN_PARAM_S    m_params;            /* 显示参数 */

	// 拉流线程
	void GrabFrameThreadProc(Dahua::Infra::CThreadLite& lite);

	// 显示线程
	void DisplayThreadProc(Dahua::Infra::CThreadLite& lite);

	// 设置显示频率，默认一秒钟显示30帧
	void setDisplayFPS(int nFPS);

	// 计算该帧是否显示
	bool isTimeToDisplay(uintmax_t nCurTime);

	/* 显示相关函数 */
	bool open(uint32_t width, uint32_t height);
	bool close();

	void closeEvent(QCloseEvent * event);

	VR_HWND		m_hWnd;
};

#endif // DAHUA_H
