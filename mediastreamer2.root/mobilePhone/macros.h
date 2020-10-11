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
// --------------------------------------------------------------------------
//
//Utility macros used for error-checking, etc.
//
// --------------------------------------------------------------------------

#pragma once


#define _ErrorLabel Error


#define CHR(hResult) \
    if(FAILED(hResult)) { hr = (hResult); goto _ErrorLabel;} 


#define CPR(pPointer) \
    if(NULL == (pPointer)) { hr = (E_OUTOFMEMORY); goto _ErrorLabel;} 


#define CBR(fBool) \
    if(!(fBool)) { hr = (E_FAIL); goto _ErrorLabel;} 


#define ARRAYSIZE(s) (sizeof(s) / sizeof(s[0]))


#define RELEASE_OBJ(s)  \
    if ((s) != NULL)      \
    {                   \
        (s)->Release();   \
        (s) = NULL;       \
    }


  

