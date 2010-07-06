#include "FbExportTree.h"
#include "FbDatabase.h"

//-----------------------------------------------------------------------------
//  FbExportTreeModel
//-----------------------------------------------------------------------------

FbExportTreeModel::FbExportTreeModel(const wxString &books, int author)
{
	FbExportParentData * root = new FbExportParentData(*this);
	SetRoot(root);

	wxString sql = wxT("\
		SELECT DISTINCT \
			books.id, books.title, books.file_size, books.file_type, books.file_name, books.id_author, authors.letter, authors.full_name, bookseq.id_seq, sequences.value AS sequence, bookseq.number\
		FROM books \
			LEFT JOIN authors ON authors.id=books.id_author \
			LEFT JOIN bookseq ON bookseq.id_book=books.id \
			LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
		WHERE books.id IN (%s) %s \
	");

	wxString filter;
	if ( author ) filter = wxString::Format(wxT("AND (books.id_author=%d)"), author);
	sql = wxString::Format(sql, books.c_str(), filter.c_str());

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
	}

	Sort();
}

void FbExportTreeModel::Append(int book, const wxString &filename)
{
}
