#include "FbCollection.h"
#include "FbColumns.h"
#include "MyRuLibApp.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "FbDateTime.h"

#include <wx/filename.h>
#include <wx/fs_mem.h>

#include "res/ico_pdf.xpm"
#include "res/ico_djvu.xpm"

#ifdef __WXMSW__
#include <wx/mimetype.h>
#endif // __WXMSW__

//-----------------------------------------------------------------------------
//  FbBookAuths
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookAuths, wxObject)

FbBookAuths::FbBookAuths(int code, wxSQLite3Database &database)
	: m_code(code)
{
	wxString sql = wxT("SELECT AGGREGATE(DISTINCT full_name) FROM authors WHERE id IN(SELECT id_author FROM books WHERE id=?)");

	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	if (result.NextRow()) m_name = result.GetString(0);
}

wxString FbBookAuths::operator[](size_t col) const
{
	switch (col) {
		case BF_AUTH: return m_name;
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbBookAuthsArray);

//-----------------------------------------------------------------------------
//  FbBookSeqns
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookSeqns, wxObject)

FbBookSeqns::FbBookSeqns(int code, wxSQLite3Database &database)
	: m_code(code)
{
	wxString sql = wxT("SELECT AGGREGATE(DISTINCT value), MAX(bookseq.number) FROM bookseq LEFT JOIN sequences ON id_seq=id WHERE id_book=?");

	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	if (result.NextRow()) {
		m_name = result.GetString(0);
		m_numb = result.GetInt(1);
	}
}

wxString FbBookSeqns::operator[](size_t col) const
{
	switch (col) {
		case BF_SEQN: return m_name;
		case BF_NUMB: return m_numb ? wxString::Format(wxT("%d"), m_numb) : wxString();
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbBookSeqnsArray);

//-----------------------------------------------------------------------------
//  FbParamData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbParamData, wxObject)

FbParamData & FbParamData::operator=(wxSQLite3ResultSet & result)
{
	m_int = result.GetInt(1);
	m_str = result.GetString(2);
	return *this;
}

//-----------------------------------------------------------------------------
//  FbCollection
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCollection, wxObject)

wxCriticalSection FbCollection::sm_section;

FbParamHash FbCollection::sm_params;

FbCollection::FbCollection(const wxString &filename)
	: m_thread(NULL)
{
	m_database.Open(filename);
	m_database.AttachConfig();
	m_database.CreateFunction(wxT("AGGREGATE"), 1, m_aggregate);
	m_database.CreateFullText();
	LoadParams();
}

FbCollection::~FbCollection()
{
	if (m_thread) m_thread->Close();
}

bool FbCollection::IsOk() const
{
	return m_database.IsOpen();
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

	if (collection->m_seqns.count(code)) {
		return collection->m_seqns[code];
	} else {
		wxString sql = wxT("SELECT value FROM sequences WHERE id="); sql << code;
		wxSQLite3ResultSet result = collection->m_database.ExecuteQuery(sql);
		wxString name = result.NextRow() ? result.GetString(0) : wxString();
		collection->m_seqns[code] = name;
		return name;
	}
}

wxString FbCollection::GetAuth(int code, size_t col)
{
	if (code == 0 && col == 0) return _("(no Author)");

	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;

	if (collection->m_auths.count(code)) {
		return collection->m_auths[code];
	} else {
		wxString sql = wxT("SELECT full_name, number FROM authors WHERE id="); sql << code;
		wxSQLite3ResultSet result = collection->m_database.ExecuteQuery(sql);
		wxString name = result.NextRow() ? result.GetString(0) : wxString();
		collection->m_auths[code] = name;
		return name;
	}
}

void FbCollection::AddSeqn(int code, const wxString &name)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->m_seqns[code] = name;
}

void FbCollection::AddAuth(int code, const wxString &name)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->m_auths[code] = name;
}

void FbCollection::AddInfo(FbViewData * info)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddBookInfo(info);
}

FbCacheBook FbCollection::AddBook(const FbCacheBook & book)
{
	size_t count = m_books.Count();
	m_books.Insert(book, 0);
	if (count > DATA_CACHE_SIZE) m_books.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return book;
}

void FbCollection::AddBookInfo(FbViewData * info)
{
	size_t count = m_infos.Count();
	m_infos.Insert(info, 0);
	if (count > HTML_CACHE_SIZE) m_infos.RemoveAt(HTML_CACHE_SIZE, count - HTML_CACHE_SIZE);
}

void FbCollection::ResetSeqn(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->m_seqns.erase(code);
}

void FbCollection::ResetAuth(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->m_seqns.erase(code);
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
	switch (col) {
		case BF_AUTH:
			return collection->GetBookAuths(code, col);
		case BF_SEQN:
			return collection->GetBookSeqns(code, col);
		case BF_NUMB:
			return collection->GetBookSeqns(code, col);
		default:
			return collection->GetCacheBook(code).GetValue(col);

	}
}

void FbCollection::EmptyInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->m_infos.Empty();
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
	return AddBook(FbCacheBook::Get(code, m_database));
}

wxString FbCollection::GetBookAuths(int code, size_t col)
{
	size_t count = m_book_auth.Count();
	for (size_t i = 0; i < count; i++) {
		FbBookAuths & auth = m_book_auth[i];
		if (auth.GetCode() == code) return auth[col];
	}

	FbBookAuths * auth = new FbBookAuths(code, m_database);

	m_book_auth.Insert(auth, 0);
	if (count > DATA_CACHE_SIZE) m_book_auth.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return (*auth)[col];
}

