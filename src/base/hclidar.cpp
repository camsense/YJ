﻿#include "hclidar.h"
#include <vector>
#include <set>

#include <algorithm>
#include <stdio.h>
#include <numeric>
#include <sstream>
#include <fstream>
//#include <eigen3/Eigen/Eigen>


HCLidar::HCLidar()
{
	initPara();
	memset(&m_sPackUID, 0, sizeof(tsPackUID));
    //m_p8Buff = new UCHAR[READ_BUFF_SIZE];
}

HCLidar::~HCLidar()
{
    m_lstBuff.clear();
    m_lstTemp.clear();
    unInit();

    //delete [] m_p8Buff;
    //m_p8Buff = nullptr;
}

void HCLidar::initPara()
{
	m_iSDKStatus = SDK_UNINIT;
	m_bScanning = false;
	m_iLastErrorCode = 0;
	m_bDisconnect = true;
	m_bHadID = false;
	m_bGetIDTimeOut = false;
	m_bHadFact = false;
	m_bGetFactTimeOut = false;
	m_bInitTimeout = false;
	m_sStatistic.resetAll();
	m_bCheckSpeed = false;
	m_bYJStart = false;
}


void HCLidar::lidarReConnect()
{
	m_bInitTimeout = false;
    m_bHadID=false;
    m_bGetIDTimeOut=false;
    m_bHadFact=false;
    m_bGetFactTimeOut=false;
}

std::string HCLidar::getLidarID()
{
	if (m_bFactoryMode)
	{
		if (m_strLidarModel == X2N)
			return m_strDevID;

		if (m_bHadID)
			return m_strDevID;
		else
		{
			return "";
		}
	}
	else
	{
		return m_strDevID;
	}
	
		
}


bool HCLidar::startFactoryModeRun()
{
	if (m_bFactoryMode)
	{
		if (m_bScanning)
			return false;
		m_bScanning = true;
		m_threadWork = std::thread(&HCLidar::threadWork, this);
		m_threadParse = std::thread(&HCLidar::threadParse, this);

		return true;

		/* std::unique_lock<std::mutex> lck(m_mtxInit);
		 while (!m_bReady)
			 m_cvInit.wait(lck);*/
	}
	return false;

}

void HCLidar::setWorkPara(tsSDKPara& sSDKPara)
{
	m_sSDKPara = sSDKPara;
}

bool HCLidar::setLidarPara(const char* chLidarModel)
{
    m_strLidarModel = chLidarModel;
    if(m_strLidarModel == X1B)
    {
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
    else if(m_strLidarModel == X1D)
    {
        m_sAttr.dAngleOffsetD = 21;
        m_sAttr.dBaseline_mm = 20;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
    else if(m_strLidarModel == X1E)
    {
        m_sAttr.dAngleOffsetD = 21;
        m_sAttr.dBaseline_mm = 20;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
    else if(m_strLidarModel == X1F)
    {
        m_sAttr.dAngleOffsetD = 21;
        m_sAttr.dBaseline_mm = 20;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
	else if (m_strLidarModel == X1G)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X1K)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X1L)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
    else if(m_strLidarModel == X1M)
    {
        m_sAttr.dAngleOffsetD = 21;
        m_sAttr.dBaseline_mm = 20;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
	else if (m_strLidarModel == X1N)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000_52HZ;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000_52HZ;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;
		m_sAttr.bBlockSpeed = false;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X1S)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_1800_MAX;
		m_sAttr.iFPSMin = FPS_1800_MIN;
		m_sAttr.iSpeedMax = SPEED_300_MAX;
		m_sAttr.iSpeedMin = SPEED_300_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_1800;
		m_sAttr.dCirclePoints = CICRLE_MAX_1800;
		m_sAttr.u64TSStepNs = 1e9 / FPS_1800_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_1800_NOR;
		m_sAttr.iSpeedNor = SPEED_300_NOR;
	}
    else if(m_strLidarModel == X2A)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_3000_MAX;
        m_sAttr.iFPSMin = FPS_3000_MIN;
        m_sAttr.iSpeedMax = SPEED_360_MAX;
        m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
    }
    else if(m_strLidarModel == X2B)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
    else if(m_strLidarModel == X2C)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_3000_MAX;
        m_sAttr.iFPSMin = FPS_3000_MIN;
        m_sAttr.iSpeedMax = SPEED_360_MAX;
        m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
    }
    else if(m_strLidarModel == X2E)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_3000_MAX;
        m_sAttr.iFPSMin = FPS_3000_MIN;
        m_sAttr.iSpeedMax = SPEED_360_MAX;
        m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
    }
    else if(m_strLidarModel == X2F)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_3000_MAX;
        m_sAttr.iFPSMin = FPS_3000_MIN;
        m_sAttr.iSpeedMax = SPEED_360_MAX;
        m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
    }
    else if(m_strLidarModel == X2M)
    {
        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
    else if(m_strLidarModel == X2N)
    {

        m_sAttr.dAngleOffsetD = 28.5;
        m_sAttr.dBaseline_mm = 17.92;
        m_sAttr.iFPSMax = FPS_2000_MAX;
        m_sAttr.iFPSMin = FPS_2000_MIN;
        m_sAttr.iSpeedMax = SPEED_312_MAX;
        m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
    }
	else if (m_strLidarModel == X2BF)//speed 6Hz/3Hz
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == X2BZ)
	{

		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X2B2)
	{

		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X2DE)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == X2MF)//speed 6Hz/3Hz
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000_6HZ;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000_6HZ;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == X2MX)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == X2MM)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17.92;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == D2A)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2B)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == D2M8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2A8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2B8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_3000_MAX;
		m_sAttr.iFPSMin = FPS_3000_MIN;
		m_sAttr.iSpeedMax = SPEED_360_MAX;
		m_sAttr.iSpeedMin = SPEED_360_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
		m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_3000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_3000_NOR;
		m_sAttr.iSpeedNor = SPEED_360_NOR;
	}
	else if (m_strLidarModel == D2S8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_1800_MAX;
		m_sAttr.iFPSMin = FPS_1800_MIN;
		m_sAttr.iSpeedMax = SPEED_300_MAX;
		m_sAttr.iSpeedMin = SPEED_300_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_1800;
		m_sAttr.dCirclePoints = CICRLE_MAX_1800;
		m_sAttr.u64TSStepNs = 1e9 / FPS_1800_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_1800_NOR;
		m_sAttr.iSpeedNor = SPEED_300_NOR;
	}
	else if (m_strLidarModel == D2SA)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_1800_MAX;
		m_sAttr.iFPSMin = FPS_1800_MIN;
		m_sAttr.iSpeedMax = SPEED_300_MAX;
		m_sAttr.iSpeedMin = SPEED_300_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_1800;
		m_sAttr.dCirclePoints = CICRLE_MAX_1800;
		m_sAttr.u64TSStepNs = 1e9 / FPS_1800_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_1800_NOR;
		m_sAttr.iSpeedNor = SPEED_300_NOR;
	}
	else if (m_strLidarModel == D2P8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2PD)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2M1)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2AE)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2CE)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2AG)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2AC)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2CM)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2M7)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2MK)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D3M8)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_4000_MAX;
		m_sAttr.iFPSMin = FPS_4000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_4000;
		m_sAttr.dCirclePoints = CICRLE_MAX_4000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_4000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_4000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D3A3)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == A200)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == A201)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
		m_sAttr.dCirclePoints = CICRLE_MAX_2000;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == D2YE)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_4000_MAX;
		m_sAttr.iFPSMin = FPS_4000_MIN;
		m_sAttr.iSpeedMax = SPEED_420_MAX;
		m_sAttr.iSpeedMin = SPEED_420_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_4000;
		m_sAttr.dCirclePoints = CICRLE_MAX_4000_7HZ;
		m_sAttr.u64TSStepNs = 1e9 / FPS_4000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_4000_NOR;
		m_sAttr.iSpeedNor = SPEED_420_NOR;

		m_bCircle = true;
	}
	else if (m_strLidarModel == D3AN)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_4000_MAX;
		m_sAttr.iFPSMin = FPS_4000_MIN;
		m_sAttr.iSpeedMax = SPEED_480_MAX;
		m_sAttr.iSpeedMin = SPEED_480_MIN;
		m_sAttr.dAngleStep = 360.0 * SPEED_480_NOR / 60 / FPS_4000_NOR;
		m_sAttr.dCirclePoints = FPS_4000_NOR * 60 / SPEED_480_NOR;
		m_sAttr.u64TSStepNs = 1e9 / FPS_4000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_4000_NOR;
		m_sAttr.iSpeedNor = SPEED_480_NOR;
	}
	else if (m_strLidarModel == D3YD)
	{
		m_sAttr.dAngleOffsetD = 28.5;
		m_sAttr.dBaseline_mm = 17;
		m_sAttr.dTheta_d = 0;
		m_sAttr.iFPSMax = FPS_2000_MAX;
		m_sAttr.iFPSMin = FPS_2000_MIN;
		m_sAttr.iSpeedMax = SPEED_312_MAX;
		m_sAttr.iSpeedMin = SPEED_312_MIN;
		m_sAttr.dAngleStep = 360.0 * SPEED_312_NOR / 60 / FPS_2000_NOR;
		m_sAttr.dCirclePoints = FPS_2000_NOR * 60 / SPEED_312_NOR;
		m_sAttr.u64TSStepNs = 1e9 / FPS_2000_NOR;
		m_sAttr.bAngOffset = true;

		m_sAttr.iFPSNor = FPS_2000_NOR;
		m_sAttr.iSpeedNor = SPEED_312_NOR;
	}
	else if (m_strLidarModel == T3B)
	{
		m_sAttr.dAngleOffsetD = 21;
		m_sAttr.dBaseline_mm = 20;
		m_sAttr.iFPSMax = FPS_TOF_MAX;
		m_sAttr.iFPSMin = FPS_TOF_MIN;
		m_sAttr.iSpeedMax = SPEED_TOF_MAX;
		m_sAttr.iSpeedMin = SPEED_TOF_MIN;
		m_sAttr.dAngleStep = ANGLE_RESOLV_TOF;
		m_sAttr.dCirclePoints = CICRLE_MAX_TOF;

		m_sAttr.iFPSNor = FPS_TOF_NOR;
		m_sAttr.iSpeedNor = SPEED_TOF_NOR;
	}
    else
    {

        return false;
    }
    return true;
}
//bGetLoopData 是否输出一圈的数据
BOOL HCLidar::initialize(const char* chPort, const char* chLidarModel,int iBaud, int iReadTimeoutMs,  bool bDistQ2,bool bGetLoopData, bool bPollMode)
{
	LOG_INFO("Init sdk\n");
    if(m_bScanning)
    {

		LOG_WARNING( "Had been init.\n");
        setReadCharsError(ERR_SDK_HAD_BEEN_INIT);
        return false;
    }
    if(chPort == NULL)
    {
		LOG_ERROR("port is null.\n" );
        setReadCharsError(ERR_SDK_INIT_PARA);
        return false;
    }

    if(chLidarModel == NULL)
    {
		LOG_ERROR("lidar mode is null.\n");
        setReadCharsError(ERR_SDK_INIT_PARA);
        return false;
    }

    m_iReadTimeOutms = iReadTimeoutMs;
    if( m_iReadTimeOutms<= 0 || m_iReadTimeOutms >= 50)
    {
        m_iReadTimeOutms = READ_TIMEOUT_MS;
    }
	
    if(!setLidarPara(chLidarModel))
    {
		LOG_ERROR("Lidar mode does not exists.\n");
        setReadCharsError(ERR_MODEL_NOT_EXISTS);
        return false;
    }
	resetParam();

	m_iBaud = iBaud;

    if (m_serial.openDevice(chPort, m_iBaud) != 1)
    {

		LOG_ERROR("open serial port failed.\n");
        setReadCharsError(ERR_SERIAL_INVALID_HANDLE);
        return false;
    }

    m_iSDKStatus = SDK_INIT;
	m_bScanning = false;
    m_bGetLoopData = bGetLoopData;
    m_bPollMode = bPollMode;
    //m_bDistQ2 = bDistQ2;

	if (!m_bFactoryMode)
	{
		
		m_bScanning = true;
		m_threadWork = std::thread(&HCLidar::threadWork, this);
		m_threadParse = std::thread(&HCLidar::threadParse, this);
	}
    

	LOG_INFO("Init complete\n");


	return true;
}

bool HCLidar::getLidarInfo()
{
	if (m_bHadID)
	{
		return true;
	}
	std::unique_lock<std::mutex> lck(m_mtxInit);
	while (!m_bReady)
		m_cvInit.wait(lck);

	return m_bHadID;
}

BOOL HCLidar::unInit()
{
	setYJStart(false);
	if (m_bFactoryMode)
	{
		if (!m_bScanning)
		{
			m_serial.closeDevice();
			return false;
		}
			

		m_bScanning = false;

		m_threadWork.detach();		
		m_threadParse.detach();
		/*if (m_threadWork.joinable())
			m_threadWork.join();


		if (m_threadParse.joinable())
			m_threadParse.join();*/

	}
	else
	{
		if (!m_bScanning)
			return false;

		m_bScanning = false;
		if (m_threadWork.joinable())
			m_threadWork.join();


		if(m_threadParse.joinable())
			m_threadParse.join();

	}
	
	   
	m_serial.closeDevice();
	initPara();
    return true;
}


