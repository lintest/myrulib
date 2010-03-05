#ifndef __FBBOOKMODEL_H__
#define __FBBOOKMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/wxsqlite3.h>

class FbBookModel: public wxDataViewVirtualListModel
{
    public:
        enum
        {
            Col_EditableText,
            Col_IconText,
            Col_TextWithAttr,
            Col_Custom,
            Col_Max
        };

        FbBookModel(const wxString &filename);

        virtual ~FbBookModel();

        // implementation of base class virtuals to define model

        virtual unsigned int GetColumnCount() const
        {
            return Col_Max;
        }

        virtual wxString GetColumnType( unsigned int col ) const
        {
            if (col == Col_IconText)
                return wxT("wxDataViewIconText");

            return wxT("string");
        }

        virtual void GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
        virtual bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const;
        virtual bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col ) { return false; };
    private:
        long InitDatabase(const wxString &filename);
    private:
        wxSQLite3Database * m_database;
};

#endif // __FBBOOKMODEL_H__
