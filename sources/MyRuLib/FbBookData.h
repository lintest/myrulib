#ifndef __FBBOOKDATA_H__
#define __FBBOOKDATA_H__

#include <wx/wx.h>
#include "FbThread.h"

class FbBookData
{
	public:
		FbBookData(int id): m_id(id) {}
		FbBookData(const FbBookData & data): m_id(data.m_id) {}
		virtual int GetId() const { return m_id; }
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const;
		virtual void Open() const {}
		void LoadIcon() const;
	private:
		void DoDownload() const;
		void DoOpen(wxInputStream * in, const wxString &md5sum) const;
		wxString GetExt() const;
	private:
		int m_id;
};

#endif // __FBBOOKDATA_H__
