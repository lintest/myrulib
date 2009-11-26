#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

class FbThreadSkiper
{
	public:
		FbThreadSkiper(): m_number(0) {};
		bool Skipped(const int number) { return number != m_number; };
		int NewNumber() { return ++m_number; };
	private:
		wxCriticalSection m_queue;
		int m_number;
};

class FbThread: public wxThread
{
	public:
		void Execute() { if ( Create() == wxTHREAD_NO_ERROR ) Run(); };
};

#endif // __FBTHREAD_H__