void HCLidar::setReadCharsError(int errCode)
{
    switch (errCode)
    {
	case -6:
		m_iLastErrorCode = ERR_SERIAL_READFILE_FAILED;
		break;
    case -5:
        m_iLastErrorCode = ERR_SERIAL_INVALID_HANDLE;
        break;
    case -4:
        m_iLastErrorCode = ERR_SERIAL_SETCOMMTIMEOUTS_FAILED;
        break;
    case -3:
        m_iLastErrorCode = ERR_SERIAL_READFILE_FAILED;
        break;
    case -1:
        m_iLastErrorCode = ERR_SERIAL_READFILE_FAILED;
        break;
    case 0:
        m_iLastErrorCode = ERR_SERIAL_READFILE_ZERO;
        break;
    default:
        m_iLastErrorCode = errCode;
        break;
    }
	int iTemp = m_iLastErrorCode;
	LOG_ERROR("Error code:%d\n", iTemp);

    if(m_funErrorCode)
    {
        m_funErrorCode(m_iLastErrorCode);
    }

	std::lock_guard<std::mutex> lock(m_mtxError);
	m_mapErrorCode[m_iLastErrorCode] = HCHead::getCurrentTimestampUs();
}

void HCLidar::threadWork()
{
    while (m_bScanning)
    {
		if (m_strLidarModel == D2YE)
		{
			readDataYJ();
		}
		else
		{
			readData();
		}
    }
}

void HCLidar::readData()
{

    //memset(m_p8Buff,0, READ_BUFF_SIZE);

    //read data number
    int iRC = m_serial.readData(m_p8Buff, READ_BUFF_SIZE, m_iReadTimeOutms);
    if (iRC < 1)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::this_thread::yield();

        if(iRC == 0)
        {

			if (m_u64StartTimeNoData > 0)
			{
				UINT64 endTime = HCHead::getCurrentTimestampUs();
				//if ((endTime - m_u64StartTimeNoData) >= m_sSDKPara.iNoDataMS * 1000)
				if (endTime >= (m_u64StartTimeNoData+m_sSDKPara.iNoDataMS * 1000))
				{
					LOG_WARNING("Lidar read timeout!\n");

					setReadCharsError(iRC);
					m_u64StartTimeNoData = 0;

					m_iReadTimeoutCount++;

					int iTemp2 = m_sSDKPara.iDisconnectMS / m_sSDKPara.iNoDataMS;
					if (m_iReadTimeoutCount >= iTemp2)
					{
						LOG_ERROR("Lidar disconnect! status=%d\n", static_cast<int>(m_iSDKStatus));
						if (m_iSDKStatus == SDK_INIT)
						{
							checkHadInitSuccess(true);
							return;
						}
						m_iReadTimeoutCount = 0;
						m_bDisconnect = true;
						lidarReConnect();
						m_iSDKStatus = SDK_DISCONNECT;
						if (m_bFactoryMode)
						{
							m_bHadID = false;
							m_bGetIDTimeOut = false;
							m_bHadFact = false;
							m_bGetFactTimeOut = false;
						}


						setReadCharsError(ERR_DISCONNECTED);
					}
				}
			}
			else
			{
				m_u64StartTimeNoData = HCHead::getCurrentTimestampUs();
			}
			

        }
        else
        {
            setReadCharsError(iRC);
        }

        return;
    }



    m_iReadTimeoutCount=0;
	m_u64StartTimeNoData = 0;
    m_bDisconnect = false;

	std::lock_guard<std::mutex> lock(m_mtxBuff);
	for (int i = 0; i < iRC; i++)
	{
		m_lstTemp.push_back(m_p8Buff[i]);
	}

    
}




void HCLidar::readDataYJ()
{

	memset(m_p8Buff, 0, READ_BUFF_SIZE);

	//read data number
	int iRC = m_serial.readData(m_p8Buff, READ_BUFF_SIZE, m_iReadTimeOutms);
	if (iRC < 1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::this_thread::yield();

		if (iRC == 0)
		{
			if (!m_bYJStart)
			{
				m_u64StartTimeNoData = 0;
				return;
			}
				

			if (m_u64StartTimeNoData > 0)
			{
				UINT64 endTime = HCHead::getCurrentTimestampUs();
				//if ((endTime - m_u64StartTimeNoData) >= m_sSDKPara.iNoDataMS * 1000)
				if (endTime >= (m_u64StartTimeNoData + m_sSDKPara.iNoDataMS * 1000))
				{
					LOG_WARNING("Lidar read timeout!\n");

					setReadCharsError(iRC);
					m_u64StartTimeNoData = 0;

					m_iReadTimeoutCount++;

					int iTemp2 = m_sSDKPara.iDisconnectMS / m_sSDKPara.iNoDataMS;
					if (m_iReadTimeoutCount >= iTemp2)
					{
						LOG_ERROR("Lidar disconnect! status=%d\n", static_cast<int>(m_iSDKStatus));
						if (m_iSDKStatus == SDK_INIT)
						{
							checkHadInitSuccess(true);
							return;
						}
						m_iReadTimeoutCount = 0;
						m_bDisconnect = true;
						lidarReConnect();
						m_iSDKStatus = SDK_DISCONNECT;
						if (m_bFactoryMode)
						{
							m_bHadID = false;
							m_bGetIDTimeOut = false;
							m_bHadFact = false;
							m_bGetFactTimeOut = false;
						}


						setReadCharsError(ERR_DISCONNECTED);
					}
				}
			}
			else
			{
				m_u64StartTimeNoData = HCHead::getCurrentTimestampUs();
			}


		}
		else
		{
			setReadCharsError(iRC);
		}

		return;
	}



	m_iReadTimeoutCount = 0;
	m_u64StartTimeNoData = 0;
	m_bDisconnect = false;

	std::lock_guard<std::mutex> lock(m_mtxBuff);
	for (int i = 0; i < iRC; i++)
	{
		m_lstTemp.push_back(m_p8Buff[i]);
	}


}
void HCLidar::threadParse()
{

	if (m_strLidarModel == D2YE)
	{
		threadParseYJ();
	}
	else
	{
		threadParseHC();
	}
}

void HCLidar::threadParseHC()
{

	m_u64TSEnd = HCHead::getCurrentTimestampNs();
	UINT64 u64Start = HCHead::getCurrentTimestampUs();

	while (m_bScanning)
	{
		checkReadPacketData();

		{
			std::lock_guard<std::mutex> lock(m_mtxBuff);
			if (m_lstTemp.size() > 0)
			{
				std::vector<UCHAR>  lstTemp;
				lstTemp.swap(m_lstTemp);

				m_lstBuff.insert(m_lstBuff.end(), lstTemp.begin(), lstTemp.end());
			}

		}


		if (!m_bInitTimeout)
		{
			UINT64 u64End = HCHead::getCurrentTimestampUs();
			//UINT64 u64Int = (u64End - u64Start) / 1000;

			//if (u64Int >= m_iWaitIDTimeMs)
			if (u64End >= (u64Start + m_iWaitIDTimeMs * 1000))
			{
				if (!m_bHadID && !m_bGetIDTimeOut)
				{
					m_bGetIDTimeOut = true;
					m_bHadID = false;
					LOG_INFO("Get Device ID timeout\n");
					checkHadInitSuccess(true);
				}

				if (!m_bHadFact && !m_bGetFactTimeOut && m_bHadID)
				{

					LOG_INFO("Get Two CMD info timeout\n");
					checkHadInitSuccess(false);
				}

				//m_iSDKStatus = SDK_WORKING;
				m_bInitTimeout = true;
			}
		}
		else
		{
			u64Start = HCHead::getCurrentTimestampUs();
		}


		if (m_bDisconnect)
		{
			std::vector<UCHAR>  lstTemp;
			lstTemp.swap(m_lstBuff);

			u64Start = HCHead::getCurrentTimestampUs();

			m_sStatistic.resetAll();
		}
		else
		{
			if (m_sStatistic.u64TSRxPacketFirst == 0)
			{
				m_sStatistic.u64TSRxPacketFirst = HCHead::getCurrentTimestampUs();
				m_u64StartMS = m_sStatistic.u64TSRxPacketFirst;
			}
			processMain();


		}



		checkChangeSpeed();

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		std::this_thread::yield();
	}
}


void HCLidar::threadParseYJ()
{

	m_u64TSEnd = HCHead::getCurrentTimestampNs();
	UINT64 u64Start = HCHead::getCurrentTimestampUs();

	while (m_bScanning)
	{
		checkReadPacketData();

		{
			std::lock_guard<std::mutex> lock(m_mtxBuff);
			if (m_lstTemp.size() > 0)
			{
				std::vector<UCHAR>  lstTemp;
				lstTemp.swap(m_lstTemp);

				m_lstBuff.insert(m_lstBuff.end(), lstTemp.begin(), lstTemp.end());
			}

		}
					

		if (m_bDisconnect)
		{
			std::vector<UCHAR>  lstTemp;
			lstTemp.swap(m_lstBuff);

			u64Start = HCHead::getCurrentTimestampUs();

			m_sStatistic.resetAll();
		}
		else
		{
			if (m_sStatistic.u64TSRxPacketFirst == 0)
			{
				m_sStatistic.u64TSRxPacketFirst = HCHead::getCurrentTimestampUs();
				m_u64StartMS = m_sStatistic.u64TSRxPacketFirst;
			}
			processMain();


		}
		
		//checkChangeSpeed();

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		std::this_thread::yield();
	}
}

void HCLidar::checkReadPacketData()
{

   // if(m_sStatistic.u64TSRxPacketFirst == 0)
        //return;


    if(bIntervalOneSecond(m_u64StartMS))
    {
        m_sStatistic.u64CurrentS++;
		if (m_strLidarModel != D2YE)
		{
			m_sStatistic.u64FPS = m_sStatistic.iPacketPerSecond*m_sStatistic.iNumPerPacket;
		}

        m_sStatistic.u64TimeStampS = HCHead::getCurrentTimestampUs()/1e6;
        checkInvalidFPS(m_sStatistic.u64FPS);

		m_sStatistic.dRMS = 0;
		double dSum = std::accumulate(m_lstSpeed.begin(), m_lstSpeed.end(), 0);
		if (m_lstSpeed.size() > 0)
		{
			m_sStatistic.dRMS = dSum / m_lstSpeed.size(); //均值
			m_lstSpeed.clear();
		}
			
		calLidarParaBySpeed();


        if(m_funSecondInfo)
        {
            m_funSecondInfo(m_sStatistic);
        }
        m_sStatistic.reset();

		m_u64CountS++;
		checkLDSVoltage();

		
    }
}

void HCLidar::calLidarParaBySpeed()
{
	if (m_strLidarModel == D3YD)
	{
		if (m_sStatistic.dRMS > SPEED_180_MIN && m_sStatistic.dRMS < SPEED_312_MAX)
		{
			m_sAttr.iSpeedMax = m_sStatistic.dRMS + SPEED_250_RANGE;
			m_sAttr.iSpeedMin = m_sStatistic.dRMS - SPEED_250_RANGE;
			m_sAttr.dAngleStep = 360.0 * m_sStatistic.dRMS / 60 / FPS_2000_NOR;
			m_sAttr.dCirclePoints = FPS_2000_NOR * 60 / m_sStatistic.dRMS;
			m_sAttr.iSpeedNor = m_sStatistic.dRMS;
		}
		
	}
	
}
void HCLidar::processMain()
{
    if(m_lstBuff.size()==0)
        return;
    while (true)
    {
		if (m_strLidarModel == D2YE)
		{
			if (!processDataYJ())
				break;
		}
		else
		{
			if (!processData())
				break;
		}
        if(m_lstBuff.size()==0)
            break;

    }
    //printf("Info: buff size = %d\n",m_lstBuff.size());
}

/*
void HCLidar::sendGetIDInfoSignal(bool bGetID)
{
    if(!bGetID)
    {
        setReadCharsError(ERR_NOT_ID);
    }

    m_bHadID = bGetID;
    checkHadInitSuccess(false);
}
*/


void HCLidar::sendGetFactoryInfoSignal(bool bGetFact)
{
    if(!bGetFact)
    {
        setReadCharsError(ERR_NOT_ID);
    }

    m_bHadFact = bGetFact;
    checkHadInitSuccess(false);
}

void HCLidar::checkHadInitSuccess(bool bTimeout)
{
    if(bTimeout)
    {
        m_iSDKStatus = SDK_ID_TIMEOUT;
        std::unique_lock <std::mutex> lck(m_mtxInit);
        m_bReady = true;
        m_cvInit.notify_all();
    }
    else
    {
        if(m_bHadID && m_bHadFact)
        {
            m_iSDKStatus = SDK_WORKING;
            std::unique_lock <std::mutex> lck(m_mtxInit);
            m_bReady = true;
            m_cvInit.notify_all();
        }
		else// if (m_bHadID && !m_bHadFact)
		{
			m_iSDKStatus = SDK_ID_TIMEOUT;
			std::unique_lock <std::mutex> lck(m_mtxInit);
			m_bReady = true;
			m_cvInit.notify_all();
		}
    }
}

