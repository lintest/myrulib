/////////////////////////////////////////////////////////////////////////////
// Name:        bzipstream.cpp
// Purpose:     BZip stream classes
// Author:      Ryan Norton
// Modified by: David Hart (numerous fixes)
// Created:     10/11/03
// RCS-ID:      $Id: bzipstream.cpp,v 1.3 2006/12/12 17:28:07 ryannpcs Exp $
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "bzipstream.h"

#define BZ_NO_STDIO
#include "bzlib.h"

#ifndef BZ_MAX_UNUSED
#define BZ_MAX_UNUSED 5000
#endif

//===========================================================================
//
//                          IMPLEMENTATION
//
//===========================================================================

//---------------------------------------------------------------------------
//
// wxBZipInputStream
//
//---------------------------------------------------------------------------

wxBZipInputStream::wxBZipInputStream(wxInputStream& Stream, 
									 bool bLessMemory) : 
    wxFilterInputStream(Stream),
	m_nBufferPos(0)
{
	m_hZip = (void*) new bz_stream;

    bz_stream* hZip = (bz_stream*)m_hZip;
	
    hZip->bzalloc = NULL;
	hZip->bzfree = NULL;
	hZip->opaque = NULL;

	//param 2 - verbosity = 0-4, 4 more stuff to stdio
	//param 3 - small = non-zero means less memory and more time
	if (BZ2_bzDecompressInit(hZip, 0, bLessMemory)!= BZ_OK)
	{
		delete hZip;
		wxLogSysError(wxT("Could not initialize bzip ")
                      wxT("decompression engine!"));
	}
}

wxBZipInputStream::~wxBZipInputStream()
{
    bz_stream* hZip = (bz_stream*)m_hZip;

    BZ2_bzDecompressEnd(hZip);
	delete hZip;
}

wxInputStream& wxBZipInputStream::ReadRaw(void* pBuffer, size_t size)
{
	return m_parent_i_stream->Read(pBuffer, size);
}

off_t wxBZipInputStream::TellRawI()
{
	return m_parent_i_stream->TellI();
}

off_t wxBZipInputStream::SeekRawI(off_t pos, wxSeekMode sm)
{
	return 0;
}

size_t wxBZipInputStream::OnSysRead(void* buffer, size_t bufsize)
{
    bz_stream* hZip = (bz_stream*)m_hZip;

	hZip->next_out = (char*)buffer;
	hZip->avail_out = bufsize;

	while (hZip->avail_out != 0)
	{
		if (m_nBufferPos == 0 || m_nBufferPos == WXBZBS)
		{
			ReadRaw(m_pBuffer, WXBZBS);
			
            m_nBufferPos = 0;
			hZip->next_in = m_pBuffer;
			hZip->avail_in = WXBZBS;

            if (m_parent_i_stream->LastRead() != WXBZBS)
			{
                // Full amount not read, so do a last
                // minute tidy up and decompress what is left
				hZip->avail_in = m_parent_i_stream->LastRead();

				int nRet = BZ2_bzDecompress(hZip);

				if (nRet == BZ_OK || nRet == BZ_STREAM_END)
					return bufsize - hZip->avail_out;
				else
					return 0;
			}
		}

        // Buffer full, decompress some bytes
        hZip->next_in = &m_pBuffer[m_nBufferPos];
		hZip->avail_in = WXBZBS - m_nBufferPos;

		int nRet = BZ2_bzDecompress(hZip);

		if (nRet == BZ_OK)	
		{
			m_nBufferPos = WXBZBS - hZip->avail_in;
		}
		else if(nRet == BZ_STREAM_END)
			return bufsize - hZip->avail_out;
		else
			return 0;	
	}
 
	return bufsize - hZip->avail_out;	
}

//---------------------------------------------------------------------------
//
// wxBZipOutputStream
//
//---------------------------------------------------------------------------

wxBZipOutputStream::wxBZipOutputStream(wxOutputStream& Stream,
									   wxInt32 nCompressionFactor) : 
    wxFilterOutputStream(Stream)
{
	m_hZip = new bz_stream;

    bz_stream* hZip = (bz_stream*)m_hZip;

	hZip->bzalloc = NULL;
	hZip->bzfree = NULL;
	hZip->opaque = NULL;

	//param 2 - compression factor = 1-9 9 more compression but slower
	//param 3 - verbosity = 0-4, 4 more stuff to stdio (ignored)
	//param 4 - workfactor = reliance on standard comp alg, 0-250, 
    //                                                      0==30 default
	if (BZ2_bzCompressInit(hZip, nCompressionFactor, 0, 0)!= BZ_OK)
	{
		delete hZip;
		wxLogSysError(wxT("Could not initialize bzip compression engine!"));
	}
}

wxBZipOutputStream::~wxBZipOutputStream()
{
    bz_stream* hZip = (bz_stream*)m_hZip;

    BZ2_bzCompressEnd(hZip);
	delete hZip;
}

