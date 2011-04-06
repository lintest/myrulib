#include "FbParsingCtx.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbParsingContext
//-----------------------------------------------------------------------------

static const wxUint16 encoding_table__CP1251[128] = {
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0000, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
 };

unsigned int FAXPP_cp1251_decode(const void *buffer, const void *buffer_end, Char32 *ch)
{
	uint8_t *buf = (uint8_t*)buffer;
	if (*buf < 0x80) {
		*ch = *buf;
	} else {
		unsigned char i = (unsigned char)(*buf) - 0x80;
		*ch = encoding_table__CP1251[i];
	}
	return 1;
}

static unsigned int ReadCallback(void * context, void * buffer, unsigned int length)
{
	return ((wxInputStream*)context)->Read((char*)buffer, length).LastRead();
}

wxString FbParsingContext::Str(const FAXPP_Text & text)
{
	return wxString((char*)text.ptr, wxConvUTF8, text.len);
}

wxString FbParsingContext::Low(const FAXPP_Text & text)
{
	wxString data = Str(text);
	data.MakeLower();
	data.Trim(false).Trim(true);
	return data;
}

// returns true if the given string contains only whitespaces
bool FbParsingContext::IsWhite(const FAXPP_Text & text)
{
	char * buffer = (char*) text.ptr;
	char * buffer_end = buffer + text.len;
	for (const char * c = buffer; c < buffer_end; c++) {
		if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r')) return false;
	}
	return true;
}

FbParsingContext::FbParsingContext()
	: m_section(fbsNone)
{
	m_parser = FAXPP_create_parser(NO_CHECKS_PARSE_MODE, FAXPP_utf8_transcoder);
	if (m_parser) FAXPP_set_normalize_attrs(m_parser, 1);
}

FbParsingContext::~FbParsingContext()
{
	if (m_parser) FAXPP_free_parser(m_parser);
}

bool FbParsingContext::Parse(wxInputStream & stream)
{
    FAXPP_Error err = FAXPP_init_parse_callback(m_parser, ReadCallback, &stream);
    if (err != NO_ERROR) err = FAXPP_next_event(m_parser);
    while (err == NO_ERROR) {
		const FAXPP_Event * event = FAXPP_get_current_event(m_parser);
		if (event->type == START_DOCUMENT_EVENT) {
			if (Low(event->encoding) == wxT("windows-1251")) {
				FAXPP_set_decode(m_parser, FAXPP_cp1251_decode);
			}
		} else if (event->type == END_DOCUMENT_EVENT) {
			break;
		} else if (!OnProcessEvent(*event)) {
			break;
		}
    	err = FAXPP_next_event(m_parser);
    }
	if (err != NO_ERROR) {
		wxString text(FAXPP_err_to_string(err), wxConvUTF8);
		unsigned int line = FAXPP_get_error_line (m_parser);		
		wxLogError(_("XML parsing error: '%s' at line %d"), text.c_str(), line);
	}
	return err == NO_ERROR;
}

void FbParsingContext::Inc(const wxString &tag)
{
	if (m_tags.Count() == 1) {
		if (tag == wxT("body")) {
			m_section = fbsBody;
		} else if (tag == wxT("description")) {
			m_section = fbsDescr;
		} else if (tag == wxT("binary")) {
			m_section = fbsBinary;
		} else {
			m_section = fbsNone;
		}
	}
	m_tags.Add(tag);
	m_name = tag;
}

void FbParsingContext::Dec(const wxString &tag)
{
	size_t count = m_tags.Count();
	size_t index = count;
	while (index > 0) {
		index--;
		if (m_tags[index] == tag) {
			m_tags.RemoveAt(index, count - index);
			break;
		}
	}
	if (m_tags.Count() == 1) m_section = fbsNone;
}

bool FbParsingContext::operator == (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}

	return (count == index);
}

bool FbParsingContext::operator >= (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}
	return (count >= index);
}

bool FbParsingContext::operator > (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}

	return (count > index);
}

wxString FbParsingContext::Path() const
{
	wxString result;
	size_t count = m_tags.Count();
	for (size_t i = 0; i < count; i++) {
		result << wxT("/") << m_tags[i];
	}
	return result;
}
