#include "FbParsingCtx.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbMemoryInputStream
//-----------------------------------------------------------------------------

FbMemoryInputStream::FbMemoryInputStream(wxInputStream & in, size_t size)
	: wxMemoryBuffer(size), wxMemoryInputStream(wxMemoryBuffer::GetData(), size)
{
	in.Read(GetWriteBuf(size), size);
}

FbMemoryInputStream::FbMemoryInputStream(wxInputStream * in, size_t size)
	: wxMemoryBuffer(size), wxMemoryInputStream(wxMemoryBuffer::GetData(), size)
{
	in->Read(GetWriteBuf(size), size);
	delete in;
}

//-----------------------------------------------------------------------------
//  FbHandlerXML
//-----------------------------------------------------------------------------

FbHandlerXML::~FbHandlerXML()
{
    wxDELETE(m_handler);
}

void FbHandlerXML::OnNewNode(const wxString &name, const FbStringHash &atts)
{
    if (m_handler) return m_handler->OnNewNode(name, atts);
	m_handler = NewNode(name, atts);
	if (!m_handler) m_handler = new FbHandlerXML(name);
}

void FbHandlerXML::OnTxtNode(const wxString &text)
{
	if (m_handler) m_handler->OnTxtNode(text); else TxtNode(text);
}

void FbHandlerXML::OnEndNode(const wxString &name, bool &exists)
{
    if (m_handler) {
    	bool found = exists || name == m_name;
        m_handler->OnEndNode(name, found);
		if (m_handler->m_closed) wxDELETE(m_handler);
		if (found) { exists = true; return; }
    }
    bool found = name == m_name;
	m_closed = found || exists;
	if (m_closed) EndNode(m_name);
	exists = found;
}

wxString FbHandlerXML::Value(const FbStringHash &atts, const wxString &name)
{
	FbStringHash::const_iterator i = atts.find(name);
	return i == atts.end() ? wxString() : i->second;
}

//-----------------------------------------------------------------------------
//  FbParserXML
//-----------------------------------------------------------------------------

static wxString Local(const wxString &name)
{
	return wxString(name).AfterLast(wxT(':')).Lower();
}

bool FbParserXML::Parse(wxInputStream & stream, bool md5calc)
{
	m_md5calc = md5calc;
	if (m_md5calc) md5_starts( &m_md5cont );

	bool ok = DoParse(stream);

	if (ok && m_md5calc) {
		m_md5sum.Empty();
		unsigned char output[16];
		md5_finish( &m_md5cont, output );
		for (size_t i = 0; i < 16; i++) {
			m_md5sum << wxString::Format(wxT("%02x"), output[i]);
		}
	}

	return ok;
}

void FbParserXML::OnNewNode(const wxString &name, const FbStringHash &atts)
{
	if (m_handler) return m_handler->OnNewNode(name, atts);
	m_handler = CreateHandler(name.Lower());
}

void FbParserXML::OnTxtNode(const wxString &text)
{
	if (m_handler) m_handler->OnTxtNode(text);
}

void FbParserXML::OnEndNode(const wxString &name)
{
	bool exists = false;
	if (m_handler) m_handler->OnEndNode(name, exists);
}

#ifdef FB_PARSE_FAXPP

//-----------------------------------------------------------------------------
//  FbParsingContextFaxpp
//-----------------------------------------------------------------------------

#include "wx/unictabl.inc"

#define FAXPP_define_decode(name, table)                                      \
unsigned int name(const void *buffer, const void *buffer_end, Char32 *ch) {   \
  uint8_t *buf = (uint8_t*)buffer;                                            \
    if (*buf < 0x80) {                                                        \
      *ch = *buf;                                                             \
    } else {                                                                  \
      unsigned char i = (unsigned char)(*buf) - 0x80;                         \
      *ch = table[i];                                                         \
    }                                                                         \
  return 1;                                                                   \
}