bool HCLidar::checkContinuePacketErr()
{
	if (m_sStatistic.iErrorCountContinue >= NUMBER_CONTINUE_ERROR_PACKET)
	{
		m_sStatistic.iErrorCountContinue = 0;
		if (m_lstBuff.size() > 3)
			HCHead::eraseBuff(m_lstBuff, m_lstBuff.size() - 1);
		else
			m_lstBuff.clear();
		LOG_ERROR("Continuous Rx error packet:%d!\n", NUMBER_CONTINUE_ERROR_PACKET);
		setReadCharsError(ERR_RX_CONTINUE);
		return true;
	}
	return false;
}
bool HCLidar::processData()
{
    if(m_lstBuff.size()<=2)
        return false;

	if (checkContinuePacketErr())
		return false;
	
    int iIndex = -1;
    int iMsgID = 0;
    for(int i=0;i<m_lstBuff.size()-1;i++)
    {
        
        if(m_iSDKStatus == SDK_INIT || (m_bFactoryMode && m_iSDKStatus == SDK_DISCONNECT))
        {
			if (m_strLidarModel == X1S)
			{
				if (!m_bHadID && !m_bGetIDTimeOut)
				{
					if ((UCHAR)m_lstBuff.at(i) == 0xAA)//ID
					{
						iMsgID = MSG_ID;
						iIndex = i;
						break;
					}
				}
			}
			else
			{
				if (!m_bHadID && !m_bGetIDTimeOut)
				{
					if ((UCHAR)m_lstBuff.at(i) == 0x5A && (UCHAR)m_lstBuff.at(i + 1) == 0xA5)//ID
					{
						iMsgID = MSG_ID;
						iIndex = i;
						break;
					}
					
				}
				if (!m_bHadFact && !m_bGetFactTimeOut)
				{
					if ((UCHAR)m_lstBuff.at(i) == 0x5A && (UCHAR)m_lstBuff.at(i + 1) == 0xAA)//SN
					{
						iMsgID = MSG_NEW_SN;
						iIndex = i;
						break;
					}

					if ((UCHAR)m_lstBuff.at(i) == 0xA5 && (UCHAR)m_lstBuff.at(i + 1) == 0x5A)//Two CMD
					{
						iMsgID = MSG_CMD;
						iIndex = i;
						break;
					}
				}
			}
                
        }
        else if((UCHAR)m_lstBuff.at(i) == 0x55 && (UCHAR)m_lstBuff.at(i+1) == 0xAA)//point cloud
        {
            iMsgID = MSG_POINTCLOUD;
            iIndex = i;
            break;
        }
        
    }
    if(iIndex < 0)
    {
		//LOG_INFO("HCSDK Error: rx data not mes header\n" );
        m_lstBuff.clear();
		
		checkFindPackHeader();
        return false;
    }
	else
	{
		m_u64StartTimeFindPackHeader = 0;
	}

    if(iIndex>0)
    {
        HCHead::eraseBuff(m_lstBuff,iIndex);
		//LOG_INFO("HCSDK Error: find mes header,buff size=%d\n" , m_lstBuff.size());
    }


    switch (iMsgID) {
    case MSG_ID:
            return getDevID(m_lstBuff);
        //break;
    case MSG_CMD:
            return getStartInfo(m_lstBuff);
	case MSG_NEW_SN:
		return getNewSNInfo(m_lstBuff);
       // break;
    case MSG_POINTCLOUD:
		return getPointCloud(m_lstBuff);

    }

    return false;

}
bool HCLidar::processDataYJ()
{
	if (m_lstBuff.size() <= 2)
		return false;

	//if (checkContinuePacketErr())
		//return false;

	int iIndex = -1;
	int iMsgID = 0;
	for (int i = 0; i < m_lstBuff.size() - 1; i++)
	{

		if ((UCHAR)m_lstBuff.at(i) == 0xA5 && (UCHAR)m_lstBuff.at(i + 1) == 0x5A)//ID
		{
			iMsgID = MSG_YJ_CMD;
			iIndex = i;
			break;
		}
		else if ((UCHAR)m_lstBuff.at(i) == 0xAA && (UCHAR)m_lstBuff.at(i + 1) == 0x55)//point cloud
		{
			iMsgID = MSG_YJ_PC;
			iIndex = i;
			break;
		}

		
	}
	if (iIndex < 0)
	{
		if (((UCHAR)m_lstBuff.at(m_lstBuff.size() - 1) == 0xA5) || ((UCHAR)m_lstBuff.at(m_lstBuff.size() - 1) == 0xAA))
		{
			LOG_INFO("HCSDK Error: find mes header,buff size=%d, erase len=%d\n", m_lstBuff.size(), iIndex);
			return false;
		}


		m_lstBuff.clear();

		checkFindPackHeader();
		return false;
	}
	else
	{
		m_u64StartTimeFindPackHeader = 0;
	}

	if (iIndex > 0)
	{
		HCHead::eraseBuff(m_lstBuff, iIndex);
		//LOG_INFO("HCSDK Error: find mes header,buff size=%d, erase len=%d\n" , m_lstBuff.size(), iIndex);
	}

	m_iMsgID = iMsgID;
	switch (iMsgID) {

	case MSG_YJ_CMD:
		return parseYJCMD(m_lstBuff);
		// break;
	case MSG_YJ_PC:
		return getPointCloudYJ(m_lstBuff);

	}

	return false;

}

void HCLidar::checkFindPackHeader()
{
	if (m_bInitTimeout)
	{
		if (m_u64StartTimeFindPackHeader > 0)
		{
			UINT64 u64Temp = HCHead::getCurrentTimestampUs();
			//UINT64 u64Int = u64Temp - m_u64StartTimeFindPackHeader;

			//if (u64Int >= 100000) //100ms
			if (u64Temp >= (m_u64StartTimeFindPackHeader+100000)) //100ms
			{
				m_u64StartTimeFindPackHeader = 0;
				setReadCharsError(ERR_FIND_HEAD_TIMEOUT);
			}
		}
		else
		{
			m_u64StartTimeFindPackHeader = HCHead::getCurrentTimestampUs();
		}
	}
	else
	{
		m_u64StartTimeFindPackHeader = 0;
	}
}

bool HCLidar::calIDX2(char* ch,int iLen)
{
    UINT sum = 0;
    const UINT id_len = iLen-1;
    for (UINT i = 0; i < id_len; i++)
    {
       sum += (UCHAR)ch[i];
    }

    const UINT sum_check = (UCHAR)ch[id_len];
    unsigned char temp_sum = sum & 0xff;
    return (sum_check == temp_sum);
}

bool HCLidar::calIDX1(char* ch,int iLen)
{
    UINT sum = 0;
    const UINT id_len = iLen-2;
    for (UINT i = 0; i < id_len; i++)
    {
        sum += (UCHAR)ch[i];
    }

    const UINT sum_check = ((UCHAR)ch[id_len + 1] << 8) | (UCHAR)ch[id_len];
    return (sum_check == sum);
}

bool HCLidar::getDevID(std::vector<UCHAR>& lstBuff)
{
	LOG_INFO("getDevID!\n");

	char chTemp[128] = { 0 };
	if (m_strLidarModel == "X1S")//rock
	{
		unLidarInfo lds;

		int iLen = sizeof(unLidarInfo) + 1;
		if (lstBuff.size() < iLen)
		{
			return false;
		}
		bool bOK = rockCheckLDSInfo((uint8_t*)lstBuff.data() + 1, lds);
		if (bOK)
		{
			memset(chTemp, 0, 128);
			sprintf(chTemp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				(lds.sAttr.sUid.UID_0 & 0x00ff),
				(lds.sAttr.sUid.UID_0 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_1 & 0x00ff),
				(lds.sAttr.sUid.UID_1 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_2 & 0x00ff),
				(lds.sAttr.sUid.UID_2 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_3 & 0x00ff),
				(lds.sAttr.sUid.UID_3 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_4 & 0x00ff),
				(lds.sAttr.sUid.UID_4 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_5 & 0x00ff),
				(lds.sAttr.sUid.UID_5 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_6 & 0x00ff),
				(lds.sAttr.sUid.UID_6 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_7 & 0x00ff),
				(lds.sAttr.sUid.UID_7 & 0xff00) >> 8,
				(lds.sAttr.sUid.UID_8 & 0x00ff),
				(lds.sAttr.sUid.UID_8 & 0xff00) >> 8);

			m_strDevID = chTemp;


			LOG_INFO("Get ID ok!\n");
			//sendGetIDInfoSignal(true);
			m_bHadID = true;
			
			
			UINT16 u16Temp = ~lds.sAttr.u16AngleOffset + 1;//原码
			float fZeroAngle = u16Temp *0.01;
	
			memset(chTemp, 0, 128);
			sprintf(chTemp, "ZeroAngle=%0.2f\n",fZeroAngle);
			LOG_INFO(chTemp);


			/*memset(chTemp, 0, 128);
			sprintf(chTemp, "00.00.%02X.%02X",
				(lds.sAttr.u16Version & 0xFF00) >> 8, lds.sAttr.u16Version & 0x00FF);

			m_strFirmwareVer = chTemp;*/

			sendGetFactoryInfoSignal(true);

		}

		HCHead::eraseBuff(lstBuff, iLen);
		return bOK;

	}

    
    if(m_bX2ID)//X2 series
    {
        int iLen = sizeof(tsIDX2);
        if(lstBuff.size() < iLen)
            return false;


        tsIDX2  sID;
        memcpy(&sID, lstBuff.data(), iLen);
        //memcpy(&m_unDevID.u32ID, sID.u8ID, ID_LEN);

        if(calIDX2((char*)lstBuff.data(),iLen))
        {

			sprintf(chTemp, "%02X%02X%02X%02X%02X%02X%02X",
				sID.u8Ver[0], sID.u8Ver[1], sID.u8Ver[2],
				sID.u8ID[0], sID.u8ID[1], sID.u8ID[2], sID.u8ID[3]);

            m_strDevID = chTemp;


            /*memset(chTemp,0,128);
            sprintf(chTemp, "00.%02X.%02X.%02X",
                                sID.u8Ver[0], sID.u8Ver[1], sID.u8Ver[2]);

            m_strFirmwareVer = chTemp;
*/

			LOG_INFO("Get ID ok!\n");
            //sendGetIDInfoSignal(true);
			m_bHadID = true;

        }
        else
        {
			if (m_strLidarModel == X2N)
				m_strDevID = std::string(DEFAULT_ID);
			else
				m_strDevID = "";


			LOG_ERROR("ID cal error!\n" );


        }
        //lstBuff.erase(lstBuff.begin(), lstBuff.begin() + iLen);
        HCHead::eraseBuff(lstBuff,iLen);

        return true;
    }
    else//X1
    {
        int iLen = sizeof(tsIDX1);
        if(lstBuff.size() < iLen)
            return false;


        tsIDX1  sID;
        memcpy(&sID, lstBuff.data(), iLen);
        //memcpy(&m_unDevID.u32ID, sID.u8ID, ID_LEN);

        if(calIDX1((char*)lstBuff.data(),iLen))
        {

			sprintf(chTemp, "%02X%02X%02X%02X",
				sID.u8ID[3], sID.u8ID[2], sID.u8ID[1], sID.u8ID[0]);


            m_strDevID = chTemp;
            //m_bHadID = true;
			LOG_INFO("Get ID ok!\n" );
            //sendGetIDInfoSignal(true);
			m_bHadID = true;

        }
        else
        {
            m_strDevID = std::string(DEFAULT_ID);

			LOG_ERROR("ID cal error!\n");


        }
        //lstBuff.erase(lstBuff.begin(), lstBuff.begin() + iLen);
        HCHead::eraseBuff(lstBuff,iLen);

        return true;
    }

}

bool HCLidar::calStartInfo(char* ch,int iLen)
{
    UINT sum = 0;

    for (UINT i = 0; i < iLen; i++)
    {
        if(i==4 || i==5)
        {

        }
        else
            sum += (UCHAR)ch[i];
    }

    const UINT sum_check = ((UCHAR)ch[5] << 8) | (UCHAR)ch[4];
    return (sum_check == sum);
}

