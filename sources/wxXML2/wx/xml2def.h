/////////////////////////////////////////////////////////////////////////////
// Name:        xml2def.h
// Purpose:     shared build defines
// Author:      Francesco Montorsi
// Created:
// RCS-ID:      $Id: xml2def.h 438 2007-03-01 11:59:00Z frm $
// Copyright:   (c) 2004-2007 Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_WXXML2_DEFS_H_
#define _WX_WXXML2_DEFS_H_

// Defines for component version.
// The following symbols should be updated for each new component release
// since some kind of tests, like those of AM_WXCODE_CHECKFOR_COMPONENT_VERSION()
// for "configure" scripts under unix, use them.
#define wxWXXML2_MAJOR          1
#define wxWXXML2_MINOR          8
#define wxWXXML2_RELEASE        0

// For non-Unix systems (i.e. when building without a configure script),
// users of this component can use the following macro to check if the
// current version is at least major.minor.release
#define wxCHECK_WXXML2_VERSION(major,minor,release) \
    (wxWXXML2_MAJOR > (major) || \
    (wxWXXML2_MAJOR == (major) && wxWXXML2_MINOR > (minor)) || \
    (wxWXXML2_MAJOR == (major) && wxWXXML2_MINOR == (minor) && wxWXXML2_RELEASE >= (release)))

// Defines for shared builds.
// Simple reference for using these macros and for writin components
// which support shared builds:
//
// 1) use the WXDLLIMPEXP_WXXML2 in each class declaration:
//          class WXDLLIMPEXP_WXXML2 WXXML2Class {   [...]   };
//
// 2) use the WXDLLIMPEXP_WXXML2 in the declaration of each global function:
//          WXDLLIMPEXP_WXXML2 int myGlobalFunc();
//
// 3) use the WXDLLIMPEXP_DATA_WXXML2() in the declaration of each global
//    variable:
//          WXDLLIMPEXP_DATA_WXXML2(int) myGlobalIntVar;
//
#ifdef WXMAKINGDLL_WXXML2
    #define WXDLLIMPEXP_WXXML2                  WXEXPORT
    #define WXDLLIMPEXP_DATA_WXXML2(type)       WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_WXXML2                  WXIMPORT
    #define WXDLLIMPEXP_DATA_WXXML2(type)       WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_WXXML2
    #define WXDLLIMPEXP_DATA_WXXML2(type)	    type
#endif

#endif // _WX_WXXML2_DEFS_H_

