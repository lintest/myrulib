#include "FbExportTree.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbConvertDlg.h"
#include <wx/encconv.h>

#define fbMAX_FILENAME_LENGTH 100

//-----------------------------------------------------------------------------
//  FbExportParentData
//-----------------------------------------------------------------------------

static int ComareFiles(FbModelData ** x, FbModelData ** y)
{
	{
		FbExportParentData * xx = wxDynamicCast(*x, FbExportParentData);
		FbExportParentData * yy = wxDynamicCast(*y, FbExportParentData);
		if (xx && yy) return xx->Compare(*yy);
		if (xx) return -1;
		if (yy) return +1;
	}
	{
		FbExportChildData * xx = wxDynamicCast(*x, FbExportChildData);
		FbExportChildData * yy = wxDynamicCast(*y, FbExportChildData);
		if (xx && yy) return xx->Compare(*yy);
	}

	return 0;
}

IMPLEMENT_CLASS(FbExportParentData, FbParentData)

FbExportParentData * FbExportParentData::GetDir(FbModel & model, wxArrayString &dirs)
{
	if (dirs.Count() == 0) return this;

	wxString name = dirs[0];
	dirs.RemoveAt(0);

	bool not_found = true;
	FbExportParentData * child = NULL;
	size_t count = Count(model);
	for (size_t i = 0; i < count; i++) {
		FbModelData * data = Items(model, i);
		child = wxDynamicCast(data, FbExportParentData);
		if (child && child->m_name == name) { not_found = false; break; }
	}

	if (not_found) child = new FbExportParentData(model, this, name);

	return child->GetDir(model, dirs);
}

void FbExportParentData::Append(FbModel & model, int book, wxFileName &filename, int size)
{
	int number = 0;
	wxString name = filename.GetName();
	while (true) {
		wxString fullname = Lower(filename.GetFullName());
		size_t count = Count(model);
		bool ok = true;
		for (size_t i = 0; i < count; i++) {
			FbModelData * data = Items(model, i);
			if (!data) continue;
			if (Lower(data->GetValue(model)) == fullname) { ok = false; break; }
		}
		if (ok) break;
		filename.SetName(name + wxString::Format(wxT("(%d)"), ++number));
	}
	new FbExportChildData(model, this, book, filename, size);
}

int FbExportParentData::Compare(const FbExportParentData &data) const
{
	return m_name.CmpNoCase(data.m_name);
}

void FbExportParentData::Sort(FbModel & model)
{
	m_items.Sort(ComareFiles);
	size_t count = Count(model);
	for (size_t i = 0; i < count; i++) {
		FbModelData * data = Items(model, i);
		FbExportParentData * child = wxDynamicCast(data, FbExportParentData);
		if (child) child->Sort(model);
	}
}

void FbExportParentData::GetFiles(FbModel & model, FbConvertArray & files) const
{
	size_t count = Count(model);
	for (size_t i = 0; i < count; i++) {
		FbModelData * data = Items(model, i);
		FbExportChildData * child = wxDynamicCast(data, FbExportChildData);
		if (child) {
			files.Add(new FbConvertItem(child->GetBook(), child->GetPath(model)));
		} else {
			FbExportParentData * folder = wxDynamicCast(data, FbExportParentData);
			if (folder) folder->GetFiles(model, files);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbExportChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportChildData, FbChildData)

wxString FbExportChildData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0: {
			wxString ext;
			wxString arc;
			FbExportTreeModel * owner = wxDynamicCast(&model, FbExportTreeModel);
			if (owner) {
				ext = owner->GetExt();
				arc = owner->GetArc();
			}
			if (ext.IsEmpty()) ext = m_type;
			wxString filename = m_name;
			filename << wxT('.') << ext;
			if (!arc.IsEmpty()) filename << wxT('.') <<arc;
			return filename;
		} break;
		case 1: {
			int scale = 0;
			FbExportTreeModel * owner = wxDynamicCast(&model, FbExportTreeModel);
			if (owner) scale = owner->GetScale();
			if (scale == 0) scale = 100;
			return Format(m_size * scale / 100 / 1024);
		} break;
		default: return wxEmptyString;
	}
}

int FbExportChildData::Compare(const FbExportChildData &data) const
{
	return m_name.CmpNoCase(data.m_name);
}

