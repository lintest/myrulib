#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/wxsqlite3.h>
#include <wx/arrimpl.cpp>
#include <wx/renderer.h>
#include "FbDataModel.h"

class FbTreeDataNode
{
	public:
		FbTreeDataNode() {};
		virtual bool IsContainer() = 0;
		virtual wxDataViewItem GetParent() = 0;
        virtual void GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col) = 0;
        virtual bool SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col) = 0;
        virtual bool GetAttr(unsigned int col, wxDataViewItemAttr &attr) = 0;
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children ) = 0;
};

WX_DECLARE_OBJARRAY(FbTreeDataNode*, FbTreeDataArray);

class FbLetterDataNode: public FbTreeDataNode
{
	public:
		FbLetterDataNode(wxChar letter, unsigned int count, bool checked = false): m_letter(letter), m_count(count), m_checked(checked) {};
		virtual bool IsContainer() { return true; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(NULL); };
        virtual void GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col);
        virtual bool SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col);
        virtual bool GetAttr(unsigned int col, wxDataViewItemAttr &attr);
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children );
        void CheckChildren(wxSQLite3Database * database);
	private:
		wxChar m_letter;
		unsigned int m_count;
		FbTreeDataArray m_children;
		bool m_checked;
};

class FbAuthorDataNode: public FbTreeDataNode
{
	public:
		FbAuthorDataNode(FbLetterDataNode * owner, bool checked = false): m_owner(owner), m_id(0), m_checked(checked) {};
		virtual bool IsContainer() { return false; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(m_owner); };
        virtual void GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col);
        virtual bool SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col);
        virtual bool GetAttr(unsigned int col, wxDataViewItemAttr &attr) { return false; };
        virtual unsigned int GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children ) { return 0; };
        void SetId(int id) { m_id = id; };
	private:
		FbLetterDataNode * m_owner;
		int m_id;
		FbTreeDataArray m_children;
		bool m_checked;
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

class FbTreeModelCashe: private FbTreeDataArray
{
	public:
        FbTreeModelCashe(const wxString &filename);
		bool GetValue(wxVariant &variant, unsigned int row, unsigned int col);
        bool SetValue(const wxVariant &variant, unsigned int row, unsigned int col);
	private:
        wxSQLite3Database m_database;
		FbTreeDataArray m_children;
};

class FbTreeModel: public wxDataViewModel
{
	public:
		enum COL
		{
			COL_TITLE,
			COL_ROWID,
			COL_BOOKID,
			COL_SIZE,
			COL_GENRE,
			COL_NUMBER,
			COL_RATING,
			COL_TYPE,
			COL_LANG,
			COL_MAX,
		};

    public:
        FbTreeModel(const wxString &filename);

        virtual ~FbTreeModel();

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

        virtual bool GetAttr(const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr) const;

        virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const;

	private:
        unsigned int GetLetters( wxDataViewItemArray &children ) const;

    private:
        wxSQLite3Database * m_database;
		FbTreeDataArray m_children;
};

#endif // __FBTREEMODEL_H__
