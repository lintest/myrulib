/////////////////////////////////////////////////////////////////////////////
// Name:        httpbuilder.h
// Purpose:     HTTP builder assists with building data to send to a web server
//				including the query string, cookies, urlencoded post variables, and multipart post data.
// Author:      Angelo Mandato
// Created:     2005/08/10
// RCS-ID:      $Id: httpbuilder.h,v 1.2 2005/08/12 03:58:08 amandato Exp $
// Copyright:   (c) 2005 Angelo Mandato (http://www.spaceblue.com)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTTPBUILDER_H_
#define _WX_HTTPBUILDER_H_

// optimization for GCC
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "httpbuilder.h"
#endif

//#define HTTPBUILDER_BASE64 // Use base 64 for HTTP Authentication
#define HTTPBUILDER_BOUNDARY_LENGTH 40 // Boundary length for multipart posts
#define HTTPBUILDER_VERSION wxT("1.2") // Version of wxHTTPBuilder

#define HTTPBUILDER_NEWLINE wxT("\r\n") // For future use

#include <wx/protocol/http.h>
#include "wx/httpenginedef.h"

//! wxProxySettings class, makes getting and setting proxy settings easier.
class WXDLLIMPEXP_HTTPENGINE wxProxySettings
{
public:
	wxString m_strProxyHostname;
	wxString m_strProxyUsername;
	wxString m_strProxyPassword;
	wxString m_strProxyExceptions;
	wxString m_strProxyExceptionsDelim;
	long m_nProxyPort;
	bool m_bUseProxy;
  bool m_bRequiresAuth;

  void SetUseProxy(const bool bUse) { m_bUseProxy = bUse; };

public:
	wxProxySettings() { m_bRequiresAuth = 0; m_bUseProxy = 0; m_nProxyPort = 0; m_strProxyExceptionsDelim = wxT(","); }
	wxProxySettings(const wxProxySettings& data);
	virtual ~wxProxySettings() {};
	void operator=(const wxProxySettings& data);

};

//! wxHTTPAuthSettings class, makes getting and setting HTTP auth settings easier.
class WXDLLIMPEXP_HTTPENGINE wxHTTPAuthSettings
{
public:
	enum wxHTTP_Auth
  {
    wxHTTP_AUTH_NONE = 0,
    wxHTTP_AUTH_BASIC
  };

	//bool m_bUseAuth;
	wxString m_strAuthUsername;
	wxString m_strAuthPassword;
	wxHTTP_Auth   m_authType;
	bool m_bRememberPasswd; // When set to false, next time dialog uses this, it will ignore the password

	void SetBasicAuth() { m_authType = wxHTTP_AUTH_BASIC; };
	void SetNoAuth() { m_authType = wxHTTP_AUTH_NONE; };

public:
	wxHTTPAuthSettings() { m_bRememberPasswd = 0; m_authType = wxHTTP_AUTH_NONE; }
	wxHTTPAuthSettings(const wxHTTPAuthSettings& data);
	virtual ~wxHTTPAuthSettings() {};
	void operator=(const wxHTTPAuthSettings& data);
};


class WXDLLIMPEXP_HTTPENGINE wxHTTPBuilder : public wxHTTP
{
public:

  // Constructor
  wxHTTPBuilder();

  // Destructor
  ~wxHTTPBuilder();

  // variable and authentication types
  enum wxHTTP_Type
  {
    wxHTTP_TYPE_ANY = 0, // Any except cookie
    wxHTTP_TYPE_GET,
    wxHTTP_TYPE_POST,
    wxHTTP_TYPE_FILE,
    wxHTTP_TYPE_MULTIPARTDATA,
    wxHTTP_TYPE_COOKIE,
    wxHTTP_TYPE_COOKIERAW
  };

  // Content type functions:
	void InitContentTypes(void);
	wxString GetContentType( const wxString &filename );
  bool RemoveContentType( const wxString &abv );
  void AddContentType( const wxString &abv, const wxString &desc ) { RemoveContentType(abv); m_ContentTypeAbvs.Add(abv); m_ContentTypeDescs.Add(desc); };

  // Proxy server settings
  void HttpProxyAuth( const wxString &user, const wxString &password, const bool bRequiresAuth = true ) { m_proxySettings.m_bRequiresAuth = bRequiresAuth; m_proxySettings.m_strProxyUsername = user, m_proxySettings.m_strProxyPassword = password; };
	void HttpProxy( const wxString &host, const int port );
  void UseHttpProxy(const bool use ) { m_proxySettings.m_bUseProxy = use; m_HttpProxyUsed = false; };
  void HttpProxyExceptions(const wxString &exceptions, const wxString &delim = wxT(","))  { m_proxySettings.m_strProxyExceptions = exceptions; m_proxySettings.m_strProxyExceptionsDelim = delim; };
	wxProxySettings GetProxySettings();
	void SetProxySettings(const wxProxySettings &settings);

