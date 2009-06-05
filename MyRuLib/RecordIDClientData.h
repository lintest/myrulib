#ifndef _RECORD_ID_CLIENT_DATA_H
#define _RECORD_ID_CLIENT_DATA_H

#include <wx/clntdata.h>

class RecordIDClientData : public wxClientData
{
	int m_ID;
public:
	RecordIDClientData(int id) : m_ID(id) {}

	int GetID() {return m_ID;}
	void SetID(int id) {m_ID = id;}
};

#endif
