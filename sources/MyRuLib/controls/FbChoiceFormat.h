#ifndef __FBCHOICEFORMAT_H__
#define __FBCHOICEFORMAT_H__

#include <wx/choice.h>

class FbChoiceFormat : public wxChoice
{
	public:
		FbChoiceFormat() {}

		FbChoiceFormat(wxWindow *parent, wxWindowID id,
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

		virtual ~FbChoiceFormat() {}

		int Append(const wxString& item, int data)
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
			if (index <= GetCount()) return 0;
			IntData * data = (IntData*) wxChoice::GetClientObject(index);
			return data ? data->GetData() : 0;
		}

	protected:
		class IntData: public wxClientData
		{
			public:
				IntData() : m_data(0) { }
				IntData( int data ) : m_data(data) { }
				void SetData( int data ) { m_data = data; }
				int GetData() const { return m_data; }
			private:
				int  m_data;
		};

		DECLARE_DYNAMIC_CLASS(FbChoiceFormat)
};

#endif // __FBCHOICEFORMAT_H__
