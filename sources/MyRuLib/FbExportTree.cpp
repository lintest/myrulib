#include "FbExportTree.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbParams.h"

//-----------------------------------------------------------------------------
//  FbExportParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportParentData, FbParentData)

//-----------------------------------------------------------------------------
//  FbExportTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportChildData, FbChildData)

//-----------------------------------------------------------------------------
//  FbExportTreeContext
//-----------------------------------------------------------------------------

FbExportTreeContext::FbExportTreeContext()
{
	m_translit_folder = FbParams::GetValue(FB_TRANSLIT_FOLDER);
	m_translit_file = FbParams::GetValue(FB_TRANSLIT_FILE);
	m_template = FbParams::GetText(FB_FOLDER_FORMAT);
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
					text = Get(result, wxT("full_name"));
				} break;
				case wxT('s'): {
					text = Get(result, wxT("sequence"));
				} break;
				case wxT('n'): {
					text = Get(result, wxT("number"));
				} break;
				case wxT('i'): {
					text = Get(result, wxT("id"));
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

	wxFileName filename = res;
	if (m_translit_folder) {
		wxArrayString folders = filename.GetDirs();
	}
	return filename;
}

//-----------------------------------------------------------------------------
//  FbExportTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportTreeModel, FbTreeModel)

FbExportTreeModel::FbExportTreeModel(const wxString &books, int author)
{
	FbExportParentData * root = new FbExportParentData(*this);
	SetRoot(root);

	wxString sql = wxT("\
		SELECT DISTINCT \
			books.id, title, file_type, file_name, lang, md5sum, letter, full_name, sequences.value AS sequence, bookseq.number\
		FROM books \
			LEFT JOIN authors ON authors.id=books.id_author \
			LEFT JOIN bookseq ON bookseq.id_book=books.id \
			LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
		WHERE books.id IN (%s) %s \
		ORDER BY full_name, sequence\
	");

	wxString filter;
	if ( author ) filter = wxString::Format(wxT("AND (books.id_author=%d)"), author);
	sql = wxString::Format(sql, books.c_str(), filter.c_str());

	FbExportTreeContext context;
	FbSortedArrayInt items(FbArrayEvent::CompareInt);

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		Append(result.GetInt(0), context.GetFilename(result));
	}
	Sort();
}

void FbExportTreeModel::Append(int book, const wxFileName &filename)
{

}

void FbExportTreeModel::Sort()
{
}