bool HCLidar::getNewSNInfo(std::vector<UCHAR>& lstBuff)
{

	LOG_INFO("getNewSNInfo!\n");

	if (lstBuff.size() < sizeof(tsSDKSN))
		return false;

	tsSDKSN sNewInfo;
	memcpy(&sNewInfo, lstBuff.data(), sizeof(tsSDKSN));

	if (sNewInfo.u16Len == 112)//D2M7
	{
		if (lstBuff.size() < sizeof(tsSDKIDD2M7))
			return false;

		tsSDKIDD2M7 sD2M7;
		memcpy(&sD2M7, lstBuff.data(), sizeof(tsSDKIDD2M7));
		if (calStartInfo((char*)lstBuff.data(), sizeof(tsSDKIDD2M7)))
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKIDD2M7));

			char chTemp[128] = { 0 };
			sprintf(chTemp, "%c%c",
				sD2M7.sSN.u8FacInfo[0], sD2M7.sSN.u8FacInfo[1]);
			m_strFactoryInfo = chTemp;

			memset(chTemp, 0, 128);
			if (m_bSetAngOffset) {
				if (m_sAttr.bAngOffset && sD2M7.sSN.u16Ang != 0xffff) {
					m_dAngOffset = (short)sD2M7.sSN.u16Ang * 0.01;
					m_dAngOffset = fabs(m_dAngOffset) > 3.0 ? 0 : m_dAngOffset;
					LOG_INFO("ZeroAngle=%0.2f\n", m_dAngOffset);
				}
			}
			if (sD2M7.sSN.u8Reserve1[0] == 0xFF)
				sprintf(chTemp, "%c%d%c", sD2M7.sSN.u8FacInfo[2], sD2M7.sSN.u8FacInfo[3], sD2M7.sSN.u8FacInfo[4]);
			else
				sprintf(chTemp, "%c%d%c%c", sD2M7.sSN.u8FacInfo[2], sD2M7.sSN.u8FacInfo[3], sD2M7.sSN.u8FacInfo[4], sD2M7.sSN.u8Reserve1[0]);

			std::string strModel = chTemp;
			if (strModel != m_strLidarModel)
			{
				LOG_WARNING("Lidar model error init:%s,device:%s\n", (char*)m_strLidarModel.c_str(), (char*)strModel.c_str());
				//setReadCharsError(ERR_DEV_MODEL);
				setLidarPara(chTemp);
			}

			memset(chTemp, 0, 128);

			sprintf(chTemp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sD2M7.sSN.u8SN[0],
				sD2M7.sSN.u8SN[1],
				sD2M7.sSN.u8SN[2],
				sD2M7.sSN.u8SN[3],
				sD2M7.sSN.u8SN[4],
				sD2M7.sSN.u8SN[5],
				sD2M7.sSN.u8SN[6],
				sD2M7.sSN.u8SN[7],
				sD2M7.sSN.u8SN[8],
				sD2M7.sSN.u8SN[9],
				sD2M7.sSN.u8SN[10],
				sD2M7.sSN.u8SN[11],
				sD2M7.sSN.u8SN[12],
				sD2M7.sSN.u8SN[13],
				sD2M7.sSN.u8SN[14],
				sD2M7.sSN.u8SN[15],
				sD2M7.sSN.u8SN[16],
				sD2M7.sSN.u8SN[17],
				sD2M7.sSN.u8SN[18],
				sD2M7.sSN.u8SN[19]);


			LOG_INFO("Get SN:%s\n", chTemp);


			//memset(chTemp, 0, 128);
			//sprintf(chTemp, "00.00.%02X.%02X",
			//	sD2M7.sSN.u8CalVer[0], sD2M7.sSN.u8CalVer[1]);

			//m_strFirmwareVer = chTemp;


			m_bHadID = true;

			sendGetFactoryInfoSignal(true);


			memcpy(&m_sPackUID, &sD2M7.sSN, sizeof(tsSDKSN));

			memcpy(&m_sD2M7, &sD2M7, sizeof(tsSDKIDD2M7));


			//LOG_INFO("New lidar factory info:%s,Hardware ver:%s\n", (char*)m_strFactoryInfo.c_str(), (char*)m_strHardwareVer.c_str());
			return true;
		}
		else
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKIDD2M7));

			LOG_ERROR("New lidar factory info cal error\n");
			setReadCharsError(ERR_START_INFO);
			return true;

		}
	}
	else if (sNewInfo.u16Len == 70)
	{
		if (lstBuff.size() < sizeof(tsSDKIDD3YD))
			return false;
		tsSDKIDD3YD sD3YD;
		memcpy(&sD3YD, lstBuff.data(), sizeof(tsSDKIDD3YD));
		if (calStartInfo((char*)lstBuff.data(), sizeof(tsSDKIDD3YD)))
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKIDD3YD));

			char chTemp[128] = { 0 };
			sprintf(chTemp, "%c%c",
				sNewInfo.u8FacInfo[0], sNewInfo.u8FacInfo[1]);
			m_strFactoryInfo = chTemp;

			memset(chTemp, 0, 128);
			if (m_bSetAngOffset) {
				if (m_sAttr.bAngOffset && sNewInfo.u16Ang != 0xffff) {
					m_dAngOffset = (short)sNewInfo.u16Ang * 0.01;
					m_dAngOffset = fabs(m_dAngOffset) > 3.0 ? 0 : m_dAngOffset;
					LOG_INFO("ZeroAngle=%0.2f\n", m_dAngOffset);
				}
			}
			if (sNewInfo.u8Reserve1[0] == 0xFF)
				sprintf(chTemp, "%c%d%c", sNewInfo.u8FacInfo[2], sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4]);
			else
				sprintf(chTemp, "%c%d%c%c", sNewInfo.u8FacInfo[2], sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4], sNewInfo.u8Reserve1[0]);

			std::string strModel = chTemp;
			if (strModel != m_strLidarModel)
			{
				LOG_WARNING("Lidar model error init:%s,device:%s\n", (char*)m_strLidarModel.c_str(), (char*)strModel.c_str());
				//setReadCharsError(ERR_DEV_MODEL);
				setLidarPara(chTemp);
			}

			memset(chTemp, 0, 128);

			sprintf(chTemp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sNewInfo.u8SN[0],
				sNewInfo.u8SN[1],
				sNewInfo.u8SN[2],
				sNewInfo.u8SN[3],
				sNewInfo.u8SN[4],
				sNewInfo.u8SN[5],
				sNewInfo.u8SN[6],
				sNewInfo.u8SN[7],
				sNewInfo.u8SN[8],
				sNewInfo.u8SN[9],
				sNewInfo.u8SN[10],
				sNewInfo.u8SN[11],
				sNewInfo.u8SN[12],
				sNewInfo.u8SN[13],
				sNewInfo.u8SN[14],
				sNewInfo.u8SN[15],
				sNewInfo.u8SN[16],
				sNewInfo.u8SN[17],
				sNewInfo.u8SN[18],
				sNewInfo.u8SN[19]);


			LOG_INFO("Get SN:%s\n", chTemp);


			//memset(chTemp, 0, 128);
			//sprintf(chTemp, "00.00.%02X.%02X",
			//	sNewInfo.u8CalVer[0], sNewInfo.u8CalVer[1]);
			//m_strFirmwareVer = chTemp;


			m_bHadID = true;
			//sendGetIDInfoSignal(true);

			sendGetFactoryInfoSignal(true);


			memcpy(&m_sPackUID, &sNewInfo, sizeof(tsSDKSN));

			////YJ SN
			memset(chTemp, 0, 128);

			sprintf(chTemp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sD3YD.u8YJSN[0],
				sD3YD.u8YJSN[1],
				sD3YD.u8YJSN[2],
				sD3YD.u8YJSN[3],
				sD3YD.u8YJSN[4],
				sD3YD.u8YJSN[5],
				sD3YD.u8YJSN[6],
				sD3YD.u8YJSN[7],
				sD3YD.u8YJSN[8],
				sD3YD.u8YJSN[9],
				sD3YD.u8YJSN[10],
				sD3YD.u8YJSN[11],
				sD3YD.u8YJSN[12],
				sD3YD.u8YJSN[13],
				sD3YD.u8YJSN[14],
				sD3YD.u8YJSN[15]);
			LOG_INFO("Get SN:%s\n", chTemp);

			//LOG_INFO("New lidar factory info:%s,Hardware ver:%s\n", (char*)m_strFactoryInfo.c_str(), (char*)m_strHardwareVer.c_str());
			return true;
		}
		else
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKSN));

			LOG_ERROR("New lidar factory info cal error\n");
			setReadCharsError(ERR_START_INFO);
			return true;

		}
	}
	else
	{
		if (calStartInfo((char*)lstBuff.data(), sizeof(tsSDKSN)))
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKSN));

			char chTemp[128] = { 0 };
			sprintf(chTemp, "%c%c",
				sNewInfo.u8FacInfo[0], sNewInfo.u8FacInfo[1]);
			m_strFactoryInfo = chTemp;

			memset(chTemp, 0, 128);
			if (m_bSetAngOffset) {
				if (m_sAttr.bAngOffset && sNewInfo.u16Ang != 0xffff) {
					m_dAngOffset = (short)sNewInfo.u16Ang * 0.01;
					m_dAngOffset = fabs(m_dAngOffset) > 3.0 ? 0 : m_dAngOffset;
					LOG_INFO("ZeroAngle=%0.2f\n", m_dAngOffset);
				}
			}
			if (sNewInfo.u8Reserve1[0] == 0xFF)
				sprintf(chTemp, "%c%d%c", sNewInfo.u8FacInfo[2], sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4]);
			else
				sprintf(chTemp, "%c%d%c%c", sNewInfo.u8FacInfo[2], sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4], sNewInfo.u8Reserve1[0]);

			std::string strModel = chTemp;
			if (strModel != m_strLidarModel)
			{
				LOG_WARNING("Lidar model error init:%s,device:%s\n", (char*)m_strLidarModel.c_str(), (char*)strModel.c_str());
				//setReadCharsError(ERR_DEV_MODEL);
				setLidarPara(chTemp);
			}

			memset(chTemp, 0, 128);

			sprintf(chTemp, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				sNewInfo.u8SN[0],
				sNewInfo.u8SN[1],
				sNewInfo.u8SN[2],
				sNewInfo.u8SN[3],
				sNewInfo.u8SN[4],
				sNewInfo.u8SN[5],
				sNewInfo.u8SN[6],
				sNewInfo.u8SN[7],
				sNewInfo.u8SN[8],
				sNewInfo.u8SN[9],
				sNewInfo.u8SN[10],
				sNewInfo.u8SN[11],
				sNewInfo.u8SN[12],
				sNewInfo.u8SN[13],
				sNewInfo.u8SN[14],
				sNewInfo.u8SN[15],
				sNewInfo.u8SN[16],
				sNewInfo.u8SN[17],
				sNewInfo.u8SN[18],
				sNewInfo.u8SN[19]);


			LOG_INFO("Get SN:%s\n", chTemp);


			//memset(chTemp, 0, 128);
			//sprintf(chTemp, "00.00.%02X.%02X",
			//	sNewInfo.u8CalVer[0], sNewInfo.u8CalVer[1]);

			//m_strFirmwareVer = chTemp;


			m_bHadID = true;
			//sendGetIDInfoSignal(true);

			sendGetFactoryInfoSignal(true);


			memcpy(&m_sPackUID, &sNewInfo, sizeof(tsSDKSN));


			//LOG_INFO("New lidar factory info:%s,Hardware ver:%s\n", (char*)m_strFactoryInfo.c_str(), (char*)m_strHardwareVer.c_str());
			return true;
		}
		else
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKSN));

			LOG_ERROR("New lidar factory info cal error\n");
			setReadCharsError(ERR_START_INFO);
			return true;

		}
	}
	
    return false;
}
bool HCLidar::getStartInfo(std::vector<UCHAR>& lstBuff)
{

	LOG_INFO("getStartInfo!\n");

	int iLen = sizeof(tsCmdInfo);
	int iMin = iLen < lstBuff.size() ? iLen : lstBuff.size();
	if (lstBuff.size() < sizeof(tsCmdStart))
		return false;

	tsCmdInfo  sCmd;
	memcpy(&sCmd, lstBuff.data(), iMin);
	if (sCmd.u16Len == 0)
	{

		HCHead::eraseBuff(lstBuff, sizeof(tsCmdStart));

		LOG_ERROR("lidar start message\n");
		setReadCharsError(ERR_START_INFO);
		return true;
	}

	if (sCmd.u16Len == 20)//old
	{
		if (calStartInfo((char*)lstBuff.data(), iLen))
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsCmdStart));

			m_strFactoryInfo = std::string((char*)sCmd.u8FacInfo);


			sendGetFactoryInfoSignal(true);
			LOG_INFO("lidar factory info:%s\n", (char*)sCmd.u8FacInfo);
			return true;
		}
		else
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsCmdStart));

			LOG_ERROR("lidar start message cal error\n");
			setReadCharsError(ERR_START_INFO);
			return true;

		}
	}
	else if (sCmd.u16Len == 25)//NEW
	{
		if (lstBuff.size() < sizeof(tsSDKIDNew))
			return false;

		tsSDKIDNew sNewInfo;
		memcpy(&sNewInfo, lstBuff.data(), sizeof(tsSDKIDNew));

		if (calStartInfo((char*)lstBuff.data(), sizeof(tsSDKIDNew)))
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKIDNew));

			char chTemp[128] = { 0 };
			sprintf(chTemp, "%c%c",
				sNewInfo.u8FacInfo[0], sNewInfo.u8FacInfo[1]);
			m_strFactoryInfo = chTemp;

			memset(chTemp, 0, 128);
			sprintf(chTemp, "%c%d%c", sNewInfo.u8FacInfo[2], sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4]);
			std::string strModel = chTemp;
			if (strModel != m_strLidarModel)
			{
				LOG_WARNING("Lidar model error init:%s,device:%s\n", (char*)m_strLidarModel.c_str(), (char*)strModel.c_str());
				//setReadCharsError(ERR_DEV_MODEL);
				setLidarPara(chTemp);
			}

			memset(chTemp, 0, 128);

			if (m_strLidarModel == X2N)
			{
				sprintf(chTemp, "00000000000000000000%02X%02X%02X%02X%02X%02X%02X",
					sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4], sNewInfo.u8FacInfo[5],
					sNewInfo.u8ID[0], sNewInfo.u8ID[1], sNewInfo.u8ID[2], sNewInfo.u8ID[3]);
			}
			else
			{
				sprintf(chTemp, "%02X%02X%02X%02X%02X%02X%02X",
					sNewInfo.u8FacInfo[3], sNewInfo.u8FacInfo[4], sNewInfo.u8FacInfo[5],
					sNewInfo.u8ID[0], sNewInfo.u8ID[1], sNewInfo.u8ID[2], sNewInfo.u8ID[3]);
			}


			m_strDevID = chTemp;

			LOG_INFO("Get ID ok ID:%s\n", chTemp);


			//memset(chTemp, 0, 128);
			//sprintf(chTemp, "00.00.%02X.%02X",
			//	sNewInfo.u8CalVer[0], sNewInfo.u8CalVer[1]);

			//m_strFirmwareVer = chTemp;


			//memset(chTemp, 0, 128);
			//sprintf(chTemp, "00.%02X.%02X.%02X",
			//	sNewInfo.u8HardVer[0], sNewInfo.u8HardVer[1], sNewInfo.u8HardVer[2]);

			//m_strHardwareVer = chTemp;

			m_bHadID = true;
			//sendGetIDInfoSignal(true);

			sendGetFactoryInfoSignal(true);
			//LOG_INFO("New lidar factory info:%s,Hardware ver:%s\n", (char*)m_strFactoryInfo.c_str(), (char*)m_strHardwareVer.c_str());
			return true;
		}
		else
		{
			HCHead::eraseBuff(lstBuff, sizeof(tsSDKIDNew));

			LOG_ERROR("New lidar factory info cal error\n");
			setReadCharsError(ERR_START_INFO);
			return true;

		}
	}	
	else
	{
		HCHead::eraseBuff(lstBuff, sizeof(tsCmdStart));

		LOG_ERROR("lidar start message\n");
		setReadCharsError(ERR_START_INFO);
		return true;
	}
	return false;
}
bool HCLidar::calMCUFrame(char* ch,int iLen)
{
    UINT iSUM = 0;
    for (UINT i = 0; i < iLen-1; i++)
    {
        iSUM += (UCHAR)ch[i];
    }

    UCHAR u8Sum = (UCHAR)iSUM;
    return (u8Sum == (UCHAR)ch[iLen-1]);
}

