#ifndef __FBSERVICETHREAD_H__
#define __FBSERVICETHREAD_H__

#include "FbThread.h"

enum FbServiceEnum {
	fbSV_EMPTY_NULL,
	fbSV_FULL_TEXT,
	fbSV_LAST,
};

class FbServiceThread : public FbThread
{
	public:
		FbServiceThread();
		virtual ~FbServiceThread();
	protected:
	private:
};

#endif // __FBSERVICETHREAD_H__
