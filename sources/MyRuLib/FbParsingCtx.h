#ifndef __FBPARSINGCTX_H__
#define __FBPARSINGCTX_H__

#include <wx/wx.h>

class FbParsingContextBase: public wxObject
{
	public:
		FbParsingContextBase() : m_section(fbsNone) {}
		virtual ~FbParsingContextBase() {}
		virtual bool Parse(wxInputStream & stream) = 0;
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
		virtual void Stop() = 0;
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
		static wxString Str(const FAXPP_Text & text);
		static wxString Low(const FAXPP_Text & text);
	public:
		FbParsingContextFaxpp();
		virtual ~FbParsingContextFaxpp();
		bool Parse(wxInputStream & stream);
	protected:
		virtual void OnProcessEvent(const FAXPP_Event & event);
		virtual void NewNode(const FAXPP_Event & event, bool closed) = 0;
		virtual void EndNode(const FAXPP_Event & event) = 0;
		virtual void TxtNode(const FAXPP_Event & event) = 0;
		virtual void Stop() { m_stop = true; }
	private:
		FAXPP_Parser * m_parser;
		bool m_stop;
};

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
		static wxString Str(const XML_Char *s, size_t len = wxString::npos);
		static wxString Low(const XML_Char *s, size_t len = wxString::npos);
		static bool IsWhiteOnly(const wxChar *buf);
	public:
		FbParsingContextExpat();
		virtual ~FbParsingContextExpat();
		virtual bool Parse(wxInputStream & stream);
	public:
		virtual void NewNode(const XML_Char *name, const XML_Char **atts) = 0;
		virtual void TxtNode(const XML_Char *text, int len) = 0;
		virtual void EndNode(const XML_Char* name) = 0;
		wxString encoding;
		wxString version;
	protected:
		virtual void Stop();
	private:
		XML_Parser m_parser;
};

#endif // FB_PARSE_EXPAT

#endif // __FBPARSINGCTX_H__
