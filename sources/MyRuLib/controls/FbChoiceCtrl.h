#ifndef __FBCHOICECTRL_H__
#define __FBCHOICECTRL_H__

#include <wx/choice.h>

class FbChoiceInt : public wxChoice
{
	public:
		FbChoiceInt() {}

		FbChoiceInt(wxWindow *parent, wxWindowID id,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0)
		{
			Create(parent, id, pos, size, style);
		}

		bool Create(wxWindow *parent, wxWindowID id,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = 0)
		{
			return wxChoice::Create(parent, id, pos, size, style);
		}

		virtual ~FbChoiceInt() {}

		int Append(const wxString &item, int data)
		{
			return wxChoice::Append(item, new IntData(data));
		}

		int GetCurrentData() const
		{
			int index = GetCurrentSelection();
			return (index == wxNOT_FOUND) ? 0 : GetClientData(index);
		}

		int GetClientData(unsigned int index) const
		{
			if (index >= GetCount()) return 0;
			IntData * data = (IntData*) wxChoice::GetClientObject(index);
			return data ? data->GetData() : 0;
		}

	protected:
		class IntData: public wxClientData
		{
			public:
				IntData( int data = 0 ) : m_data(data) { }
				void SetData( int data ) { m_data = data; }
				int GetData() const { return m_data; }
			private:
				int  m_data;
		};

		DECLARE_DYNAMIC_CLASS(FbChoiceInt)
};

class FbChoiceStr : public wxChoice
{
	public:
		FbChoiceStr() {}

		FbChoiceStr(wxWindow *parent, wxWindowID id,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0)
		{
			Create(parent, id, pos, size, style);
		}

		bool Create(wxWindow *parent, wxWindowID id,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = 0)
		{
			return wxChoice::Create(parent, id, pos, size, style);
		}

		virtual ~FbChoiceStr() {}

		int Append(const wxString &item, const wxString &data = wxEmptyString)
		{
			return wxChoice::Append(item, new StrData(data));
		}

		wxString GetCurrentData() const
		{
			int index = GetCurrentSelection();
			return (index == wxNOT_FOUND) ? wxString() : GetClientData(index);
		}

		wxString GetClientData(unsigned int index) const
		{
			if (index >= GetCount()) return wxEmptyString;
			StrData * data = (StrData*) wxChoice::GetClientObject(index);
			return data ? data->GetData() : wxString();
		}

	protected:
		class StrData: public wxClientData
		{
			public:
				StrData( const wxString &data = wxEmptyString): m_data(data) {}
				void SetData( const wxString &data = wxEmptyString) { m_data = data; }
				wxString GetData() const { return m_data; }
			private:
				wxString m_data;
		};

		DECLARE_DYNAMIC_CLASS(FbChoiceStr)
};

#endif // __FBCHOICECTRL_H__

