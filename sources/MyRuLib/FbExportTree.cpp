#include "FbExportTree.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"

//-----------------------------------------------------------------------------
//  FbExportParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportParentData, FbParentData)

//-----------------------------------------------------------------------------
//  FbExportTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbExportChildData, FbChildData)

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
			books.id, title, file_size, file_type, file_name, lang, md5sum, id_author, letter, full_name, sequences.value AS sequence, bookseq.number\
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

	FbSortedArrayInt items(FbArrayEvent::CompareInt);

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
	}

	Sort();
}

void FbExportTreeModel::Append(int book, const wxString &filename)
{
}

void FbExportTreeModel::Sort()
{
}
