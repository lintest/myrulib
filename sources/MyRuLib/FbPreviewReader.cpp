#include "FbPreviewReader.h"
#include "FbViewThread.h"
#include "FbViewData.h"

#ifdef FB_PARSE_FAXPP

//-----------------------------------------------------------------------------
//  FbPreviewReaderFaxpp
//-----------------------------------------------------------------------------

void FbPreviewReaderFaxpp::NewNode(const FAXPP_Event & event, bool closed)
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
	if (closed) Dec(name);
}

void FbPreviewReaderFaxpp::TxtNode(const FAXPP_Event & event)
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

void FbPreviewReaderFaxpp::EndNode(const FAXPP_Event & event)
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

void FbPreviewReaderFaxpp::AppendImg(const FAXPP_Event & event)
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

void FbPreviewReaderFaxpp::StartImg(const FAXPP_Event & event)
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

#endif // FB_PARSE_FAXPP

#ifdef FB_PARSE_EXPAT

//-----------------------------------------------------------------------------
//  FbPreviewReaderExpat
//-----------------------------------------------------------------------------

void FbPreviewReaderExpat::NewNode(const XML_Char *tag, const XML_Char **atts)
{
	wxString name = Low(tag);
	Inc(name);
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description/title-info/coverpage/image")) {
				AppendImg(atts);
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn.Empty();
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << wxString::Format(wxT("<%s>"), name.c_str());
		} break;
		case fbsBinary: {
			StartImg(atts);
		} break;
		case fbsNone: {
		} break;
	}
}

void FbPreviewReaderExpat::TxtNode(const XML_Char *text, int len)
{
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << Str(text, len);
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn << Str(text, len);
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << Str(text, len);
		} break;
		case fbsBinary: {
			if (m_saveimage) m_imagedata << Str(text, len);
		} break;
		case fbsNone: {
		} break;
	}
}

void FbPreviewReaderExpat::EndNode(const XML_Char* tag)
{
	wxString name = Low(tag);
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

void FbPreviewReaderExpat::AppendImg(const XML_Char **atts)
{
	const XML_Char **a = atts;
	while (*a) {
		wxString name = Low(a[0]);
		if (name.Right(5) == wxT(":href")) {
			wxString value = Low(a[1]);
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_images.Add(value);
			break;
		}
	}
}

void FbPreviewReaderExpat::StartImg(const XML_Char **atts)
{
	const XML_Char **a = atts;
	while (*a) {
		wxString name = Low(a[0]);
		if (name == wxT("id")) {
			wxString value = Low(a[1]);
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

#endif // FB_PARSE_EXPAT
