#ifndef __FBCLIENTDATA_H__
#define __FBCLIENTDATA_H__

#include <wx/clntdata.h>

class FbClientData : public wxClientData
{
	public:
		FbClientData(const int id) : m_ID(id) {}
		const int GetID() {return m_ID;}
	private:
		int m_ID;
};

#endif // __FBCLIENTDATA_H__
