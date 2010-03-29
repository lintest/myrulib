#ifndef __FBLISTMODEL_H__
#define __FBLISTMODEL_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include <wx/arrimpl.cpp>
#include "FbModelData.h"

class FbListModelData
{
    public:
        FbListModelData(unsigned int id = 0): m_rowid(id), m_bookid(0), m_filesize(0) {};
        FbListModelData(unsigned int id, wxSQLite3ResultSet &result);
        FbListModelData(const FbListModelData &data);
        wxString GetValue(unsigned int col);
		wxString GetAuthors(wxSQLite3Database &database);
        unsigned int Id() { return m_rowid; };
    private:
		wxString Format(const int number);
    private:
        int m_rowid;
        int m_bookid;
        wxString m_title;
        wxString m_authors;
        wxString m_AuthIds;
        int m_filesize;
};

WX_DECLARE_OBJARRAY(FbListModelData, FbListModelArray);

class FbListModelCashe: private FbListModelArray
{
	public:
        FbListModelCashe(const wxString &filename);
        wxString GetValue(unsigned int row, unsigned int col);
		bool GetValue(wxVariant &variant, unsigned int row, unsigned int col);
        bool SetValue(const wxVariant &variant, unsigned int row, unsigned int col);
        unsigned int RowCount();
	private:
        FbListModelData FindRow(unsigned int rowid);
	private:
        wxSQLite3Database m_database;
        unsigned int m_rowid;
        wxArrayInt m_checked;
};


class FbListModel: public wxDataViewVirtualListModel
{
	public:
		enum COL
		{
			COL_TITLE,
			COL_ROWID,
			COL_BOOKID,
			COL_SIZE,
			COL_AUTHOR,
			COL_GENRE,
			COL_NUMBER,
			COL_RATING,
			COL_TYPE,
			COL_LANG,
			COL_MAX,
		};

    public:

        FbListModel(const wxString &filename);

        virtual ~FbListModel();

        // implementation of base class virtuals to define model

        virtual unsigned int GetColumnCount() const
        {
            return COL_MAX;
        }

        virtual wxString GetColumnType( unsigned int col ) const
        {
            return wxT("string");
        }

        virtual void GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
        virtual bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const;
        virtual bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col );

    public:

    private:
        long Init(const wxString &filename);

    private:
        FbListModelCashe * m_datalist;
};

#endif // __FBLISTMODEL_H__
