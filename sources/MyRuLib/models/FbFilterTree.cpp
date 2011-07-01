#include "FbFilterTree.h"
#include <wx/tokenzr.h>
#include "FbBookEvent.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbParams.h"
#include "dialogs/FbFilterDlg.h"

//-----------------------------------------------------------------------------
//  FbFilterParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFilterParentData, FbParentData)

//-----------------------------------------------------------------------------
//  FbFilterChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFilterChildData, FbChildData)

//-----------------------------------------------------------------------------
//  FbFilterTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFilterTreeModel, FbTreeModel)

FbFilterTreeModel::FbFilterTreeModel(const wxSortedArrayString &array, const wxString & first)
{
	FbFilterParentData * root = new FbFilterParentData(*this);
	new FbFilterChildData(*this, root, first);

	size_t count = array.Count();
	for (size_t i = 0; i < count; i++) {
		wxString string = array[i];
		if (string != first) new FbFilterChildData(*this, root, string);
	}

	SetRoot(root);
}

FbFilterTreeModel::FbFilterTreeModel(const wxString & list, const wxString &sel)
{
	FbFilterParentData * root = new FbFilterParentData(*this);

	wxStringTokenizer tokenizer(list, wxT(','));
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		new FbFilterChildData(*this, root, token);
	}

	SetRoot(root);
	SetSel(sel);
}

void FbFilterTreeModel::SetSel(const wxString &sel)
{
	wxSortedArrayString strings;
	wxStringTokenizer tokenizer(sel, wxT(','));
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		token.Replace(wxT("'"), wxEmptyString);
		strings.Add(token);
	}

	FbModelItem root = GetRoot();
	size_t count = root.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = root.Items(i);
		int state = strings.Index(child[0]) != wxNOT_FOUND ? 1 : 0;
		child.SetState(state);
	}
}

wxString FbFilterTreeModel::GetAll()
{
	wxString result;
	FbModelItem root = GetRoot();
	size_t count = root.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = root.Items(i);
		if (i) result << wxT(',');
		result << child[0];
	}
	return result;
}

wxString FbFilterTreeModel::GetSel()
{
	wxString result;
	FbModelItem root = GetRoot();
	if (root.GetState() != 2) return wxEmptyString;
	size_t count = root.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = root.Items(i);
		if (child.GetState()) {
			if (!result.IsEmpty()) result << wxT(',');
			result << wxT('\'') << child[0] << wxT('\'');
		}
	}
	return result;
}

//-----------------------------------------------------------------------------
//  FbFilterTreeThread
//-----------------------------------------------------------------------------

void FbFilterTreeThread::Add(wxSortedArrayString &array, const wxString & string)
{
	wxString text = string;
	text.Replace(wxT("\""), wxEmptyString, true);
	text.Replace(wxT("'"), wxEmptyString, true);
	text.Replace(wxT(","), wxEmptyString, true);
	if (text.IsEmpty()) return;
	if (array.Index(text) == wxNOT_FOUND) array.Add(text);
}

void * FbFilterTreeThread::Entry()
{
	wxSortedArrayString types;
	wxSortedArrayString langs;

	FbCommonDatabase database;
	database.JoinThread(this);
	wxString sql = wxT("SELECT DISTINCT file_type, lang FROM books");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (!result.IsOk()) return NULL;
	while (result.NextRow()) {
		Add(types, result.GetString(0));
		Add(langs, result.GetString(1));
	}

	FbFilterTreeModel * type_tree = new FbFilterTreeModel(types, wxT("fb2"));
	FbFilterTreeModel * lang_tree = new FbFilterTreeModel(langs, wxT("ru"));

	FbParams(DB_TYPE_LIST) = type_tree->GetAll();
	FbParams(DB_LANG_LIST) = lang_tree->GetAll();
	FbParams(DB_LAST_BOOK) = m_last_book;

	FbModelEvent(FbFilterDlg::ID_TREE_TYPE, type_tree).Post(m_frame);
	FbModelEvent(FbFilterDlg::ID_TREE_LANG, lang_tree).Post(m_frame);

	return NULL;
}
