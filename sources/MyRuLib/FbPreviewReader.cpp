#include "FbPreviewReader.h"
#include "FbParsingCtx.h"
#include "FbViewThread.h"
#include "FbViewData.h"

//-----------------------------------------------------------------------------
//  FbPreviewReader
//-----------------------------------------------------------------------------

extern const wxUint16 encoding_table__CP1251[128];

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
	return ((FbPreviewReader*) context)->Read((char*)buffer, length);
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
    FAXPP_Error err = FAXPP_init_parse_callback(m_parser, ReadPreviewCallback, this);
    if (err != NO_ERROR) err = FAXPP_next_event(m_parser);
    while (err != NO_ERROR) {
    	const FAXPP_Event * event = FAXPP_get_current_event(m_parser);
    	if (ProcessEvent(event)) break;
    	err = FAXPP_next_event(m_parser);
    }
    if (err != NO_ERROR) wxLogError(FAXPP_err_to_string(err));
	return err == NO_ERROR;
}

bool FbPreviewReader::ProcessEvent(const FAXPP_Event * event)
{
	FAXPP_set_decode(m_parser, FAXPP_cp1251_decode);
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