	// Virtual and public in case someone wants to write their own check for exceptions
	virtual bool UseProxyForHost( const wxString &host);

  // Basic authenfication:
  void Authenticate( const wxString &user, const wxString &password, wxHTTPAuthSettings::wxHTTP_Auth type = wxHTTPAuthSettings::wxHTTP_AUTH_BASIC);
  void SetAuthentication(const wxHTTPAuthSettings &settings) { m_httpAuthSettings = settings; };
  wxHTTPAuthSettings GetAuthentication() { return m_httpAuthSettings; };

	// Stop what we are doing gracefully, let it read the last received bytes then stop
  void Stop(const bool stop = true) { m_Stop = stop; };

  // Get the collected values url encoded
  wxString GetPostBuffer(void);
	wxString GetQueryString(bool includeAny = false);
  wxString GetCookieString(void);

	// Handle the name/value pairs for the GET/POST/COOKIEs
	wxString GetValue( const wxString &name, wxHTTP_Type type = wxHTTP_TYPE_ANY );
	bool SetValue( const wxString &name, const wxString &value, wxHTTP_Type type = wxHTTP_TYPE_ANY );
  bool RemoveValue( wxString &name, wxHTTP_Type type );

  // Same as SetValue except it also verifies no invalid characters are used for the cookie name or value
	bool SetRequestCookie( const wxString &name, const wxString &value, const bool urlencode = false );


  ////////////////////////////////////////////////////////////
  // Static methods that may be used for other purposes
  ////////////////////////////////////////////////////////////

  // URL coding and characters:
	static wxString URLEncode( const wxString &value );
	static wxString URLDecode( const wxString &value );

  // Convert chars to HTML special characters
  static wxString HTMLSpecialChars( const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);

  // Base 64 coding:
	static wxString Base64Decode( const wxString &value );
	static wxString Base64Encode( const wxString &value );

  // Parsing URLs:
  static bool ParseURL( const wxString &url, wxString &protocol, wxString &host, int &port, wxString &request, wxString &username, wxString &password );

  ////////////////////////////////////////////////////////////
  // End static methods
  ////////////////////////////////////////////////////////////

  // Send built HTTP request and save the returned information as a file
	bool SaveFile(const wxString& filename, const wxString& path, const wxString& tempDirOrPrefix = wxEmptyString);

  // Send the built HTTP request and return a stream of the returned data from server.
  wxInputStream* GetInputStream(const wxString& url, const wxString& tempDirOrPrefix = wxEmptyString);

  // Send the build HTTP requeset and return a string returned from the server.
  wxString GetInputString(const wxString &url, const wxString& tempDirOrPrefix = wxEmptyString);


  // Error message
  wxString GetLastError(void) { return m_error; };
  wxString GetStatusCode(void);
  wxString GetResponseString(void);

  int GetBytesRead(void) { return m_bytesRead; };
  int GetBytesSent(void) { return m_bytesSent; };

protected:

  // helper functions that may be accessed by an extended class
  bool SendRequest(const wxString& path, const wxString& tempDirOrPrefix = wxEmptyString );
  wxString CreateBoundary( const int length );

  // Bytes Read
  int           m_bytesRead;
  int           m_bytesSent;

  // Stop what we are doing flag
  bool          m_Stop;

private:

	// Class specific funtions:
  int FindNamePosition( const wxString &name, wxHTTP_Type type = wxHTTP_TYPE_ANY );
  wxInputStream* GetPostBufferStream(const wxString &szTempFile);
  static wxString HexFromInt( const int &value );
	static int IntFromHex( const wxString &chars);
  static int Rand(void);
	static void SRand(void);

  // Set of virtual functions that a thread may want to use
  virtual void AddBytesRead(const int bytes) { m_bytesRead += bytes; };
  virtual void AddBytesSent(const int bytes) { m_bytesSent += bytes; };
  virtual bool StopCheck(void) { return m_Stop; };

  // GET/POST/COOKIE/FILE names and values:
  wxArrayString m_Fields;
  wxArrayString m_Values;
  wxArrayInt    m_Types;
  int           m_MultiPartCount; // Future use

  // Basic Authentication
	wxHTTPAuthSettings m_httpAuthSettings;

  // Proxy server settings
	wxProxySettings m_proxySettings;

	// Internal value to let the class know if we actually used the proxy server
  bool          m_HttpProxyUsed;

  // Content types
  wxArrayString m_ContentTypeDescs;
  wxArrayString m_ContentTypeAbvs;

  // Error string
  wxString      m_error;
};

#endif