bool HCLidar::getMCUCmd(std::vector<UCHAR>& lstBuff)
{

    tsBlockMessage  sBlockMessage;
    memcpy(&sBlockMessage, lstBuff.data(), sizeof(tsBlockMessage));

    int iPackLen = sBlockMessage.u8Len; // u8Num is length
    if(iPackLen <= lstBuff.size())//have a good packet
    {
        if(calMCUFrame((char *)lstBuff.data(),iPackLen))
        {
            if(sBlockMessage.u8MsgID == 0x45) //block message for motor
            {
                if(sBlockMessage.u8Code == 0x01)
                {
					LOG_INFO("MCU message <Motor blocked>\n" );

                    /*if(m_iLastErrorCode != ERR_MOTOR_BLOCKED)
                    {
                        setReadCharsError(ERR_MOTOR_BLOCKED);


                    }*/
                    checkSharkBlocked();
                }
                else
                {

					LOG_INFO("MCU message <Lidar reboot>\n");
                    /*if(m_iLastErrorCode != ERR_REBOOT_LIDAR)
                        setReadCharsError(ERR_REBOOT_LIDAR);*/
                }
            }
        }
        HCHead::eraseBuff(lstBuff,iPackLen);
        return true;
    }
    else
    {
        return false;
    }
}

bool HCLidar::getHealthMsg(std::vector<UCHAR>& lstBuff)
{

	tsHealthMessage  sMessage;
	memcpy(&sMessage, lstBuff.data(), sizeof(tsHealthMessage));

	int iPackLen = sMessage.u8Len; // u8Num is length
	if (iPackLen <= lstBuff.size())//have a good packet
	{
		
		if (sMessage.u8Status > 0)
		{
			//LOG_INFO("New Health msg status exceptional\n");
		}
		HCHead::eraseBuff(lstBuff, iPackLen);
		return true;
	}
	else
	{
		return false;
	}
}

bool HCLidar::getPointCloud(std::vector<UCHAR>& lstBuff)
{

	//printf("Info: getPointCloud!\n");

	int iLen = sizeof(tsPointCloudHead);
	int iMin = iLen < lstBuff.size() ? iLen : lstBuff.size();
	if (lstBuff.size() < iLen)
		return false;

	tsPointCloudHead  sPointCloudHead;
	memcpy(&sPointCloudHead, lstBuff.data(), iMin);

	bool bBlockMessage = false;
	int iPointBytes = 3;
	if (sPointCloudHead.u8Info == 0x02)
	{
		m_sStatistic.iGrayBytes = 0;
		iPointBytes = 2;
	}
	else if (sPointCloudHead.u8Info == 0x07)
	{
		m_sStatistic.iGrayBytes = 2;
		iPointBytes = 4;
	}
	else if (sPointCloudHead.u8Info == 0x80)//MCU message
	{
		bBlockMessage = true;
	}
	else if (sPointCloudHead.u8Info == 0xC2)//MCU message
	{
		return getHealthMsg(lstBuff);
	}
	else
	{
		m_sStatistic.iGrayBytes = 1;
		iPointBytes = 3;
	}

	if (bBlockMessage)
	{
		return getMCUCmd(lstBuff);
	}


	int iPackLen = sPointCloudHead.u8Num * iPointBytes + sizeof(tsPointCloudHead) + sizeof(tsPointCloudTail);

	if (iPackLen <= lstBuff.size())//have a good packet
	{
		/*if (m_sStatistic.u64TSRxPacketFirst == 0)
		{

			m_sStatistic.u64TSRxPacketFirst = HCHead::getCurrentTimestampUs();
			m_u64StartMS = m_sStatistic.u64TSRxPacketFirst;

		}*/
		if (checkDataCal(lstBuff, iPackLen))//good packet
		{

			m_sStatistic.iErrorCountContinue=0;
			m_sStatistic.u64RxPacketCount++;
			m_sStatistic.iNumPerPacket = sPointCloudHead.u8Num;


			if (m_bPollMode)
			{
				std::lock_guard<std::mutex> lock(m_mtxData);
				parserRangeEX(m_resultRange, (char *)lstBuff.data(), iPackLen, sPointCloudHead.u8Num, iPointBytes);
				pollModePointCloud();
			}
			else//call back mode
			{
				parserRangeEX(m_resultRange, (char *)lstBuff.data(), iPackLen, sPointCloudHead.u8Num, iPointBytes);
				callBackFunPointCloud();
				
			}

			//LOG_INFO("lidar point cal ok! Point cloud buff size=%d\n" ,m_resultRange.size() );
		}
		else
		{
			m_sStatistic.iErrorCountContinue++;
			LOG_ERROR("lidar point cloud error!\n");
			m_sStatistic.u64ErrorPacketCount++;
			setReadCharsError(ERR_CHECKDATA_INVALID);

		}
		m_sStatistic.iPacketPerSecond++;

		HCHead::eraseBuff(lstBuff, iPackLen);

		return true;
	}
	return false;

}

bool HCLidar::getPointCloudYJ(std::vector<UCHAR>& lstBuff)
{

	int iLen = sizeof(tsPointCloudHeadYJ);
	int iMin = iLen < lstBuff.size() ? iLen : lstBuff.size();
	if (lstBuff.size() < iLen)
		return false;

	tsPointCloudHeadYJ  sPointCloudHead;
	memcpy(&sPointCloudHead, lstBuff.data(), iMin);

	
    //LOG_INFO("Packet point size error =%d\n", sPointCloudHead.u8Num);

	int iPointBytes = 3;
	

	int iPackLen = sPointCloudHead.u8Num * iPointBytes + sizeof(tsPointCloudHeadYJ) ;

	if (iPackLen <= lstBuff.size())//have a good packet
	{
		if (checkDataCalYJ(lstBuff, iPackLen))//good packet
		{

			m_sStatistic.iErrorCountContinue = 0;
			m_sStatistic.u64RxPacketCount++;
			m_sStatistic.iNumPerPacket = sPointCloudHead.u8Num;
			m_sStatistic.u64FPS += sPointCloudHead.u8Num;

			if (m_bPollMode)
			{
				std::lock_guard<std::mutex> lock(m_mtxData);
				parserRangeEXYJ(m_resultRange, (char *)lstBuff.data(), iPackLen, sPointCloudHead.u8Num, iPointBytes);
				pollModePointCloud();

				
			}
			else//call back mode
			{
				parserRangeEXYJ(m_resultRange, (char *)lstBuff.data(), iPackLen, sPointCloudHead.u8Num, iPointBytes);
				callBackFunPointCloud();

			}

			//LOG_INFO("lidar point cal ok! Point cloud buff size=%d\n" ,m_resultRange.size() );
		}
		else
		{
			m_sStatistic.iErrorCountContinue++;
			LOG_ERROR("lidar point cloud error!\n");
			m_sStatistic.u64ErrorPacketCount++;
			setReadCharsError(ERR_CHECKDATA_INVALID);

		}
		m_sStatistic.iPacketPerSecond++;

		//LOG_ERROR("pack len=%d, points=%d\n", iPackLen, sPointCloudHead.u8Num);
		HCHead::eraseBuff(lstBuff, iPackLen);

		return true;
	}
	return false;

}

bool HCLidar::parserRangeEX(LstPointCloud &resultRange, const char * chBuff, int iIndex, int in_numData, int iPointSize)
{

	UINT64 epochTime = HCHead::getCurrentTimestampNs();

	m_u64TSStart = epochTime;

	UINT64 u64Inter = (m_u64TSStart - m_u64TSEnd)/ (in_numData - 1);
	//LOG_INFO("TS:%lld , %lld\n", u64Inter, m_sAttr.u64TSStepNs);

	const int data_size = iPointSize;//const int data_size = 3;
	int id_start = 6;//int id_start = 2;
	double FA = ((UCHAR)chBuff[id_start + 1] - 0xA0 + (UCHAR)chBuff[id_start] / 256.0) * 4;
	int id_LA_start = id_start + in_numData * data_size + 2;
	double LA = ((UCHAR)chBuff[id_LA_start + 1] - 0xA0 + (UCHAR)chBuff[id_LA_start] / 256.0) * 4;
	if (LA < FA) { LA += 360; }
	double dAngle = (LA - FA) / (in_numData - 1);        // angle info for each sampling
	UCHAR *data = new UCHAR[data_size];//unsigned char data[3];
	int pre_bytes = 8;          // 4 bytes before sampling data in each data package
	unsigned int speed = ((UCHAR)chBuff[5] << 8 | (UCHAR)chBuff[4]) / 64;
	m_lstSpeed.push_back(speed);


	//LOG_INFO("Start=%0.4f,end=%0.4f\n", FA, LA);

	double angle_cur = 0;
	const double angle_offset = m_sAttr.dAngleOffsetD;
	for (int i = 0; i < in_numData; ++i)
	{
		//double Beforecompensation = FA + dAngle * i;
		//double angle_cur = FA + dAngle * i + angle_offset;

		if (m_strLidarModel == "T3B")
			angle_cur = FA + dAngle * i;
		else
			angle_cur = FA + dAngle * i + angle_offset;

		//double Aftercompensation = angle_cur;
		if (angle_cur > 360)
		{
			angle_cur -= 360;
		}


		tsPointCloud sData;
		sData.u64TimeStampNs = epochTime - u64Inter *(in_numData-1-i);
		//LOG_INFO("TS:%lld\n", sData.u64TimeStampNs);

		memcpy(data, (UCHAR*)chBuff + pre_bytes + i * data_size, sizeof(UCHAR) * data_size);
		if (((data[1] >> 7) & 0x01) == 1)
			sData.bValid = false;//(data[1] >> 7) & 0x01;

		
		sData.u16DistRaw = ((data[1] & 0x003F) << 8) | data[0];

		if (m_strLidarModel == "T3B" && !sData.bValid)
		{
			sData.u16DistRaw = 0;
		}

		sData.u16Dist = sData.u16DistRaw;
		if (data_size == 2)
			sData.u16Gray = 0;
		else if (data_size == 3)
			sData.u16Gray = data[2] & 0xff;
		else if (data_size == 4)
		{
			sData.bGrayTwoByte = true;
			sData.u16Gray = ((data[3] & 0x00ff) << 8) | data[2];//data[2] + ((data[3]&0x00ff) << 8);
		}
		else
			sData.u16Gray = 0;
		
		sData.u16Speed = speed;
		sData.dAngleRaw = angle_cur;
		sData.dAngle = sData.dAngleRaw;
		sData.dAngleDisp = sData.dAngle;

		
		// Compensate angle & dist
		if (0 == sData.u16Dist || LA - FA < 1e-6)
		{
			m_sStatistic.iInvalid++;
			sData.bValid = false;        // invalid point

			compensate(sData.dAngle, sData.u16Dist, m_sAttr.dTheta_d, m_sAttr.dBaseline_mm);

		}
		else if (m_bCompensate)
		{
			m_sStatistic.iValid++;
			if (m_strLidarModel == "T3B")
			{
			}
			else
			{
				compensate(sData.dAngle, sData.u16Dist, m_sAttr.dTheta_d, m_sAttr.dBaseline_mm);
				if (m_strLidarModel == D2M7)
				{
					compensateDistSubsection(m_sD2M7.sSub, sData.u16Dist);
				}
				
			}
			sData.dAngleDisp = sData.dAngle;
		}

		if (m_bSetAngOffset) {
			if (m_sAttr.bAngOffset) {
				sData.dAngle += m_dAngOffset;
			}
		}

		if (m_bDistinguish)
			sData.u16Gray = getPointCloudLatticePeak(sData.u16Dist, sData.u16Gray);


		m_sStatistic.dRMS = speed;
		resultRange.push_back(sData);

		//std::cout << "Info: Angle=" << sData.dAngle  << ",AngleRaw=" << sData.dAngleRaw << ",Dist=" << sData.u16Dist << std::endl;
		checkSharkInvalidPoints(sData);
	}
	if (data)
	{
		delete[] data;
		data = NULL;
	}

	checkInvalidLowSpeed(speed);
	checkInvalidHighSpeed(speed);
	checkEncoderError(speed);

	if (!m_sAttr.bBlockSpeed) {
		checkBlockSpeed(speed);
	}


	m_u64TSEnd = HCHead::getCurrentTimestampNs();

	return true;
}


