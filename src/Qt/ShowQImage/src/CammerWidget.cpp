#include "CammerWidget.h"
#include "ui_cammerwidget.h"
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"

// Ĭ����ʾ֡��
// defult display frequency
#define DEFAULT_SHOW_RATE (30)
#define DEFAULT_ERROR_STRING ("N/A") 
#define MAX_FRAME_STAT_NUM (50) 
#define MIN_LEFT_LIST_NUM (2)

// List�ĸ���ʱ������һ֡��ʱ�������
// The maximum time interval between the update of list and the latest frame
#define MAX_STATISTIC_INTERVAL (5000000000)

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

CammerWidget::CammerWidget(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CammerWidget)
	, m_thdDisplayThread(CThreadLite::ThreadProc(&CammerWidget::DisplayThreadProc, this), "Display")
	, m_nDisplayInterval(0)
	, m_nFirstFrameTime(0)
	, m_nLastFrameTime(0)
	, m_bNeedUpdate(true)
	, m_nTotalFrameCount(0)
{
    ui->setupUi(this);

	qRegisterMetaType<uint64_t>("uint64_t");
	connect(this, SIGNAL(signalShowImage(uint8_t*, int, int, uint64_t)), this, SLOT(ShowImage(uint8_t*, int, int, uint64_t)));

	// Ĭ����ʾ30֡
	// defult display 30 frames 
	setDisplayFPS(DEFAULT_SHOW_RATE);

	m_elapsedTimer.start();

	// ������ʾ�߳�
	// start display thread
	if (!m_thdDisplayThread.isThreadOver())
	{
		m_thdDisplayThread.destroyThread();
	}
	m_thdDisplayThread.createThread();
}

CammerWidget::~CammerWidget()
{
    // �ر���ʾ�߳�
	// close display thread
    if (!m_thdDisplayThread.isThreadOver())
    {
        m_thdDisplayThread.destroyThread();
    }

    delete ui;
}

// ȡ���ص�����
// get frame callback function 
void CammerWidget::FrameCallback(const CFrame& frame)
{
	CFrameInfo frameInfo;
	frameInfo.m_nWidth = frame.getImageWidth();
	frameInfo.m_nHeight = frame.getImageHeight();
	frameInfo.m_nBufferSize = frame.getImageSize();
	frameInfo.m_nPaddingX = frame.getImagePadddingX();
	frameInfo.m_nPaddingY = frame.getImagePadddingY();
    frameInfo.m_ePixelType = frame.getImagePixelFormat();
	frameInfo.m_pImageBuf = (uint8_t *)malloc(sizeof(uint8_t)* frameInfo.m_nBufferSize);
	frameInfo.m_nTimeStamp = frame.getImageTimeStamp();

	// �ڴ�����ʧ�ܣ�ֱ�ӷ���
	// memory application failed, return directly
	if (frameInfo.m_pImageBuf != NULL)
	{
		memcpy(frameInfo.m_pImageBuf, frame.getImage(), frame.getImageSize());

		if (m_qDisplayFrameQueue.size() > 16)
		{
			CFrameInfo frameOld;
			m_qDisplayFrameQueue.get(frameOld);
			free(frameOld.m_pImageBuf);
		}

		m_qDisplayFrameQueue.push_back(frameInfo);
	}

	recvNewFrame(frame);
}

