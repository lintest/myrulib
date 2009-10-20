/////////////////////////////////////////////////////////////////////////////
// Name:        httpenginedef.h
// Purpose:     shared build defines
// Author:      Francesco Montorsi
// Created:     2005/07/26
// RCS-ID:      $Id: httpenginedef.h,v 1.1 2005/08/11 04:05:14 amandato Exp $
// Copyright:   (c) 2005 Francesco Montorsi and Angelo Mandato
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTTPENGINE_DEFS_H_
#define _WX_HTTPENGINE_DEFS_H_

// Defines for shared builds.
// Simple reference for using these macros and for writin components
// which support shared builds:
//
// 1) use the WXDLLIMPEXP_HTTPENGINE in each class declaration:
//          class WXDLLIMPEXP_HTTPENGINE HTTPENGINEClass {   [...]   };
//
// 2) use the WXDLLIMPEXP_HTTPENGINE in the declaration of each global function:
//          WXDLLIMPEXP_HTTPENGINE int myGlobalFunc();
//
// 3) use the WXDLLIMPEXP_DATA_HTTPENGINE() in the declaration of each global
//    variable:
//          WXDLLIMPEXP_DATA_HTTPENGINE(int) myGlobalIntVar;
//
#ifdef WXMAKINGDLL_HTTPENGINE
    #define WXDLLIMPEXP_HTTPENGINE                  WXEXPORT
    #define WXDLLIMPEXP_DATA_HTTPENGINE(type)       WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_HTTPENGINE                  WXIMPORT
    #define WXDLLIMPEXP_DATA_HTTPENGINE(type)       WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_HTTPENGINE
    #define WXDLLIMPEXP_DATA_HTTPENGINE(type)	    type
#endif

#endif // _WX_HTTPENGINE_DEFS_H_