wxOutputStream& wxBZipOutputStream::WriteRaw(void* pBuffer, size_t size)
{
	return m_parent_o_stream->Write(pBuffer, size);
}

off_t wxBZipOutputStream::TellRawO()
{
	return m_parent_o_stream->TellO();
}

off_t wxBZipOutputStream::SeekRawO(off_t pos, wxSeekMode sm)
{
	return 0;
}

size_t wxBZipOutputStream::OnSysWrite(const void* buffer, size_t bufsize)
{
    bz_stream* hZip = (bz_stream*)m_hZip;

    hZip->next_in = (char*)buffer;
    hZip->avail_in = bufsize;
	hZip->next_out = m_pBuffer;
	hZip->avail_out = WXBZBS;

    size_t nWrote = 0;
    int nRet = BZ_RUN_OK;  // for the 'while' statement

    while( nRet== BZ_RUN_OK && hZip->avail_in > 0 )
    {
        // Compress the data in buffer, resulting data -> pbuffer
        nRet = BZ2_bzCompress(hZip, BZ_RUN);  
        if (nRet != BZ_RUN_OK)
        { 
            wxLogDebug(wxT("Error from BZ2_bzCompress in Write()")); 
            return 0; 
        }

        // This is how much newly compressed data is available
        size_t nCurWrite = WXBZBS - hZip->avail_out;  
        if (nCurWrite != 0)
		{
            // Deliver the compressed data to the parent stream
            WriteRaw(m_pBuffer, nCurWrite);    
            if (m_parent_o_stream->LastWrite() != nCurWrite)
            { 
                wxLogDebug(wxT("Error writing to underlying stream")); 
                break; 
            }

            //Reset the buffer
            hZip->avail_out = WXBZBS;  
            hZip->next_out = m_pBuffer;
            nWrote += nCurWrite;
		}
	}

    // I'm not sure if this is necessary as, if it worked, 
    // the loop continued until avail_in was zero
    nWrote = bufsize - hZip->avail_in ;  
    return nWrote;	
}


bool wxBZipOutputStream::Close() // Flushes any remaining compressed data
{
    bz_stream* hZip = (bz_stream*)m_hZip;
    int nRet = BZ_FINISH_OK;

    while ( nRet == BZ_FINISH_OK )
	{
		hZip->next_out = m_pBuffer;
		hZip->avail_out = WXBZBS;

        // Call BZ2_bzCompress with the parameter BZ_FINISH
		int nRet = BZ2_bzCompress(hZip, BZ_FINISH);
		if (nRet != BZ_FINISH_OK && nRet != BZ_STREAM_END)
		{ 
            wxLogDebug(wxT("BZ2_bzCompress error in Close()")); 
            break; 
        }
		
		size_t nCurWrite = WXBZBS - hZip->avail_out;

        if (nCurWrite != 0)
		{
			WriteRaw(m_pBuffer, nCurWrite);

			if (m_parent_o_stream->LastWrite() != nCurWrite)
            { 
                wxLogDebug(wxT("Error writing to underlying ")
                           wxT("stream during the Close() phase")); 
                break; 
            }
        }

        if (nRet == BZ_STREAM_END)
		{
            return true; //hrm, restructure here???
		}
	}
 
    return false;
}

//---------------------------------------------------------------------------
//
// wxBZipStream
//
//---------------------------------------------------------------------------

wxBZipStream::wxBZipStream(wxInputStream& i, wxOutputStream& o) : 
	wxBZipInputStream(i), wxBZipOutputStream(o) {}

wxBZipStream::~wxBZipStream(){}

//---------------------------------------------------------------------------
//
// wxBZipClassFactory
//
//---------------------------------------------------------------------------

#if wxVERSION_NUMBER > 2701

IMPLEMENT_DYNAMIC_CLASS(wxBZipClassFactory, wxFilterClassFactory)

static wxBZipClassFactory g_wxBZipClassFactory;

wxBZipClassFactory::wxBZipClassFactory()
{
    if (this == &g_wxBZipClassFactory)
        PushFront();
}

const wxChar * const *
wxBZipClassFactory::GetProtocols(wxStreamProtocolType type) const
{
    static const wxChar *protos[] =     
        { _T("bzip"), _T("bzip2"), NULL };
    static const wxChar *mimes[] = 
        { _T("a\application/x-bzip"), 
          _T("application/x-bzip-compressed-tar"), 
          NULL };
    static const wxChar *encs[] = 
        { _T("bzip"), _T("bzip2"), NULL };
    static const wxChar *exts[] =    
        { _T(".bz"), _T(".bz2"), NULL };
    static const wxChar *empty[] =
        { NULL };

    switch (type) {
        case wxSTREAM_PROTOCOL: return protos;
        case wxSTREAM_MIMETYPE: return mimes;
        case wxSTREAM_ENCODING: return encs;
        case wxSTREAM_FILEEXT:  return exts;
        default:                return empty;
    }
}

#endif // wxVERSION_NUMBER > 2701

#endif  //wxUSE_ZLIB && wxUSE_STREAMS