// �����ع�
// set exposeTime
bool CammerWidget::SetExposeTime(double dExposureTime)
{
	if (NULL == m_pCamera)
	{
		printf("Set ExposureTime fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeExposureTime(m_pCamera, "ExposureTime");

	if (false == nodeExposureTime.isValid())
	{
		printf("get ExposureTime node fail.\n");
		return false;
	}

	if (false == nodeExposureTime.isAvailable())
	{
		printf("ExposureTime is not available.\n");
		return false;
	}

    if (false == nodeExposureTime.setValue(dExposureTime))
	{
        printf("set ExposureTime value = %f fail.\n", dExposureTime);
		return false;
	}

	return true;
}

// ��������
// set gain
bool CammerWidget::SetAdjustPlus(double dGainRaw)
{
	if (NULL == m_pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeGainRaw(m_pCamera, "GainRaw");

	if (false == nodeGainRaw.isValid())
	{
		printf("get GainRaw node fail.\n");
		return false;
	}

	if (false == nodeGainRaw.isAvailable())
	{
		printf("GainRaw is not available.\n");
		return false;
	}

    if (false == nodeGainRaw.setValue(dGainRaw))
	{
		printf("set GainRaw value = %f fail.\n", dGainRaw);
		return false;
	}

	return true;
}

// �����
// open camera
bool CammerWidget::CameraOpen(void)
{
	if (NULL == m_pCamera)
	{
		printf("connect camera fail. No camera.\n");
		return false;
	}

	if (true == m_pCamera->isConnected())
	{
		printf("camera is already connected.\n");
		return true;
	}

	if (false == m_pCamera->connect())
	{
		printf("connect camera fail.\n");
		return false;
	}

	return true;
}

// �ر����
// close camera
bool CammerWidget::CameraClose(void)
{
	if (NULL == m_pCamera)
	{
		printf("disconnect camera fail. No camera.\n");
		return false;
	}

	if (false == m_pCamera->isConnected())
	{
		printf("camera is already disconnected.\n");
		return true;
	}

	if (false == m_pCamera->disConnect())
	{
		printf("disconnect camera fail.\n");
		return false;
	}

	return true;
}

// ��ʼ�ɼ�
// start grabbing
bool CammerWidget::CameraStart()
{
	if (NULL == m_pStreamSource)
	{
		m_pStreamSource = CSystem::getInstance().createStreamSource(m_pCamera);
	}

	if (NULL == m_pStreamSource)
	{
		return false;
	}

	if (m_pStreamSource->isGrabbing())
	{
		return true;
	}

	bool bRet = m_pStreamSource->attachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));
    if (!bRet)
	{
		return false;
	}

	if (!m_pStreamSource->startGrabbing())
	{
		return false;
	}

	return true;
}

// ֹͣ�ɼ�
// stop grabbing
bool CammerWidget::CameraStop()
{
	if (m_pStreamSource != NULL)
	{
		m_pStreamSource->detachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));

		m_pStreamSource->stopGrabbing();
		m_pStreamSource.reset();
	}

	// �����ʾ���� 
	// clear display queue
	m_qDisplayFrameQueue.clear();

	return true;
}

