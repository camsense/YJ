﻿#ifndef HCLIDAR_H
#define HCLIDAR_H


#include "hchead.h"
#include "HC_serial.h"

#include <condition_variable>
#include <thread>
#include <mutex>
#include <cmath>
#include <list>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <map>

#include "HcData.h"

//typedef std::function<void(int)>              CallBackFunErroCode;
//typedef std::function<void(tsSDKStatistic)>   CallBackFunSecondInfo;
//typedef std::function<void(LstPointCloud)>    CallBackFunPointCloud;
//typedef std::function<void(LstNodeDistQ2)>    CallBackFunDistQ2;
//
//typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> MicroClock_type;

typedef struct tsLDSAttr
{
	double  dAngleOffsetD;
	double  dBaseline_mm;
	double  dTheta_d;
	int     iFPSMax;
	int     iFPSMin;
	int     iSpeedMax;
	int     iSpeedMin;
	double  dAngleStep;
	double  dCirclePoints;
	UINT64  u64TSStepNs;
	bool    bBlockSpeed;
	bool    bAngOffset;
	int     iFPSNor;
	int     iSpeedNor;
	tsLDSAttr()
	{
		dAngleOffsetD = 21;
		dBaseline_mm = 20;
		dTheta_d = 10;
		iFPSMax = FPS_2000_MAX;
		iFPSMin = FPS_2000_MIN;
		iSpeedMax = SPEED_312_MAX;
		iSpeedMin = SPEED_312_MIN;
		dAngleStep = ANGLE_RESOLV_2000;
		dCirclePoints = CICRLE_MAX_2000;
		u64TSStepNs = 1e9/FPS_2000_NOR;
		bBlockSpeed = true;
		bAngOffset = false;
		iFPSNor = FPS_2000_NOR;
		iSpeedNor = SPEED_312_NOR;
	}
}tsLDSAttr;

class HCLidar
{
public:
    
    ~HCLidar();

#if SHARK_ENABLE
	HCLidar();
#else
	static HCLidar& getInstance()
	{
		static HCLidar instance;  //局部静态变量
		return instance;
	}
#endif

	//init sdk
    BOOL initialize(const char* chPort, const char* chLidarModel, int iBaud, int iReadTimeoutMs, bool bDistQ2,bool bGetLoopData, bool bPollMode);
    BOOL unInit();

	//set callback function for error code
    void setCallBackFunErrorCode(CallBackFunErroCode fun)
    {
        m_funErrorCode = fun;
    }

	//set callback function for Statistic infomation
    void setCallBackFunSecondInfo(CallBackFunSecondInfo fun)
    {
        m_funSecondInfo = fun;
    }

	//set callback function for rx pointclouds
    void setCallBackFunPointCloud(CallBackFunPointCloud fun)
    {
        m_funPointCloud = fun;
    }



	//get error code
    int getLastErrCode()
    {
        int err = m_iLastErrorCode;
        if (m_iLastErrorCode != 0)
            m_iLastErrorCode = 0;
        return err;
    }

	bool getLidarInfo();

	//get SDK status
    int getSDKStatus()
    {
        return m_iSDKStatus;
    }

	// get SDK Version
	std::string getSDKVersion() const
    {
        return SDK_VER;
    }

	//get lidar ID
	std::string getLidarID();


	// get factory infomation
	std::string getFactoryInfo() const
    {
        return m_strFactoryInfo;
    }

	//get lidar model
	std::string getLidarModel() const
    {
        return m_strLidarModel;
    }

	//get firmware version
	std::string getFirmwareVersion() const
    {
        return m_strFirmwareVer;
    }

	//get hardware version
	std::string getHardwareVersion() const
    {
        return m_strHardwareVer;
    }

	// set work parameter
	void setWorkPara(tsSDKPara& sSDKPara);

	//poll mode,get pointclouds
    bool getRxPointClouds(LstPointCloud& lstG);



	void setFactoryMode(bool bFactoryMode=true)
	{
		m_bFactoryMode = bFactoryMode;
		m_iWaitIDTimeMs = 5000;
	}
	bool startFactoryModeRun();

	void setCircleDataMode()
	{
		m_bCircle = true;
	}

	void setAngOffset(bool bSetAngOffset)
	{
		m_bSetAngOffset = bSetAngOffset;
	}

	void setPointCloudLattice(bool bDistinguish)
	{
		m_bDistinguish = bDistinguish;

	}

	void setD2M7SubComp(bool bComp)
	{
		m_bComp = bComp;

	}

	//void setFilter(bool bfilter)
	//{
	//	m_bFilter = bfilter;
	//}


	void setLidarPowerOn(bool bPowerOn=true);

