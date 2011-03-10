/////////////////////////////////////////////////////////////////////////////
// Name:        dtd.cpp
// Purpose:     wxXml2DTD and various DTD nodes wrappers
// Author:      Francesco Montorsi
// Created:     2005/01/01
// RCS-ID:      $Id: dtd.cpp 1235 2010-03-10 19:47:01Z frm $
// Copyright:   (c) 2005 Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
    #include "wx/datstrm.h"
    #include "wx/zstream.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/strconv.h"
#endif

#include "wx/wfstream.h"        // not included by wxprec.h
#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/xml2.h"
#include "wx/dtd.h"



// implementation of dynamic classes
IMPLEMENT_DYNAMIC_CLASS(wxXml2ElemDecl, wxXml2BaseNode)
IMPLEMENT_DYNAMIC_CLASS(wxXml2AttrDecl, wxXml2BaseNode)
IMPLEMENT_DYNAMIC_CLASS(wxXml2EntityDecl, wxXml2BaseNode)
IMPLEMENT_DYNAMIC_CLASS(wxXml2DTD, wxXml2Wrapper)

IMPLEMENT_ABSTRACT_CLASS(wxXml2HelpWrapper, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXml2Enumeration, wxXml2HelpWrapper)
IMPLEMENT_DYNAMIC_CLASS(wxXml2ElemContent, wxXml2HelpWrapper)


// global instances (they wraps NULL pointers)
wxXml2DTD wxXml2EmptyDTD(NULL);
wxXml2Enumeration wxXml2EmptyEnumeration(NULL);
wxXml2ElemDecl wxXml2EmptyElemDecl(NULL);
wxXml2AttrDecl wxXml2EmptyAttrDecl(NULL);
wxXml2EntityDecl wxXml2EmptyEntityDecl(NULL);
wxXml2ElemContent wxXml2EmptyElemContent(NULL);





//-----------------------------------------------------------------------------
//  wxXml2HelpWrapper
//-----------------------------------------------------------------------------

void wxXml2HelpWrapper::DestroyIfUnlinked()
{
    if (!m_bLinked) {

        Destroy();
        wxLogDebug(wxS("%s::DestroyIfUnlinked - destroyed"),
            GetClassInfo()->GetClassName());

    } else {

        wxLogDebug(wxS("%s::DestroyIfUnlinked - NOT destroyed (because linked)"),
            GetClassInfo()->GetClassName());
    }
}





//-----------------------------------------------------------------------------
//  wxXml2ElemDecl
//-----------------------------------------------------------------------------

void wxXml2ElemDecl::Create(const wxXml2DTD &parent, const wxString &name,
                            wxXml2ElementTypeVal val, wxXml2ElemContent &content)
{
    // we should do:
    // if (content != wxXml2EmptyElemContent)
    //     content.Link();
    // as wxXml2AttrDecl does with the wxXml2Enumeration objects;
    // however, for some reason, xmlAddElementDecl will _copy_ the
    // given xmlElementContent object and thus we must *not*
    // tell to the given wxXml2ElemContent that libxml2 will free
    // its memory...

    UnwrappingOld();
    m_obj = (wxXml2BaseNodeObj*)xmlAddElementDecl(NULL, parent.GetObj(), WX2XML(name),
                                            (xmlElementTypeVal)val, content.GetObj());
    JustWrappedNew();
}

bool wxXml2ElemDecl::operator==(const wxXml2ElemDecl &n) const
{
    wxCHECK_NULL_POINTERS(GetObj(), n.GetObj());

    if (GetObj()->etype == n.GetObj()->etype &&
        GetObj()->content == n.GetObj()->content &&
        GetObj()->attributes == n.GetObj()->attributes &&
        XML2WX(GetObj()->prefix).Cmp(XML2WX(n.GetObj()->prefix)) == 0)
        return TRUE;
    return FALSE;
}

wxXml2AttrDecl wxXml2ElemDecl::GetAttributes() const
{ if (GetObj()) return wxXml2AttrDecl(GetObj()->attributes); return wxXml2EmptyAttrDecl; }

wxXml2DTD wxXml2ElemDecl::GetParent() const
{ if (GetObj()) return wxXml2DTD(GetObj()->parent); return wxXml2EmptyDTD; }




//-----------------------------------------------------------------------------
//  wxXml2AttrDecl
//-----------------------------------------------------------------------------