// �л��ɼ���ʽ��������ʽ �������ɼ����ⲿ����������������
// Switch acquisition mode and triggering mode (continuous acquisition, external triggering and software triggering)
bool CammerWidget::CameraChangeTrig(ETrigType trigType)
{
	if (NULL == m_pCamera)
	{
		printf("Change Trig fail. No camera or camera is not connected.\n");
		return false;
	}

	if (trigContinous == trigType)
	{
		// ���ô���ģʽ
		// set trigger mode
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
            return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("Off"))
		{
			printf("set TriggerMode value = Off fail.\n");
            return false;
		}
	}
	else if (trigSoftware == trigType)
	{
		// ���ô�����
		// set trigger
		CEnumNode nodeTriggerSelector(m_pCamera, "TriggerSelector");
		if (false == nodeTriggerSelector.isValid())
		{
			printf("get TriggerSelector node fail.\n");
            return false;
		}
		if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
		{
			printf("set TriggerSelector value = FrameStart fail.\n");
            return false;
		}

		// ���ô���ģʽ
		// set trigger mode
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
            return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("On"))
		{
			printf("set TriggerMode value = On fail.\n");
            return false;
		}

        // ���ô���ԴΪ������
		// set triggerSource as software trigger
        CEnumNode nodeTriggerSource(m_pCamera, "TriggerSource");
        if (false == nodeTriggerSource.isValid())
        {
            printf("get TriggerSource node fail.\n");
            return false;
        }
        if (false == nodeTriggerSource.setValueBySymbol("Software"))
        {
            printf("set TriggerSource value = Software fail.\n");
            return false;
        }
	}
	else if (trigLine == trigType)
	{
		// ���ô�����
		// set trigger
		CEnumNode nodeTriggerSelector(m_pCamera, "TriggerSelector");
		if (false == nodeTriggerSelector.isValid())
		{
			printf("get TriggerSelector node fail.\n");
            return false;
		}
		if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
		{
			printf("set TriggerSelector value = FrameStart fail.\n");
            return false;
		}

		// ���ô���ģʽ
		// set trigger mode
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
            return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("On"))
		{
			printf("set TriggerMode value = On fail.\n");
            return false;
		}

        // ���ô���ԴΪLine1����
		// set trigggerSource as Line1 trigger 
        CEnumNode nodeTriggerSource(m_pCamera, "TriggerSource");
        if (false == nodeTriggerSource.isValid())
        {
            printf("get TriggerSource node fail.\n");
            return false;
        }
        if (false == nodeTriggerSource.setValueBySymbol("Line1"))
        {
            printf("set TriggerSource value = Line1 fail.\n");
            return false;
        }
	}

    return true;
}

// ִ��һ��������
// execute one software trigger
bool CammerWidget::ExecuteSoftTrig(void)
{
	if (NULL == m_pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
        return false;
	}

	CCmdNode nodeTriggerSoftware(m_pCamera, "TriggerSoftware");
	if (false == nodeTriggerSoftware.isValid())
	{
		printf("get TriggerSoftware node fail.\n");
        return false;
	}
	if (false == nodeTriggerSoftware.execute())
	{
		printf("set TriggerSoftware fail.\n");
        return false;
	}

	printf("ExecuteSoftTrig success.\n");
    return true;
}

// ���õ�ǰ���  
// set current camera
void CammerWidget::SetCamera(const QString& strKey)
{
	CSystem &systemObj = CSystem::getInstance();
	m_pCamera = systemObj.getCameraPtr(strKey.toStdString().c_str());
}

// ��ʾ
// diaplay
bool CammerWidget::ShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat)
{
	QImage image;
	if (NULL == pRgbFrameBuf ||
		nWidth == 0 ||
		nHeight == 0)
	{
		printf("%s image is invalid.\n", __FUNCTION__);
		return false;
	}
    if (Dahua::GenICam::gvspPixelMono8 == nPixelFormat)
	{
		image = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_Grayscale8);
	}
	else
	{
		image = QImage(pRgbFrameBuf,nWidth, nHeight, QImage::Format_RGB888);
	}
    // ��QImage�Ĵ�С���������죬��label�Ĵ�С����һ�¡�����label������ʾ������ͼƬ
	// Shrink or stretch the size of Qimage to match the size of the label. In this way, the complete image can be displayed in the label
	QImage imageScale = image.scaled(QSize(ui->label_Pixmap->width(), ui->label_Pixmap->height()));
	QPixmap pixmap = QPixmap::fromImage(imageScale);
	ui->label_Pixmap->setPixmap(pixmap);
	free(pRgbFrameBuf);

	return true;
}

