#ifndef __FBFRAMEFIND_H__
#define __FBFRAMEFIND_H__

#include "FbFrameBase.h"

class FbFrameFind : public FbFrameBase
{
	public:
		FbFrameFind(wxAuiMDIParentFrame * parent, const FbMasterInfo &info, const wxString &title);
		virtual FbMasterInfo GetInfo() { return m_info; }
	protected:
		virtual void CreateControls();
		virtual void CreateColumns() {}
	private:
		void CreateBookInfo();
		void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
	private:
		FbMasterInfo m_info;
	private:
		void OnFoundNothing(wxCommandEvent& event);
		void OnInitSearch(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameFind)
};

#endif // __FBFRAMEFIND_H__