bool HCLidar::parserRangeEXYJ(LstPointCloud &resultRange, const char * chBuff, int iIndex, int in_numData, int iPointSize)
{

	UINT64 epochTime = HCHead::getCurrentTimestampNs();

	m_u64TSStart = epochTime;
	

	const int data_size =  3;

	double FA = ((((UCHAR)chBuff[4] & 0x00ff) + (((UCHAR)chBuff[5] & 0x00ff) << 8)) >>1) / 64.0;

	double LA = ((((UCHAR)chBuff[6] & 0x00ff) + (((UCHAR)chBuff[7] & 0x00ff) << 8)) >> 1) / 64.0;
	if (LA < FA) { LA += 360; }

	double dAngle;
	UINT64 u64Inter ;
	if (in_numData > 1)
	{
		dAngle = (LA - FA) / (in_numData - 1);
		u64Inter = (m_u64TSStart - m_u64TSEnd) / (in_numData - 1);
	}
	else
	{
		dAngle = (LA - FA);
		u64Inter = (m_u64TSStart - m_u64TSEnd);
	}

	UCHAR *data = new UCHAR[data_size];//unsigned char data[3];


	int bStart = (UCHAR)chBuff[2] & 0x01;
	if (bStart > 0)
		m_iPackIndex = 0;
	else
		m_iPackIndex++;

	

	double fStartAng = FA + m_sAttr.dAngleOffsetD;
	double fTemp = fStartAng - m_fLastAng;
	//LOG_INFO("----------last angle is:%0.2f, Diff=%0.2f\n", m_fLastAng, fTemp);

	if (bStart)
	{
		m_iSpeed = ((UCHAR)chBuff[2] >> 1) * 60 / 10;

		//LOG_INFO("----------Start frame last angle:%0.2f, first angle:%0.2f\n", m_fLastAng, fStartAng);

		//if(in_numData !=1)
			//LOG_INFO("----------Start frame points nubmer is:%d\n", in_numData);
	}
			
	unsigned int speed = m_iSpeed;

	//LOG_INFO("speed:%d\n", speed);
	
	if(m_iPackIndex !=1)
		m_lstSpeed.push_back(speed);

	
	   

	double angle_cur = 0;
	const double angle_offset = m_sAttr.dAngleOffsetD;
	for (int i = 0; i < in_numData; ++i)
	{

		angle_cur = FA + dAngle * i + angle_offset;

		if (angle_cur > 360)
		{
			angle_cur -= 360;
		}
		m_fLastAng = angle_cur;

		tsPointCloud sData;
		sData.u64TimeStampNs = epochTime - u64Inter * (in_numData - 1 - i);
		//LOG_INFO("TS:%lld\n", sData.u64TimeStampNs);

		memcpy(data, (UCHAR*)chBuff + 10 + i * data_size, sizeof(UCHAR) * data_size);

		if(data[0] & 0x03)
			m_bNorMode = true; //normal
		else
			m_bNorMode = false;//光强

	

		sData.u16Gray = (data[0] >> 2)  + (data[1] & 0x03)*64;

	
		sData.u16DistRaw = data[2] *64 + (data[1] >> 2);
			   
		sData.u16Dist = sData.u16DistRaw;
	

		sData.u16Speed = speed;
		sData.dAngleRaw = angle_cur;
		sData.dAngle = sData.dAngleRaw;
		sData.dAngleDisp = sData.dAngle;


		// Compensate angle & dist
		if (0 == sData.u16Dist || LA - FA < 1e-6)
		{
			m_sStatistic.iInvalid++;
			sData.bValid = false;        // invalid point
			sData.u16Dist = 0;

			compensate(sData.dAngle, sData.u16Dist, m_sAttr.dTheta_d, m_sAttr.dBaseline_mm);

		}
		else if (m_bCompensate)
		{
			sData.bValid = true;
			m_sStatistic.iValid++;
			compensate(sData.dAngle, sData.u16Dist, m_sAttr.dTheta_d, m_sAttr.dBaseline_mm);

			sData.dAngleDisp = sData.dAngle;
		}

		if (m_bSetAngOffset) {
			if (m_sAttr.bAngOffset) {
				sData.dAngle += m_dAngOffset;
			}
		}

		if (m_bDistinguish)
			sData.u16Gray = getPointCloudLatticePeak(sData.u16Dist, sData.u16Gray);


		m_sStatistic.dRMS = speed;
		resultRange.push_back(sData);

		//std::cout << "Info: Angle=" << sData.dAngle  << ",AngleRaw=" << sData.dAngleRaw << ",Dist=" << sData.u16Dist << std::endl;
		checkSharkInvalidPoints(sData);
	}
	if (data)
	{
		delete[] data;
		data = NULL;
	}

	

	checkInvalidLowSpeed(speed);
	checkInvalidHighSpeed(speed);
	//checkEncoderError(speed);

	if (!m_sAttr.bBlockSpeed) {
		checkBlockSpeed(speed);
	}


	m_u64TSEnd = HCHead::getCurrentTimestampNs();

	return true;
}

void HCLidar::compensate(double &angle, UINT16 &dist, const double theta_d, const double baseline_mm)
{
    if (0 != dist)
    {
        const double theta = theta_d / 180. * PI_HC;
        const double beta = atan(dist / baseline_mm);
        double angle_correct = PI_HC / 2. - (theta + beta);
        angle = angle + angle_correct / PI_HC * 180;
        //angle = angle + 80 - atan(dist / baseline_mm)/ PI_HC * 180;  //X2
		//angle = angle + 118.5 - atan(dist / baseline_mm) / PI_HC * 180; //D2
        if (angle > 360)
        {
            angle -= 360;
        }
        else if (angle < 0)
        {
            angle += 360;
        }

        dist = sqrt(baseline_mm * baseline_mm + dist * dist);
    }
	else
	{
		double dTemp = atan(999999999 / baseline_mm) / PI_HC * 180;
		angle = angle + 80 - dTemp;
		if (angle > 360)
		{
			angle -= 360;
		}
		else if (angle < 0)
		{
			angle += 360;
		}
	}
}

bool HCLidar::checkDataCalTof(std::vector<UCHAR>& lstBuff, int iIndex)
{
	int sum = 0;
	for (int i = 0; i < iIndex - 2; ++i)
	{
		if (i % 2 == 0)
		{
			sum = sum + lstBuff[i];
		}
		else
		{
			sum = sum + (lstBuff[i] << 8);
		}
	}
	unsigned char low = sum;
	unsigned char high = (sum >> 8);
	if (low != lstBuff[iIndex - 2] || high != lstBuff[iIndex - 1])
	{
		//printf("low:%02x %02x, hight:%02x, %02x\n", low, lstBuff[iIndex - 2], high, lstBuff[iIndex - 1]);
		//printf("MSG:");
		//for (int i = 0; i < iIndex; ++i)
		//{
			//printf("%02x  ", lstBuff[i]);
		//}

		//printf("\n");
		return false;
	}
	return true;
}

bool HCLidar::checkDataCal(std::vector<UCHAR>& lstBuff, int iIndex)
{
	const int iB = iIndex - 2;
	std::vector<int> temp(iB / 2, 0);
	//temp[0] = 0x55 + (0xAA << 8);
	for (int i = 0; i < temp.size(); i++)
	{
		temp[i] = lstBuff.at(2 * i) + (lstBuff.at(2 * i + 1) << 8);
	}

	int chk32 = 0;
	for (int i = 0; i < temp.size(); i++)
	{
		chk32 = (chk32 << 1) + temp[i];
	}
	
	UINT16 checksum_target = (chk32 & 0x7FFF) + (chk32 >> 15);
	checksum_target = checksum_target & 0x7FFF;
	UINT16 checksum_cur = lstBuff.at(iIndex - 2) + (lstBuff.at(iIndex - 1) << 8);
	bool is_equal = (checksum_target == checksum_cur);
	return is_equal;
}


bool HCLidar::checkDataCalYJ(std::vector<UCHAR>& lstBuff, int iIndex)
{
	const int iB = iIndex;
	int iNumb = lstBuff.at(3);
	std::vector<UINT16> temp(4+ iNumb*2, 0);

	int iCount = 0;
	for (int i = 0; i < 4; i++)
	{
		temp[iCount] = (lstBuff.at(2 * i) & 0x00ff) + ((lstBuff.at(2 * i + 1) & 0x00ff) << 8);
		iCount++;
	}

	for (int i = 0; i < iNumb; i++)
	{
		temp[iCount] = (lstBuff.at(10 + 3 * i) & 0x00ff);
		iCount++;
		temp[iCount] = (lstBuff.at(10 + 3 * i + 1) & 0x00ff) + ((lstBuff.at(10 + 3 * i + 2) & 0x00ff) << 8);
		iCount++;
	}

	UINT16 chk32 = temp[0];
	for (int i = 1; i < temp.size(); i++)
	{
		chk32 ^=  temp[i];
	}

	UINT16 checksum_cur = lstBuff.at(8) + (lstBuff.at(9) << 8);
	bool is_equal = (chk32 == checksum_cur);
	return is_equal;
}

bool HCLidar::getRxPointClouds(LstPointCloud& lstG)
{
	if (m_strLidarModel == X2N)
	{
		if (!m_bPollMode)
		{
			setReadCharsError(ERR_POLL_MODE);
			return false;
		}
		if (lstG.size() > 0)
		{
			LstPointCloud tmp;
			tmp.swap(lstG);
		}
		int iError = m_iLastErrorCode;

		bool bOK = true;
		switch (iError)
		{
		case ERR_SHARK_MOTOR_BLOCKED:

		case ERR_SHARK_INVALID_POINTS:

		case ERR_LIDAR_SPEED_LOW:

		case ERR_LIDAR_SPEED_HIGH:

		case ERR_DISCONNECTED:
		case ERR_LIDAR_FPS_INVALID:
			bOK = false;

			break;

		default:
			break;
		}

		if (bOK)
		{
			std::lock_guard<std::mutex> lock(m_mtxData);
			if (m_resultRange.size() > 0)
			{
				lstG.swap(m_resultRange);
				std::stable_sort(lstG.begin(), lstG.end(), newComparator);
			}
		}
		else
		{
			std::lock_guard<std::mutex> lock(m_mtxData);
			if (m_resultRange.size() > 0)
			{
				LstPointCloud tmp;
				m_resultRange.swap(tmp);
			}
		}

		return bOK;
	}
	else
	{
		if (!m_bPollMode)
		{
			setReadCharsError(ERR_POLL_MODE);
			return false;
		}
		if (lstG.size() > 0)
		{
			LstPointCloud tmp;
			tmp.swap(lstG);
		}
		std::lock_guard<std::mutex> lock(m_mtxData);
		if (m_bCircle)
		{
			if (m_Circles.size() > 0)
			{
				LstPointCloud tmp = *m_Circles.begin();

				lstG.swap(tmp);
				m_Circles.erase(m_Circles.begin());
			}

		}
		else
		{
			if (m_resultRange.size() > 0)
			{
				lstG.swap(m_resultRange);
			}
		}

		return true;
	}
}


void HCLidar::checkInvalidFPS(int iFPS)
{
	if (m_sStatistic.u64CurrentS < 2)
		return;
    if (iFPS < m_sAttr.iFPSMin || iFPS > m_sAttr.iFPSMax)
    {
        ++m_iInvalidFPSSecond;
		int iTemp = m_sSDKPara.iFPSContinueMS / 1000;
        if (m_iInvalidFPSSecond > iTemp)
        {
			LOG_ERROR("FPS:%d \n" , iFPS );
            setReadCharsError(ERR_LIDAR_FPS_INVALID);

            m_iInvalidFPSSecond = 0;
        }
    }
    else
    {
        m_iInvalidFPSSecond = 0;
    }

	if (m_u64CountS>= SENSOR_ERROR_SECOND && (iFPS == 0 || m_iLastErrorCode == ERR_LIDAR_FPS_INVALID))
	{
		if (!getErrorCode(ERR_MOTOR_BLOCKED, SENSOR_ERROR_TIME_MS))
			setReadCharsError(ERR_LIDAR_SENSOR);
	}
}


void HCLidar::checkInvalidLowSpeed(UINT16 u16Speed)
{
	if (m_sStatistic.u64CurrentS < 2)
		return;
    if (u16Speed < m_sAttr.iSpeedMin)
    {
        if (m_u64StartTimeLowSpeed != 0)
        {
            UINT64 endTime = HCHead::getCurrentTimestampUs();
            //if ((endTime - m_u64StartTimeLowSpeed) >= m_sSDKPara.iSpeedContinueMS*1000)
			if (endTime >= (m_u64StartTimeLowSpeed + m_sSDKPara.iSpeedContinueMS * 1000))
            {
                setReadCharsError(ERR_LIDAR_SPEED_LOW);
                m_u64StartTimeLowSpeed = 0;
            }
        }
        else
        {
            m_u64StartTimeLowSpeed = HCHead::getCurrentTimestampUs();
        }
    }
    else
    {
        m_u64StartTimeLowSpeed = 0;
    }
}

void HCLidar::checkInvalidHighSpeed(UINT16 u16Speed)
{
	if (m_sStatistic.u64CurrentS < 2)
		return;
    if (u16Speed > m_sAttr.iSpeedMax)
    {
        if (m_u64StartTimeHighSpeed != 0)
        {
            int64_t endTime = HCHead::getCurrentTimestampUs();
            //if ((endTime - m_u64StartTimeHighSpeed) >= m_sSDKPara.iSpeedContinueMS*1000)
			if (endTime   >= (m_u64StartTimeHighSpeed + m_sSDKPara.iSpeedContinueMS * 1000))
            {
                setReadCharsError( ERR_LIDAR_SPEED_HIGH);
                m_u64StartTimeHighSpeed = 0;
            }
        }
        else
        {
            m_u64StartTimeHighSpeed = HCHead::getCurrentTimestampUs();
        }
    }
    else
    {
        m_u64StartTimeHighSpeed = 0;
    }
}

void HCLidar::checkEncoderError(UINT16 u16Speed)
{
	if (u16Speed == 0 || u16Speed == 1023)
	{
		//LOG_INFO("Speed error=%d\n", u16Speed);
		if (m_u64StartTimeSpeed != 0)
		{
			UINT64 endTime = HCHead::getCurrentTimestampUs();
			//if ((u16Speed != m_u16Speed) && (endTime - m_u64StartTimeSpeed) <= ENCODER_ERROR_TIME_MS*1000)
			if ((u16Speed != m_u16Speed) && (endTime  <= (m_u64StartTimeSpeed+ENCODER_ERROR_TIME_MS * 1000)))
			{
				if (m_sStatistic.u64CurrentS > ENCODER_ERROR_SECOND)
				{
					if(!getErrorCode(ERR_MOTOR_BLOCKED, ENCODER_ERROR_TIME_MS))
						setReadCharsError(ERR_LIDAR_ENCODER);
				}
					
				m_u64StartTimeSpeed = 0;
			}
		}
		else
		{
			m_u64StartTimeSpeed = HCHead::getCurrentTimestampUs();
		}

		
	}
	else
	{
		m_u64StartTimeSpeed = 0;
	}
	m_u16Speed = u16Speed;
}

void HCLidar::checkLDSVoltage()
{
	if (m_u64CountS >= LDS_VOLTAGE_ERROR_SECOND && m_sStatistic.u64FPS == 0 && !m_bHadID && m_bDisconnect)
	{
		setReadCharsError(ERR_LIDAR_VOLTAGE);
	}
}

void HCLidar::checkPDCurrent()
{
	if(!getErrorCode(ERR_LIDAR_FPS_INVALID, PD_ERROR_TIME_MS) && (!getErrorCode(ERR_LIDAR_SPEED_LOW, PD_ERROR_TIME_MS) || !getErrorCode(ERR_LIDAR_SPEED_HIGH, PD_ERROR_TIME_MS)))
	{
		setReadCharsError(ERR_LIDAR_PD_CURRENT);
	}
		
}

