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
	wxString sql = wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
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
		root->SortItems(database);
	}
	model->SetRoot(root);

	FbModelEvent(ID_BOOKS_LISTCTRL, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbAuthParentData
//-----------------------------------------------------------------------------

static int ComareBooks(FbModelData ** x, FbModelData ** y)
{
	FbSeqnParentData * xx = wxDynamicCast(*x, FbSeqnParentData);
	FbSeqnParentData * yy = wxDynamicCast(*y, FbSeqnParentData);
	if (xx && yy) return xx->Compare(*yy);
	return 0;
}

IMPLEMENT_CLASS(FbAuthParentData, FbParentData)

void FbAuthParentData::SortItems(wxSQLite3Database &database)
{
	m_items.Sort(ComareBooks);
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

int FbSeqnParentData::Compare(const FbSeqnParentData &data)
{
	if (GetCode() == 0) return +1;
	if (data.GetCode() == 0) return -1;
	return GetTitle().CmpNoCase(data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbBookChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookChildData, FbChildData)

wxString FbBookChildData::GetValue(FbModel & model, size_t col) const
{
	if (col == BF_NUMB)
		return m_numb ? wxString::Format(wxT("%d"), m_numb) : wxString();
	else
		return FbCollection::GetBook(m_code, col);
}
