#include "InfoThread.h"
#include "ZipReader.h"
#include "InfoCash.h"
#include "ParseCtx.h"

wxCriticalSection InfoThread::sm_queue;

void *InfoThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	ZipReader reader(m_id, false, true);
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
		ctx->StartTag(node_name, atts);
	} else if (ctx->parsebody && path.StartsWith(wxT("/fictionbook/body"))) {
		ctx->StartTag(node_name, atts);
	} else if (path == wxT("/fictionbook/description/title-info")) {
		ctx->isbn.Empty();
	} else if ((node_name == wxT("image")) && path == wxT("/fictionbook/description/title-info/coverpage")) {
		ctx->AppendImg(atts);
	} else if ((node_name == wxT("binary")) && path == wxT("/fictionbook")) {
		ctx->StartImg(atts);
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
		ctx->FinishTag(node_name);
	} else if (ctx->parsebody && path.StartsWith(wxT("/fictionbook/body"))) {
		ctx->FinishTag(node_name);
	} else if (path == wxT("/fictionbook/description/title-info")) {
		InfoCash::SetAnnotation(ctx->GetId(), ctx->annotation);
	} else if (path == wxT("/fictionbook/description/publish-info/isbn")) {
		InfoCash::SetISBN(ctx->GetId(), ctx->isbn);
	} else if (path == wxT("/fictionbook/description")) {
		ctx->parsebody = ctx->annotation.IsEmpty();
		if (ctx->images.Count()==0 && !ctx->parsebody) ctx->Stop();
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
		ctx->WriteText(s, len);
	} else if (ctx->parsebody && path.StartsWith(wxT("/fictionbook/body"))) {
		ctx->WriteText(s, len);
		ctx->CheckLength();
	} else if (path == wxT("/fictionbook/binary")) {
		ctx->WriteImg(s, len);
	} else if (path == wxT("/fictionbook/description/publish-info/isbn")) {
		wxString str = ctx->CharToString(s, len);
		if (!ParsingContext::IsWhiteOnly(str)) ctx->isbn += str.Trim(true).Trim(false);
	}
}
}

void InfoThread::WriteText(const XML_Char *s, int len)
{
	wxString str = CharToString(s, len);
	if (!ParsingContext::IsWhiteOnly(str)) annotation += str;
}

void InfoThread::CheckLength()
{
	if (annotation.Length()>1024) {
		InfoCash::SetAnnotation(GetId(), annotation + wxT("…"));
		if (!images.Count()) Stop();
		parsebody = false;
		UpdateInfo();
	}
}

void InfoThread::WriteImg(const XML_Char *s, int len)
{
	if (skipimage) return ;
	wxString str = CharToString(s, len);
	if (!ParsingContext::IsWhiteOnly(str)) imagedata += str;
}

void InfoThread::StartTag(wxString &name, const XML_Char **atts)
{
	wxString result = wxString::Format(wxT("<%s"), name.c_str());

	const XML_Char **a = atts;
	while (*a) {
		wxString name = CharToLower(a[0]);
		wxString value = CharToLower(a[1]);
		result += wxString::Format(wxT(" %s=%s"), name.c_str(), value.c_str());
		a += 2;
	}
	result += wxT(">");

	annotation += result;
}

void InfoThread::FinishTag(wxString &name)
{
	annotation += wxString::Format(wxT("</%s>"), name.c_str());
}

void InfoThread::AppendImg(const XML_Char **atts)
{
	const XML_Char **a = atts;
	while (*a) {
		wxString name = CharToLower(a[0]);
		wxString value = CharToLower(a[1]);
		if (name.Right(5) == wxT(":href")) {
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			wxString imagename = wxString::Format(wxT("%d/%s"), GetId(), value.c_str());
			images.Add(value);
		}
		a += 2;
	}
}

void InfoThread::StartImg(const XML_Char **atts)
{
	skipimage = true;
	imagedata.Empty();
	imagetype.Empty();
	imagename.Empty();
	const XML_Char **a = atts;
	while (*a) {
		wxString name = CharToLower(a[0]);
		wxString value = CharToLower(a[1]);
		if (name == wxT("id")) {
			skipimage = (images.Index(value) == wxNOT_FOUND);
			imagename = value;
		} else if (name == wxT("content-type")) {
			imagetype = value;
		}
		a += 2;
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