wxString FbCollection::GetBookSeqns(int code, size_t col)
{
	size_t count = m_book_seqn.Count();
	for (size_t i = 0; i < count; i++) {
		FbBookSeqns & seqn = m_book_seqn[i];
		if (seqn.GetCode() == code) return seqn[col];
	}

	FbBookSeqns * seqn = new FbBookSeqns(code, m_database);

	m_book_seqn.Insert(seqn, 0);
	if (count > DATA_CACHE_SIZE) m_book_seqn.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return (*seqn)[col];
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
	if (sm_icons.Index(extension) != wxNOT_FOUND) return;
	if (sm_noico.Index(extension) != wxNOT_FOUND) return;

	#ifdef __WXMSW__
	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(extension);
	if ( ft ) {
		wxIconLocation location;
		if ( ft->GetIcon(&location) && location.IsOk() ) {
			wxLogNull log;
			wxIcon icon(location);
			if (icon.IsOk()) {
				wxBitmap bitmap;
				bitmap.CopyFromIcon(icon);
				wxString filename = wxT("icon.") + extension;
				wxMemoryFSHandler::AddFile(filename, bitmap, wxBITMAP_TYPE_PNG);
				sm_icons.Add(extension);
				return;
			}
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

void FbCollection::LoadConfig()
{
	FbConfigDatabase database;
	database.Open();
	wxString sql = wxT("SELECT id, value, text FROM config WHERE id>=100");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int id = result.GetInt(0);
		sm_params[id] = result;
		if (id == FB_TEMP_DEL) FbTempEraser::sm_erase = result.GetInt(1);
	}
}

void FbCollection::LoadParams()
{
	wxString sql = wxT("SELECT id, value, text FROM params WHERE id<100");
	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int id = result.GetInt(0);
		m_params[id] = result;
		if (id == FB_TEMP_DEL) FbTempEraser::sm_erase = result.GetInt(1);
	}
}

int FbCollection::GetParamInt(int param)
{
	wxCriticalSectionLocker locker(sm_section);
	if (param >= 100) {
		if (sm_params.count(param)) return sm_params[param].m_int;
	} else {
		FbCollection * collection = GetCollection();
		if (collection && collection->m_params.count(param))
			return collection->m_params[param].m_int;
	}
	return FbParamItem::DefaultInt(param);
}

wxString FbCollection::GetParamStr(int param)
{
	wxCriticalSectionLocker locker(sm_section);
	if (param >= 100) {
		if (sm_params.count(param)) return sm_params[param].m_str;
	} else {
		FbCollection * collection = GetCollection();
		if (collection && collection->m_params.count(param))
			return collection->m_params[param].m_str;
	}
	return FbParamItem::DefaultStr(param);
}

void FbCollection::SetParamInt(int param, int value)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();

	if (param == FB_TEMP_DEL) FbTempEraser::sm_erase = value;

	if (param >= 100) {
		sm_params[param].m_int = value;
	} else {
		if (collection) collection->m_params[param].m_int = value;
	}

	if (collection == NULL) return;

	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	if (value == FbParamItem::DefaultInt(param)) {
		wxString sql = wxString::Format( wxT("DELETE FROM %s WHERE id=?"), table);
		wxSQLite3Statement stmt = collection->m_database.PrepareStatement(sql);
		stmt.Bind(1, param);
		stmt.ExecuteUpdate();
	} else {
		wxString sql = wxString::Format( wxT("INSERT OR REPLACE INTO %s (id, value) VALUES (?,?)"), table);
		wxSQLite3Statement stmt = collection->m_database.PrepareStatement(sql);
		stmt.Bind(1, param);
		stmt.Bind(2, value);
		stmt.ExecuteUpdate();
	}
}

void FbCollection::SetParamStr(int param, const wxString &value)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();

	if (param >= 100) {
		sm_params[param].m_str = value;
	} else {
		if (collection) collection->m_params[param].m_str = value;
	}

	if (collection == NULL) return;

	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	if (value == FbParamItem::DefaultStr(param)) {
		wxString sql = wxString::Format( wxT("DELETE FROM %s WHERE id=?"), table);
		wxSQLite3Statement stmt = collection->m_database.PrepareStatement(sql);
		stmt.Bind(1, param);
		stmt.ExecuteUpdate();
	} else {
		wxString sql = wxString::Format( wxT("INSERT OR REPLACE INTO %s (id, text) VALUES (?,?)"), table);
		wxSQLite3Statement stmt = collection->m_database.PrepareStatement(sql);
		stmt.Bind(1, param);
		stmt.Bind(2, value);
		stmt.ExecuteUpdate();
	}
}

void FbCollection::ResetParam(int param)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();

	if (param >= 100) {
		sm_params.erase(param);
	} else {
		if (collection) collection->m_params.erase(param);
	}

	if (collection == NULL) return;

	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	wxString sql = wxString::Format( wxT("DELETE FROM %s WHERE id=?"), table);
	wxSQLite3Statement stmt = collection->m_database.PrepareStatement(sql);
	stmt.Bind(1, param);
	stmt.ExecuteUpdate();
}
