#include "FbPreviewReader.h"
#include "FbImportReader.h"
#include "FbViewThread.h"
#include "FbViewData.h"
#include "FbSmartPtr.h"
#include <wx/zipstrm.h>
#include <wx/mstream.h>

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
	wxString value = Value(atts, wxT("href"));
	if (value.Left(1) == wxT('#')) value = value.Mid(1);
	m_images.Add(value);
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
	m_data.AddImage(m_thread, file, data);
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

//-----------------------------------------------------------------------------
//  FbPreviewReaderEPUB::RootHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReaderEPUB::RootHandler)
	KEY( "dc-metadata" , Metadata );
	KEY( "metadata"    , Metadata );
	KEY( "manifest"    , Manifest );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReaderEPUB::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Metadata: return new MetadataHandler(*this, name);
		case Manifest: return new ManifestHandler(*this, name);
		default: return NULL;
	}
}

void FbPreviewReaderEPUB::RootHandler::AppendCover(const FbStringHash &atts)
{
	wxString value = Value(atts, wxT("content"));
	if (value.IsEmpty()) return;
	m_covers.Add(value);
}

bool FbPreviewReaderEPUB::RootHandler::CheckCover(const FbStringHash &atts)
{
	wxString name = Value(atts, wxT("id"));
	if (name.IsEmpty()) return false;
	if (m_covers.Count()) {
		int index = m_covers.Index(name);
		if (index == wxNOT_FOUND) return false;
		m_covers.RemoveAt(index);
		return true;
	} else {
		wxString type = Value(atts, wxT("media-type")).Lower().BeforeFirst(wxT('/'));
		return type == wxT("image");
	}
}

void FbPreviewReaderEPUB::RootHandler::AppendFile(const FbStringHash &atts)
{
	if (m_covers.Count() == 0) m_reader.Stop();
	wxString value = Value(atts, wxT("href"));
	if (value.IsEmpty()) return;

	if (!m_path.IsEmpty()) {
		wxFileName filename = value;
		filename.MakeAbsolute(m_path, wxPATH_UNIX);
		value = filename.GetFullPath(wxPATH_UNIX);
	}
	m_files.Add(value);
}

//-----------------------------------------------------------------------------
//  FbPreviewReaderEPUB::MetadataHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbPreviewReaderEPUB::MetadataHandler)
	KEY( "metadata"    , Metadata );
	KEY( "meta"        , Meta     );
	KEY( "description" , Descr  );
	KEY( "identifier"  , Ident  );
FB2_END_KEYHASH

FbHandlerXML * FbPreviewReaderEPUB::MetadataHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Meta: {
			wxString value =  Value(atts, wxT("name")).Lower();
			if (value == wxT("cover")) m_root.AppendCover(atts);
			return NULL;
		}
		case Ident : {
			wxString value = Value(atts, wxT("scheme")).Lower();
			return value == wxT("isbn") ? new TextHandler(name, m_root.m_isbn) : NULL;
		}
		case Metadata : return new MetadataHandler(m_root, name);
		case Descr    : return new TextHandler(name, m_root.m_annt);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbPreviewReaderEPUB::ManifestHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbPreviewReaderEPUB::ManifestHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT("item") && m_root.CheckCover(atts)) m_root.AppendFile(atts);
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbPreviewReaderEPUB
//-----------------------------------------------------------------------------

void FbPreviewReaderEPUB::Preview(wxInputStream &stream)
{
	wxString rootfile = FbRootReaderEPUB(stream).GetRoot();
	m_path = wxFileName(rootfile).GetPath();

	{
		wxZipInputStream zip(stream);
		while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
			if (entry->GetInternalName() == rootfile) {
                if (!zip.OpenEntry(*entry)) return;
                if (!Parse(zip)) return;
				break;
			}
		}
		m_data.SetText(FbViewData::ANNT, m_annt);
		m_data.SetText(FbViewData::ISBN, m_isbn);
		m_thread.SendHTML(m_data);
	}

	if (m_files.IsEmpty()) return;
	stream.SeekI(0);

	{
		wxZipInputStream zip(stream);
		while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
			int size = entry->GetSize();
			if (size <= 0) continue;
			wxString filename = entry->GetName(wxPATH_UNIX);
			int index = m_files.Index(filename);
			if (index == wxNOT_FOUND) continue;
			if (!zip.OpenEntry(*entry)) return;
			FbMemoryInputStream in(zip, size);
			m_data.AddImage(m_thread, filename, in);
			break;
		}
	}
}

FbHandlerXML * FbPreviewReaderEPUB::CreateHandler(const wxString &name)
{
	return name == wxT("package") ? new RootHandler(*this, m_files, m_path, name) : NULL;
}

