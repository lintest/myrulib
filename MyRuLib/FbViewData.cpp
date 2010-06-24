#include "FbViewData.h"
#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include <wx/mimetype.h>
#include "wx/base64.h"

//-----------------------------------------------------------------------------
//  FbViewData
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbViewDataArray);

FbViewData::FbViewData(const FbViewData &info)
	: m_id(info.m_id)
{
	for (size_t i = 0; i < FILE; i++) m_text[i] = info.m_text[i];
}

FbViewData::~FbViewData()
{
	size_t count = m_images.Count();
	for (size_t i = 0; i < count; i++) {
		wxMemoryFSHandler::RemoveFile(m_images[i]);
	}
}

void FbViewData::SetText(size_t index, const wxString &text)
{
	if (index < FILE) m_text[index] = text;
}

wxString FbViewData::GetText(size_t index) const
{
	return index < FILE ? m_text[index] : wxString();
}

void FbViewData::AddImage(wxString &filename, wxString &imagedata, wxString &imagetype)
{
	if (m_images.Index(filename) != wxNOT_FOUND) return;
	wxString imagename = wxString::Format(wxT("%d/%s"), m_id, filename.c_str());
	wxMemoryBuffer buffer = wxBase64Decode(imagedata);
	wxMemoryInputStream stream(buffer.GetData(), buffer.GetDataLen());
	wxImage image(stream);
	if (image.GetWidth() <= MAX_IMAGE_WIDTH) {
		wxMemoryFSHandler::AddFile(imagename, buffer.GetData(), buffer.GetDataLen());
	} else {
		wxBitmap bitmap(image);
		int w = MAX_IMAGE_WIDTH;
		int h = image.GetHeight() * MAX_IMAGE_WIDTH / image.GetWidth();
		double scale = double(MAX_IMAGE_WIDTH) / image.GetWidth();
		wxMemoryDC srcDC;
		srcDC.SelectObject(bitmap);

		wxBitmap result(w, h);
		wxMemoryDC memDC;
		memDC.SetUserScale(scale, scale);
		memDC.SelectObject(result);
		memDC.Blit(0, 0, image.GetWidth(), image.GetHeight(), &srcDC, 0, 0, wxCOPY, true);
		memDC.SelectObject(wxNullBitmap);
		srcDC.SelectObject(wxNullBitmap);

		wxMemoryFSHandler::AddFile(imagename, result, wxBITMAP_TYPE_PNG);
	}
	m_images.Add(filename);
}

wxString FbViewData::GetHTML(const FbViewContext &ctx, const FbCacheBook &book) const
{
	return wxT("book info");
}
