#include "FbBookTree.h"
#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbBookTreeThread
//-----------------------------------------------------------------------------

void * FbBookTreeThread::Entry()
{
	try {
		FbCommonDatabase database;
		DoAuthor(database);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbBookTreeThread::DoAuthor(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id_seq, id_book, number FROM bookseq WHERE id_author=? ORDER BY id_seq");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_author);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(database, result);
}

void FbBookTreeThread::MakeModel(wxSQLite3Database &database, wxSQLite3ResultSet &result)
{
	FbTreeModel * model = new FbTreeModel;
	FbAuthParentData * root = new FbAuthParentData(*model, NULL, m_author);
	FbAuthParentData * author = new FbAuthParentData(*model, NULL, m_author);
	FbSeqnParentData * parent = NULL;

	while (result.NextRow()) {
		int seqn = result.GetInt(0);
		if (parent == NULL || parent->GetCode() != seqn) {
			parent = new FbSeqnParentData(*model, root, seqn);
		}
		new FbBookChildData(*model, parent, result.GetInt(1), result.GetInt(2));
		new FbBookChildData(*model, author, result.GetInt(1), result.GetInt(2));
	}

	if (root->Count(*model) == 1 && parent && parent->GetCode() == 0) {
		wxDELETE(root);
		root = author;
	} else {
		wxDELETE(author);
		author = root;
	}
	model->SetRoot(root);
	root->SortItems(database);

	FbModelEvent(ID_MODEL_CREATE, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbAuthParentData
//-----------------------------------------------------------------------------

static int ComareBooks(FbModelData ** x, FbModelData ** y)
{
	{
		FbBookChildData * xx = wxDynamicCast(*x, FbBookChildData);
		FbBookChildData * yy = wxDynamicCast(*y, FbBookChildData);
		if (xx && yy) return xx->GetOrder() - yy->GetOrder();
	}

	{
		FbSeqnParentData * xx = wxDynamicCast(*x, FbSeqnParentData);
		FbSeqnParentData * yy = wxDynamicCast(*y, FbSeqnParentData);
		if (xx && yy) {
			if (xx->GetCode() == 0) return +1;
			if (yy->GetCode() == 0) return -1;
			return xx->GetTitle().CmpNoCase(yy->GetTitle());
		}
	}

	return 0;
}

IMPLEMENT_CLASS(FbAuthParentData, FbParentData)

void FbAuthParentData::SortItems(wxSQLite3Database &database)
{
	{
		wxString sql = wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_code);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		while (result.NextRow()) m_books.Add(result.GetInt(0));
	}

	m_items.Sort(ComareBooks);

	size_t count = m_items.Count();
	for (size_t i = 0; i < count; i++) {
		FbSeqnParentData * child = wxDynamicCast(&m_items[i], FbSeqnParentData);
		if (child) child->SortItems();
	}

	m_books.Empty();
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

void FbSeqnParentData::SortItems()
{
	m_items.Sort(ComareBooks);
}

//-----------------------------------------------------------------------------
//  FbBookChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookChildData, FbChildData)

wxString FbBookChildData::GetValue(FbModel & model, size_t col) const
{
	if (col == BF_NUMB)
		return wxString::Format(wxT("%d"), m_numb);
	else
		return FbCollection::GetBook(m_code, col);
}

int FbBookChildData::GetOrder() const
{
	FbAuthParentData * author = NULL;
	FbParentData * parent = GetParent();
	while (parent && (author == NULL)) {
		author = wxDynamicCast(parent, FbAuthParentData);
		parent = parent->GetParent();
	}
	return author ? author->GetBookOrder(m_code) : wxNOT_FOUND;
}
