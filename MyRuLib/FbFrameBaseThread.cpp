#include "FbFrameBaseThread.h"
#include "FbConst.h"
#include "FbBookEvent.h"

// Concatenate all values
void FbAggregateFunction::Aggregate(wxSQLite3FunctionContext& ctx)
{
	// Get the temporary memory for storing the intermediate result
	wxArrayString** acc = (wxArrayString**) ctx.GetAggregateStruct(sizeof (wxArrayString**));

	// Allocate a wxString instance in the first aggregate step
	if (*acc == NULL) {
	  *acc = new wxArrayString;
	}

	// Concatenate all arguments
	for (int i = 0; i < ctx.GetArgCount(); i++) {
	   (*acc)->Add(ctx.GetString(i));
}
}

  // Set the result of the aggregate function
void FbAggregateFunction::Finalize(wxSQLite3FunctionContext& ctx)
{
	// Get the temporary memory conatining the result
	wxArrayString** acc = (wxArrayString**) ctx.GetAggregateStruct(sizeof (wxArrayString**));

	(*acc)->Sort();

	wxString result;
	size_t iCount = (*acc)->Count();
	for (size_t i=0; i<iCount; i++) {
		if (!result.IsEmpty()) result += wxT(", ");
		result += (*acc)->Item(i).Trim(true).Trim(false);
	}

	// Set the result
	ctx.SetResult(result);

	// Important: Free the allocated wxString
	delete *acc;
	*acc = 0;
}

wxCriticalSection FbFrameBaseThread::sm_queue;

wxString FbFrameBaseThread::GetSQL(const wxString & condition)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE:
			sql = wxT("\
				SELECT (CASE WHEN bookseq.id_seq IS NULL THEN 1 ELSE 0 END) AS key, \
					books.id, books.title, books.file_size, books.file_type, books.id_author, \
					states.rating, authors.full_name, sequences.value AS sequence, bookseq.number\
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id  \
					LEFT JOIN bookseq ON bookseq.id_book=books.id AND bookseq.id_author = books.id_author \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				ORDER BY authors.search_name, key, sequences.value, bookseq.number, books.title \
			");
			break;
		case FB2_MODE_LIST:
			sql = wxT("\
				SELECT \
					books.id, books.title, books.file_size, books.file_type, \
					states.rating as rating, books.created, AGGREGATE(authors.full_name) as full_name \
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				GROUP BY books.id, books.title, books.file_size, books.file_type, states.rating, books.created \
				ORDER BY \
			") + GetOrder();
			break;
	}

	wxString str = wxT("(%s)");
	if (m_FilterFb2) str += wxT("AND(books.file_type='fb2')");
	if (m_FilterLib) str += wxT("AND(books.id>0)");
	if (m_FilterUsr) str += wxT("AND(books.id<0)");
	sql = wxString::Format(sql, str.c_str());

	return wxString::Format(sql, condition.c_str());
}

wxString FbFrameBaseThread::GetOrder()
{
	return m_ListOrder;
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
	while (result.NextRow()) {
		BookTreeItemData data(result);
		wxString full_name = result.GetString(wxT("full_name"));
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

void FbFrameBaseThread::InitDatabase(FbCommonDatabase &database)
{
	database.AttachConfig();
	database.CreateFunction(wxT("AGGREGATE"), 1, m_aggregate);
}
