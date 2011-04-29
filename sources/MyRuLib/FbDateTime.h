#include <wx/datetime.h>

class FbDateTime: public wxDateTime
{
	public:
		static FbDateTime Today();
		FbDateTime(const wxDateTime &date): wxDateTime(date) {}
		FbDateTime(int year, int month, int day);
		FbDateTime(int code);
		int Code() const;
	private:
		static wxDateTime::Month GetMonth(int month);
};
