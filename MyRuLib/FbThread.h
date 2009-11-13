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

#endif // __FBTHREAD_H__
