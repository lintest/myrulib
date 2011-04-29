#include "FbDateTime.h"

FbDateTime::FbDateTime(int year, int month = 1, int day = 1)
	: wxDateTime(day, GetMonth(month), year)
{
}

FbDateTime::FbDateTime(int code)
	: wxDateTime(code % 100, GetMonth(code / 100 % 100), code / 10000 + 2000)
{
}

wxDateTime::Month FbDateTime::GetMonth(int month)
{
	return wxDateTime::Month((month - 1) % 12 + wxDateTime::Jan);
}

FbDateTime FbDateTime::Today()
{
	return wxDateTime::Today();
}

int FbDateTime::Code() const
{
	long code;
	Format(wxT("%y%m%d")).ToLong(&code);
	return code;
}
