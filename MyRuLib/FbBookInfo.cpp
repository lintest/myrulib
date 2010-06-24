#include "FbBookInfo.h"
#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include <wx/mimetype.h>
#include "wx/base64.h"

//-----------------------------------------------------------------------------
//  FbBookInfo
//-----------------------------------------------------------------------------

void FbBookInfo::AddImage(wxString &filename, wxString &imagedata, wxString &imagetype)
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

