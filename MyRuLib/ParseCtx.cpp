#include "ParseCtx.h"

extern "C" {
static void DefaultHnd(void *userData, const XML_Char *s, int len)
{
	// XML header:
	if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
	{
		ParsingContext *ctx = (ParsingContext*)userData;

		wxString buf = ctx->CharToString(s, (size_t)len);
		int pos;
		pos = buf.Find(wxT("encoding="));
		if (pos != wxNOT_FOUND)
			ctx->encoding = buf.Mid(pos + 10).BeforeFirst(buf[(size_t)pos+9]);
		pos = buf.Find(wxT("version="));
		if (pos != wxNOT_FOUND)
			ctx->version = buf.Mid(pos + 9).BeforeFirst(buf[(size_t)pos+8]);
	}
}
}

extern "C" {
static int UnknownEncodingHnd(void * WXUNUSED(encodingHandlerData),
							  const XML_Char *name, XML_Encoding *info)
{
	// We must build conversion table for expat. The easiest way to do so
	// is to let wxCSConv convert as string containing all characters to
	// wide character representation:
	wxString str(name, wxConvLibc);
	wxCSConv conv(str);
	char mbBuf[2];
	wchar_t wcBuf[10];
	size_t i;

	mbBuf[1] = 0;
	info->map[0] = 0;
	for (i = 0; i < 255; i++)
	{
		mbBuf[0] = (char)(i+1);
		if (conv.MB2WC(wcBuf, mbBuf, 2) == (size_t)-1)
		{
			// invalid/undefined byte in the encoding:
			info->map[i+1] = -1;
		}
		info->map[i+1] = (int)wcBuf[0];
	}

	info->data = NULL;
	info->convert = NULL;
	info->release = NULL;

	return 1;
}
}

ParsingContext::ParsingContext()
	: encoding(wxT("UTF-8"))
{
	m_parser = XML_ParserCreate(NULL);
	XML_SetUserData(m_parser, (void*)this);
	XML_SetDefaultHandler(m_parser, DefaultHnd);
	XML_SetUnknownEncodingHandler(m_parser, UnknownEncodingHnd, NULL);
}

ParsingContext::~ParsingContext()
{
	XML_ParserFree(m_parser);
}

void ParsingContext::AppendTag(wxString &tag)
{
	m_path += wxT("/") + tag;
}

void ParsingContext::RemoveTag(wxString &tag)
{
	wxString str = m_path;
	do {
		int pos = str.Find(wxT('/'), true);
		if (pos == -1) break;
		if (str.Mid(pos + 1) == tag) {
			m_path = str.Left(pos);
			return;
		}
		str = str.Left(pos);
	} while (true);
}

void ParsingContext::Stop()
{
	XML_StopParser(m_parser, XML_FALSE);
}

//-----------------------------------------------------------------------------
//  XML_Parser loading routines
//-----------------------------------------------------------------------------

// converts Expat-produced string in UTF-8 into wxString using the specified
// conv or keep in UTF-8 if conv is NULL
wxString ParsingContext::CharToString(const char *s, size_t len)
{
	return wxString(s, wxConvUTF8, len);
}

wxString ParsingContext::CharToLower(const char *s, size_t len)
{
	wxString data = wxString(s, wxConvUTF8, len);
	data.MakeLower();
	data.Trim(false).Trim(true);
	return data;
}

// returns true if the given string contains only whitespaces
bool ParsingContext::IsWhiteOnly(const wxChar *buf)
{
	for (const wxChar *c = buf; *c != wxT('\0'); c++)
		if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r'))
			return false;
	return true;
}

