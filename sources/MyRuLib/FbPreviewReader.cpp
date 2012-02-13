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
		case Descr: 
			return new DescrHandler(m_reader, name);
		case Binary: {
			wxString file = Value(atts, wxT("id"));
			if (int index = m_reader.m_images.Index(file) != wxNOT_FOUND) {
				m_reader.m_images.RemoveAt(index);
				return new ImageHandler(m_reader, name, file);
			} else return NULL; 
		} break;
		default:
			return NULL;
	}
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
		case Title   : return new TitleHandler(m_reader, name);
		case Publish :
		default: return NULL;
	}
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
		case Annot: return new AnnotHandler(m_reader, name);
		case Cover: return new CoverHandler(m_reader, name);
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::CoverHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::CoverHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT("image")) m_reader.AppendImg(atts);
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbPreviewReader::ImageHandler
//-----------------------------------------------------------------------------

void FbPreviewReader::ImageHandler::EndNode(const wxString &name)
{
	m_reader.ImageData(m_file, m_data);
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

void FbPreviewReader::AppendImg(const FbStringHash &atts)
{
	FbStringHash::const_iterator it;
	for( it = atts.begin(); it != atts.end(); ++it ) {
		wxString name = it->first;
		if (name.Right(4) == wxT("href")) {
			wxString value = it->second;
			if (value.Left(1) == wxT("#")) value = value.Mid(1);
			m_images.Add(value);
			break;
		}
	}
}

void FbPreviewReader::ImageData(const wxString &file, const wxString &data)
{
	m_data.AddImage(file, data);
	m_thread.SendHTML(m_data);
}