#ifndef __FBPARSINGCTX_H__
#define __FBPARSINGCTX_H__

#include <wx/wx.h>
#include "polarssl/md5.h"
#include "FbStringHash.h"

class FbParsingContextBase: public wxObject
{
	public:
		FbParsingContextBase() : m_md5calc(false), m_section(fbsNone) {}
		virtual ~FbParsingContextBase() {}
		bool Parse(wxInputStream & stream, bool md5 = false);
		wxString GetMd5() const { return m_md5sum; };
	protected:
		virtual bool DoParse(wxInputStream & stream) = 0;
		virtual void NewNode(const wxString &name, const FbStringHash &atts) = 0;
		virtual void TxtNode(const wxString &text) = 0;
		virtual void EndNode(const wxString &name) = 0;
		virtual void Stop() = 0;
	protected:
		enum FbSectionEnum {
			fbsNone,
			fbsBody,
			fbsDescr,
			fbsBinary,
		};
		void Inc(const wxString &tag);
		void Dec(const wxString &tag);
		bool operator == (const wxString & tags);
		bool operator >= (const wxString & tags);
		bool operator > (const wxString & tags);
		FbSectionEnum Section() { return m_section; }
		wxString Path() const;
	protected:
		md5_context m_md5cont;
		wxString m_md5sum;
		bool m_md5calc;
	private:
		FbSectionEnum m_section;
		wxArrayString m_tags;
		wxString m_name;
		wxString m_text;
};

#ifdef FB_PARSE_FAXPP

extern "C" {
	#undef NO_ERROR
	#include <faxpp/parser.h>
	#include <faxpp/error.h>
	#define NO_ERROR 0
}

class FbParsingContextFaxpp: public FbParsingContextBase
{
	public:
		FbParsingContextFaxpp();
		virtual ~FbParsingContextFaxpp();
	protected:
		virtual bool DoParse(wxInputStream & stream);
		virtual void Stop() { m_stop = true; }
	private:
		static wxString Str(const FAXPP_Text & text);
		static wxString Low(const FAXPP_Text & text);
		static void GetAtts(const FAXPP_Event & event, FbStringHash &hash);
		void OnProcessEvent(const FAXPP_Event & event);
	private:
		FAXPP_Parser * m_parser;
		bool m_stop;
		friend class FbFaxppStreamReader;
};

typedef FbParsingContextFaxpp FbParsingContext;

#else
	#ifndef FB_PARSE_EXPAT
		#define FB_PARSE_EXPAT
	#endif // FB_PARSE_EXPAT
#endif // FB_PARSE_FAXPP

#ifdef FB_PARSE_EXPAT

#include <expat.h>

class FbParsingContextExpat: public FbParsingContextBase
{
	public:
		static void GetAtts(const XML_Char **atts, FbStringHash &hash);
	public:
		FbParsingContextExpat();
		virtual ~FbParsingContextExpat();
	public:
		wxString encoding;
		wxString version;
	protected:
		virtual bool DoParse(wxInputStream & stream);
		virtual void Stop();
	private:
		XML_Parser m_parser;
		friend class FbExpatEventMaker;
};

typedef FbParsingContextExpat FbParsingContext;

#endif // FB_PARSE_EXPAT

#endif // __FBPARSINGCTX_H__
