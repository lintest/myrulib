#ifndef __FBBOOKMODEL_H__
#define __FBBOOKMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/wxsqlite3.h>
#include <wx/arrimpl.cpp>
#include <wx/renderer.h>

class FbBookModelData
{
    public:
        FbBookModelData(unsigned int id = 0): m_rowid(id) {};
        FbBookModelData(wxSQLite3ResultSet &result);
        FbBookModelData(const FbBookModelData &data);
        wxString GetValue(unsigned int col);
        unsigned int Id() { return m_rowid; };
    public:
        unsigned int m_rowid;
        wxString m_title;
        wxArrayString m_values;
};

WX_DECLARE_OBJARRAY(FbBookModelData, FbBookModelArray);

class FbBookModelCashe: private FbBookModelArray
{
	public:
        FbBookModelCashe(const wxString &filename);
        wxString GetValue(unsigned int row, unsigned int col);
        unsigned int RowCount();
	private:
        FbBookModelData FindRow(unsigned int rowid);
	private:
        wxSQLite3Database m_database;
        unsigned int m_rowid;
};


class FbBookModel: public wxDataViewVirtualListModel
{
    public:
        enum COL
        {
            COL_ROWID,
            COL_NUM,
            COL_TITLE,
            COL_AUTHOR,
            COL_GENRE,
            COL_NUMBER,
            COL_RATING,
            COL_TYPE,
            COL_LANG,
            COL_SIZE,
            COL_MAX,
        };

        FbBookModel(const wxString &filename);

        virtual ~FbBookModel();

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
        virtual bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col ) { return false; };

    public:

    private:
        long Init(const wxString &filename);

    private:
        FbBookModelCashe * m_datalist;
};

class FbTitleData : public wxObject
{
	public:
		FbTitleData( const wxString &text = wxEmptyString, bool checked = false, int level = 0 )
			: m_title(text), m_checked(checked), m_level(level)
		{ }

		FbTitleData( const FbTitleData &data )
			: m_title(data.m_title), m_checked(data.m_checked), m_level(data.m_level)
		{ }

		inline bool operator==(const FbTitleData& data)
		{
			return m_title == data.m_title && m_checked == m_checked && m_level == data.m_level;
		}

		inline bool operator!=(const FbTitleData& data)
		{
			return !(*this == data);
		}

		friend class FbTitleRenderer;

	private:
		wxString m_title;
		bool m_checked;
		int m_level;

		DECLARE_DYNAMIC_CLASS(wxDataViewIconText)
};

DECLARE_VARIANT_OBJECT(FbTitleData)

class FbTitleRenderer : public wxDataViewCustomRenderer
{
	public:
		FbTitleRenderer()
			: wxDataViewCustomRenderer("FbTitleData", wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT)
		{
			EnableEllipsize(wxELLIPSIZE_END);
		}

		virtual bool Render( wxRect rect, wxDC *dc, int state );

		virtual bool LeftClick( wxPoint cursor, wxRect WXUNUSED(cell),
							   wxDataViewModel *WXUNUSED(model),
							   const wxDataViewItem &WXUNUSED(item),
							   unsigned int WXUNUSED(col) )
		{
			wxLogMessage( "FbTitleRenderer LeftClick( %d, %d )", cursor.x, cursor.y );
			return false;
		}

		bool Activate( wxRect WXUNUSED(cell),
						wxDataViewModel *model,
						const wxDataViewItem & item, unsigned int col)
		{
	//		model->ChangeValue(!m_toggle, item, col);
			wxLogMessage( "FbTitleRenderer Activate" );
			return false;
		}

		virtual wxSize GetSize() const
		{
			return wxSize(GetOwner()->GetWidth(),20);
		}

		virtual bool SetValue( const wxVariant &value )
		{
			FbTitleData data;
			data << value;
			m_title = data.m_title;
			m_checked = data.m_checked;
			m_level = data.m_level;
			return true;
		}

		virtual bool GetValue( wxVariant &WXUNUSED(value) ) const { return true; }

	private:
		wxString m_title;
		bool m_checked;
		int m_level;
};

#endif // __FBBOOKMODEL_H__
