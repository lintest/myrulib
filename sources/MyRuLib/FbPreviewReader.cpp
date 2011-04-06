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

bool FbPreviewReader::OnProcessEvent(const FAXPP_Event & event)
{
	switch (event.type) {
		case SELF_CLOSING_ELEMENT_EVENT:
		case START_ELEMENT_EVENT: {
			NewNode(event);
		} break;
		case END_ELEMENT_EVENT: {
			EndNode(event);
		} break;
		case CHARACTERS_EVENT: {
			TxtNode(event);
		} break;
	}
	return true;
}

void FbPreviewReader::NewNode(const FAXPP_Event & event)
{
	wxString name = Low(event.name);
	Inc(name);
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description/title-info/coverpage/image")) {
				AppendImg(event);
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn.Empty();
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << wxString::Format(wxT("<%s>"), name.c_str());
		} break;
		case fbsBinary: {
			StartImg(event);
		} break;
		case fbsNone: {
		} break;
	}
	if (event.type == SELF_CLOSING_ELEMENT_EVENT) Dec(name);
}

void FbPreviewReader::TxtNode(const FAXPP_Event & event)
{
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << Str(event.value);
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn << Str(event.value);
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << Str(event.value);
		} break;
		case fbsBinary: {
			if (m_saveimage) m_imagedata << Str(event.value);
		} break;
		case fbsNone: {
		} break;
	}
}

void FbPreviewReader::EndNode(const FAXPP_Event & event)
{
	wxString name = Low(event.name);
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("</%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description")) {
				m_parsebody = m_annt.IsEmpty();
				if (!m_parsebody) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count() == 0) Stop();
				}
				m_data.SetText(FbViewData::ISBN, m_isbn);
				m_thread.SendHTML(m_data);
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

void FbPreviewReader::AppendImg(const FAXPP_Event & event)
{
	for (unsigned int i = 0; i < event.attr_count; ++i) {
		wxString name = Low(event.attrs[i].name);
		if (name == wxT("href")) {
			wxString value = Low(event.attrs[i].value.value);
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_images.Add(value);
			break;
		}
	}
}

void FbPreviewReader::StartImg(const FAXPP_Event & event)
{
	for (unsigned int i = 0; i < event.attr_count; ++i) {
		wxString name = Low(event.attrs[i].name);
		if (name == wxT("id")) {
			wxString value = Low(event.attrs[i].value.value);
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_saveimage = (m_images.Index(value) != wxNOT_FOUND);
			if (m_saveimage) {
			    m_imagename = value;
			    m_imagedata.Empty();
			}
			break;
		}
	}
}