wxFileName FbExportChildData::GetPath(FbModel &model) const
{
	wxString path;

	FbModelData * parent = GetParent();
	while (parent) {
		path.Prepend(wxT('/'));
		path.Prepend(parent->GetValue(model));
		parent = parent->GetParent(model);
	}

	wxFileName filename;
	filename.SetExt(m_type);
	filename.SetPath(path);
	filename.SetName(m_name);
	return filename;
}

//-----------------------------------------------------------------------------
//  FbExportTreeContext
//-----------------------------------------------------------------------------

FbExportTreeContext::FbExportTreeContext()
{
	m_translit_folder = FbParams::GetInt(FB_TRANSLIT_FOLDER);
	m_translit_file = FbParams::GetInt(FB_TRANSLIT_FILE);
	m_template = FbParams::GetStr(FB_FOLDER_FORMAT);
	m_underscores = FbParams::GetInt(FB_USE_UNDERSCORE);

	if (m_template.IsEmpty()) m_template = FbParams::DefaultStr(FB_FOLDER_FORMAT);
}

wxString FbExportTreeContext::Normalize(const wxString &filename, bool translit)
{
    const wxString forbidden = wxT("*?\\/:\"<>|");

	wxString oldname = filename;
	oldname.Trim(false).Trim(true);

	bool space = false;
	wxString newname;
	size_t length = oldname.Length();
	for (size_t i = 0; i < length; i++) {
		wxChar ch = oldname[i];
		if (0 <= ch && ch < 0x20) continue;
		if (forbidden.Find(ch) != wxNOT_FOUND) continue;
		if (ch == (wxChar)0x0401) ch = (wxChar)0x0415;
		if (ch == (wxChar)0x0451) ch = (wxChar)0x0435;
		space = ch == 0x20;
		if (m_underscores && space) ch = (wxChar)0x5F;
		newname << ch;
	}
	newname = newname.Trim(false).Trim(true).Left(fbMAX_FILENAME_LENGTH);

	wxEncodingConverter ec;
	ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_CP1251, wxCONVERT_SUBSTITUTE);
	newname = ec.Convert(newname);

	if (translit) {
		const wxChar * transchar[32] = {
			wxT("a"), wxT("b"), wxT("v"), wxT("g"), wxT("d"), wxT("e"), wxT("zh"), wxT("z"),
			wxT("i"), wxT("j"), wxT("k"), wxT("l"), wxT("m"), wxT("n"), wxT("o"), wxT("p"),
			wxT("r"), wxT("s"), wxT("t"), wxT("u"), wxT("f"), wxT("h"), wxT("c"), wxT("ch"),
			wxT("sh"), wxT("shh"), wxT("'"), wxT("y"), wxT("'"), wxT("e"), wxT("yu"), wxT("ya"),
		};
		oldname = newname;
		newname.Empty();
		size_t length = oldname.Length();
		for (size_t i = 0; i < length; i++) {
			unsigned char ch = (wxChar)oldname[i] % 0x100;
			if (0xC0 <= ch && ch <= 0xDF) {
				newname << wxString(transchar[ch - 0xC0]).Upper();
			} else if (0xE0 <= ch && ch <= 0xFF) {
				newname << wxString(transchar[ch - 0xE0]);
			} else newname << wxChar(ch);
		}
	}

	ec.Init(wxFONTENCODING_CP1251, wxFONTENCODING_UNICODE, wxCONVERT_SUBSTITUTE);
	newname = ec.Convert(newname);

	while (newname.Left(1) == wxT(".")) newname = newname.Mid(1);
	while (newname.Right(1) == wxT(".")) newname = newname.Mid(0, newname.Len()-1);

	return newname;
}

wxString FbExportTreeContext::Get(wxSQLite3ResultSet &result, const wxString &field)
{
	wxString res;
	wxString value = result.GetString(field);
	size_t length = value.Length();
	for (size_t i=0; i < length; i++) {
		wxChar ch = value[i];
		switch (ch) {
			case wxT('/'):
			case wxT('\\'):
				ch = wxT('-') ;
				break;
		}
		res << ch;
	}
	return res;
}

