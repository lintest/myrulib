#ifndef __FBPARSINGCTX_H__
#define __FBPARSINGCTX_H__

#include <wx/wx.h>
#include "polarssl/md5.h"
#include "FbStringHash.h"

#ifndef FB_PARSE_LIBXML2
	#ifndef FB_PARSE_EXPAT
		#ifndef FB_PARSE_FAXPP
			#define FB_PARSE_EXPAT
		#endif // FB_PARSE_FAXPP
	#endif // FB_PARSE_EXPAT
#endif // FB_PARSE_LIBXML2

#define FB2_BEGIN_KEYLIST \
	private: enum Keyword {

#define FB2_END_KEYLIST \
	None };                                                              \
	WX_DECLARE_STRING_HASH_MAP(Keyword, StringHash);                     \
	class KeywordHash : public StringHash {                              \
		public: KeywordHash();                                           \
		private: void KEY(const char * name, Keyword key)                \
			{ operator[](wxString(name, wxConvUTF8)) = key; }            \
	};                                                                   \
	static Keyword toKeyword(const wxString &name); private:

#define FB2_BEGIN_KEYHASH(T) \
	T::Keyword T::toKeyword(const wxString &name)                        \
	{                                                                    \
		static const KeywordHash map;                                    \
		KeywordHash::const_iterator i = map.find(name);                  \
		return i == map.end() ? None : i->second;                        \
	}                                                                    \
	T::KeywordHash::KeywordHash()                                        \
	{

#define FB2_END_KEYHASH \
	}

class FbParserXML: public wxObject
{
protected:
	class BaseHandler: public wxObject
	{
	public:
		explicit BaseHandler(const wxString &name) : m_handler(NULL), m_name(name), m_closed(false) {}
		virtual ~BaseHandler();
		virtual bool NewNode(const wxString &name, const FbStringHash &atts);
		virtual bool TxtNode(const wxString &text);
		virtual bool EndNode(const wxString &name, bool &skip);
	protected:
		wxString Value(const FbStringHash &atts, const wxString &name);
	protected:
		BaseHandler * m_handler;
		const wxString m_name;
		bool m_closed;
	};

	class TextHandler : public BaseHandler
	{
	public:
		explicit TextHandler(const wxString &name, wxString &text)
			: BaseHandler(name), m_text(text) { m_text.Empty(); }
		virtual ~TextHandler()
			{ m_text.Trim(false).Trim(true); }
		virtual bool TxtNode(const wxString &text)
			{ m_text << text; return true; }
	private:
		wxString & m_text;
	};

public:
	explicit FbParserXML() : m_handler(NULL), m_md5calc(false) {}
	virtual ~FbParserXML() {}
	bool Parse(wxInputStream & stream, bool md5 = false);
	wxString GetMd5() const { return m_md5sum; };
	virtual void Stop() = 0;

protected:
	virtual bool DoParse(wxInputStream & stream) = 0;
	virtual bool NewNode(const wxString &name, const FbStringHash &atts);
	virtual bool TxtNode(const wxString &text);
	virtual bool EndNode(const wxString &name);

protected:
	BaseHandler * m_handler;
	md5_context m_md5cont;
	wxString m_md5sum;
	bool m_md5calc;

protected:
	enum FbSectionEnum {
		fbsNone,
		fbsBody,
		fbsDescr,
		fbsBinary,
	};
	void Inc(const wxString &tag) {}
	void Dec(const wxString &tag) {}
	bool operator == (const wxString & tags) { return false; }
	bool operator >= (const wxString & tags) { return false; }
	bool operator > (const wxString & tags) { return false; }
	FbSectionEnum Section() { return fbsNone; }
	wxString Path() const { return wxEmptyString; }
};

#ifdef FB_PARSE_FAXPP

extern "C" {
	#undef NO_ERROR
	#include <faxpp/parser.h>
	#include <faxpp/error.h>
	#define NO_ERROR 0
}

class FbParsingContextFaxpp: public FbParserXML
{
	public:
		FbParsingContextFaxpp();
		virtual ~FbParsingContextFaxpp();
		virtual void Stop() { m_stop = true; }
	protected:
		virtual bool DoParse(wxInputStream & stream);
	private:
		static FAXPP_DecodeFunction StrToDecode(const wxString & encoding);
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

#endif // FB_PARSE_FAXPP

#ifdef FB_PARSE_EXPAT

#include <expat.h>

class FbParsingContextExpat: public FbParserXML
{
	public:
		static void GetAtts(const XML_Char **atts, FbStringHash &hash);
	public:
		FbParsingContextExpat();
		virtual ~FbParsingContextExpat();
		virtual void Stop();
	public:
		wxString encoding;
		wxString version;
	protected:
		virtual bool DoParse(wxInputStream & stream);
	private:
		XML_Parser m_parser;
		friend class FbExpatEventMaker;
};

typedef FbParsingContextExpat FbParsingContext;

#endif // FB_PARSE_EXPAT

#ifdef FB_PARSE_LIBXML2

#include <libxml/xmlreader.h>

class FbParsingContextLibxml2: public FbParserXML
{
	public:
		FbParsingContextLibxml2();
		virtual ~FbParsingContextLibxml2();
		int Read(void * buffer, int length);
		virtual void Stop() { m_stop = true; }
	protected:
		virtual bool DoParse(wxInputStream & stream);
	private:
		void ProcessNode(xmlTextReaderPtr & reader);
		wxString Low(const xmlChar * text);
	private:
		wxInputStream * m_stream;
		bool m_stop;
};

typedef FbParsingContextLibxml2 FbParsingContext;

#endif // FB_PARSE_LIBXML2

#endif // __FBPARSINGCTX_H__
