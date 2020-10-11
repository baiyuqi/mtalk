//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//


#include "stdafx.h"
#include "GPSSink.h"
// **************************************************************************
// Function Name: CGPSSink:SetGPSDeviceInfo
// 
// Purpose: Receive callbacks from CGPSController
//
// Arguments:
//    GPS_DEVICE gps_Device
//
// Return Values:
//    HRESULT: S_OK
//
// Side effects:  
//    None
// 
// Description:  
//    This function is called by CGPSController on the first time CGPSSink 
//    is registered to receive callbacks.  Then, it is called whenever
//    there is a hardware/GPS Intermediate Driver level changes. This function
//    then relies on CDisplayGPSData to show the data to the user.
//    Note that the return value is ignored by CGPSController and that it 
//    is safe to rely on the compiler to perform a bitwise 
//    copy on GPS_DEVICE.
// **************************************************************************
CGPSSink::~CGPSSink()
{
	if(m_pShowMessage!=NULL)
		delete m_pShowMessage;
}
HRESULT CGPSSink::SetGPSDeviceInfo(GPS_DEVICE gps_Device)
{ 
	this->m_GPS_Device  =   gps_Device;
	ShowData();
	return S_OK;
}

// **************************************************************************
// Function Name: CGPSSink:SetGPSPosition
// 
// Purpose: Receive callbacks from CGPSController
//
// Arguments:
//    GPS_POSITION gps_Position
//
// Return Values:
//    HRESULT: S_OK
//
// Side effects:  
//    None
// 
// Description:  
//    This function is called by CGPSController whenever the Intermediate Driver
//    had outsanding location data. it then relies on CDisplayGPSData to show 
//    the data to the user.
//    Note that the return value is ignored by CGPSController and that it 
//    is safe to rely on the compiler to perform a bitwise 
//    copy on GPS_POSITION.
// **************************************************************************
HRESULT CGPSSink::SetGPSPosition(GPS_POSITION gps_Position)
{ 
	this->m_GPS_Position  = gps_Position;
	ShowData();
	return S_OK;
}


HRESULT CGPSSink::ShowData()
{

	WCHAR wszTmp[MAX_PATH] = L"";
	if(m_pShowMessage == NULL)
		return -1;
	if(m_pShowMessage->m_pMessageWindow == NULL)
		return -1;

	CWnd *pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->satelliteId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%d",
			m_GPS_Position.dwSatelliteCount);

		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->lattitudeId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%2.4f",
			m_GPS_Position.dblLatitude);
		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->longitudeId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%2.4f",
			m_GPS_Position.dblLongitude);
		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->speedId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%2.4f M/H",
			m_GPS_Position.flSpeed);
		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->heightId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%2.4f",
			m_GPS_Position.flHeading);
		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->gpsNameId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%s",
			m_GPS_Device.szGPSFriendlyName);
		pWnd->SetWindowText(wszTmp);
	}

	pWnd = m_pShowMessage->m_pMessageWindow->GetDlgItem(m_pShowMessage->timeId);
	if(pWnd!=NULL)
	{
		StringCchPrintfEx(wszTmp,
			sizeof(wszTmp)/sizeof(wszTmp[0]),
			NULL,
			NULL,
			STRSAFE_NULL_ON_FAILURE,
			L"%d-%d-%d %d:%d:%d",
			m_GPS_Position.stUTCTime.wYear,m_GPS_Position.stUTCTime.wMonth,m_GPS_Position.stUTCTime.wDay
			,m_GPS_Position.stUTCTime.wHour,m_GPS_Position.stUTCTime.wMinute,m_GPS_Position.stUTCTime.wSecond);
		pWnd->SetWindowText(wszTmp);
	}

	return 0;
}
void CGPSSink::SetShowMessage(ShowMessage* sm)
{
	this->m_pShowMessage = sm;
}
