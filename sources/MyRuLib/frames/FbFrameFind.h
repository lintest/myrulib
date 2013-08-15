#ifndef __FBFRAMEFIND_H__
#define __FBFRAMEFIND_H__

#include "FbFrameBase.h"

class FbFrameFind : public FbFrameBase
{
	public:
		FbFrameFind(wxAuiNotebook * parent, wxWindowID winid, const FbMasterInfo &info, const wxString &title);
		virtual FbMasterInfo GetInfo() { return m_info; }
		virtual void CreateColumns() {}
	private:
		void CreateBookInfo();
		void FillBooks(FbSQLite3ResultSet & result, const wxString &caption);
	private:
		FbMasterInfo m_info;
		wxString m_title;
	private:
		void OnFoundNothing(wxCommandEvent& event);
		void OnInitSearch(wxCommandEvent& event);
		DECLARE_CLASS(FbFrameFind)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEFIND_H__
