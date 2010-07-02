#include "FbCollection.h"
#include "FbColumns.h"
#include "MyRuLibApp.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "FbDateTime.h"
#include "FbZipCatalogue.h"

#include <wx/filename.h>
#include <wx/fs_mem.h>

#include "res/ico_pdf.xpm"
#include "res/ico_djvu.xpm"

#ifdef __WXMSW__
#include <wx/mimetype.h>
#endif // __WXMSW__

//-----------------------------------------------------------------------------
//  FbCacheData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCacheData, wxObject)

FbCacheData::FbCacheData(int code, const wxString &name, int count)
	: m_code(code), m_name(name), m_count(count)
{
}

FbCacheData::FbCacheData(wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCacheData::FbCacheData(int code, wxSQLite3ResultSet &result)
	: m_code(code), m_name(result.GetString(0)), m_count(result.GetInt(1))
{
}

wxString FbCacheData::GetValue(size_t col) const
{
	switch (col) {
		case  0: return m_name;
		case  1: return FbCollection::Format(m_count);
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbCasheDataArray);

//-----------------------------------------------------------------------------
//  FbCollection
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCollection, wxObject)

wxCriticalSection FbCollection::sm_section;

FbCollection::FbCollection(const wxString &filename)
	: m_thread(NULL)
{
	m_database.AttachConfig();
	m_database.CreateFunction(wxT("AGGREGATE"), 1, m_aggregate);

	DoResetDir();
}

FbCollection::~FbCollection()
{
	if (m_thread) m_thread->Delete();
}

wxString FbCollection::Format(int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) << wxChar(0xA0) << wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

FbCollection * FbCollection::GetCollection()
{
	return wxGetApp().GetCollection();
}

wxString FbCollection::GetSeqn(int code, size_t col)
{
	if (code == 0 && col == 0) return _("(Misc.)");

	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	wxString sql = wxT("SELECT value, number FROM sequences WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_seqns, sql);
	return data ? data->GetValue(col) : wxString();
}

wxString FbCollection::GetAuth(int code, size_t col)
{
	if (code == 0 && col == 0) return _("(no Author)");

	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	wxString sql = wxT("SELECT full_name, number FROM authors WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_auths, sql);
	return data ? data->GetValue(col) : wxString();
}

void FbCollection::AddSeqn(FbCacheData * data)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddData(collection->m_seqns, data);
}

void FbCollection::AddAuth(FbCacheData * data)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddData(collection->m_auths, data);
}

void FbCollection::AddInfo(FbViewData * info)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddBook(info);
}

FbCacheData * FbCollection::AddData(FbCasheDataArray &items, FbCacheData * data)
{
	size_t count = items.Count();
	items.Insert(data, 0);
	if (count > DATA_CACHE_SIZE) items.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return data;
}

FbCacheBook * FbCollection::AddBook(FbCacheBook * book)
{
	size_t count = m_books.Count();
	m_books.Insert(book, 0);
	if (count > DATA_CACHE_SIZE) m_books.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return book;
}

void FbCollection::AddBook(FbViewData * info)
{
	size_t count = m_infos.Count();
	m_infos.Insert(info, 0);
	if (count > HTML_CACHE_SIZE) m_infos.RemoveAt(HTML_CACHE_SIZE, count - HTML_CACHE_SIZE);
}

void FbCollection::ResetSeqn(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->ResetData(collection->m_seqns, code);
}

void FbCollection::ResetAuth(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->ResetData(collection->m_auths, code);
}

void FbCollection::ResetInfo(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->DoResetInfo(code);
}

void FbCollection::ResetBook(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->DoResetBook(code);
}

void FbCollection::ResetBook(const wxArrayInt &books)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->DoResetBook(books);
}

void FbCollection::ResetDir()
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->DoResetDir();
}

void FbCollection::DoResetDir()
{
	if (m_thread) m_thread->Delete();
	m_thread = NULL;

	wxString dirname = FbParams::GetText(DB_LIBRARY_DIR);
	if (wxFileName::DirExists(dirname)) {
		m_thread = new FbZipCatalogueThread(dirname);
		m_thread->Execute();
	}
}

FbCacheData * FbCollection::GetData(int code, FbCasheDataArray &items, const wxString &sql)
{
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		FbCacheData & data = items[i];
		if (data.GetCode() == code) return &data;
	}

	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return AddData(items, new FbCacheData(code, result));
	else return NULL;
}

void FbCollection::ResetData(FbCasheDataArray &items, int code)
{
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		if (items[i].GetCode() == code) {
			items.RemoveAt(i);
			break;
		}
	}
}

