#include "FbPreviewReader.h"
#include "FbParsingCtx.h"
#include "FbViewThread.h"
#include "FbViewData.h"

//-----------------------------------------------------------------------------
//  FbPreviewReader
//-----------------------------------------------------------------------------

FbPreviewReader::FbPreviewReader(FbViewThread & thread, FbViewData & data)
	: m_thread(thread), m_data(data)
{
}

FbPreviewReader::~FbPreviewReader()
{
}

bool FbPreviewReader::OnProcessEvent(const FAXPP_Event * event)
{
	return true;
}

void FbPreviewReader::NewNode(const wxString &name, int level)
{
	Inc(name);
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description/title-info/coverpage/image")) {
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
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << text;
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
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
	switch (Section()) {
		case fbsDescr: {
			if (*this == wxT("fictionbook/description")) {
				m_parsebody = m_annt.IsEmpty();
				if (!m_parsebody) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count() == 0) Stop();
				}
				m_data.SetText(FbViewData::ISBN, m_isbn);
				m_thread.SendHTML(m_data);
			} else if (*this > wxT("fictionbook/description/title-info/annotation")) {
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
	Dec(name);
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

