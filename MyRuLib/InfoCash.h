#ifndef __INFOCASH_H__
#define __INFOCASH_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>
#include <wx/html/htmlwin.h>
#include "FbConst.h"
#include "FbBookThread.h"

#define INFO_CASH_SIZE 20

class InfoImage
{
public:
	InfoImage(const wxString &name, const wxImage &image);
public:
	const wxString & GetName() { return m_name; };
	const int GetWidth();
	const int GetHeight();
private:
	wxString m_name;
	int m_width;
	int m_height;
};

WX_DECLARE_OBJARRAY(InfoImage, InfoImageArray);

class InfoNode
{
public:
	InfoNode(): m_id(0) {};
	virtual ~InfoNode();
	void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
	wxString GetHTML(const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype);
private:
	wxString GetComments(const wxString md5sum, bool bEditable);
public:
	int m_id;
	wxString m_isbn;
	wxString m_title;
	wxString m_annotation;
	wxString m_description;
	wxString m_filelist;
	InfoImageArray m_images;
};

WX_DECLARE_OBJARRAY(InfoNode, InfoNodeArray);

class InfoCash
{
public:
	static void UpdateInfo(wxEvtHandler *frame, int id, bool bVertical, bool bEditable = false);
	static wxString GetInfo(int id, const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype);
	static void Empty();
	static wxString GetIcon(const wxString &extension);
	static void LoadIcon(const wxString &extension);
public:
	static void EmptyInfo(int id);
	static void SetISBN(int id, wxString html);
	static void SetTitle(int id, wxString title, wxString body);
	static void SetFilelist(int id, wxString html);
	static void SetAnnotation(int id, wxString html);
	static void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
	static void AddIcon(wxString extension, wxBitmap bitmap);
private:
	static InfoNodeArray sm_cash;
	static InfoNode * GetNode(int id);
	static InfoNode * FindNode(int id);
	static wxCriticalSection sm_locker;
	static wxArrayString sm_icons;
	static wxArrayString sm_noico;
};

class ShowThread: public FbBookThread
{
	public:
		ShowThread(wxEvtHandler *frame, int id, const bool bVertical, const bool bEditable)
			: FbBookThread(frame, id, bVertical, bEditable) {};
		ShowThread(FbBookThread * thread)
			: FbBookThread(thread) {};
	protected:
		virtual void * Entry();
	private:
		wxString m_filetype;
		wxString m_md5sum;
};

#endif // __INFOCASH_H__
