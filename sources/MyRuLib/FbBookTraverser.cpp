#include "FbBookTraverser.h"

IMPLEMENT_CLASS(FbBookTraverser, wxObject)

//-----------------------------------------------------------------------------
//  FbBookArrayTraverser
//-----------------------------------------------------------------------------

void FbBookArrayTraverser::DoBook(FbModelItem & item, int level, size_t row)
{
	int book = item.GetBook();
	if (book) m_items.Add(book);
}


//-----------------------------------------------------------------------------
//  FbBookTextTraverser
//-----------------------------------------------------------------------------

void FbBookTextTraverser::DoBook(FbModelItem & item, int level, size_t row)
{
	wxString str;

	for (int i = 0; i < level; i++) {
		str << wxT("\t");
	}
	
	if (item.FullRow()) {
		size_t count = m_columns.Count();
		if (count) str << item[m_columns[0]];
	} else {
		size_t count = m_columns.Count();
		for (size_t i = 0; i < count; i++) {
			if (i) str << wxT("\t");
			str << item[m_columns[i]];;
		}
	}

	#ifdef __WXMSW__
	str << wxT("\r");
	#endif
	str << wxT("\n");

	m_text.Prepend(str);
}

