#ifndef __FBBOOKTRAVERSER_H__
#define __FBBOOKTRAVERSER_H__

#include <wx/wx.h>
#include "controls/FbTreeModel.h"
#include "controls/FbTreeView.h"

class FbBookTraverser :
	public wxObject
{
public:
	FbBookTraverser() {}

	virtual ~FbBookTraverser() {}

	void OnBook(FbModelItem item, int level = 0)
		{ DoBook(item, level); }

protected:
	virtual void DoBook(FbModelItem & item, int level) = 0;

protected:
	DECLARE_DYNAMIC_CLASS(FbBookTraverser)
};

class FbBookArrayTraverser :
	public FbBookTraverser
{
public:
	FbBookArrayTraverser(wxArrayInt &items)
		: m_items(items) {}
protected:
	virtual void DoBook(FbModelItem & item, int level);

private:
	wxArrayInt & m_items;
};

class FbBookTextTraverser :
	public FbBookTraverser
{
public:
	FbBookTextTraverser(wxArrayInt &columns)
		: m_columns(columns) {}

	wxString GetText()
		{ return m_text; }

protected:
	virtual void DoBook(FbModelItem & item, int level);

private:
	wxArrayInt m_columns;
	wxString m_text;
};

#endif // __FBBOOKTRAVERSER_H__

