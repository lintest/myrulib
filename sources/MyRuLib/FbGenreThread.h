#ifndef __FBGENRETHREAD_H__
#define __FBGENRETHREAD_H__

#include "FbThread.h"

class FbGenreThread: public FbThread
{
	public:
		FbGenreThread() {}
	protected:
		virtual void * Entry();
};

#endif // __FBGENRETHREAD_H__