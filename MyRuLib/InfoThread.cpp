#include "InfoThread.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "BookInfo.h"
#include "ZipReader.h"

#define XML_STATIC
#include <expat.h> 

void InfoThread::Execute(const int id)
{
    if (!id) return;
	InfoThread *thread = new InfoThread(wxGetApp().GetTopWindow(), id);
    if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

InfoThread::InfoThread(wxEvtHandler *frame, const int id)
        : wxThread(), m_id(id), m_frame(frame)
{
}

void *InfoThread::Entry()
{
    ZipReader reader(m_id);
    if (!reader.IsOK()) return NULL;

	Load(reader.GetZip());

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbDocument loading routines
//-----------------------------------------------------------------------------

// converts Expat-produced string in UTF-8 into wxString using the specified
// conv or keep in UTF-8 if conv is NULL
static wxString CharToString(wxMBConv *conv, const char *s, size_t len = wxString::npos)
{
    wxUnusedVar(conv);
    return wxString(s, wxConvUTF8, len);
}

static wxString CharToLower(wxMBConv *conv, const char *s, size_t len = wxString::npos)
{
    wxUnusedVar(conv);
    wxString data = wxString(s, wxConvUTF8, len);
    data.MakeLower();

    return data;
}

// returns true if the given string contains only whitespaces
bool wxIsWhiteOnly(const wxChar *buf)
{
    for (const wxChar *c = buf; *c != wxT('\0'); c++)
        if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r'))
            return false;
    return true;
}

class InfoParsingContext
{
public:
	InfoParsingContext():conv(NULL), encoding(wxT("UTF-8")) {};
	wxString Path(size_t count = 0);
public:
    wxMBConv  *conv;
    wxString   encoding;
    wxString   version;
    wxString   annotation;
    wxString   imagedata;
	XML_Parser parser;
	wxArrayString tags;
	wxArrayString images;
    wxEvtHandler *m_frame;
    int m_id;
};

wxString InfoParsingContext::Path(size_t count) 
{
	size_t lastNum;
	wxString result;

	if (count>tags.Count())
		lastNum = tags.Count();
	else
		lastNum = (count ? count : tags.Count());

	for (size_t i=0; i<lastNum; i++)
		result += tags[i] + wxT("/");

	return result;
}

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

    wxString node_name = CharToLower(ctx->conv, name);

	if (ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
		ctx->annotation += wxString::Format(wxT("<%s"), node_name.c_str());
		const XML_Char **a = atts;
		while (*a) {
			ctx->annotation += wxString::Format(wxT(" %s=%s"), CharToLower(ctx->conv, a[0]).c_str(), CharToString(ctx->conv, a[1]).c_str());
			a += 2;
		}
		ctx->annotation += wxT(">");
	} else {
		wxString path = ctx->Path();
		if (path == wxT("fictionbook/description/title-info/coverpage/")) {
			if (node_name == wxT("image")) {
				const XML_Char **a = atts;
				while (*a) {
					wxString name = CharToLower(ctx->conv, a[0]).Trim(false).Trim(true);
					if (name == wxT("l:href")) {
						wxString value = CharToLower(ctx->conv, a[1]).Trim(false).Trim(true);
						ctx->images.Add(value);
					}
					a += 2;
				}
			}
		}
	}
	ctx->tags.Add(node_name);
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;
    wxString node_name = CharToLower(ctx->conv, name);

	if (ctx->tags.Count()>4 && ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
		ctx->annotation.Trim(false).Trim(true);
		ctx->annotation += wxString::Format(wxT("</%s>"), node_name.c_str());
	} else {
		wxString path = ctx->Path();
		if (path == wxT("fictionbook/description/title-info/")) {
			if (!ctx->images.Count()) XML_StopParser(ctx->parser, XML_FALSE);
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_SET_ANNOTATION );
			event.SetInt(ctx->m_id);
			event.SetString(ctx->annotation);
			wxPostEvent( ctx->m_frame, event );
			ctx->annotation = wxEmptyString;
		} else if (path == wxT("fictionbook/binary/")) {

		}
	}

	for (size_t i = ctx->tags.Count(); i>0 ; i--) {
		if ( ctx->tags[i-1] == node_name) {
			while (ctx->tags.Count()>=i) ctx->tags.RemoveAt(i-1);
			break;
		}
	}
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

	if (ctx->Path(4) == wxT("fictionbook/description/title-info/annotation/")) {
	    wxString str = CharToString(ctx->conv, s, len);
	    if (!wxIsWhiteOnly(str)) ctx->annotation += str;
	}
}
}

extern "C" {
static void DefaultHnd(void *userData, const XML_Char *s, int len)
{
    // XML header:
    if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
    {
        InfoParsingContext *ctx = (InfoParsingContext*)userData;

        wxString buf = CharToString(ctx->conv, s, (size_t)len);
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

bool InfoThread::Load(wxInputStream& stream)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    InfoParsingContext ctx;
    bool done;
    XML_Parser parser = XML_ParserCreate(NULL);

	ctx.parser = parser;
	ctx.m_frame = m_frame;
	ctx.m_id = m_id;

    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);
    XML_SetUnknownEncodingHandler(parser, UnknownEncodingHnd, NULL);

    bool ok = true;
    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

        if ( !XML_Parse(parser, buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(parser);
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(parser));
				ok = false;
	            break;
			}
        }
    } while (!done);

    XML_ParserFree(parser);

    return ok;
}

