#ifndef __FBBOOKDATA_H__
#define __FBBOOKDATA_H__

#include <wx/wx.h>
#include "FbThread.h"

class FbTempEraser {
	public:
		static void Add(const wxString &filename);
		static bool sm_erase;
	private:
		wxStringList filelist;
		FbTempEraser() {};
		virtual ~FbTempEraser();
};

class FbBookData
{
	public:
		FbBookData(int id): m_id(id) {}
		FbBookData(const FbBookData & data): m_id(data.m_id) {}
		virtual int GetId() const { return m_id; }
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const;
		virtual void Open() const;
		void LoadIcon() const;
	private:
		void DoDownload() const;
		void DoOpen(wxInputStream * in, const wxString &md5sum) const;
		void SaveFile(wxInputStream & in, const wxString &filepath) const;
		bool GetSystemCommand(const wxString &filepath, const wxString &filetype, wxString &command) const;
		wxString GetExt() const;
	private:
		int m_id;
};

#endif // __FBBOOKDATA_H__
