#include "FbThread.h"

wxCriticalSection FbMutex::sm_section;

IMPLEMENT_CLASS(FbMutexLocker, wxObject)

IMPLEMENT_CLASS(FbMutexTester, wxObject)

IMPLEMENT_CLASS(FbThreadSkiper, wxObject)

