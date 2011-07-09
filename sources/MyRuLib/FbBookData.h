#ifndef __FBBOOKDATA_H__
#define __FBBOOKDATA_H__

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/wxsqlite3.h>
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


class FbItemData
{
	public:
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const = 0;
		virtual void Open() const {};
		virtual int GetId() const { return 0; };
		bool operator == (const FbItemData & data) const { return GetId() == data.GetId(); };
};

class FbBookData: public FbItemData
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

class FbAuthorData: public FbItemData
{
	public:
		FbAuthorData(int author): m_author(author) {};
		int GetAuthor() const { return m_author; };
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const;
	private:
		int m_author;

		class ShowThread: public FbThread
		{
			public:
				ShowThread(wxEvtHandler * frame, int author): m_frame(frame), m_author(author) {};
			protected:
				virtual void * Entry();
			private:
				wxEvtHandler * m_frame;
				int m_author;
		};
};

#endif // __FBBOOKDATA_H__