void wxXml2AttrDecl::Create(const wxXml2DTD &parent, const wxString &element,
                            const wxString &name, const wxXml2Namespace &ns,
                            wxXml2AttributeType type, wxXml2AttributeDefault def,
                            const wxString &defaultval, wxXml2Enumeration &e)
{
    xmlEnumeration *enumeration = NULL;
    if (e != wxXml2EmptyEnumeration) {
        enumeration = e.GetObj();

        // libxml2 will free the xmlEnumeration when freeing this xmlAttribute...
        e.Link();
    }

    xmlChar *prefix = NULL;
    if (ns != wxXml2EmptyNamespace) prefix = WX2XML(ns.GetPrefix());

    UnwrappingOld();
    m_obj = (wxXml2BaseNodeObj*)
            xmlAddAttributeDecl(NULL, parent.GetObj(), WX2XML(element),
                                WX2XML(name), prefix,
                                (xmlAttributeType)type, (xmlAttributeDefault)def,
                                WX2XML(defaultval), enumeration);
    JustWrappedNew();
}

bool wxXml2AttrDecl::operator==(const wxXml2AttrDecl &n) const
{
    wxCHECK_NULL_POINTERS(GetObj(), n.GetObj());

    if (GetObj()->atype == n.GetObj()->atype &&
        GetObj()->def == n.GetObj()->def &&
        XML2WX(GetObj()->defaultValue).Cmp(XML2WX(n.GetObj()->defaultValue)) == 0 &&
        XML2WX(GetObj()->prefix).Cmp(XML2WX(n.GetObj()->prefix)) == 0 &&
        XML2WX(GetObj()->elem).Cmp(XML2WX(n.GetObj()->elem)) == 0 &&
        GetObj()->tree == n.GetObj()->tree)
        return TRUE;
    return FALSE;
}

wxXml2DTD wxXml2AttrDecl::GetParent() const
{ if (GetObj()) return wxXml2DTD(GetObj()->parent); return wxXml2EmptyDTD; }





//-----------------------------------------------------------------------------
//  wxXml2EntityDecl
//-----------------------------------------------------------------------------

void wxXml2EntityDecl::Create(const wxXml2DTD &parent, const wxString &name,
                            wxXml2EntityType type, const wxString &externalID,
                            const wxString &systemID, const wxString &content)
{
    xmlDoc tmp;
    memset(&tmp, 0, sizeof(xmlDoc));
    tmp.intSubset = parent.GetObj();

    UnwrappingOld();
    m_obj = (wxXml2BaseNodeObj*)
            xmlAddDocEntity(&tmp, WX2XML(name),
                            (xmlEntityType)type, WX2XML(externalID),
                            WX2XML(systemID), WX2XML(content));
    JustWrappedNew();
}

bool wxXml2EntityDecl::operator==(const wxXml2EntityDecl &n) const
{
    wxCHECK_NULL_POINTERS(GetObj(), n.GetObj());

    if (GetObj()->etype == n.GetObj()->etype &&
        XML2WX(GetObj()->ExternalID).Cmp(XML2WX(n.GetObj()->ExternalID)) == 0 &&
        XML2WX(GetObj()->SystemID).Cmp(XML2WX(n.GetObj()->SystemID)) == 0)
        return TRUE;
    return FALSE;
}

wxXml2DTD wxXml2EntityDecl::GetParent() const
{ if (GetObj()) return wxXml2DTD(GetObj()->parent); return wxXml2EmptyDTD; }




//-----------------------------------------------------------------------------
//  wxXml2ElemContent
//-----------------------------------------------------------------------------

void wxXml2ElemContent::Create(const wxString &name, wxXml2ElementContentType val,
                            wxXml2ElementContentOccur occ)
{
    // create the new object
    m_cont = xmlNewElementContent(WX2XML(name), (xmlElementContentType)val);
    m_cont->ocur = (xmlElementContentOccur)occ;
}

bool wxXml2ElemContent::operator==(const wxXml2ElemContent &n) const
{
    wxCHECK_NULL_POINTERS(n.GetObj(), GetObj());

    if (XML2WX(GetObj()->name).Cmp(XML2WX(n.GetObj()->name)) == 0 &&
        XML2WX(GetObj()->prefix).Cmp(XML2WX(n.GetObj()->prefix)) == 0 &&
        GetObj()->type == n.GetObj()->type)
        return TRUE;
    return FALSE;
}