FAXPP_define_decode(FAXPP_ISO8859_1_decode, encoding_table__ISO8859_1);
FAXPP_define_decode(FAXPP_ISO8859_2_decode, encoding_table__ISO8859_2);
FAXPP_define_decode(FAXPP_ISO8859_3_decode, encoding_table__ISO8859_3);
FAXPP_define_decode(FAXPP_ISO8859_4_decode, encoding_table__ISO8859_4);
FAXPP_define_decode(FAXPP_ISO8859_5_decode, encoding_table__ISO8859_5);
FAXPP_define_decode(FAXPP_ISO8859_6_decode, encoding_table__ISO8859_6);
FAXPP_define_decode(FAXPP_ISO8859_7_decode, encoding_table__ISO8859_7);
FAXPP_define_decode(FAXPP_ISO8859_8_decode, encoding_table__ISO8859_8);
FAXPP_define_decode(FAXPP_ISO8859_9_decode, encoding_table__ISO8859_9);
FAXPP_define_decode(FAXPP_ISO8859_10_decode, encoding_table__ISO8859_10);
FAXPP_define_decode(FAXPP_ISO8859_13_decode, encoding_table__ISO8859_13);
FAXPP_define_decode(FAXPP_ISO8859_14_decode, encoding_table__ISO8859_14);
FAXPP_define_decode(FAXPP_ISO8859_15_decode, encoding_table__ISO8859_15);

FAXPP_define_decode(FAXPP_cp1251_decode, encoding_table__CP1251);
FAXPP_define_decode(FAXPP_cp1252_decode, encoding_table__CP1252);
FAXPP_define_decode(FAXPP_cp1253_decode, encoding_table__CP1253);
FAXPP_define_decode(FAXPP_cp1254_decode, encoding_table__CP1254);
FAXPP_define_decode(FAXPP_cp1255_decode, encoding_table__CP1255);
FAXPP_define_decode(FAXPP_cp1256_decode, encoding_table__CP1256);
FAXPP_define_decode(FAXPP_cp1257_decode, encoding_table__CP1257);
FAXPP_define_decode(FAXPP_cp1250_decode, encoding_table__CP1250);
FAXPP_define_decode(FAXPP_koi8r_decode , encoding_table__KOI8  );
FAXPP_define_decode(FAXPP_koi8u_decode , encoding_table__KOI8_U);

class FbFaxppStreamReader {
	public:
		FbFaxppStreamReader(wxInputStream & stream, FbParsingContextFaxpp & context)
			: m_stream(stream), m_md5cont(context.m_md5cont), m_md5calc(context.m_md5calc) {}
		unsigned int Read(void * buffer, unsigned int length);
	private:
		wxInputStream & m_stream;
		md5_context & m_md5cont;
		bool m_md5calc;
};

unsigned int FbFaxppStreamReader::Read(void * buffer, unsigned int length)
{
	size_t count = m_stream.Read((char*)buffer, length).LastRead();
	if (m_md5calc) md5_update(&m_md5cont, (unsigned char*)buffer, (int)count);
	return count;
}

static unsigned int ReadCallback(void * context, void * buffer, unsigned int length)
{
	return ((FbFaxppStreamReader*)context)->Read(buffer, length);
}

wxString FbParsingContextFaxpp::Str(const FAXPP_Text & text)
{
	return wxString((char*)text.ptr, wxConvUTF8, text.len);
}

wxString FbParsingContextFaxpp::Low(const FAXPP_Text & text)
{
	wxString data = Str(text);
	data.MakeLower();
	data.Trim(false).Trim(true);
	return data;
}

FbParsingContextFaxpp::FbParsingContextFaxpp()
	: m_stop(false)
{
	m_parser = FAXPP_create_parser(NO_CHECKS_PARSE_MODE, FAXPP_utf8_transcoder);
	if (m_parser) FAXPP_set_normalize_attrs(m_parser, 1);
}

FbParsingContextFaxpp::~FbParsingContextFaxpp()
{
	if (m_parser) FAXPP_free_parser(m_parser);
}