void HCLidar::checkSharkBlocked()
{
    if (m_u64StartTimeSharkBlock != 0)
    {
        UINT64 endTime = HCHead::getCurrentTimestampUs();
        UINT64 u64Temp = endTime - m_u64StartTimeSharkBlock;

        if(u64Temp >= (m_sSDKPara.iBlockContinueMS+2000)*1000) //3.5+2S
        {
            m_u64StartTimeSharkBlock = 0;
            m_iSharkBlockCount=0;
        }
        else if(u64Temp >= m_sSDKPara.iBlockContinueMS*1000 && u64Temp < (m_sSDKPara.iBlockContinueMS+2000)*1000)//3.5 - 4.5s
        {
            if(m_iSharkBlockCount>0)
            {
				//LOG_ERROR("Shark moto block\n");
                setReadCharsError(ERR_SHARK_MOTOR_BLOCKED);
                m_u64StartTimeSharkBlock = 0;
                m_iSharkBlockCount=0;
            }
            else
            {
                m_u64StartTimeSharkBlock = 0;
                m_iSharkBlockCount=0;
            }

        }
        else if(u64Temp < 100*1000)//100ms
        {
            m_u64StartTimeSharkBlock = endTime;
        }
        else if(u64Temp >= MCU_BLOCK_TIME_MS*1000)
        {
            m_iSharkBlockCount++;
        }
        else
        {
        }
    }
    else
    {

        m_u64StartTimeSharkBlock = HCHead::getCurrentTimestampUs();
        m_iSharkBlockCount=0;

    }
}


void HCLidar::checkSharkInvalidPoints(tsPointCloud& sData)
{
    if (!sData.bValid)
    {
		m_iInvalidPointsCount++;
        if (m_u64StartTimeInvalidPoints != 0)
        {
            int64_t endTime = HCHead::getCurrentTimestampUs();
			int iTemp = m_sSDKPara.iCoverContinueMS / 1000;
            //if ((endTime - m_u64StartTimeInvalidPoints) >= m_sSDKPara.iCoverContinueMS*1000 && m_iInvalidPointsCount> m_sAttr.iFPSNor*iTemp)

			if ((endTime  >= (m_u64StartTimeInvalidPoints + m_sSDKPara.iCoverContinueMS * 1000 )) && (m_iInvalidPointsCount > m_sAttr.iFPSNor*iTemp))
            {
                setReadCharsError( ERR_SHARK_INVALID_POINTS);
                m_u64StartTimeInvalidPoints = 0;
            }
        }
        else
        {
            m_u64StartTimeInvalidPoints = HCHead::getCurrentTimestampUs();
        }
    }
    else
    {
        m_u64StartTimeInvalidPoints = 0;
		m_iInvalidPointsCount = 0;
    }
}




void HCLidar::checkInvalidLidarNumber(int validNumber)
{
    if (validNumber < VALID_NUMBER_COUNT)
    {
        ++m_iInvalidNumberContinue;
        if (m_iInvalidNumberContinue >= NUMBER_CONTINUE_CIRCLE)
        {
            setReadCharsError(ERR_LIDAR_NUMBER_INVALID);
            m_iInvalidNumberContinue = 0;

			checkPDCurrent();
        }
    }
    else
    {
        m_iInvalidNumberContinue = 0;
    }
}


void HCLidar::pollModePointCloud()
{
	//LOG_INFO("pollModePointCloud\n");

	
	if (m_bCircle)
	{
		if (getOneCircleData())
		{
			m_Circles.push_back(m_lstCircle);
			LstPointCloud tmp;
			tmp.swap(m_lstCircle);

		}
		if (m_Circles.size() > m_sSDKPara.iCirclesBuffSize)
		{
			m_Circles.erase(m_Circles.begin());
			setReadCharsError(ERR_BUFF_FULL);
		}
	}
	else
	{
		if (m_resultRange.size() > m_sSDKPara.iPollBuffSize)
		{
			HCHead::eraseRangeData(m_resultRange, m_resultRange.size() - m_sSDKPara.iPollBuffSize);
			setReadCharsError(ERR_BUFF_FULL);
		}
	}
}
void HCLidar::callBackFunPointCloud()
{
	if (m_bCircle)
	{
		if (getOneCircleData())
		{
			if (m_funPointCloud)
			{

				m_funPointCloud(m_lstCircle);
			}
			else
			{
				setReadCharsError(ERR_CALLBACK_FUN);
			}
			LstPointCloud tmp;
			tmp.swap(m_lstCircle);
		}
	}
	else
	{
		if (m_resultRange.size() >= m_sSDKPara.iCallbackBuffSize)
		{
			if (m_funPointCloud)
			{
				m_funPointCloud(m_resultRange);
			}
			else
			{
				setReadCharsError(ERR_CALLBACK_FUN);
			}
			LstPointCloud tmp;
			tmp.swap(m_resultRange);
		}
	}

	
}

bool HCLidar::getOneCircleData()
{

	if (m_strLidarModel == D2YE)
	{
		if (m_iPackIndex == 0)
		{
			//LOG_ERROR("Get one circle-----------%d\n", m_lstCircle.size());
			checkInvalidLidarNumber(m_iValidNumber);
			m_iValidNumber = 0;

			return true;

		}
		else
		{
			for (auto sInfo : m_resultRange)
			{
				if (sInfo.bValid)
					++m_iValidNumber;

				m_lstCircle.push_back(sInfo);
			}

			//LOG_ERROR("=====m_lstCircle=%d,m_resultRange=%d\n", m_lstCircle.size(), m_resultRange.size());
			LstPointCloud tmp;
			tmp.swap(m_resultRange);
		}
		return false;
	}

	bool bHadOne = false;
	int iCount = 0;
	for (auto sInfo : m_resultRange)
	{
		m_dAngleCur = sInfo.dAngleRaw;
		if (sInfo.bValid)
			++m_iValidNumber;

		if (m_lstCircle.size() > 0)
		{
			auto sTemp = *m_lstCircle.begin();
			double dTemp = sTemp.dAngleRaw - m_dAngleCur;
			if ((dTemp <= m_sAttr.dAngleStep && dTemp > 0) || (m_lstCircle.size() >= m_sAttr.dCirclePoints))
			{
				bHadOne = true;
				iCount++;
				m_lstCircle.push_back(sInfo);
				m_resultRange.erase(m_resultRange.begin(), m_resultRange.begin() + iCount);
				checkInvalidLidarNumber(m_iValidNumber);
				m_iValidNumber = 0;
				break;
			}

		}

		iCount++;

		m_lstCircle.push_back(sInfo);

		m_dAnglePre = m_dAngleCur;
	}
		

	if (bHadOne)
	{
		//std::stable_sort(m_lstCircle.begin(), m_lstCircle.end(), newComparator);		
	}
	else
	{
		LstPointCloud tmp;
		tmp.swap(m_resultRange);
	}

	return bHadOne;
	   	
}


bool HCLidar::getErrorCode(int iError, int iMs)
{
	std::lock_guard<std::mutex> lock(m_mtxError);
	std::map<int, UINT64>::iterator it = m_mapErrorCode.find(iError);
	if (it != m_mapErrorCode.end())
	{
		UINT64 u64TS = it->second;
		UINT64 u64EndUs = HCHead::getCurrentTimestampUs();
		//UINT64 u64Int = u64EndUs - u64TS;

		//if (u64Int <= iMs*1000)
		if (u64EndUs <= (u64TS + iMs * 1000))
		{
			return true;
		}
	}
	return false;
}

void HCLidar::setLidarPowerOn(bool bPowerOn)
{
	m_bDisconnect = !bPowerOn;
}

///////////////////////////////
bool HCLidar::rockCheckLDSInfo(UINT8* buffer, unLidarInfo& lds_info)
{

	int size = 0;
	int decode_size = 0;

	UINT8 decode_buff[RCV_SIZE];

	rockDecodeLDSInfoPacket(buffer, decode_buff, decode_size);

	if (decode_size < sizeof(unLidarInfo))
	{
		//qDebug() << "Fail to Check LDS info, decoded buff size error.";
		return false;
	}

	memcpy(&lds_info, &decode_buff, sizeof(unLidarInfo));

	if (!rockCheckCRC(lds_info))
	{
		//qDebug() << "Wrong CRC for LDS info.";
		return false;
	}

	//qDebug() << "Get LDS Info success";

	return true;
}

void HCLidar::rockDecodeLDSInfoPacket(UINT8* src, UINT8* dest, int& decode_size)
{
	decode_size = 0;
	for (int i = 0; i < RCV_SIZE; i++)
	{
		if (src[i] == (LDS_INFO_START - 1))
		{
			if (i < RCV_SIZE - 1)
			{
				dest[decode_size++] = (src[i + 1] == 0x01 ? LDS_INFO_START : (LDS_INFO_START - 1));
				i++;
			}
		}
		else
		{
			dest[decode_size++] = src[i];
		}
	}
}

bool HCLidar::rockCheckCRC(unLidarInfo lds_info)
{
	UINT16 u16CheckSum = 0;
	for (int i = 0; i < (sizeof(unLidarInfo) / sizeof(UINT16) - 1); i++)
	{
		u16CheckSum += lds_info.u16DataHex[i];
	}
	if (u16CheckSum != lds_info.sAttr.u16CheckSum)
	{
		//qDebug() << "LDS info checksum: 0x" << lds_info.lds_attributes.check_sum << " VS  Count_result: 0x" << checksum;
		return false;
	}

	return true;
}



void HCLidar::checkChangeSpeed()
{
	if (!m_bCheckSpeed)
	{
		return;
	}

	if (m_sStatistic.dRMS >= m_sAttr.iSpeedMin && m_sStatistic.dRMS <= m_sAttr.iSpeedMax)
	{
		m_iCheckSpeedCount = 0;
	}
	else
	{
		m_iCheckSpeedCount++;
	}

	UINT64 endTime = HCHead::getCurrentTimestampUs();
	//UINT64 u64Temp = endTime - m_u64StartTimeCheckSpeed;

	//if (u64Temp >= (m_sSDKPara.iChangeSpeedMS ) * 1000) //2.5S
	if (endTime >= (m_u64StartTimeCheckSpeed +m_sSDKPara.iChangeSpeedMS * 1000)) //2.5S
	{
		if (m_iCheckSpeedCount>10)
		{
			setReadCharsError(ERR_LIDAR_CHANGE_SPEED);
		}

		
		m_bCheckSpeed = false;
		m_u64StartTimeCheckSpeed = 0;
	}

}



void HCLidar::checkBlockSpeed(UINT16 u16speed)
{
	if (u16speed <= m_sSDKPara.iMinBlockSpeed) {
		if (m_u64checkBlockSpeed != 0) {
			std::chrono::milliseconds curtime(HCHead::getCurrentTimestampMs());
			std::chrono::milliseconds st(m_u64checkBlockSpeed);
			if (curtime - st >= std::chrono::milliseconds(m_sSDKPara.iBlockContinueMS)) {
				setReadCharsError(ERR_SHARK_MOTOR_BLOCKED);
				m_u64checkBlockSpeed = 0;
			}

		}
		else {
			m_u64checkBlockSpeed = HCHead::getCurrentTimestampMs();
		}
	}
	else {
		m_u64checkBlockSpeed = 0;

	}
}

void HCLidar::resetParam()
{
	LOG_INFO("resetParam\n");
	m_iInvalidFPSSecond = 0;
	m_u64StartTimeLowSpeed = 0;
	m_u64StartTimeHighSpeed = 0;
	m_u64StartTimeSharkBlock = 0;
	m_iSharkBlockCount = 0;
	m_u64StartTimeInvalidPoints = 0;
	m_iValidNumber = 0;
	m_u64checkBlockSpeed = 0;
	m_iLastErrorCode = 0;
	m_iInvalidPointsCount = 0;
}


void HCLidar::setLidarLowSpeed(bool bLow)
{
	LOG_INFO("Change speed\n");
	if (bLow)
	{
		if (m_strLidarModel == X2YE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_NARWAL_LOW)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_250_MAX;
			m_sAttr.iSpeedMin = SPEED_250_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_NARWAL_LOW;
			m_sAttr.dCirclePoints = CICRLE_MAX_NARWAL_LOW;

			UCHAR u8Temp[5] = { 0xAA,0x55,0xF1,0x0E };
			m_serial.writeData2(u8Temp, 4);
		}
		else if (m_strLidarModel == X2BF)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_3000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_3000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_3000_3HZ;

		}
		else if (m_strLidarModel == X2MF)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == D2PD)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == D2AE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == A200)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == A201)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == D2AG)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		else if (m_strLidarModel == D2CE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_3HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_180_MAX;
			m_sAttr.iSpeedMin = SPEED_180_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_3HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_3HZ;

		}
		
	}
	else
	{
		if (m_strLidarModel == X2YE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_NARWAL_NOR)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}


			m_sAttr.iSpeedMax = SPEED_315_MAX;
			m_sAttr.iSpeedMin = SPEED_315_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_NARWAL_NOR;
			m_sAttr.dCirclePoints = CICRLE_MAX_NARWAL_NOR;

			UCHAR u8Temp[5] = { 0xAA,0x55,0xF2,0x0D };
			m_serial.writeData2(u8Temp, 4);
		}
		else if (m_strLidarModel == X2BF)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_3000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}


			m_sAttr.iSpeedMax = SPEED_360_MAX;
			m_sAttr.iSpeedMin = SPEED_360_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_3000;
			m_sAttr.dCirclePoints = CICRLE_MAX_3000;
		}
		else if (m_strLidarModel == X2MF)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000_6HZ)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_360_MAX;
			m_sAttr.iSpeedMin = SPEED_360_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000_6HZ;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000_6HZ;

		}
		else if (m_strLidarModel == D2PD)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
		else if (m_strLidarModel == D2AE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
		else if (m_strLidarModel == A200)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
		else if (m_strLidarModel == A201)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
		else if (m_strLidarModel == D2AG)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
		else if (m_strLidarModel == D2CE)
		{
			if (m_sAttr.dAngleStep != ANGLE_RESOLV_2000)
			{
				m_bCheckSpeed = true;
				m_u64StartTimeCheckSpeed = HCHead::getCurrentTimestampUs();
			}

			m_sAttr.iSpeedMax = SPEED_312_MAX;
			m_sAttr.iSpeedMin = SPEED_312_MIN;
			m_sAttr.dAngleStep = ANGLE_RESOLV_2000;
			m_sAttr.dCirclePoints = CICRLE_MAX_2000;

		}
	}
}