//-----------------------------------------------------------------------------
//  wxXml2Enumeration
//-----------------------------------------------------------------------------

void wxXml2Enumeration::Create(const wxString &name, const wxXml2Enumeration &next)
{
    // create the enumeration
    m_enum = xmlCreateEnumeration(WX2XML(name));
    m_enum->next = next.GetObj();
}

void wxXml2Enumeration::Create(const wxString &list)
{
    wxStringTokenizer tknzr(list, wxS("|"));
    if (!tknzr.HasMoreTokens()) return;

    Create(tknzr.GetNextToken(), wxXml2EmptyEnumeration);
    while (tknzr.HasMoreTokens()) {

        // create a new enumeration and append it to this list
        xmlEnumeration *next = xmlCreateEnumeration(WX2XML(tknzr.GetNextToken()));

        //
        Append(next);
    }
}

void wxXml2Enumeration::Append(xmlEnumeration *e)
{
    wxXml2Enumeration &p = *this;
    while (p.GetObj()->next != NULL)
        p = p.GetObj()->next;

    p.GetObj()->next = e;
}

bool wxXml2Enumeration::operator==(const wxXml2Enumeration &n) const
{
    wxCHECK_NULL_POINTERS(GetObj(), n.GetObj());

    if (XML2WX(n.GetObj()->name).Cmp(XML2WX(GetObj()->name)) == 0)
        return TRUE;
    return FALSE;
}




//-----------------------------------------------------------------------------
//  wxXml2DTD
//-----------------------------------------------------------------------------

void wxXml2DTD::Create(const wxXml2Document &doc,
                       const wxString &name, const wxString &externalID,
                        const wxString &systemid)
{
    UnwrappingOld();

    // xmlNewDtd API will link this DTD with the given document...
    m_dtd = xmlNewDtd(doc.GetObj(), WX2XML(name),
                    WX2XML(externalID), WX2XML(systemid));
    JustWrappedNew();
}

bool wxXml2DTD::operator==(const wxXml2DTD &dtd) const
{
    wxCHECK_NULL_POINTERS(dtd.GetObj(), GetObj());

    if (GetName() == dtd.GetName() &&
        GetSystemID() == dtd.GetSystemID() &&
        GetExternalID() == dtd.GetExternalID())
        return TRUE;
    return FALSE;
}

bool wxXml2DTD::IsOk() const
{
    if (m_dtd == NULL)
        return FALSE;

    if (GetName() == wxEmptyString)
        return FALSE;

    // if we are an external reference, then we should
    // be a SYSTEM or a PUBLIC subset...
    if (IsExternalReference() &&
        !IsSystemSubset() &&
        !IsPublicSubset())
        return FALSE;

    return TRUE;
}

bool wxXml2DTD::Load(const wxString &filename, wxString *pErr)
{
    wxFileInputStream stream(filename);
    if (!stream.IsOk() || !wxFileName::FileExists(filename)) return FALSE;
    return Load(stream, pErr);
}

bool wxXml2DTD::Save(const wxString &filename, long flags) const
{
    wxFileOutputStream stream(filename);
    if (!stream.IsOk()) return FALSE;
    return (Save(stream, flags) != -1);
}

// helper function for wxXml2DTD::Load
static void XMLDTDMsg(void *ctx, const char *pszFormat, ...)
{
    // get the variable argument list
    va_list argptr;
    va_start(argptr, pszFormat);
    wxString str = wxString::FormatV(wxString(pszFormat, wxConvUTF8), argptr);
    va_end(argptr);

    // append the error string to the private member of the parser context
    xmlParserCtxt *p = (xmlParserCtxt *)ctx;
    wxString *err = (wxString *)p->_private;
    if (err != NULL) *err += str;
}

// helper function for wxXml2DTD::Load
static int XMLDTDRead(void *ctx, char *buffer, int len)
{
    wxInputStream *stream = (wxInputStream*)ctx;
    return stream->Read(buffer, len).LastRead();
}