FAXPP_DecodeFunction FbParsingContextFaxpp::StrToDecode(const wxString & encoding)
{
	if (encoding.IsEmpty()) return FAXPP_utf8_decode;

	FAXPP_DecodeFunction decode = FAXPP_string_to_decode(encoding.mb_str());
	if (decode) return decode;

	if (encoding == wxT("windows-1251")) return FAXPP_cp1251_decode;
	if (encoding == wxT("windows-1252")) return FAXPP_cp1252_decode;
	if (encoding == wxT("windows-1253")) return FAXPP_cp1253_decode;
	if (encoding == wxT("windows-1254")) return FAXPP_cp1254_decode;
	if (encoding == wxT("windows-1255")) return FAXPP_cp1255_decode;
	if (encoding == wxT("windows-1256")) return FAXPP_cp1256_decode;
	if (encoding == wxT("windows-1257")) return FAXPP_cp1257_decode;
	if (encoding == wxT("windows-1250")) return FAXPP_cp1250_decode;
	if (encoding == wxT("koi8")) return FAXPP_koi8r_decode;
	if (encoding == wxT("koi8-r")) return FAXPP_koi8r_decode;
	if (encoding == wxT("koi8-u")) return FAXPP_koi8u_decode;

	if (encoding == wxT("iso-8859-1")) return FAXPP_ISO8859_1_decode;
	if (encoding == wxT("iso-8859-2")) return FAXPP_ISO8859_2_decode;
	if (encoding == wxT("iso-8859-3")) return FAXPP_ISO8859_3_decode;
	if (encoding == wxT("iso-8859-4")) return FAXPP_ISO8859_4_decode;
	if (encoding == wxT("iso-8859-5")) return FAXPP_ISO8859_5_decode;
	if (encoding == wxT("iso-8859-6")) return FAXPP_ISO8859_6_decode;
	if (encoding == wxT("iso-8859-7")) return FAXPP_ISO8859_7_decode;
	if (encoding == wxT("iso-8859-8")) return FAXPP_ISO8859_8_decode;
	if (encoding == wxT("iso-8859-9")) return FAXPP_ISO8859_9_decode;
	if (encoding == wxT("iso-8859-10")) return FAXPP_ISO8859_10_decode;
	if (encoding == wxT("iso-8859-13")) return FAXPP_ISO8859_13_decode;
	if (encoding == wxT("iso-8859-14")) return FAXPP_ISO8859_14_decode;
	if (encoding == wxT("iso-8859-15")) return FAXPP_ISO8859_15_decode;

	wxLogError(_("Unknown encoding: %s"), encoding.c_str());
	return FAXPP_utf8_decode;
}

bool FbParsingContextFaxpp::DoParse(wxInputStream & stream)
{
	FbFaxppStreamReader reader(stream, *this);
	FAXPP_Error err = FAXPP_init_parse_callback(m_parser, ReadCallback, &reader);
	if (err != NO_ERROR) err = FAXPP_next_event(m_parser);
	while (err == NO_ERROR) {
		const FAXPP_Event * event = FAXPP_get_current_event(m_parser);
		switch (event->type) {
			case START_DOCUMENT_EVENT: {
				wxString encoding = Low(event->encoding);
				FAXPP_DecodeFunction decode = StrToDecode(encoding);
				if (decode) FAXPP_set_decode(m_parser, decode);
			} break;
			case END_DOCUMENT_EVENT: {
				Stop();
			} break;
			default: {
				OnProcessEvent(*event);
			}
		}
		if (m_stop) break;
		err = FAXPP_next_event(m_parser);
	}
	if (m_md5calc) {
		const size_t BUFSIZE = 1024;
		unsigned char buf[BUFSIZE];
		bool eof = false;
		do {
			size_t len = stream.Read(buf, BUFSIZE).LastRead();
			md5_update( &m_md5cont, buf, (int) len );
			eof = (len < BUFSIZE);
		} while (!eof);
	}
	if (err != NO_ERROR) {
		wxString text(FAXPP_err_to_string(err), wxConvUTF8);
		unsigned int line = FAXPP_get_error_line (m_parser);
		OnError(wxLOG_Error, text, line);
	}
	return err == NO_ERROR;
}

void FbParsingContextFaxpp::OnProcessEvent(const FAXPP_Event & event)
{
	switch (event.type) {
		case SELF_CLOSING_ELEMENT_EVENT: {
			wxString name = Local(Str(event.name));
			FbStringHash hash;
			GetAtts(event, hash);
			OnNewNode(name, hash);
			OnEndNode(name);
		} break;
		case START_ELEMENT_EVENT: {
			wxString name = Local(Str(event.name));
			FbStringHash hash;
			GetAtts(event, hash);
			OnNewNode(name, hash);
		} break;
		case END_ELEMENT_EVENT: {
			wxString name = Local(Str(event.name));
			OnEndNode(name);
		} break;
		case CHARACTERS_EVENT: {
			OnTxtNode(Str(event.value));
		} break;
		default: ;
	}
}

void FbParsingContextFaxpp::GetAtts(const FAXPP_Event & event, FbStringHash &hash)
{
	for (unsigned int i = 0; i < event.attr_count; ++i) {
		wxString name = Low(event.attrs[i].name);
		hash[name] = Str(event.attrs[i].value.value);
	}
}

#endif // FB_PARSE_FAXPP

#ifdef FB_PARSE_EXPAT

