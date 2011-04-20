#include "FbBookTraverser.h"

IMPLEMENT_CLASS(FbBookTraverser, wxObject)

//-----------------------------------------------------------------------------
//  FbBookArrayTraverser
//-----------------------------------------------------------------------------

void FbBookArrayTraverser::DoBook(FbModelItem & item, int level)
{
	int book = item.GetBook();
	if (book) m_items.Add(book);
}


//-----------------------------------------------------------------------------
//  FbBookTextTraverser
//-----------------------------------------------------------------------------

void FbBookTextTraverser::DoBook(FbModelItem & item, int level)
{
	for (int i = 0; i < level; i++) {
		m_text << wxT("\t");
	}
	
	if (item.FullRow()) {
		size_t count = m_columns.Count();
		if (count) m_text << item[m_columns[0]];
	} else {
		size_t count = m_columns.Count();
		for (size_t i = 0; i < count; i++) {
			if (i) m_text << wxT("\t");
			m_text << item[m_columns[i]];;
		}
	}

	#ifdef __WXMSW__
	m_text << wxT("\r");
	#endif
	m_text << wxT("\n");
}