bool wxXml2DTD::Load(wxInputStream &stream, wxString *pErr)
{
    if (stream.GetSize() <= 0) {
        if (pErr) *pErr = wxS("Invalid size");
        return FALSE;
    }

    // are we attached to a wxXml2Document ?
    xmlDoc *doc = (m_dtd ? m_dtd->parent : NULL);
    if (doc) xmlUnlinkNode((xmlNode *)m_dtd);

    // init the SAX handler
    xmlSAXHandler h;
    memset(&h, 0, sizeof(h));
    xmlSAX2InitDefaultSAXHandler(&h, 0);

    // set the error & warning handlers
    h.error = XMLDTDMsg;
    h.warning = XMLDTDMsg;
    h._private = pErr;

    // xmlSAXHandler::fatalError is unused (in LibXML2 2.5.x),
    // however, we set it anyway to support future versions...
    h.fatalError = XMLDTDMsg;

    // setup our input buffer
    xmlParserInputBuffer *myparserbuf = xmlAllocParserInputBuffer(XML_CHAR_ENCODING_NONE);
    myparserbuf->readcallback = XMLDTDRead;
    myparserbuf->context = &stream;

    // parse from buffer
    UnwrappingOld();
    m_dtd = xmlIOParseDTD(&h, myparserbuf, XML_CHAR_ENCODING_NONE);
    JustWrappedNew();        // don't forget this

    // NOT REQUIRED: xmlIOParseDTD automatically does it
    // xmlFreeParserInputBuffer(myparserbuf);

    if (!IsOk())
        return FALSE;        // don't proceed

    // reattach to the old wxXml2Document, if there was one...
    if (doc)
        wxXml2Document(doc).SetDTD(*this);

    // the xmlParserInputBuffer has been already freed by libxml2
    // and the SAX handler is on the stack... we can return
    return TRUE;
}

// helper function for wxXml2DTD::Save
static int XMLDTDWrite(void *context, const char *buffer, int len)
{
    wxOutputStream *stream = (wxOutputStream *)context;
    wxASSERT_MSG(stream && stream->IsOk(), wxS("Invalid stream"));
    int written = 0;

    // even if our m_dtd->doc pointer is NULL, xmlNodeDumpOutput
    // will create the header:
    //
    //  <!DOCTYPE none PUBLIC "none" "none" [    and the footer    ]>
    //
    // so, we must remove them...

    const char *towrite = buffer;
    if (strncmp(buffer, "<!DOCTYPE", 9) == 0) {

        // discard all characters placed before the first "[" symbol
        while (towrite[0] != '[' && len > 0) {
            towrite++;
            written++;        // consider this char as written
            len--;
        }

        // remove also the "[" symbol...
        towrite++;
        written++;        // consider this char as written
        len--;

        // eventually remove also the newline which follow it...
        if (towrite[0] == '\n') {
            towrite++;
            written++;        // consider this char as written
            len--;
        }
    }

    if (strncmp(towrite+len-2, "]>", 2) == 0) {

        // remove the "]>" symbol...
        len -= 2;
        written += 2;        // consider this char as written
    }

    // write
    stream->Write(towrite, len);

    // the last write in the stream could report a number of bytes written
    // different from the buffer lenght we gave to the wxOutputStream:
    // see wxNativeNewlinesFilterStream::OnSysWrite for example...

    // ...so, we check if everything is okay in the stream...
    if (!stream->IsOk())
        return -1;

    // ...if the stream did not report any error, this is okay and
    // we will return the "written+len" value which is what libxml2 expects....
    return written+len;
}

int wxXml2DTD::Save(wxOutputStream &stream, long flags) const
{
    wxNativeNewlinesFilterStream *filter = NULL;        // created only if required
    wxOutputStream *mystream = &stream;

    // will we use the native newline mode ?
    if (flags & wxXML2DOC_USE_NATIVE_NEWLINES) {

        // use a filter stream to change libxml2 newlines to native newlines
        filter = new wxNativeNewlinesFilterStream(stream);
        mystream = filter;
    }

    int originalsize = stream.GetSize();

    // setup the output buffer
    xmlOutputBuffer *buf = xmlAllocOutputBuffer(NULL);
    buf->context = mystream;
    buf->writecallback = XMLDTDWrite;

    // dump this DTD node
    xmlNodeDumpOutput(buf, NULL, (xmlNode *)m_dtd, 1, 0, NULL);
    xmlOutputBufferFlush(buf);

    // free the output buffer
    xmlOutputBufferClose(buf);

    // free our filter if required
    if (filter) delete filter;

    // we won't return buf->written because our XMLDTDWrite()
    // function could have tweaked that number to cheat libxml2:
    // the real number of bytes written can be safely taken from
    // the wxOutputStream...
    return stream.GetSize()-originalsize;
}

