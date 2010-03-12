#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/wxsqlite3.h>
#include <wx/arrimpl.cpp>
#include <wx/renderer.h>

class FbDataViewItem: public wxDataViewItem
{
	public:
		FbDataViewItem(): wxDataViewItem(this) {};
        virtual wxString GetValue(unsigned int col) = 0;
        virtual bool SetValue(const wxVariant &variant, unsigned int col) = 0;
		virtual wxDataViewItem GetParent() = 0;
		virtual bool IsContainer() = 0;
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children ) = 0;
};

class FbLetterItem: public FbDataViewItem
{
	public:
		FbLetterItem(wxChar letter): m_letter(letter) {};
        virtual wxString GetValue(unsigned int col) { return m_letter; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(NULL); };
		virtual bool IsContainer() { return true; };
        virtual bool SetValue(const wxVariant &variant, unsigned int col) { return false; };
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children );
	private:
		wxChar m_letter;
};

class FbAuthorItem: public FbDataViewItem
{
	public:
		FbAuthorItem(void * owner, int id, const wxString &name): m_owner(m_owner), m_id(id), m_name(name) {};
        virtual wxString GetValue(unsigned int col) { return m_name; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(m_owner); };
		virtual bool IsContainer() { return false; };
        virtual bool SetValue(const wxVariant &variant, unsigned int col) { return false; };
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children ) { return 0; };
	private:
		void * m_owner;
		int m_id;
		wxString m_name;

};

class FbTreeModelData
{
    public:
        FbTreeModelData(unsigned int id = 0): m_rowid(id), m_bookid(0), m_filesize(0) {};
        FbTreeModelData(wxSQLite3ResultSet &result);
        FbTreeModelData(const FbTreeModelData &data);
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
        int m_filesize;
};

WX_DECLARE_OBJARRAY(FbTreeModelData, FbTreeModelArray);

class FbTreeModelCashe: private FbTreeModelArray
{
	public:
        FbTreeModelCashe(const wxString &filename);
        wxString GetValue(unsigned int row, unsigned int col);
		bool GetValue(wxVariant &variant, unsigned int row, unsigned int col);
        bool SetValue(const wxVariant &variant, unsigned int row, unsigned int col);
        unsigned int RowCount();
        unsigned int GetLetters( wxDataViewItemArray &children );
	private:
        FbTreeModelData FindRow(unsigned int rowid);
	private:
        wxSQLite3Database m_database;
        unsigned int m_rowid;
        wxArrayInt m_checked;
};


class FbTreeModel: public wxDataViewModel
{
    public:
        enum COL
        {
            COL_ROWID,
            COL_BOOKID,
            COL_TITLE,
            COL_SIZE,
            COL_AUTHOR,
            COL_GENRE,
            COL_NUMBER,
            COL_RATING,
            COL_TYPE,
            COL_LANG,
            COL_MAX,
        };

        FbTreeModel(const wxString &filename);

        virtual ~FbTreeModel();

        // implementation of base class virtuals to define model

        virtual unsigned int GetColumnCount() const
        {
            return COL_MAX;
        }

        virtual wxString GetColumnType( unsigned int col ) const
        {
            return wxT("string");
        }

		virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;

		virtual bool IsContainer( const wxDataViewItem &item ) const;

        virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const;

        virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);

        virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const;

	private:
        unsigned int GetLetters( wxDataViewItemArray &children ) const;

    private:
        wxSQLite3Database * m_database;
};

#endif // __FBTREEMODEL_H__
