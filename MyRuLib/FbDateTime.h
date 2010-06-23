#include <wx/datetime.h>

class FbDateTime: public wxDateTime
{
	public:
		FbDateTime(const wxDateTime &date): wxDateTime(date) {}
	    FbDateTime(int year, int month, int day);
		FbDateTime(int code);
	private:
		static wxDateTime::Month GetMonth(int month);
};
