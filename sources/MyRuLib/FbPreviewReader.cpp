#include "FbPreviewReader.h"
#include "FbViewThread.h"
#include "FbViewData.h"

//-----------------------------------------------------------------------------
//  FbPreviewReader::RootHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReader::RootHandler)
	KEY( "body"        , Body   );
	KEY( "description" , Descr  );
	KEY( "binary"      , Binary );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReader::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Body   : return m_annt.IsEmpty() ? new BodyHandler(*this, name) : NULL;
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
			if (value.Left(1) == wxT('#')) value = value.Mid(1);
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

void FbPreviewReader::RootHandler::EndImg(const wxString &file, const wxString &data)
{
	if (m_images.Count() == 0) m_reader.Stop();
	m_data.AddImage(file, data);
	m_thread.SendHTML(m_data);
}

void FbPreviewReader::RootHandler::SendDescr()
{
	if (!m_annt.IsEmpty() && m_images.Count() == 0) m_reader.Stop();
	m_data.SetText(FbViewData::ANNT, m_annt);
	m_data.SetText(FbViewData::ISBN, m_isbn);
	m_thread.SendHTML(m_data);
}

void FbPreviewReader::RootHandler::BreakBody()
{
	m_parsebody = false;
	m_data.SetText(FbViewData::ANNT, m_annt);
	if (m_images.Count() == 0) m_reader.Stop();
	m_thread.SendHTML(m_data);
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
		case Annot: return new AnnotHandler(m_root, name);
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
	if (name == wxT('p')) m_root.m_annt << wxT('<') << name << wxT('>');
	return new AnnotHandler(*this, name);
}

void FbPreviewReader::AnnotHandler::TxtNode(const wxString &text) 
{ 
	m_root.m_annt << text; 
}

void FbPreviewReader::AnnotHandler::EndNode(const wxString &name)
{
	if (name == wxT('p')) m_root.m_annt << wxT('<') << wxT('/') << name << wxT('>');
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
//  FbPreviewReader::BodyHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::BodyHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (!m_root.m_parsebody) return NULL;
	if (name == wxT('p')) m_root.m_annt << wxT('<') << name << wxT('>');
	return new BodyHandler(*this, name);
}

void FbPreviewReader::BodyHandler::TxtNode(const wxString &text) 
{ 
	if (m_root.m_parsebody) m_root.m_annt << text; 
}


void FbPreviewReader::BodyHandler::EndNode(const wxString &name)
{
	if (name == wxT('p')) m_root.m_annt << wxT('<') << wxT('/') << name << wxT('>');
	if (m_root.m_parsebody && m_root.m_annt.length() > 1000) m_root.BreakBody();
}

//-----------------------------------------------------------------------------
//  FbPreviewReader
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReader::CreateHandler(const wxString &name)
{
	return name == wxT("fictionbook") ? new RootHandler(*this, m_thread, m_data, name) : NULL;
}
