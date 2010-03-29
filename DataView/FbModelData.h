#ifndef __FBMODELDATA_H__
#define __FBMODELDATA_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/renderer.h>

class FbTitleData : public wxObject
{
    public:
        enum STATE {
            ST_EMPTY = 0,
            ST_CHECK = 1,
            ST_GRAY = 2,
        };

	public:
		FbTitleData( const wxString &text = wxEmptyString, STATE checked = ST_EMPTY, int level = 0 )
			: m_title(text), m_checked(checked), m_level(level)
		{ }

		FbTitleData( const wxString &text, bool checked, int level = 0 )
			: m_title(text), m_checked(State(checked)), m_level(level)
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

		STATE IsChecked() { return m_checked; };

        static STATE State(bool checked)
        {
            return checked ? FbTitleData::ST_CHECK : FbTitleData::ST_EMPTY;
        }

		friend class FbTitleRenderer;
		friend class FbListModelCashe;

	private:
		wxString m_title;
		STATE m_checked;
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

		virtual bool LeftClick( wxPoint cursor, wxRect cell, wxDataViewModel *model, const wxDataViewItem &item, unsigned int col );

		virtual bool Activate( wxRect cell, wxDataViewModel *model, const wxDataViewItem & item, unsigned int col);

		virtual wxSize GetSize() const;

		virtual bool SetValue( const wxVariant &value );

		virtual bool GetValue( wxVariant &WXUNUSED(value) ) const { return true; }

	private:
        FbTitleData m_data;
        int m_offsets[4];
};

#endif // __FBMODELDATA_H__