	void setLidarLowSpeed(bool bLow = true);
private:
	
#if SHARK_ENABLE
	
#else
	HCLidar();
	HCLidar(const HCLidar& other);
#endif

    void threadWork();
    void threadParse();
    void readData();

    void setReadCharsError(int errCode);


private:
    CallBackFunErroCode      m_funErrorCode=nullptr;
    CallBackFunSecondInfo    m_funSecondInfo=nullptr;
    CallBackFunPointCloud    m_funPointCloud=nullptr;
    CallBackFunDistQ2        m_funDistQ2=nullptr;

	bool                     m_bFactoryMode = false;
	int                      m_iWaitIDTimeMs = INIT_TIMEOUT_MS;
    int                      m_iBaud = 115200;
    std::string              m_strLidarModel = X1B;
    std::string              m_strFirmwareVer = DEFAULT_FIRMWARE;
    std::string              m_strHardwareVer = DEFAULT_HARDWARE;
    std::vector<UCHAR>       m_lstBuff;
    std::vector<UCHAR>       m_lstTemp;
    //UCHAR                   *m_p8Buff;
	UCHAR                    m_p8Buff[READ_BUFF_SIZE] = {0};

	std::atomic<bool>        m_bScanning;
	std::vector<UINT16>      m_lstSpeed;

    std::thread              m_threadWork;
    std::thread              m_threadParse;
    std::mutex               m_mtxBuff;

    std::mutex               m_mtxData;
    std::list<tsNodeInfo>    m_sNodeList;
    std::list<tsNodeInfo>    m_sLoopNodeList;
	LstPointCloud            m_resultRange;

    bool                     m_bGetLoopData = false;
    bool                     m_bPollMode=false;
    //bool                     m_bDistQ2=false;
    int                      m_iInvalidNumberContinue = 0;
    bool                     m_bGreaterThan = false;
    HC_serial                m_serial ;
    int                      m_iReadTimeOutms = 30;
    std::atomic<int>         m_iLastErrorCode ;
    std::atomic<int>         m_iSDKStatus ;

    double                   m_dAngleCur = 0;
    double                   m_dAnglePre = 0;
    bool                     m_bTurn = false;
    bool                     m_bFirsLoop = false;
	bool                     m_bCircle = false;
	LstPointCloud            m_lstCircle;
	std::vector<LstPointCloud> m_Circles;

	UINT64                   m_u64TSStart = 0;
	UINT64                   m_u64TSEnd = 0;

	std::atomic<bool>        m_bInitTimeout;
    std::atomic<bool>        m_bDisconnect;
    std::atomic<bool>        m_bHadID;
    std::atomic<bool>        m_bGetIDTimeOut;
    std::atomic<bool>        m_bHadFact;
    std::atomic<bool>        m_bGetFactTimeOut;
	std::atomic<bool>        m_bCheckSpeed;

    bool                     m_bHadInfo;
    bool                     m_bX2ID = true;
#if SHARK_ENABLE
    std::string              m_strDevID=DEFAULT_ID;
#else
	std::string              m_strDevID = "";
#endif
    std::string              m_strFactoryInfo=DEFAULT_FACTORY;

    tsSDKStatistic           m_sStatistic;
    std::mutex               m_mtxStatistic;

	 
    bool                     m_bThreadStart;
    bool                     m_bCompensate = true;
	tsLDSAttr                m_sAttr;//lidar attribute

    std::mutex               m_mtxInit;
    std::condition_variable  m_cvInit;
    bool                     m_bReady = false;
    tsSDKPara                m_sSDKPara;
    int                      m_iReadTimeoutCount=0;
	UINT64                   m_u64StartTimeNoData = 0;
    

    int                      m_iInvalidFPSSecond=0;
    UINT64                   m_u64StartTimeLowSpeed=0;
    UINT64                   m_u64StartTimeHighSpeed=0;
	UINT64                   m_u64StartTimeSpeed = 0;
    UINT64                   m_u64StartTimeSharkBlock=0;
    int                      m_iSharkBlockCount=0;
    UINT64                   m_u64StartTimeInvalidPoints=0;
	int                      m_iInvalidPointsCount = 0;
    int                      m_iValidNumber = 0;
	UINT64                   m_u64StartTimeFindPackHeader = 0;
	UINT64                   m_u64StartTimeCheckSpeed = 0;
	int                      m_iCheckSpeedCount = 0;
	UINT64					 m_u64checkBlockSpeed = 0;
	UINT16                   m_u16Speed = 0;
	std::map<int, UINT64>    m_mapErrorCode;
	std::mutex               m_mtxError;

	bool                     m_bSetAngOffset = false;
	double                   m_dAngOffset = 0;
	//bool                     m_bFilter = false;