// ��ʾ�߳�
// display thread
void CammerWidget::DisplayThreadProc(Dahua::Infra::CThreadLite& lite)
{
	while (lite.looping())
	{
		CFrameInfo frameInfo;

		if (false == m_qDisplayFrameQueue.get(frameInfo, 500))
		{
			continue;
		}

		// �ж��Ƿ�Ҫ��ʾ��������ʾ���ޣ�30֡�����Ͳ���ת�롢��ʾ����
		// Judge whether to display. If the upper display limit (30 frames) is exceeded, transcoding and display processing will not be performed
		if (!isTimeToDisplay())
		{
			// �ͷ��ڴ�
			// release memory
			free(frameInfo.m_pImageBuf);
			continue;
		}

		// mono8��ʽ�ɲ���ת�룬ֱ����ʾ��������ʽ��Ҫ����ת�������ʾ 
		// mono8 format can be displayed directly without transcoding. Other formats can be displayed only after transcoding
        if (Dahua::GenICam::gvspPixelMono8 == frameInfo.m_ePixelType)
		{
            // ��ʾ�߳��з�����ʾ�źţ������߳�����ʾͼ��
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(frameInfo.m_pImageBuf, frameInfo.m_nWidth, frameInfo.m_nHeight, frameInfo.m_ePixelType);

		}
		else
		{
			// ת��
			// transcoding
			uint8_t *pRGBbuffer = NULL;
			int nRgbBufferSize = 0;
			nRgbBufferSize = frameInfo.m_nWidth * frameInfo.m_nHeight * 3;
			pRGBbuffer = (uint8_t *)malloc(nRgbBufferSize);
			if (pRGBbuffer == NULL)
			{
				// �ͷ��ڴ�
				// release memory
				free(frameInfo.m_pImageBuf);
				printf("RGBbuffer malloc failed.\n");
				continue;
			}

			IMGCNV_SOpenParam openParam;
			openParam.width = frameInfo.m_nWidth;
			openParam.height = frameInfo.m_nHeight;
			openParam.paddingX = frameInfo.m_nPaddingX;
			openParam.paddingY = frameInfo.m_nPaddingY;
			openParam.dataSize = frameInfo.m_nBufferSize;
            openParam.pixelForamt = frameInfo.m_ePixelType;

			IMGCNV_EErr status = IMGCNV_ConvertToRGB24(frameInfo.m_pImageBuf, &openParam, pRGBbuffer, &nRgbBufferSize);
			if (IMGCNV_SUCCESS != status)
			{
				// �ͷ��ڴ� 
				// release memory
				printf("IMGCNV_ConvertToRGB24 failed.\n");
				free(frameInfo.m_pImageBuf);
				free(pRGBbuffer);
				return;
			}

			// �ͷ��ڴ�
			// release memory
			free(frameInfo.m_pImageBuf);

            // ��ʾ�߳��з�����ʾ�źţ������߳�����ʾͼ��
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(pRGBbuffer, openParam.width, openParam.height, openParam.pixelForamt);

		}
	}
}

bool CammerWidget::isTimeToDisplay()
{
	CGuard guard(m_mxTime);

	// ����ʾ
	// don't display
	if (m_nDisplayInterval <= 0)
	{
		return false;
	}

	// ��һ֡������ʾ
	// the frist frame must be displayed
	if (m_nFirstFrameTime == 0 || m_nLastFrameTime == 0)
	{
		m_nFirstFrameTime = m_elapsedTimer.nsecsElapsed();
		m_nLastFrameTime = m_nFirstFrameTime;

		return true;
	}

	// ��ǰ֡����һ֡�ļ�����������ʾ�������ʾ
	// display if the interval between the current frame and the previous frame is greater than the display interval
	uint64_t nCurTimeTmp = m_elapsedTimer.nsecsElapsed();
	uint64_t nAcquisitionInterval = nCurTimeTmp - m_nLastFrameTime;
	if (nAcquisitionInterval > m_nDisplayInterval)
	{
		m_nLastFrameTime = nCurTimeTmp;
		return true;
	}

	// ��ǰ֡����ڵ�һ֡��ʱ����
	// Time interval between the current frame and the first frame
	uint64_t nPre = (m_nLastFrameTime - m_nFirstFrameTime) % m_nDisplayInterval;
	if (nPre + nAcquisitionInterval > m_nDisplayInterval)
	{
		m_nLastFrameTime = nCurTimeTmp;
		return true;
	}

	return false;
}

