#include <wx/wx.h>

#define CR_WX_SUPPORT 1
#define CR_EMULATE_GETTEXT 1
#define CHM_SUPPORT_ENABLED 0
#define USE_BITMAP_FONTS 0
#define ENABLE_ANTIWORD 0

#ifdef _WIN32
#define CYGWIN 1
#else
#define USE_FREETYPE 1
#endif
