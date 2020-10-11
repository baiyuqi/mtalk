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


#pragma once
#include "GPSController.h"

struct ShowMessage
{
    CWnd* m_pMessageWindow;
	int heightId;
	int lattitudeId;
	int longitudeId;
	int speedId;
	int satelliteId;
	int gpsNameId;
	int timeId;
};

// *************************************************************************
// Class CGPSSink 
//
// Inheritance:
//      IGPSSink:
//
// Purpose:
//      Provide implement functioned required by CGPSController to perform 
//      the call back
// **************************************************************************
class CGPSSink : public IGPSSink
{
public:
    ~CGPSSink();
public:
    HRESULT SetGPSPosition(__in GPS_POSITION gps_Position);
    HRESULT SetGPSDeviceInfo(__in GPS_DEVICE gps_Device);
private:
	ShowMessage* m_pShowMessage;

	GPS_DEVICE m_GPS_Device;
    GPS_POSITION m_GPS_Position;
public:
	
HRESULT CGPSSink::ShowData();
	//ÊÓÆµ´°¿Ú
	
void SetShowMessage(ShowMessage* sm);
};