// ������ʾƵ��
// set display frequency
void CammerWidget::setDisplayFPS(int nFPS)
{
	if (nFPS > 0)
	{
		CGuard guard(m_mxTime);

		m_nDisplayInterval = 1000 * 1000 * 1000.0 / nFPS;
	}
	else
	{
		CGuard guard(m_mxTime);
		m_nDisplayInterval = 0;
	}
}

// ���ڹر���Ӧ����
// window close response function
void CammerWidget::closeEvent(QCloseEvent * event)
{
	if (NULL != m_pStreamSource && m_pStreamSource->isGrabbing())
		m_pStreamSource->stopGrabbing();
	if (NULL != m_pCamera && m_pCamera->isConnected())
		m_pCamera->disConnect();
}

// ״̬��ͳ����Ϣ ��ʼ
// Status bar statistics begin
void CammerWidget::resetStatistic()
{
	QMutexLocker locker(&m_mxStatistic);
	m_nTotalFrameCount = 0;
	m_listFrameStatInfo.clear();
	m_bNeedUpdate = true;
}
QString CammerWidget::getStatistic()
{
	if (m_mxStatistic.tryLock(30))
	{
		if (m_bNeedUpdate)
		{
			updateStatistic();
		}

		m_mxStatistic.unlock();
		return m_strStatistic;
	}
	return "";
}
void CammerWidget::updateStatistic()
{
	size_t nFrameCount = m_listFrameStatInfo.size();
	QString strFPS = DEFAULT_ERROR_STRING;
	QString strSpeed = DEFAULT_ERROR_STRING;

	if (nFrameCount > 1)
	{
		quint64 nTotalSize = 0;
		FrameList::const_iterator it = m_listFrameStatInfo.begin();

		if (m_listFrameStatInfo.size() == 2)
		{
            nTotalSize = m_listFrameStatInfo.back().m_nFrameSize;
		}
		else
		{
			for (++it; it != m_listFrameStatInfo.end(); ++it)
			{
                nTotalSize += it->m_nFrameSize;
			}
		}

		const FrameStatInfo& first = m_listFrameStatInfo.front();
		const FrameStatInfo& last = m_listFrameStatInfo.back();

        qint64 nsecs = last.m_nPassTime - first.m_nPassTime;

		if (nsecs > 0)
		{
			double dFPS = (nFrameCount - 1) * ((double)1000000000.0 / nsecs);
			double dSpeed = nTotalSize * ((double)1000000000.0 / nsecs) / (1000.0) / (1000.0) * (8.0);
			strFPS = QString::number(dFPS, 'f', 2);
			strSpeed = QString::number(dSpeed, 'f', 2);
		}
	}

	m_strStatistic = QString("Stream: %1 images   %2 FPS   %3 Mbps")
		.arg(m_nTotalFrameCount)
		.arg(strFPS)
		.arg(strSpeed);
	m_bNeedUpdate = false;
}
void CammerWidget::recvNewFrame(const CFrame& pBuf)
{
	QMutexLocker locker(&m_mxStatistic);
	if (m_listFrameStatInfo.size() >= MAX_FRAME_STAT_NUM)
	{
		m_listFrameStatInfo.pop_front();
	}
	m_listFrameStatInfo.push_back(FrameStatInfo(pBuf.getImageSize(), m_elapsedTimer.nsecsElapsed()));
	++m_nTotalFrameCount;

	if (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM)
	{
		FrameStatInfo infoFirst = m_listFrameStatInfo.front();
		FrameStatInfo infoLast = m_listFrameStatInfo.back();
        while (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM && infoLast.m_nPassTime - infoFirst.m_nPassTime > MAX_STATISTIC_INTERVAL)
		{
			m_listFrameStatInfo.pop_front();
			infoFirst = m_listFrameStatInfo.front();
		}
	}

	m_bNeedUpdate = true;
}
// ״̬��ͳ����Ϣ end 
// Status bar statistics ending