wxFileName FbExportTreeContext::GetFilename(wxSQLite3ResultSet &result)
{
	wxString res;
	bool skip = false;
	bool param = false;
	size_t length = m_template.Length();
	for (size_t i=0; i < length; i++) {
		wxChar ch = m_template[i];
		if (param) {
			wxString text;
			switch (ch) {
				case wxT('a'): {
					text = Upper(Get(result, wxT("full_name")).Left(1));
				} break;
				case wxT('f'): {
					text = Get(result, wxT("full_name"));
				} break;
				case wxT('c'): {
					text = Get(result, wxT("last_name"));
					wxString f = Get(result, wxT("first_name")).Left(1);
					wxString m = Get(result, wxT("middle_name")).Left(1);
					text << wxT(' ') << Upper(f + m).Trim(false);
					text.Trim(true);
				} break;
				case wxT('s'): {
					text = Get(result, wxT("sequence"));
				} break;
				case wxT('n'): {
					int number = result.GetInt(wxT("number"));
					if (number) text << number;
				} break;
				case wxT('i'): {
					int id = result.GetInt(wxT("id"));
					if (id < 0) { text << wxT('0'); id *= -1; }
					text << id;
				} break;
				case wxT('t'): {
					text = Get(result, wxT("title"));
				} break;
				case wxT('m'): {
					text = Get(result, wxT("md5sum"));
				} break;
				case wxT('l'): {
					text = Get(result, wxT("lang"));
				} break;
				case wxT('e'): {
					text = Get(result, wxT("file_type"));
				} break;
				default: {
					text = ch;
				} break;
			}
			res << text;
			skip = text.IsEmpty();
			param = false;
		} else {
			if (ch == wxT('%')) {
				param = true;
			} else if (!skip) res << ch;
		}
	}

	wxString name = res; res.Empty();
	length = name.Length();
	for (size_t i=0; i < length; i++) {
		wxChar ch = name[i];
		if (0 <= ch && ch < 0x20) continue;
		if (ch == wxT(':')) continue;
		res << ch;
	}

	wxString filetype = Normalize(Get(result, wxT("file_type")));
	if (filetype.IsEmpty()) filetype = wxT("fb2");
	res << wxT('.') << filetype;

	wxFileName filename = res;

	wxString path;
	wxArrayString dirs = filename.GetDirs();
	size_t count = dirs.Count();
	for (size_t i = 0; i < count; i++) {
		path << wxT('/') << Normalize(dirs[i], m_translit_folder);
	}
	filename.SetPath(path);

	filename.SetName(Normalize(filename.GetName(), m_translit_file));

	return filename;
}

//-----------------------------------------------------------------------------
//  FbExportTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportTreeModel, FbTreeModel)

FbExportTreeModel::FbExportTreeModel(const wxString &books, int author): m_scale(0)
{
	FbExportParentData * root = new FbExportParentData(*this, NULL, wxT('.'));
	SetRoot(root);

	wxString sql = wxT("\
		SELECT DISTINCT \
			books.id, file_size, title, file_type, file_name, lang, md5sum, letter, \
			full_name, first_name, middle_name, last_name, sequences.value AS sequence, bookseq.number\
		FROM books \
			LEFT JOIN authors ON authors.id=books.id_author \
			LEFT JOIN bookseq ON bookseq.id_book=books.id \
			LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
		WHERE books.id IN (%s) %s \
		ORDER BY full_name, sequence\
	");

	wxString filter;
	if ( author) filter = wxString::Format(wxT("AND (books.id_author=%d)"), author);
	sql = wxString::Format(sql, books.c_str(), filter.c_str());

	FbExportTreeContext context;
	FbSortedArrayInt items(FbArrayEvent::CompareInt);

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int book = result.GetInt(0);
		int size = result.GetInt(1);
		if (items.Index(book) != wxNOT_FOUND) continue;
		items.Add(book);

		wxFileName filename = context.GetFilename(result);
		wxArrayString dirs = filename.GetDirs();
		FbExportParentData * parent = root->GetDir(*this, dirs);
		if (parent) parent->Append(*this, book, filename, size);
	}
	root->Sort(*this);
}

void FbExportTreeModel::GetFiles(FbConvertArray & files)
{
	FbExportParentData * root = wxDynamicCast(m_root, FbExportParentData);
	if (root) root->GetFiles(*this, files);
}