//-----------------------------------------------------------------------------
//  FbParsingContextExpat
//-----------------------------------------------------------------------------

class FbExpatEventMaker {
	public:
		FbExpatEventMaker(void * data)
			: m_context((FbParsingContextExpat*)data) {}
		void OnNewNode(const wxString &name, const FbStringHash &atts)
			{ m_context->OnNewNode(Local(name), atts); }
		void OnTxtNode(const wxString &text)
			{ m_context->OnTxtNode(text); }
		void OnEndNode(const wxString &name)
			{ m_context->OnEndNode(Local(name)); }
	private:
		FbParsingContextExpat * m_context;
};

static wxString Str(const XML_Char *s, size_t len = wxString::npos)
{
	return wxString(s, wxConvUTF8, len);
}

static bool IsWhiteOnly(const wxChar *buf)
{
	for (const wxChar *c = buf; *c != wxT('\0'); c++)
		if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r'))
			return false;
	return true;
}

extern "C" {
static void DefaultHnd(void *userData, const XML_Char *s, int len)
{
	// XML header:
	if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
	{
		FbParsingContextExpat *ctx = (FbParsingContextExpat*)userData;
		wxString buf = Str(s, (size_t)len);
		int pos;
		pos = buf.Find(wxT("encoding="));
		if (pos != wxNOT_FOUND)
			ctx->encoding = buf.Mid(pos + 10).BeforeFirst(buf[(size_t)pos+9]);
		pos = buf.Find(wxT("version="));
		if (pos != wxNOT_FOUND)
			ctx->version = buf.Mid(pos + 9).BeforeFirst(buf[(size_t)pos+8]);
	}
}

static int UnknownEncodingHnd(void * WXUNUSED(encodingHandlerData), const XML_Char *name, XML_Encoding *info)
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
} // extern "C"

static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
	FbStringHash hash;
	FbParsingContextExpat::GetAtts(atts, hash);
	wxString code = Local(Str(name));
	FbExpatEventMaker(userData).OnNewNode(code, hash);
}

static void TextHnd(void *userData, const XML_Char *text, int len)
{
	wxString str = Str(text, len);
	if (!IsWhiteOnly(str)) FbExpatEventMaker(userData).OnTxtNode(str);
}

static void EndElementHnd(void *userData, const XML_Char* name)
{
	wxString code = Local(Str(name));
	FbExpatEventMaker(userData).OnEndNode(code);
}

FbParsingContextExpat::FbParsingContextExpat()
{
	m_parser = XML_ParserCreate(NULL);
	if (m_parser) {
		XML_SetUserData(m_parser, (void*)this);
		XML_SetDefaultHandler(m_parser, DefaultHnd);
		XML_SetUnknownEncodingHandler(m_parser, UnknownEncodingHnd, NULL);
		XML_SetElementHandler(m_parser, StartElementHnd, EndElementHnd);
		XML_SetCharacterDataHandler(m_parser, TextHnd);
	}
}

FbParsingContextExpat::~FbParsingContextExpat()
{
	XML_ParserFree(m_parser);
}

void FbParsingContextExpat::Stop()
{
	XML_StopParser(m_parser, XML_FALSE);
}

bool FbParsingContextExpat::DoParse(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	bool ok = true;
	bool parse = true;
	bool eof;

	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		if (m_md5calc) md5_update( &m_md5cont, buf, (int) len );
		eof = (len < BUFSIZE);

		if ( parse && !XML_Parse(m_parser, (char*)buf, len, eof) ) {
			XML_Error error_code = XML_GetErrorCode(m_parser);
			if ( error_code != XML_ERROR_ABORTED ) {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				XML_Size line = XML_GetCurrentLineNumber(m_parser);
				OnError(wxLOG_Error, error, line);
				ok = false;
			}
			parse = false;
		}
	} while (!eof && (parse || m_md5calc));

	return ok;
}

void FbParsingContextExpat::GetAtts(const XML_Char **atts, FbStringHash &hash)
{
	const XML_Char **a = atts;
	while (*a) {
		hash[Local(Str(a[0]))] = Str(a[1]);
		a += 2;
	}
}

#endif // FB_PARSE_EXPAT

#ifdef FB_PARSE_LIBXML2

//-----------------------------------------------------------------------------
//  FbParsingContextLibxml2
//-----------------------------------------------------------------------------

