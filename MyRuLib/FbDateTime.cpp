#include "FbDateTime.h"

FbDateTime::FbDateTime(int year, int month = 1, int day = 1)
	: wxDateTime(day, GetMonth(month), year + 2000)
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
