/////////////////////////////////////////////////////////////////////////////
// Name:        bzipstream.h
// Purpose:     BZip streams
// Author:      Ryan Norton
// Modified by:
// Created:     09/05/03
// RCS-ID:      $Id: bzipstream.h,v 1.2 2006/12/12 17:20:05 ryannpcs Exp $
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXBZSTREAM_H__
#define _WX_WXBZSTREAM_H__

#include "wx/defs.h"

#if wxUSE_STREAMS

#include "wx/stream.h"

#ifndef WXBZBS //WXBZBS == BZip buffer size
#define WXBZBS 5000
#endif

//---------------------------------------------------------------------------
//      wxBZipInputStream
//---------------------------------------------------------------------------

class wxBZipInputStream : public wxFilterInputStream
{
public:
	// if bLessMemory is true, uses decompression algorithm 
    // which uses less memory, but is also slower
	wxBZipInputStream(wxInputStream& stream, bool bLessMemory = false); 
	virtual ~wxBZipInputStream();

	wxInputStream& ReadRaw(void* pBuffer, size_t size);
	off_t TellRawI();
	off_t SeekRawI(off_t pos, wxSeekMode sm = wxFromStart);

	void* GetHandleI() {return m_hZip;}
protected:
	virtual size_t OnSysRead(void *buffer, size_t size);

	void*   m_hZip;
	char    m_pBuffer[WXBZBS];
	int     m_nBufferPos;
};

//---------------------------------------------------------------------------
//      wxBZipOutputStream
//---------------------------------------------------------------------------

class wxBZipOutputStream : public wxFilterOutputStream
{
public:
    // nCompressionFactor is from 1-9; compression is higher but slower
    // at higher numbers
	wxBZipOutputStream(wxOutputStream& stream, 
                       wxInt32 nCompressionFactor = 4);
	virtual ~wxBZipOutputStream();

	wxOutputStream& WriteRaw(void* pBuffer, size_t size);
	off_t TellRawO();
	off_t SeekRawO(off_t pos, wxSeekMode sm = wxFromStart);
     
	void* GetHandleO() {return m_hZip;}

    virtual bool  Close();

protected:
	virtual size_t OnSysWrite(const void *buffer, size_t bufsize);
	
    void*   m_hZip;
	char    m_pBuffer[WXBZBS];
};

//---------------------------------------------------------------------------
//      wxBZipStream
//---------------------------------------------------------------------------

class wxBZipStream : public wxBZipInputStream, wxBZipOutputStream
{
public:
	wxBZipStream(wxInputStream& istream, wxOutputStream& ostream);
	virtual ~wxBZipStream();
};

//---------------------------------------------------------------------------
//      wxBZipClassFactory - wxArchive integration
//---------------------------------------------------------------------------

#if wxVERSION_NUMBER > 2701

class WXEXPORT wxBZipClassFactory: public wxFilterClassFactory
{
public:
    wxBZipClassFactory();

    wxFilterInputStream *NewStream(wxInputStream& stream) const
        { return new wxBZipInputStream(stream); }
    wxFilterOutputStream *NewStream(wxOutputStream& stream) const
        { return new wxBZipOutputStream(stream); }
    wxFilterInputStream *NewStream(wxInputStream *stream) const
        { return new wxBZipInputStream(*stream); }
    wxFilterOutputStream *NewStream(wxOutputStream *stream) const
        { return new wxBZipOutputStream(*stream); }

    const wxChar * const *GetProtocols(wxStreamProtocolType type
                                       = wxSTREAM_PROTOCOL) const;

private:
    DECLARE_DYNAMIC_CLASS(wxBZipClassFactory)
};

#endif // wxVERSION_NUMBER > 2701

#endif // wxUSE_STREAMS
#endif // _WX_WXBZSTREAM_H__