//! Converts from wxStrings to xmlChars.
//! Libxml2 takes sequences of xmlChar (which is defined to be *always* unsigned char)
//! which are asupposed to be always in UTF8: thus WX2XML converts wxStrings to UTF8.
#define WX2XML(str)        ((xmlChar *)(str.mb_str(wxConvUTF8)))

//! Converts from xmlChars to wxStrings.
//! Libxml2 always outputs a sequence of xmlChar which are encoded in UTF8:
//! this macro creates a wxString which converts the given string from UTF8
//! to the format internally used by wxString (whatever it is).
#define XML2WX(str)        (wxString((const char *)str, wxConvUTF8))

static int InputReadXML(void * context, char * buffer, int len)
{
	return ((FbParsingContextLibxml2*)context)->Read(buffer, len);
}

static int InputCloseXML(void * context)
{
	return 0;
}

FbParsingContextLibxml2::FbParsingContextLibxml2()
	: m_stream(NULL), m_stop(false)
{
}

FbParsingContextLibxml2::~FbParsingContextLibxml2()
{
}

int FbParsingContextLibxml2::Read(void * buffer, int length)
{
	size_t count = m_stream->Read((char*)buffer, length).LastRead();
	if (m_md5calc) md5_update(&m_md5cont, (unsigned char*)buffer, (int)count);
	return count;
}

wxString FbParsingContextLibxml2::Low(const xmlChar * text)
{
	wxString data = XML2WX(text);
	data.MakeLower();
	data.Trim(false).Trim(true);
	return data;
}

void FbParsingContextLibxml2::ProcessNode(xmlTextReaderPtr & reader)
{
	switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_ELEMENT: {
			wxString name = Low(xmlTextReaderConstLocalName(reader));
			bool empty = xmlTextReaderIsEmptyElement(reader);

			FbStringHash hash;
			while (xmlTextReaderMoveToNextAttribute(reader)) {
				wxString name = Low(xmlTextReaderConstLocalName(reader));
				wxString value = XML2WX(xmlTextReaderConstValue(reader));
				hash[name] = value;
			}

			OnNewNode(name, hash);
			if (empty) OnEndNode(name);
		} break;

		case XML_READER_TYPE_TEXT: {
			wxString value = XML2WX(xmlTextReaderConstValue(reader));
			OnTxtNode(value);
		} break;

		case XML_READER_TYPE_END_ELEMENT: {
			wxString name = Low(xmlTextReaderConstLocalName(reader));
			OnEndNode(name);
		} break;
	}
}

static void Fb2TextReaderErrorFunc(void * arg, const char * msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
{
	wxLogLevel level = wxLOG_Info;
	switch (severity) {
		case XML_PARSER_SEVERITY_VALIDITY_WARNING: level = wxLOG_Warning;
		case XML_PARSER_SEVERITY_VALIDITY_ERROR: level = wxLOG_Error;
		case XML_PARSER_SEVERITY_WARNING: level = wxLOG_Warning;
		case XML_PARSER_SEVERITY_ERROR: level = wxLOG_Error;
	}
	int line = xmlTextReaderLocatorLineNumber(locator);
	wxString err(msg, wxConvUTF8);
	err.Replace(wxT("\n"), wxT(" "));
	err.Replace(wxT("\r"), wxT(" "));
	((FbParsingContextLibxml2*)arg)->OnError(level, err, line);
}

/////////////////////////////////////////////////
//
//  How to use error handler:
//    http://adobkin.com/?p=956
//
/////////////////////////////////////////////////

bool FbParsingContextLibxml2::DoParse(wxInputStream & stream)
{
	m_stream = &stream;

	int options = XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET;
	xmlTextReaderPtr reader = xmlReaderForIO(InputReadXML, InputCloseXML, this, NULL, NULL, options);
	if (reader) xmlTextReaderSetErrorHandler(reader, Fb2TextReaderErrorFunc, this);

    int ret = 1;
	if (reader) {
        while ((ret = xmlTextReaderRead(reader)) == 1) {
            ProcessNode(reader);
        	if (m_stop) { ret = 0; break; }
        }
        xmlFreeTextReader(reader);
	}

	if (m_md5calc) {
		const size_t BUFSIZE = 1024;
		unsigned char buf[BUFSIZE];
		bool eof = false;
		do {
			size_t len = stream.Read(buf, BUFSIZE).LastRead();
			md5_update( &m_md5cont, buf, (int) len );
			eof = (len < BUFSIZE);
		} while (!eof);
	}

	return ret == 0;
}

#endif // FB_PARSE_LIBXML2
