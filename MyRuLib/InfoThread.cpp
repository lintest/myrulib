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

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
	InfoThread * ctx = (InfoThread *)userData;

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
					wxString imagename = wxString::Format(wxT("%d/%s"), ctx->GetId(), value.c_str());
					ctx->images.Add(value);
				}
				a += 2;
			}
		}
	} else if (path == wxT("/fictionbook/description/title-info")) {
		ctx->isbn.Empty();
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
	InfoThread * ctx = (InfoThread *)userData;
	wxString node_name = ctx->CharToLower(name);

	wxString path = ctx->Path();
	if (path.StartsWith(wxT("/fictionbook/description/title-info/annotation"))) {
		ctx->annotation.Trim(false).Trim(true);
		ctx->annotation += wxString::Format(wxT("</%s>"), node_name.c_str());
	} else if (path == wxT("/fictionbook/description/title-info")) {
		InfoCash::SetAnnotation(ctx->GetId(), ctx->annotation);
		ctx->annotation.Empty();
	} else if (path == wxT("/fictionbook/description/publish-info/isbn")) {
		InfoCash::SetISBN(ctx->GetId(), ctx->isbn);
	} else if (path == wxT("/fictionbook/description/")) {
		if (!ctx->images.Count()) ctx->Stop();
		ctx->UpdateInfo();
	} else if (path == wxT("/fictionbook/binary")) {
		if (!ctx->skipimage) {
			InfoCash::AddImage(ctx->GetId(), ctx->imagename, ctx->imagedata, ctx->imagetype);
			ctx->UpdateInfo();
		}
	}
	ctx->RemoveTag(node_name);
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
	InfoThread * ctx = (InfoThread *)userData;

	wxString path = ctx->Path();
	if (path.StartsWith(wxT("/fictionbook/description/title-info/annotation"))) {
		wxString str = ctx->CharToString(s, len);
		if (!ParsingContext::IsWhiteOnly(str)) ctx->annotation += str;
	} else if (path == wxT("/fictionbook/binary")) {
		wxString str = ctx->CharToString(s, len);
		if (!ParsingContext::IsWhiteOnly(str)) ctx->imagedata += str;
	} else if (path == wxT("/fictionbook/description/publish-info/isbn")) {
		wxString str = ctx->CharToString(s, len);
		if (!ParsingContext::IsWhiteOnly(str)) ctx->isbn += str.Trim(true).Trim(false);
	}
}
}

bool InfoThread::Load(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	char buf[BUFSIZE];
	bool done;

	XML_SetElementHandler(GetParser(), StartElementHnd, EndElementHnd);
	XML_SetCharacterDataHandler(GetParser(), TextHnd);

	bool ok = true;
	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		done = (len < BUFSIZE);

		if ( !XML_Parse(GetParser(), buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(GetParser());
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(GetParser()));
				ok = false;
				break;
			}
		}
	} while (!done);

	return ok;
}

