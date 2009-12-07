/***************************************************************
 * Name:	  FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:	Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/
#include "FbManager.h"
#include <wx/zipstrm.h>
#include "FbDatabase.h"
#include "ZipReader.h"
#include <wx/mimetype.h>

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
}

void TempFileEraser::Add(const wxString &filename)
{
	static TempFileEraser eraser;
	eraser.filelist.Add(filename);
}

void FbManager::OpenBook(int id, wxString &file_type)
{
	ZipReader reader(id);
	if (!reader.IsOK()) {
		wxLogError(_("Book open error"));
		return;
	}

	wxFileName file_name = wxFileName::CreateTempFileName(wxT("~"));
	wxRemoveFile(file_name.GetFullPath());
	file_name.SetExt(file_type);

	wxString file_path = file_name.GetFullPath();
	TempFileEraser::Add(file_path);
	wxFileOutputStream out(file_path);
	out.Write(reader.GetZip());

	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, file_type);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if ( result.NextRow() ) {
		wxString command = result.GetString(0);
		#if defined(__WIN32__)
		ShellExecute(NULL, NULL, command, file_path, NULL, SW_SHOW);
		#else
		wxExecute(command + wxT(" ") + file_path);
		#endif
		return;
	}

	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(file_type);
	if ( ft ) {
		wxString cmd;
		if (ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(file_path, wxEmptyString))) {
			wxExecute(cmd);
			return;
		}
		delete ft;
	}

	wxString msg = _("Не найдено приложение для просмотра файлов типа ") + file_type;
	wxMessageBox(msg);
}

wxString BookInfo::MakeLower(wxString & data)
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
	return data;
}

wxString BookInfo::MakeUpper(wxString & data)
{
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
	return data;
}

