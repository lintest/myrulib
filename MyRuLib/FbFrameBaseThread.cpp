#include "FbFrameBaseThread.h"
#include "FbConst.h"
#include "FbBookEvent.h"

wxCriticalSection FbFrameBaseThread::sm_queue;

wxString FbFrameBaseThread::GetSQL(const wxString & condition, const wxString & order)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE: {
			sql = wxT("\
				SELECT (CASE WHEN bookseq.id_seq IS NULL THEN 1 ELSE 0 END) AS key, \
					books.id, books.title, books.file_size, books.file_type, books.id_author, \
					states.rating, authors.full_name as full_name, sequences.value AS sequence, bookseq.number as number\
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id  \
					LEFT JOIN bookseq ON bookseq.id_book=books.id AND bookseq.id_author = books.id_author \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				ORDER BY authors.search_name, key, sequences.value, bookseq.number, books.title \
			");
		} break;
		case FB2_MODE_LIST: {
			sql = wxT("\
				SELECT \
					books.id as id, books.title as title, books.file_size as file_size, books.file_type as file_type, \
					states.rating, authors.full_name as full_name \
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				ORDER BY \
			");
			sql += order.IsEmpty() ? wxT("books.title, books.id, authors.full_name") : order;
		} break;
	}

	wxString str = wxT("(%s)");
	if (m_FilterFb2) str += wxT("AND(books.file_type='fb2')");
	if (m_FilterLib) str += wxT("AND(books.id>0)");
	if (m_FilterUsr) str += wxT("AND(books.id<0)");
	sql = wxString::Format(sql, str.c_str());

	return wxString::Format(sql, condition.c_str());
}

void FbFrameBaseThread::CreateTree(wxSQLite3ResultSet &result)
{
	wxString thisAuthor = wxT("@@@");
	wxString thisSequence = wxT("@@@");
	while (result.NextRow()) {
		wxString nextAuthor = result.GetString(wxT("full_name"));
		wxString nextSequence = result.GetString(wxT("sequence"));

		if (thisAuthor != nextAuthor) {
			thisAuthor = nextAuthor;
			thisSequence = wxT("@@@");
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR, thisAuthor).Post(m_frame);
		}
		if (thisSequence != nextSequence) {
			thisSequence = nextSequence;
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE, thisSequence).Post(m_frame);
		}

		BookTreeItemData data(result);
		FbBookEvent(ID_APPEND_BOOK, &data).Post(m_frame);
	}
}

void FbFrameBaseThread::CreateList(wxSQLite3ResultSet &result)
{
	result.NextRow();
	while (!result.Eof()) {
		BookTreeItemData data(result);
		wxString full_name = result.GetString(wxT("full_name"));
		do {
			result.NextRow();
			if ( data.GetId() != result.GetInt(wxT("id")) ) break;
			full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
		} while (!result.Eof());

		FbBookEvent(ID_APPEND_BOOK, &data, full_name).Post(m_frame);
	}
}

void FbFrameBaseThread::EmptyBooks()
{
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_EMPTY_BOOKS).Post(m_frame);
}

void FbFrameBaseThread::FillBooks(wxSQLite3ResultSet &result)
{
	switch (m_mode) {
		case FB2_MODE_TREE: CreateTree(result); break;
		case FB2_MODE_LIST: CreateList(result); break;
	}
}
