/***************************************************************
 * Name:      FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/
#include "FbManager.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/progdlg.h>
#include "ImpThread.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "db/Sequences.h"
#include "db/Bookseq.h"
#include "ZipReader.h"
#include "FbConst.h"

#if defined(__WIN32__)
#include <shlwapi.h>
#endif

BookTreeItemData::BookTreeItemData(DatabaseResultSet * res):
    m_id( res->GetResultInt(wxT("id"))),
    title( res->GetResultString(wxT("title"))),
    file_size( res->GetResultInt(wxT("file_size"))),
    file_name( res->GetResultString(wxT("file_name"))),
    file_type( res->GetResultString(wxT("file_type"))),
    number(0)
{
}

class SequenceNode {
public:
    SequenceNode(const int id, const wxTreeItemId &item)
            : m_id(id), m_item(item),  m_next(NULL) {};
	virtual ~SequenceNode() {};
public:
    int m_id;
    wxTreeItemId m_item;
    SequenceNode *m_next;
};

class SequenceList {
public:
	SequenceList()
		: m_root(NULL), m_last(NULL) {};
    virtual ~SequenceList();
	void Append(const int id, const wxTreeItemId &item);
	wxTreeItemId Find(const int id, wxTreeItemId root);
public:
    SequenceNode *m_root;
    SequenceNode *m_last;
};

SequenceList::~SequenceList()
{
	while (m_root) {
		SequenceNode * node = m_root;
		m_root = node->m_next;
		delete node;
	}
}

void SequenceList::Append(const int id, const wxTreeItemId &item)
{
	SequenceNode * node = new SequenceNode(id, item);
	if (m_root)
		m_last->m_next = node;
	else
		m_root = node;
	m_last = node;
}

wxTreeItemId SequenceList::Find(const int id, wxTreeItemId root)
{
	SequenceNode * node = m_root;
	while (node) {
		if (node->m_id == id)
			return node->m_item;
		node = node->m_next;
	}
	return root;
}

class TempFileEraser {
private:
    wxStringList filelist;
    TempFileEraser() {};
    virtual ~TempFileEraser();
public:
    static void Add(const wxString &filename);
};

TempFileEraser::~TempFileEraser()
{
    for (size_t i=0; i<filelist.GetCount(); i++)
        wxRemoveFile(filelist[i]);
};

void TempFileEraser::Add(const wxString &filename)
{
    static TempFileEraser eraser;
    eraser.filelist.Add(filename);
};

void FbManager::OpenBook(int id, wxString &file_type)
{
    ZipReader reader(id);
    if (!reader.IsOK()) {
		wxLogError(_("Book open error"));
        return;
    }

    wxString command = GetOpenCommand(file_type);
    if (command.IsEmpty()) {
    	wxString msg = wxString::Format(_("Не найдено приложение для просмотра файлов типа: %s"), file_type.c_str());
    	wxMessageBox(msg);
    	return;
    }

    wxFileName file_name = wxFileName::CreateTempFileName(wxT("~"));
    wxRemoveFile(file_name.GetFullPath());
    file_name.SetExt(file_type);
    wxString file_path = file_name.GetFullPath();
    TempFileEraser::Add(file_path);
    wxFileOutputStream out(file_path);
    out.Write(reader.GetZip());

#if defined(__WIN32__)
    ShellExecute(NULL, NULL, command, file_path, NULL, SW_SHOW);
#else
    wxExecute(command + wxT(" ") + file_path);
#endif
}

void BookInfo::MakeLower(wxString & data)
{
#if defined(__WIN32__)
	int len = data.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, data.c_str());
	CharLower(buf);
	data = buf;
	delete [] buf;
#else
    data.MakeLower();
#endif
}

void BookInfo::MakeUpper(wxString & data){
#if defined(__WIN32__)
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharUpper(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeUpper();
#endif
}

int BookInfo::NewId(int param)
{
	Params params(wxGetApp().GetDatabase());
	ParamsRow * row = params.Id(param);
	row->value++;
	row->Save();

	return row->value;
}

wxString FbManager::GetSystemCommand(const wxString & file_type)
{
#if defined(__WIN32__)
	wxString ext = wxT(".") + file_type;
	DWORD dwSize = MAX_PATH;
	wxString command;
	if (AssocQueryString(0, ASSOCSTR_EXECUTABLE, ext.c_str(), wxT("open"), wxStringBuffer(command, MAX_PATH), &dwSize) == S_OK) {
		if (command.Left(1) == wxT("\"")) {
			command = command.Mid(1);
			command = command.Left(command.Find(wxT("\"")));
		} else {
			command = command.Left(command.Find(wxT(" ")));
		}
		return command;
	}
#endif
	return wxEmptyString;
}

wxString FbManager::GetOpenCommand(const wxString & file_type)
{
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();
	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	PreparedStatement* pStatement = database->PrepareStatement(sql);
	pStatement->SetParamString(1, file_type);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

    wxString command;
	if (result && result->Next())
		command = result->GetResultString(1);
    else
        command = GetSystemCommand(file_type);

	database->CloseResultSet(result);
	database->CloseStatement(pStatement);

	return command;
}
