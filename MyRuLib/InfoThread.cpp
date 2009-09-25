#include "InfoThread.h"
#include "ZipReader.h"
#include "InfoCash.h"
#include "ParseCtx.h"

wxCriticalSection InfoThread::sm_queue;

void *InfoThread::Entry()
{
    wxCriticalSectionLocker enter(sm_queue);

    ZipReader reader(m_id, false);
    if (!reader.IsOK()) return NULL;

	Load(reader.GetZip());

	return NULL;
}

void InfoThread::Execute(wxEvtHandler *frame, const int id)
{
    if (!id) return;
	InfoThread *thread = new InfoThread(frame, id);
    if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

class InfoParsingContext: public ParsingContext
{
public:
    wxString annotation;
    wxString imagedata;
    wxString imagetype;
    wxString imagename;
    bool skipimage;
	wxArrayString images;
public:
    wxEvtHandler *m_frame;
    int m_id;
};

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

    wxString node_name = ctx->CharToLower(name);

    wxString path = ctx->Path();
	if (path.StartsWith(wxT("/fictionbook/description/title-info/annotation"))) {
		ctx->annotation += wxString::Format(wxT("<%s"), node_name.c_str());
		const XML_Char **a = atts;
		while (*a) {
            wxString name = ctx->CharToLower(a[0]);
            wxString value = ctx->CharToLower(a[1]);
			ctx->annotation += wxString::Format(wxT(" %s=%s"), name.c_str(), value.c_str());
			a += 2;
		}
		ctx->annotation += wxT(">");
	} else if (path == wxT("/fictionbook/description/title-info/coverpage")) {
        if (node_name == wxT("image")) {
            const XML_Char **a = atts;
            while (*a) {
                wxString name = ctx->CharToLower(a[0]);
                wxString value = ctx->CharToLower(a[1]);
                if (name == wxT("l:href")) {
                    if (value.Left(1) == wxT("#")) value = value.Mid(1);
                    wxString imagename = wxString::Format(wxT("%d/%s"), ctx->m_id, value.c_str());
                    ctx->images.Add(value);
                }
                a += 2;
            }
        }
    } else if ((node_name == wxT("binary")) && path == wxT("/fictionbook")) {
        ctx->skipimage = true;
        ctx->imagedata.Empty();
        ctx->imagetype.Empty();
        ctx->imagename.Empty();
        const XML_Char **a = atts;
        while (*a) {
            wxString name = ctx->CharToLower(a[0]);
            wxString value = ctx->CharToLower(a[1]);
            if (name == wxT("id")) {
                ctx->skipimage = (ctx->images.Index(value) == wxNOT_FOUND);
                ctx->imagename = value;
            } else if (name == wxT("content-type")) {
                ctx->imagetype = value;
            }
            a += 2;
        }
    }
	ctx->AppendTag(node_name);
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;
    wxString node_name = ctx->CharToLower(name);

    wxString path = ctx->Path();
	if (path.StartsWith(wxT("/fictionbook/description/title-info/annotation"))) {
		ctx->annotation.Trim(false).Trim(true);
		ctx->annotation += wxString::Format(wxT("</%s>"), node_name.c_str());
	} else if (path == wxT("/fictionbook/description/title-info")) {
        InfoCash::SetAnnotation(ctx->m_id, ctx->annotation);
        if (!ctx->images.Count()) ctx->Stop();
        ctx->annotation.Empty();
		ShowThread::Execute(ctx->m_frame, ctx->m_id);
    } else if (path == wxT("/fictionbook/binary")) {
        if (!ctx->skipimage) {
            InfoCash::AddImage(ctx->m_id, ctx->imagename, ctx->imagedata, ctx->imagetype);
            ctx->annotation.Empty();
			ShowThread::Execute(ctx->m_frame, ctx->m_id);
        }
    }
	ctx->RemoveTag(node_name);
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
    InfoParsingContext *ctx = (InfoParsingContext*)userData;

    wxString path = ctx->Path();
	if (path.StartsWith(wxT("/fictionbook/description/title-info/annotation"))) {
	    wxString str = ctx->CharToString(s, len);
	    if (!ParsingContext::IsWhiteOnly(str)) ctx->annotation += str;
	} else if (path == wxT("/fictionbook/binary")) {
	    wxString str = ctx->CharToString(s, len);
	    if (!ParsingContext::IsWhiteOnly(str)) ctx->imagedata += str;
	}
}
}

bool InfoThread::Load(wxInputStream& stream)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    bool done;

    InfoParsingContext ctx;

	ctx.m_frame = m_frame;
	ctx.m_id = m_id;

    XML_SetElementHandler(ctx.GetParser(), StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(ctx.GetParser(), TextHnd);

    bool ok = true;
    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

        if ( !XML_Parse(ctx.GetParser(), buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(ctx.GetParser());
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(ctx.GetParser()));
				ok = false;
	            break;
			}
        }
    } while (!done);

    return ok;
}

