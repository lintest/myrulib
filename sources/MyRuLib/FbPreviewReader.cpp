#include "FbPreviewReader.h"
#include "FbParsingCtx.h"
#include "FbViewThread.h"
#include "FbViewData.h"

//-----------------------------------------------------------------------------
//  FbPreviewReader
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

static unsigned int ReadPreviewCallback(void * context, void * buffer, unsigned int length)
{
	return ((wxInputStream*)context)->Read((char*)buffer, length).LastRead();
}

FbPreviewReader::FbPreviewReader(wxInputStream & stream, FbViewThread & thread, FbViewData & data)
	: m_stream(stream), m_parser(NULL), m_thread(thread), m_data(data)
{
	m_parser = FAXPP_create_parser(NO_CHECKS_PARSE_MODE, FAXPP_utf8_transcoder);
	if (m_parser) FAXPP_set_normalize_attrs(m_parser, 1);
}

FbPreviewReader::~FbPreviewReader()
{
	if (m_parser) FAXPP_free_parser(m_parser);
}

int FbPreviewReader::Read(char * buffer, int len)
{
	return m_stream.Read(buffer, len).LastRead();
}

bool FbPreviewReader::Parse()
{
    FAXPP_Error err = FAXPP_init_parse_callback(m_parser, ReadPreviewCallback, &m_stream);
    if (err != NO_ERROR) err = FAXPP_next_event(m_parser);
    while (err != NO_ERROR) {
    	const FAXPP_Event * event = FAXPP_get_current_event(m_parser);
    	if (ProcessEvent(event)) break;
    	err = FAXPP_next_event(m_parser);
    }
    if (err != NO_ERROR) wxLogError(wxString(FAXPP_err_to_string(err), wxConvUTF8));
	return err == NO_ERROR;
}

bool FbPreviewReader::ProcessEvent(const FAXPP_Event * event)
{
	FAXPP_set_decode(m_parser, FAXPP_cp1251_decode);
	return true;
}

void FbPreviewReader::NewNode(const wxString &name, int level)
{
	m_context.Inc(name);
	switch (m_context.Section()) {
		case fbsDescr: {
			if (m_context > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (m_context == wxT("fictionbook/description/title-info/coverpage/image")) {
				AppendImg();
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << wxString::Format(wxT("<%s>"), name.c_str());
		} break;
		case fbsBinary: {
			StartImg();
		} break;
		case fbsNone: {
		} break;
	}
}

void FbPreviewReader::TxtNode(const wxString &name, const wxString &text)
{
	switch (m_context.Section()) {
		case fbsDescr: {
			if (m_context >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << text;
			} else if (m_context == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn << text;
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << text;
		} break;
		case fbsBinary: {
			if (m_saveimage) m_imagedata << text;
		} break;
		case fbsNone: {
		} break;
	}
}

void FbPreviewReader::EndNode(const wxString &name, int level)
{
	switch (m_context.Section()) {
		case fbsDescr: {
			if (m_context == wxT("fictionbook/description")) {
				m_parsebody = m_annt.IsEmpty();
				if (!m_parsebody) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count() == 0) Stop();
				}
				m_data.SetText(FbViewData::ISBN, m_isbn);
				m_thread.SendHTML(m_data);
			} else if (m_context > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("</%s>"), name.c_str());
			}
		} break;
		case fbsBody: {
			if (m_parsebody) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
				if (m_annt.Length() > 1000) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count()==0) Stop();
					m_thread.SendHTML(m_data);
					m_parsebody = false;
				}
			}
		} break;
		case fbsBinary: {
			if (m_saveimage) {
				m_data.AddImage(m_imagename, m_imagedata);
				m_thread.SendHTML(m_data);
			}
		} break;
		case fbsNone: {
		} break;
	}
	m_context.Dec(name);
}

void FbPreviewReader::AppendImg()
{
/*
	wxASSERT(xmlTextReaderMoveToFirstAttribute(m_reader) == 1);
	while (xmlTextReaderMoveToNextAttribute(m_reader) == 1) {
		wxString name = CharToLower(xmlTextReaderConstName(m_reader));
		if (name.Right(5) == wxT(":href")) {
			wxString value = CharToLower(xmlTextReaderConstValue(m_reader));
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_images.Add(value);
			break;
		}
	}
*/
}

void FbPreviewReader::StartImg()
{
/*
	m_saveimage = false;
	wxASSERT(xmlTextReaderMoveToFirstAttribute(m_reader) == 1);
	while (xmlTextReaderMoveToNextAttribute(m_reader) == 1) {
		wxString name = CharToLower(xmlTextReaderConstName(m_reader));
		if (name == wxT("id")) {
			wxString value = CharToLower(xmlTextReaderConstValue(m_reader));
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_saveimage = (m_images.Index(value) != wxNOT_FOUND);
			if (m_saveimage) m_imagename = value;
			break;
		}
	}
*/
}

