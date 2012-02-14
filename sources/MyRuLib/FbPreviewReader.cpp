#include "FbPreviewReader.h"
#include "FbViewThread.h"
#include "FbViewData.h"

//-----------------------------------------------------------------------------
//  FbPreviewReader::RootHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReader::RootHandler)
	KEY( "description" , Descr  );
	KEY( "binary"      , Binary );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReader::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Descr  : return new DescrHandler(*this, name);
		case Binary : return NewImage(name, atts);
		default: return NULL;
	}
}

void FbPreviewReader::RootHandler::AppendImg(const FbStringHash &atts)
{
	FbStringHash::const_iterator it;
	for( it = atts.begin(); it != atts.end(); ++it ) {
		wxString name = it->first;
		if (name.AfterLast(wxT(':')) == wxT("href")) {
			wxString value = it->second;
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_images.Add(value);
			break;
		}
	}
}

FbHandlerXML * FbPreviewReader::RootHandler::NewImage(const wxString &name, const FbStringHash &atts)
{
	wxString file = Value(atts, wxT("id"));
	int index = m_images.Index(file);
	if (index == wxNOT_FOUND) return NULL;
	m_images.RemoveAt(index);
	return new ImageHandler(*this, name, file);
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::DescrHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReader::DescrHandler)
	KEY( "title-info"   , Title   );
	KEY( "publish-info" , Publish );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReader::DescrHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Title   : return new TitleHandler(m_root, name);
		case Publish : return new PublishHandler(m_root, name);
		default: return NULL;
	}
}

void FbPreviewReader::DescrHandler::EndNode(const wxString &name)
{
	m_root.SendDescr();
}

void FbPreviewReader::RootHandler::SendDescr()
{
	m_reader.m_data.SetText(FbViewData::ANNT, m_annt);
	if (!m_annt.IsEmpty() && m_images.Count() == 0) m_reader.Stop();
	m_reader.m_data.SetText(FbViewData::ISBN, m_isbn);
	m_reader.m_thread.SendHTML(m_reader.m_data);
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::TitleHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReader::TitleHandler)
	KEY( "annotation" , Annot );
	KEY( "coverpage"  , Cover );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReader::TitleHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Annot: return new AnnotHandler(m_root, name, m_root.m_annt);
		case Cover: return new CoverHandler(m_root, name);
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::PublishHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::PublishHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	return name == wxT("isbn") ? new TextHandler(name, m_root.m_isbn) : NULL;
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::AnnotHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::AnnotHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT("p")) {
		m_text << wxT('<') << name << wxT('>');
	}
	return new AnnotHandler(*this, name);
}

void FbPreviewReader::AnnotHandler::EndNode(const wxString &name)
{
	if (name == wxT("p")) {
		m_text << wxT('<') << wxT('/') << name << wxT('>');
	}
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::CoverHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::CoverHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT("image")) m_root.AppendImg(atts);
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::ImageHandler
//-----------------------------------------------------------------------------

void FbPreviewReader::ImageHandler::EndNode(const wxString &name)
{
	Reader().AppendImage(m_file, m_data);
}

//-----------------------------------------------------------------------------
//  FbPreviewReader
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::CreateHandler(const wxString &name)
{
	return name == wxT("fictionbook") ? new RootHandler(*this, name) : NULL;
}

/*
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description/title-info/coverpage")) {
				if (name == wxT("image")) AppendImg(atts);
			} else if (*this == wxT("fictionbook/description/publish-info")) {
				if (name == wxT("isbn")) m_isbn.Empty();
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << wxString::Format(wxT("<%s>"), name.c_str());
		} break;
		case fbsBinary: {
		} break;
		case fbsNone: {
			if (name == wxT("binary")) StartImg(atts);
		} break;
	}
	return true;

bool FbPreviewReader::TxtNode(const wxString &text)
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
	return true;
}

bool FbPreviewReader::EndNode(const wxString &name)
{
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
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
		} break;
		case fbsNone: {
			if (name == wxT("description")) {
				m_parsebody = m_annt.IsEmpty();
				if (!m_parsebody) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count() == 0) Stop();
				}
				m_data.SetText(FbViewData::ISBN, m_isbn);
				m_thread.SendHTML(m_data);
			} else if (name == wxT("binary")) {
				if (m_saveimage) {
					m_data.AddImage(m_imagename, m_imagedata);
					m_thread.SendHTML(m_data);
				}
			}
		} break;
	}
	return true;
}

*/

void FbPreviewReader::AppendImage(const wxString &file, const wxString &data)
{
	m_data.AddImage(file, data);
	m_thread.SendHTML(m_data);
}