	tsPackUID                m_sPackUID;
	bool                     m_bDistinguish = false;
	tsSDKIDD2M7              m_sD2M7;
	bool                     m_bComp = true;

	UINT64                   m_u64CountS = 0;
    UINT64                   m_u64StartMS = 0;
    inline bool bIntervalOneSecond(UINT64& u64StartUS)
    {
        UINT64 u64EndUs = HCHead::getCurrentTimestampUs();
        UINT64 u64Int= u64EndUs - u64StartUS ;

        if(u64Int  >=1000000 )
        {
            u64StartUS = u64EndUs;
            return true;
        }
        return false;
    }


private:
	void initPara();
    bool setLidarPara(const char* chLidarModel);
    void lidarReConnect();
    void processMain();
    bool processData();
    bool calIDX2(char* ch,int iLen);
    bool calIDX1(char* ch,int iLen);
    bool calStartInfo(char* ch,int iLen);
    bool calMCUFrame(char* ch,int iLen);
    bool getDevID(std::vector<UCHAR>& lstBuff);
    bool getStartInfo(std::vector<UCHAR>& lstBuff);
	bool getNewSNInfo(std::vector<UCHAR>& lstBuff);
    bool getPointCloud(std::vector<UCHAR>& lstBuff);
    bool getMCUCmd(std::vector<UCHAR>& lstBuff);

    bool parserRangeEX(LstPointCloud &resultRange,const char * chBuff, int iIndex, int in_numData,int iPointSize);
	void compensate(double &angle, UINT16 &dist, const double theta_d, const double baseline_mm);
    bool checkDataCal(std::vector<UCHAR>& lstBuff, int iIndex);
	bool checkDataCalTof(std::vector<UCHAR>& lstBuff, int iIndex);
    void checkReadPacketData();
    //void sendGetIDInfoSignal(bool bGetID);
    void sendGetFactoryInfoSignal(bool bGetFact);

    void checkInvalidFPS(int iFPS);
    void checkInvalidLowSpeed(UINT16 u16Speed);
    void checkInvalidHighSpeed(UINT16 u16Speed);
	void checkEncoderError(UINT16 u16Speed);
    void checkSharkBlocked();
    void checkSharkInvalidPoints(tsPointCloud& sData);
    void checkHadInitSuccess(bool bTimeout);
	void checkLDSVoltage();
	void checkPDCurrent();
	void checkChangeSpeed();

	void checkFindPackHeader();

	void checkBlockSpeed(UINT16 u16speed);

    
    void checkInvalidLidarNumber(int validNumber);
	bool checkContinuePacketErr();

	void pollModePointCloud();
	void callBackFunPointCloud();
	bool getOneCircleData();

	bool getErrorCode(int iError, int iMs);


	//////////////////////////////////////
	bool rockCheckLDSInfo(UINT8* buffer, unLidarInfo& lds_info);
	void rockDecodeLDSInfoPacket(UINT8* src, UINT8* dest, int& decode_size);
	bool rockCheckCRC(unLidarInfo lds_info);

	void resetParam();

	UINT16 getPointCloudLatticePeak(int dist_mm, UINT16 peak);
	bool isCheckMaxPeakLattice(int a1, int b1, int a2, int b2, int dist_mm, UINT16 peak);

	void compensateDistSubsection(tsSubsection& subsection_para, UINT16 & dist);

	bool isYJCalGood(UCHAR* pData, int iLen);
	bool isYJCalGood(std::vector<UCHAR>& lstBuff, int iLen);
	int  m_iYJVer = 0;


	std::condition_variable m_cv;
	std::mutex m_cvMtx;
	bool m_bSignalReceived = false;
	int  m_iPackIndex = 0;
	int  m_iSpeed = 0;
	int  m_iVerYJ = 0;
	bool m_bNorMode = true;//光强，normal
	double m_fLastAng = 0;

	std::atomic<bool>        m_bYJStart;
	std::atomic<int>         m_iMsgID;
	std::string              m_strSN = "";
	void readDataYJ();
	bool processDataYJ();

	void threadParseHC();
	void threadParseYJ();

	bool parseYJCMD(std::vector<UCHAR>& lstBuff);
	bool getPointCloudYJ(std::vector<UCHAR>& lstBuff);
	bool checkDataCalYJ(std::vector<UCHAR>& lstBuff, int iIndex);
	bool parserRangeEXYJ(LstPointCloud &resultRange, const char * chBuff, int iIndex, int in_numData, int iPointSize);

	void calLidarParaBySpeed();
	bool getHealthMsg(std::vector<UCHAR>& lstBuff);
public:

	std::string getDevYJSN();
	bool setYJStart(bool bStart);
	bool setYJWorkMode(bool bNor);
};

#endif // HCLIDAR_H
