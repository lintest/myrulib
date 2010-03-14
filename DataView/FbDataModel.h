#ifndef __FBDATAMODEL_H__
#define __FBDATAMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>

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

		bool IsChecked() { return m_checked; };

		friend class FbTitleRenderer;
		friend class FbBookModelCashe;

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

		virtual bool LeftClick( wxPoint cursor, wxRect cell, wxDataViewModel *model, const wxDataViewItem &item, unsigned int col );

//		virtual bool Activate( wxRect cell, wxDataViewModel *model, const wxDataViewItem & item, unsigned int col);

		virtual wxSize GetSize() const;

		virtual bool SetValue( const wxVariant &value );

		virtual bool GetValue( wxVariant &WXUNUSED(value) ) const { return true; }

	private:
        FbTitleData m_data;
};

#endif // __FBDATAMODEL_H__
