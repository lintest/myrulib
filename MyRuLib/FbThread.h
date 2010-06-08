#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/thread.h>
#include <wx/object.h>

class FbMutex: public wxMutex
{
	public:
		FbMutex(): m_count(0) {}
		virtual ~FbMutex() {}
	private:
		static void IncRef(FbMutex * mutex) {
			wxCriticalSectionLocker locker(sm_section);
			mutex->m_count++;
		}
		static void DecRef(FbMutex * mutex) {
			wxCriticalSectionLocker locker(sm_section);
			mutex->m_count--;
			if (mutex->m_count == 0) delete mutex;
		}
	private:
		static wxCriticalSection sm_section;
		unsigned int m_count;
	private:
		friend class FbMutexLocker;
		friend class FbMutexTester;
};

class FbMutexLocker: public wxObject
{
	public:
		FbMutexLocker(): m_mutex(new FbMutex) {
			FbMutex::IncRef(m_mutex);
			m_mutex->Lock();
		}
		virtual ~FbMutexLocker() {
			m_mutex->Unlock();
			FbMutex::DecRef(m_mutex);
		}
	private:
		FbMutex * m_mutex;
		friend class FbMutexTester;
		DECLARE_CLASS(FbMutexLocker)
};

class FbMutexTester
{
	public:
		FbMutexTester(const FbMutexLocker &locker): m_mutex(locker.m_mutex) {
			FbMutex::IncRef(m_mutex);
		}
		virtual ~FbMutexTester() {
			FbMutex::DecRef(m_mutex);
		}
		bool Closed() const {
			return m_mutex->TryLock() != wxMUTEX_BUSY;
		}
	private:
		FbMutex * m_mutex;
		DECLARE_CLASS(FbMutexTester)
};

class FbThreadSkiper
{
	public:
		FbThreadSkiper(): m_number(0) {};
		bool Skipped(const int number) { return number != m_number; };
		int NewNumber() { return ++m_number; };
	private:
		wxCriticalSection m_queue;
		int m_number;
		DECLARE_CLASS(FbThreadSkiper)
};

class FbThread: public wxThread
{
	public:
	    FbThread(wxThreadKind kind = wxTHREAD_DETACHED)
			: wxThread(kind) {}
		void Execute() 
			{ if ( Create() == wxTHREAD_NO_ERROR ) Run(); }
};

#endif // __FBTHREAD_H__