bool wxXml2DTD::IsPublicSubset() const
{
    // if we are public, then we should have both a
    // non-empty SystemID (which contains our external URI, in reality)
    // and a non-empty ExternalID
    return m_dtd->SystemID != NULL &&
        m_dtd->ExternalID != NULL;
}

bool wxXml2DTD::IsSystemSubset() const
{
    // SYSTEM subsets only need a SystemID so if we are really
    // a system subset we should have an empty ExternalID
    return m_dtd->SystemID != NULL &&
        m_dtd->ExternalID == NULL;
}

bool wxXml2DTD::IsExternalReference() const
{
    // just check if we have any children: if we don't
    // it is because we are just a sort of link to a
    // full DTD located somewhere else...
    if (GetRoot() == wxXml2EmptyNode)
        return TRUE;
    return FALSE;
}

wxXml2BaseNode wxXml2DTD::GetRoot() const
{
    if (m_dtd == NULL || m_dtd->children == NULL)
        return wxXml2EmptyNode;

    wxXml2BaseNode ret((wxXml2BaseNodeObj*)m_dtd->children);

#ifdef __WXDEBUG__
    // be sure that our children are DTD declarations...
    wxXml2NodeType t = ret.GetType();
    wxASSERT(t == wxXML2_ELEMENT_DECL ||
        t == wxXML2_ATTRIBUTE_DECL ||
        t == wxXML2_ENTITY_DECL ||
        t == wxXML2_NAMESPACE_DECL ||
        t == wxXML2_COMMENT_NODE);        // don't forget comments: they are allowed in DTDs
#endif
    return ret;
}

void wxXml2DTD::SetRoot(wxXml2BaseNode &node)
{
    // FIXME: we need to update only parent/children pointers ?
    m_dtd->children = (xmlNode*)node.GetObj();
    node.GetObj()->parent = (wxXml2BaseNodeObj*)m_dtd;
}

bool wxXml2DTD::LoadFullDTD(wxString *perr)
{
    if (IsSystemSubset()) {

        // we have our DTD somewhere on this system:
        // load it supposing that the SystemID which
        // should be an URI for XML specification
        // is just the name of our file...
        return Load(GetSystemID(), perr);

    }

    wxASSERT(IsPublicSubset());

    // public DTD loading is not implemented yet:
    // we would need a URI parser to do that and
    // we should also create an HTTP connection, probably....
    // let me know if you need this feature.
    //wxASSERT(0);
    if (perr)
        *perr = wxS("PUBLIC DTD loading not supported.");

    return FALSE;
}

void wxXml2DTD::SetName(const wxString &str)
{
    // reset the name string
    xmlFree((void *)m_dtd->name);
    m_dtd->name = xmlStrdup(WX2XML(str));
}

void wxXml2DTD::SetSystemID(const wxString &str)
{
    // reset the SystemID string
    xmlFree((void *)m_dtd->SystemID);
    m_dtd->SystemID = xmlStrdup(WX2XML(str));
}

void wxXml2DTD::SetExternalID(const wxString &str)
{
    // reset the ExternalID string
    xmlFree((void *)m_dtd->ExternalID);
    m_dtd->ExternalID = xmlStrdup(WX2XML(str));
}

void wxXml2DTD::AddElemDecl(const wxString &name,
                            wxXml2ElementTypeVal val,
                            wxXml2ElemContent &content)
{
    // we just need to create a wxXml2ElemDecl.
    wxXml2ElemDecl n(*this, name, val, content);

    // the element will be destroyed but not its libxml2 structure
    // since it has been linked to *this object
}

void wxXml2DTD::AddAttrDecl(const wxString &element,
                const wxString &name,
                const wxXml2Namespace &ns,
                wxXml2AttributeType type,
                wxXml2AttributeDefault def,
                const wxString &defaultval,
                /*const*/ wxXml2Enumeration &e)
{
    // we just need to create a wxXml2AttrDecl.
    wxXml2AttrDecl n(*this, element, name, ns, type, def, defaultval, e);

    // the element will be destroyed but not its libxml2 structure
    // since it has been linked to *this object
}

void wxXml2DTD::AddEntityDecl(const wxString &name,
                    wxXml2EntityType type,
                    const wxString &externalID,
                    const wxString &systemID,
                    const wxString &content)
{
    // we just need to create a wxXml2EntityDecl.
    wxXml2EntityDecl n(*this, name, type, externalID, systemID, content);

    // the element will be destroyed but not its libxml2 structure
    // since it has been linked to *this object
}



