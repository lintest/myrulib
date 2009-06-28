/***************************************************************
 * Name:      FbParser.cpp
 * Purpose:   FictionBook XML parser
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-06-03
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>
#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/zstream.h>
#include <wx/strconv.h>

#include "FbParser.h"

#define XML_STATIC

#include <expat.h> // from Expat

IMPLEMENT_CLASS(FbDocument, wxObject)

// a private utility used by wxXML
static bool wxIsWhiteOnly(const wxChar *buf);

//-----------------------------------------------------------------------------
//  FbProperty
//-----------------------------------------------------------------------------
class FbProperty {
public:
    FbProperty() : m_next(NULL) {};
    FbProperty(const wxString& name, const wxString& value)
            : m_name(name), m_value(value),  m_next(NULL) {};
    virtual ~FbProperty() {};
public:
    wxString m_name;
    wxString m_value;
    FbProperty *m_next;
};

//-----------------------------------------------------------------------------
//  FbNode
//-----------------------------------------------------------------------------
FbNode::FbNode(const wxString &name, FbNodeType type)
	: m_parent(NULL), m_child(NULL), m_next(NULL), m_properties(NULL)
{
	m_name = name;
	m_type = type;
	m_text = wxEmptyString;
}

FbNode::~FbNode()
{
	while (m_child) {
		FbNode * node = m_child;
		m_child = node->m_next;
		delete node;
	}
	while (m_properties) {
		FbProperty * prop = m_properties;
		m_properties = prop->m_next;
		delete prop;
	}
}

void FbNode::Append(FbNode * node)
{
	node->m_parent = this;
	if (m_child)
		m_last_child->m_next = node;
	else
		m_child = node;
	m_last_child = node;
}

#ifdef FB_DEBUG_PARSING
void FbNode::Print(wxString &text, int level)
{
	for (int i = 0; i<level; i++)
		text += wxT("&nbsp;&nbsp;");
	text += wxString::Format(wxT("&lt;%s"), m_name.c_str());

	FbProperty * prop = m_properties;
	while (prop) {
		text += wxString::Format(wxT("&nbsp;%s=%s"), prop->m_name.c_str(), prop->m_value.c_str());
		prop = prop->m_next;
	}

	text += wxString::Format(wxT("&gt;<b>%s</b>"), m_text.c_str());

	if (m_child)
		text += wxT("<br>");

	FbNode * node = m_child;
	while (node) {
		node->Print(text, level + 1);
		node = node->m_next;
	}

	if (m_child)
		for (int i = 0; i<level; i++)
			text += wxT("&nbsp;&nbsp;");

	text += wxString::Format(wxT("&lt;/%s&gt;<br>"), m_name.c_str());
}
#endif //FB_DEBUG_PARSING

FbNode * FbNode::Find(const wxString &name)
{
	FbNode * node = m_child;
	while (node)
		if (node->m_name == name)
			return node;
		else
			node = node->m_next;
	return NULL;
}

wxString FbNode::Prop(const wxString &name)
{
	FbProperty * node = m_properties;
	while (node)
		if (node->m_name == name)
			return node->m_value;
		else
			node = node->m_next;
	return wxEmptyString;
}

void FbNode::AddProperty(const wxString& name, const wxString& value)
{
    AddProperty(new FbProperty(name, value));
}

void FbNode::AddProperty(FbProperty *prop)
{
	if (m_properties)
		m_last_prop->m_next = prop;
	else
        m_properties = prop;
	m_last_prop = prop;
}

//-----------------------------------------------------------------------------
//  FbDocument
//-----------------------------------------------------------------------------

FbDocument::FbDocument()
    : m_version(wxT("1.0")), m_fileEncoding(wxT("utf-8")), m_root(NULL)
{
}

FbDocument::FbDocument(const wxString& filename)
              :wxObject(), m_root(NULL)
{
    if (!Load(filename)) wxDELETE(m_root);
}

FbDocument::FbDocument(wxInputStream& stream)
              :wxObject(), m_root(NULL)
{
    if (!Load(stream)) wxDELETE(m_root);
}

bool FbDocument::Load(const wxString& filename)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok()) return false;
    return Load(stream);
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


struct wxXmlParsingContext
{
    wxXmlParsingContext()
        : conv(NULL),
          root(NULL),
          node(NULL),
		  annotation(false)
    {}

    wxMBConv  *conv;
    FbNode *root; // root node of document
    FbNode *node; // the node being parsed
    wxString   encoding;
    wxString   version;
	bool annotation;
	XML_Parser parser;
};

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

    wxString node_name = CharToLower(ctx->conv, name);

	if (ctx->annotation) {
		ctx->node->m_text += wxString::Format(wxT("<%s"), node_name.c_str());
		const XML_Char **a = atts;
		while (*a) {
			ctx->node->m_text += wxString::Format(wxT(" %s=%s"), CharToLower(ctx->conv, a[0]).c_str(), CharToString(ctx->conv, a[1]).c_str());
			a += 2;
		}
		ctx->node->m_text += wxT(">");
	} else {
		ctx->annotation = node_name == wxT("annotation");

		FbNode *node = new FbNode(node_name);

		const XML_Char **a = atts;
		while (*a) {
			wxString name = CharToString(ctx->conv, a[0]).Trim(false).Trim(true);
			wxString text = CharToString(ctx->conv, a[1]).Trim(false).Trim(true);
			node->AddProperty(name, text);
			a += 2;
		}

		if (ctx->root == NULL)
			ctx->root = node;
		else
			ctx->node->Append(node);

		ctx->node = node;
	}
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;
    wxString node_name = CharToLower(ctx->conv, name);

	ctx->node->m_text.Trim(false).Trim(true);

	if (ctx->annotation)
		ctx->node->m_text += wxString::Format(wxT("</%s>"), node_name.c_str());
	else if (ctx->node->m_parent)
		ctx->node = ctx->node->m_parent;

	if (node_name == wxT("title-info"))
		XML_StopParser(ctx->parser, XML_FALSE);
	else if (node_name == wxT("annotation"))
		ctx->annotation = false;
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

    wxString str = CharToString(ctx->conv, s, len);

    if (!wxIsWhiteOnly(str))
        ctx->node->m_text += str;
}
}

extern "C" {
static void DefaultHnd(void *userData, const XML_Char *s, int len)
{
    // XML header:
    if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
    {
        wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

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

bool FbDocument::Load(wxInputStream& stream)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    wxXmlParsingContext ctx;
    bool done;
    XML_Parser parser = XML_ParserCreate(NULL);

	ctx.parser = parser;
    ctx.root = ctx.node = NULL;
    ctx.encoding = wxT("UTF-8"); // default in absence of encoding=""
    ctx.conv = NULL;

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

    if (ok) {
        if (!ctx.version.empty())
            SetVersion(ctx.version);
        if (!ctx.encoding.empty())
            SetFileEncoding(ctx.encoding);
        SetRoot(ctx.root);
    } else
        delete ctx.root;

    XML_ParserFree(parser);

    return ok;
}

wxString FbDocument::GetAuthor(FbNode *root) const
{
	FbNode * node = root->m_child;
	wxString first, middle, last;

    while (node) {
        wxString current_name = node->GetName();
        if (current_name == wxT("first-name"))
			first = node->m_text;
        else if (current_name == wxT("middle-name"))
            middle = node->m_text;
        else if (current_name == wxT("last-name"))
            last = node->m_text;
		node = node->m_next;
    }

    wxString result = last;
    if (!first.IsEmpty()) result += (wxT(" ") + first);
    if (!middle.IsEmpty()) result += (wxT(" ") + middle);
    return result.Trim(false).Trim(true);
}