void FbCollection::DoResetInfo(int code)
{
	size_t count = m_infos.Count();
	for (size_t i = 0; i < count; i++) {
		if (m_infos[i].GetCode() == code) {
			m_infos.RemoveAt(i);
			break;
		}
	}
}

void FbCollection::DoResetBook(int code)
{
	size_t count = m_books.Count();
	for (size_t i = 0; i < count; i++) {
		if (m_books[i].GetCode() == code) {
			m_books.RemoveAt(i);
			break;
		}
	}
}

void FbCollection::DoResetBook(const wxArrayInt &books)
{
	size_t count = m_books.Count();
	for (size_t i = 0; i < count; i++) {
		size_t index = count - i - 1;
		int code = m_books[index].GetCode();
		if (books.Index(code) != wxNOT_FOUND) m_books.RemoveAt(index);
	}
}

wxString FbCollection::GetBook(int code, size_t col)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	return collection->GetCacheBook(code).GetValue(col);
}

void FbCollection::EmptyInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->DoEmptyInfo();
}

void FbCollection::DoEmptyInfo()
{
	m_infos.Empty();
}


FbCacheBook FbCollection::GetBookData(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return 0;
	return collection->GetCacheBook(code);
}

wxString FbCollection::GetBookHTML(const FbViewContext &ctx, const FbCacheBook &book, int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	FbViewData * info = collection->GetCacheInfo(code);
	return info ? info->GetHTML(ctx, book) : wxString();
}

FbCacheBook FbCollection::GetCacheBook(int code)
{
	size_t count = m_books.Count();
	for (size_t i = 0; i < count; i++) {
		FbCacheBook & book = m_books[i];
		if (book.GetCode() == code) return book;
	}

	wxString sql = FbCacheBook::GetSQL();
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return * AddBook(new FbCacheBook(code, result));
	else return 0;
}

FbViewData * FbCollection::GetCacheInfo(int code)
{
	size_t count = m_infos.Count();
	for (size_t i = 0; i < count; i++) {
		FbViewData & info = m_infos[i];
		if (info.GetCode() == code) return &info;
	}
	return NULL;
}

wxArrayString FbCollection::sm_icons;

wxArrayString FbCollection::sm_noico;

void FbCollection::LoadIcon(const wxString &extension)
{
	wxCriticalSectionLocker locker(sm_section);

	if (!sm_icons.Count()) {
		AddIcon((wxString)wxT("djvu"), wxBitmap(ico_djvu_xpm));
		AddIcon((wxString)wxT("pdf"), wxBitmap(ico_pdf_xpm));
	}

	if (extension.IsEmpty() || extension == wxT("fb2")) return;
	wxString filename = wxT("icon.") + extension;

	if (sm_icons.Index(extension) != wxNOT_FOUND) return;
	if (sm_noico.Index(extension) != wxNOT_FOUND) return;

	#ifdef __WXMSW__
	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(extension);
	if ( ft ) {
		wxIconLocation location;
		if ( ft->GetIcon(&location) ) {
			wxLogNull log;
			wxIcon icon(location);
			wxBitmap bitmap;
			bitmap.CopyFromIcon(icon);
			wxMemoryFSHandler::AddFile(filename, bitmap, wxBITMAP_TYPE_PNG);
			sm_icons.Add(extension);
			return;
		}
	}
	#endif // __WXMSW__
	sm_noico.Add(extension);
}

wxString FbCollection::GetIcon(const wxString &extension)
{
	wxCriticalSectionLocker locker(sm_section);
	wxString filename = wxT("icon.") + extension;
	if (sm_icons.Index(extension) != wxNOT_FOUND)
		return filename;
	else
		return wxEmptyString;
}

void FbCollection::AddIcon(wxString extension, wxBitmap bitmap)
{
	wxString filename = wxT("icon.") + extension;
	wxMemoryFSHandler::AddFile(filename, bitmap, wxBITMAP_TYPE_PNG);
	sm_icons.Add(extension);
}

wxFileName FbCollection::FindZip(const wxString &dirname, const wxString &filename)
{
	FbCommonDatabase database;

	wxString sql = wxT("SELECT file FROM zip_books WHERE book=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, filename);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	while (result.NextRow())  {
		wxString sql = wxT("SELECT path FROM zip_files WHERE file=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, result.GetInt(0));
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) {
			wxFileName zip_file = result.GetString(0);
			zip_file.SetPath(dirname);
			if (zip_file.FileExists()) return zip_file.GetFullPath();
		}
	}

	return wxEmptyString;
}
