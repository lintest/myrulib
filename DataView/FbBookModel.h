#ifndef __FBBOOKMODEL_H__
#define __FBBOOKMODEL_H__

#include <wx/dataview.h>
#include <wx/wxsqlite3.h>

class FbBookModel : public wxDataViewModel
{
    public:
        FbBookModel(const wxString &filename);
    protected:
    private:
        wxSQLite3Database m_database;
        size_t m_count;
};

#endif // __FBBOOKMODEL_H__