UINT16 HCLidar::getPointCloudLatticePeak(int dist_mm, UINT16 peak)
{
	if (peak == 0)
		return peak;

	if (m_sPackUID.u16A1 == 0xFFFF || m_sPackUID.u16B1 == 0xFFFF
		|| m_sPackUID.u16A2 == 0xFFFF || m_sPackUID.u16B2 == 0xFFFF)
	{
		return peak;
	}

	if (m_sPackUID.u16A1 == 0 && m_sPackUID.u16B1 == 0
		&& m_sPackUID.u16A2 == 0 && m_sPackUID.u16B2 == 0)
	{
		return peak;
	}

	if (isCheckMaxPeakLattice(m_sPackUID.u16A1, m_sPackUID.u16B1, m_sPackUID.u16A2,m_sPackUID.u16B2, dist_mm, peak))
	{
		return PC_GRAY_IS_LATTICE;
	}
	else
		return 0;
}

bool HCLidar::isCheckMaxPeakLattice(int a1, int b1, int a2, int b2, int dist_mm, UINT16 peak)
{
	if (a1 == 0 || b1 == 0 || b2 == 0)
	{
		return false;
	}
	double a1_tmp = (double)a1 / 1000.00;
	double a2_tmp = (double)a2 / 1000.00;

	double newpeak = 0;
	if (dist_mm >= 650)
	{
		newpeak = dist_mm * -a2_tmp + b2;
	}
	else if (dist_mm > 250 && dist_mm < 650)   //.300
	{
		newpeak = dist_mm * -a1_tmp + b1;
	}

	if (newpeak > 0 && newpeak < peak)
	{
		return true;
	}
	return false;
}

void HCLidar::compensateDistSubsection(tsSubsection& subsection_para, UINT16 & dist)
{
	if (!m_bComp)
		return;

	if (dist == 0)
	{
		return;
	}

	if (subsection_para.n1_org == 0xFFFFFFFF || subsection_para.n2_org == 0xFFFFFFFF)
	{
		return;
	}

	if (subsection_para.n1_org == 0 && subsection_para.n2_org == 0)
	{
		return;
	}

	double dist_s;
	double dist_temp = dist / 1000.0;

	//分段距离重构
	double cx = subsection_para.n1_org / dist_temp - subsection_para.n2_org;

	if (dist_temp > subsection_para.ulower_0 && dist_temp <= subsection_para.uupper_0)
	{
		dist_s = subsection_para.n1_subsection_0 / (cx + subsection_para.n2_subsection_0);
	}
	else if (dist_temp > subsection_para.ulower_1 && dist_temp <= subsection_para.uupper_1)
	{
		dist_s = subsection_para.n1_subsection_1 / (cx + subsection_para.n2_subsection_1);
	}
	else if (dist_temp > subsection_para.ulower_2 && dist_temp <= subsection_para.uupper_2)
	{
		dist_s = subsection_para.n1_subsection_2 / (cx + subsection_para.n2_subsection_2);
	}
	else if (dist_temp > subsection_para.ulower_3 && dist_temp <= subsection_para.uupper_3)
	{
		dist_s = subsection_para.n1_subsection_3 / (cx + subsection_para.n2_subsection_3);
	}
	else if (dist_temp > subsection_para.ulower_4 && dist_temp <= subsection_para.uupper_4 + 2)
	{
		dist_s = subsection_para.n1_subsection_4 / (cx + subsection_para.n2_subsection_4);
	}

	if (dist_temp <= subsection_para.uupper_4 + 2)
	{
		dist = dist_s * 1000;
	}
}

bool HCLidar::isYJCalGood(UCHAR* pData,int iLen)
{
	if (iLen < 3)
		return false;

	UCHAR u8Temp = pData[0];
	for (int i = 1; i < iLen - 1; i++)
	{
		u8Temp ^= pData[i];
	}

	if (u8Temp == pData[iLen - 1])
		return true;
	else
		return false;
}

bool HCLidar::isYJCalGood(std::vector<UCHAR>& lstBuff, int iLen)
{
	if (iLen < 3)
		return false;

	UCHAR u8Temp = (UCHAR)lstBuff.at(0);
	for (int i = 1; i < iLen - 1; i++)
	{
		u8Temp ^= (UCHAR)lstBuff.at(i);
	}

	if (u8Temp == (UCHAR)lstBuff.at(iLen - 1))
		return true;
	else
		return false;
}

bool HCLidar::parseYJCMD(std::vector<UCHAR>& lstBuff)
{
	if (lstBuff.size() < 6)
		return false;
	

	//if ((UCHAR)lstBuff.at(0) == 0xA5 && (UCHAR)lstBuff.at(1) == 0x5A
		//&& (UCHAR)lstBuff.at(2) == 0x5B )
	if((UCHAR)lstBuff.at(2) == 0x5B)//Get SN
	{
		int iRxLen = 21;
		if (lstBuff.size() >= iRxLen)
		{
			if (isYJCalGood(lstBuff, iRxLen))
			{
				//char chTemp[30] = { 0 };
				//memcpy(chTemp, u8Temp + 4, 16);
				//m_strSN = std::string(chTemp);
				m_strSN = std::string(lstBuff.begin() + 4, lstBuff.begin() + 20);
				m_iMsgID = MSG_YJ_SN;

				std::lock_guard<std::mutex> lk(m_cvMtx);
				m_bSignalReceived = true;
				m_cv.notify_one();

			}
			HCHead::eraseBuff(lstBuff, iRxLen);
			return true;
		}		
		
	}
	else if ((UCHAR)lstBuff.at(2) == 0x50)//start
	{
		int iRxLen = 12;
		if (lstBuff.size() >= iRxLen)
		{
			if (isYJCalGood(lstBuff, iRxLen))
			{
				m_iMsgID = MSG_YJ_START;
				LOG_INFO("Rx MSG_YJ_START\n");

				std::lock_guard<std::mutex> lk(m_cvMtx);
				m_bSignalReceived = true;
				m_cv.notify_one();
				
			}
			HCHead::eraseBuff(lstBuff, iRxLen);
			return true;
		}
	}
	else if ((UCHAR)lstBuff.at(2) == 0x55)//stop
	{
		int iRxLen = 12;
		if (lstBuff.size() >= iRxLen)
		{
			if (isYJCalGood(lstBuff, iRxLen))
			{
				m_iMsgID = MSG_YJ_STOP;
				LOG_INFO("Rx MSG_YJ_STOP\n");

				std::lock_guard<std::mutex> lk(m_cvMtx);
				m_bSignalReceived = true;
				m_cv.notify_one();


			}
			HCHead::eraseBuff(lstBuff, iRxLen);
			return true;
		}
	}
	else if ((UCHAR)lstBuff.at(2) == 0x14)//info
	{
		int iRxLen = 27;
		if (lstBuff.size() >= iRxLen)
		{
			if (isYJCalGood(lstBuff, iRxLen))
			{
				m_iMsgID = MSG_YJ_INFO;
				LOG_INFO("Rx MSG_YJ_INFO\n");

				std::lock_guard<std::mutex> lk(m_cvMtx);
				m_bSignalReceived = true;
				m_cv.notify_one();
			}
			HCHead::eraseBuff(lstBuff, iRxLen);
			return true;
		}
	}
	else if ((UCHAR)lstBuff.at(2) == 0x00)//DATA
	{
		int iRxLen = 7;
		if (lstBuff.size() >= iRxLen)
		{
			if (isYJCalGood(lstBuff, iRxLen))
			{
				m_iMsgID = MSG_YJ_DATA;
				LOG_INFO("Rx MSG_YJ_DATA\n");

				std::lock_guard<std::mutex> lk(m_cvMtx);
				m_bSignalReceived = true;
				m_cv.notify_one();
			}
			HCHead::eraseBuff(lstBuff, iRxLen);
			return true;
		}
	}
	else if ((UCHAR)lstBuff.at(2) == 0x04)//health
	{
		if ((UCHAR)lstBuff.at(3) == 0x00)
		{
			int iRxLen = 11;
			if (lstBuff.size() >= iRxLen)
			{
				m_iMsgID = MSG_YJ_HEALTH;
				UCHAR u8Txx = (UCHAR)lstBuff.at(9);
				LOG_INFO("MSG_YJ_HEALTH %d\n", u8Txx);
				if (u8Txx & 0x01)
				{
					LOG_INFO("MSG_YJ_HEALTH PD error\n");
				}
				if (u8Txx & 0x02)
				{
					LOG_INFO("MSG_YJ_HEALTH LD error\n");
				}
				if (u8Txx & 0x04)
				{
					LOG_INFO("MSG_YJ_HEALTH MCU Vol error\n");
				}
				if (u8Txx & 0x08)
				{
					LOG_INFO("MSG_YJ_HEALTH LED error\n");
				}
				if (u8Txx & 0x10)
				{
					LOG_INFO("MSG_YJ_HEALTH Coder error\n");
				}
				if (u8Txx & 0x20)
				{
					LOG_INFO("MSG_YJ_HEALTH DC error\n");
				}

				//std::lock_guard<std::mutex> lk(m_cvMtx);
				//m_bSignalReceived = true;
				//m_cv.notify_one();

				HCHead::eraseBuff(lstBuff, iRxLen);
				return true;
			}
		}
		
	}
	else
	{
		HCHead::eraseBuff(lstBuff, 3);
		return true;
	}
	return false;
}
std::string HCLidar::getDevYJSN()
{
	std::string strSN = "";

	UCHAR u8Temp[30] = { 0xAA,0x55,0xFB,0x10,0x14 };

	for (int i = 0; i < 5; i++)
	{
		m_serial.writeData2(u8Temp, 5);
			

		std::unique_lock<std::mutex> lk(m_cvMtx);
		m_bSignalReceived = false;
		if (m_cv.wait_for(lk, std::chrono::milliseconds(500)) == std::cv_status::timeout)
		{
			LOG_ERROR("Timeout reached without receiving signal..\n");
		}
		else
		{
			if (m_iMsgID == MSG_YJ_SN)
			{
				return m_strSN;
			}
			LOG_INFO("Signal received within timeout.\n");
		}


		/*
		memset(u8Temp, 0, 30);
		int iRxLen = 21;
		int iRC = m_serial.readData(u8Temp, iRxLen, m_iReadTimeOutms);
		if (iRC == iRxLen)
		{
			if(!isYJCalGood(u8Temp, iRxLen))
				return strSN;
			
			if (u8Temp[0] == 0xA5 && u8Temp[1] == 0x5A
				&& u8Temp[2] == 0x5B && u8Temp[3] == 0x10)
			{
				char chTemp[30] = {0};
				memcpy(chTemp, u8Temp+4,16);
				strSN = std::string(chTemp);
				return strSN;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::this_thread::yield();*/
	}	
	LOG_ERROR("Get Lidar Sn ERROR\n");
	return strSN;
}


bool HCLidar::setYJStart(bool bStart)
{

	UCHAR u8Temp[30] = { 0xAA,0x55,0xF0,0x00,0x00 };

	if (!bStart)
		u8Temp[2] = 0xF5;

	u8Temp[3] = u8Temp[0] ^ u8Temp[1] ^ u8Temp[2];

	
	m_bYJStart = bStart;
	for (int i = 0; i < 5; i++)
	{
		m_serial.writeData2(u8Temp, 4);

		std::unique_lock<std::mutex> lk(m_cvMtx);
		m_bSignalReceived = false;
		if (m_cv.wait_for(lk, std::chrono::milliseconds(500)) == std::cv_status::timeout)
		{
			LOG_ERROR("Timeout reached without receiving signal..\n");
		}
		else
		{
			if (bStart)
			{
				if (m_iMsgID == MSG_YJ_START)
				{
					return true;
				}
			}
			else
			{
				if (m_iMsgID == MSG_YJ_STOP)
				{
					return true;
				}
			}
			
			LOG_INFO("Signal received within timeout.\n");
		}

		/*
		memset(u8Temp, 0, 30);
		int iRxLen = 12;
		int iRC = m_serial.readData(u8Temp, iRxLen, m_iReadTimeOutms);
		if (iRC == iRxLen)
		{
			if (!isYJCalGood(u8Temp, iRxLen))
				return false;

			m_iYJVer = u8Temp[4];
			if (u8Temp[0] == 0xA5 && u8Temp[1] == 0x5A
				&& u8Temp[3] == 0x07)
			{
				if (bStart)
				{
					if (u8Temp[2] == 0x50)
						return true;
					else
						return false;
				}
				else
				{
					if (u8Temp[2] == 0x55)
						return true;
					else
						return false;
				}
					
			}
		}*/

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::this_thread::yield();
	}
	LOG_ERROR("Set Lidar Start ERROR\n");
	return false;
}


bool HCLidar::setYJWorkMode(bool bNor)
{

	UCHAR u8Temp[30] = { 0xAA,0x55,0xF2,0x00,0x00 };

	if (!bNor)
		u8Temp[2] = 0xF1;

	u8Temp[3] = u8Temp[0] ^ u8Temp[1] ^ u8Temp[2];

	m_serial.writeData2(u8Temp, 4);
	return true;
}