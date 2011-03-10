/////////////////////////////////////////////////////////////////////////////
// Name:        xml.cpp
// Purpose:     wxXml2Document - XML parser & data holder class
// Author:      Francesco Montorsi (based on Vaclav Slavik's design)
// Created:     2003/09/05
// RCS-ID:      $Id: xml2.cpp 1235 2010-03-10 19:47:01Z frm $
// Copyright:   (c) 2000 Vaclav Slavik and (c) 2003 Francesco Montorsi
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

#include "wx/filename.h"
#include "wx/xml2.h"
#include "wx/dtd.h"


// implementation of dynamic classes
IMPLEMENT_ABSTRACT_CLASS(wxXml2Wrapper, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXml2BaseNode, wxXml2Wrapper)
IMPLEMENT_DYNAMIC_CLASS(wxXml2Node, wxXml2BaseNode)

IMPLEMENT_DYNAMIC_CLASS(wxXml2Document, wxXml2Wrapper)
IMPLEMENT_DYNAMIC_CLASS(wxXml2Property, wxXml2Wrapper)
IMPLEMENT_DYNAMIC_CLASS(wxXml2Namespace, wxXml2Wrapper)


// global instances (they wraps NULL pointers)
wxXml2Document wxXml2EmptyDoc(NULL);
wxXml2Node wxXml2EmptyNode(NULL);
wxXml2Property wxXml2EmptyProperty(NULL);
wxXml2Namespace wxXml2EmptyNamespace(NULL, wxXml2EmptyNode);
wxXml2BaseNode wxXml2EmptyBaseNode(NULL);


// statics
int wxXml2::m_nOld;
char wxXml2::m_strIndent[];
const char *wxXml2::m_strOld;





//-----------------------------------------------------------------------------
//  wxXml2
//-----------------------------------------------------------------------------

void wxXml2::SetIndentMode(bool benable, int indentstep)
{
    // we need to call xmlKeepBlanksDefault with FALSE if we want
    // to _enable_ the indentation when saving...
    m_nOld = xmlKeepBlanksDefault(!benable);

    // save current indentation string and replace it with our
    if (benable && indentstep > 0 && indentstep < 32) {

        memset(m_strIndent, ' ', indentstep);
        m_strIndent[indentstep] = '\0';

        m_strOld = xmlTreeIndentString;
        xmlTreeIndentString = m_strIndent;
    }
}

void wxXml2::RestoreLastIndentMode()
{
    xmlKeepBlanksDefault(m_nOld);
    xmlTreeIndentString = m_strOld;
}





//-----------------------------------------------------------------------------
//  wxXml2Wrapper
//-----------------------------------------------------------------------------

void wxXml2Wrapper::DestroyIfUnlinked()
{
    if (IsNonEmpty() == FALSE) {

        wxLogDebug(wxS("%s::DestroyIfUnlinked - nothing to destroy (empty)"),
            GetClassInfo()->GetClassName());
        return;
    }

    DecRefCount();
    int refcount = GetRefCount();

    // we must check not only if this element is unlinked from a
    // wider XML tree (if it is, then the root of that tree will
    // destroy all its children recursively) but also if it is
    // wrapped by another wxXml2Wrapper-derived class:
    // in this case we must not destroy the libxml2 structure,
    // otherwise the other wrapper would almost certainly throw
    // an exception since it would try to access invalid memory...
    bool unlinked = IsUnlinked();
    if (unlinked && refcount == 0) {

        // ... anyway, if there are no parents of this structure
        // and no wrappers are embedding it, then we can safely
        // destroy it.
        Destroy();
        wxLogDebug(wxS("%s::DestroyIfUnlinked - destroyed"),
            GetClassInfo()->GetClassName());

    } else {

        // the memory associated with this property will be freed
        // by the node which owns this object or by another wrapper
        SetAsEmpty();

        wxLogDebug(wxS("%s::DestroyIfUnlinked - NOT destroyed (because %s)"),
            GetClassInfo()->GetClassName(),
            (!unlinked ? wxS("linked") :
                    wxString::Format(wxS("refcount is %d"), refcount).c_str()));
    }
}



//-----------------------------------------------------------------------------
//  wxXml2BaseNode
//-----------------------------------------------------------------------------

void wxXml2BaseNode::SetNext(const wxXml2BaseNode &next)
{
    if (wxXml2EmptyBaseNode != next)
        xmlAddNextSibling((xmlNode *)m_obj, (xmlNode *)next.GetObj());
    else
        m_obj->next = NULL;
}

void wxXml2BaseNode::SetChildren(const wxXml2BaseNode &n)
{
    // FIXME: we need to update only parent/children pointers ?
    m_obj->children = n.GetObj();
    n.GetObj()->parent = m_obj;
}

bool wxXml2BaseNode::IsUnlinked() const
{
    if (m_obj == NULL)
        return TRUE;

    // if we have all null pointers, we are not linked anywhere
    if (m_obj != NULL && m_obj->parent == NULL &&
        m_obj->prev == NULL &&    m_obj->next == NULL)
        return TRUE;

    // at least one link is valid
    return FALSE;
}

bool wxXml2BaseNode::operator==(const wxXml2BaseNode &n) const
{
    wxCHECK_NULL_POINTERS(n.GetObj(), GetObj());

    if (n.GetObj() == GetObj())
        return TRUE;
    return FALSE;
}



//-----------------------------------------------------------------------------
//  wxXml2Property
//-----------------------------------------------------------------------------

bool wxXml2Property::operator==(const wxXml2Property &p) const
{
    wxCHECK_NULL_POINTERS(p.GetObj(), GetObj());

    // the same name and same value are required
    if (GetName().CmpNoCase(p.GetName()) == 0 &&
        GetValue() == p.GetValue())
        return TRUE;
    return FALSE;
}

void wxXml2Property::Create(const wxString &name, const wxString &value, wxXml2Node &owner)
{
    // if this property was already build, first destroy it
    UnwrappingOld();

    // create the property; if the owner is an empty node, the property
    // will be created unlinked from the main tree.
    m_attr = xmlNewProp(owner.GetObj(), WX2XML(name), WX2XML(value));
    JustWrappedNew();
}

bool wxXml2Property::IsUnlinked() const
{
    if (m_attr == NULL)
        return TRUE;

    // if all links are NULL, then we are unlinked...
    if (m_attr != NULL && m_attr->parent == NULL &&
        m_attr->next == NULL && m_attr->prev == NULL)
        return TRUE;

    // at least one is set: we're linked
    return FALSE;
}

void wxXml2Property::SetValue(const wxString &value)
{
    // reset the TEXT child node
    xmlFree((void *)m_attr->children->content);
    m_attr->children->content = xmlStrdup(WX2XML(value));
}

void wxXml2Property::SetName(const wxString &name)
{
    // reset the NAME field
    xmlFree((void *)m_attr->name);
    m_attr->name = xmlStrdup(WX2XML(name));
}

void wxXml2Property::SetNext(wxXml2Property &next)
{
    // reset parent relations
    if (GetNext() != wxXml2EmptyProperty)
        GetNext().SetPrevious(wxXml2EmptyProperty);
    m_attr->next = next.GetObj();

    // cannot use "next.SetPrevious(*this)" because if SetPrevious()
    // makes use of SetNext() on the given node, a stack overflow would happen
    next.GetObj()->prev = m_attr;
}

void wxXml2Property::SetPrevious(wxXml2Property &prev)
{
    // reset parent relations
    if (GetPrevious() != wxXml2EmptyProperty)
        GetPrevious().SetNext(wxXml2EmptyProperty);
    m_attr->prev = prev.GetObj();

    // cannot use "prev.SetNext(*this)" because if SetNext()
    // makes use of SetPrevious() on the given node, a stack overflow would happen
    prev.GetObj()->next = m_attr;
}

void wxXml2Property::SetOwner(wxXml2Node &owner)
{
    // we are forced to recreate this property because the owner has changed;
    // Create() will care about the destruction and the unlinking of the old
    // property...
    Create(GetName(), GetValue(), owner);
}

wxXml2Node wxXml2Property::GetOwner() const
{
    // this function cannot be declared in the header because it makes
    // use of the contructor of wxXml2Node...
    return wxXml2Node(m_attr->parent);
}




//-----------------------------------------------------------------------------
//  wxXml2Namespace
//-----------------------------------------------------------------------------

wxXml2Namespace::wxXml2Namespace(xmlNs *ns, wxXml2Node &owner)
: m_ns(ns), m_owner(owner.GetObj())
{ JustWrappedNew(); }

bool wxXml2Namespace::operator==(const wxXml2Namespace &ns) const
{
    wxCHECK_NULL_POINTERS(ns.GetObj(), GetObj());

    if (GetPrefix() == ns.GetPrefix() &&
        GetURI().CmpNoCase(ns.GetURI()) == 0)
        return TRUE;
    return FALSE;
}

void wxXml2Namespace::Create(const wxString &prefix,
                           const wxString &uri,
                           wxXml2Node &owner)
{
    // if this namespace was already built, first destroy it
    UnwrappingOld();

    // create the property; if the owner is an empty node, the property
    // will be created unlinked from the main tree.
    wxASSERT_MSG(owner != wxXml2EmptyNode,
              wxS("If I create an unlinked namespace, its ")
              wxS("memory will never be freed..."));
    m_ns = xmlNewNs(owner.GetObj(), WX2XML(uri), WX2XML(prefix));
    m_owner = owner.GetObj();        // update also the owner pointer
    JustWrappedNew();

    // just to avoid to be forced to copy-and-paste the checks done in
    // wxXml2Namespace::SetPrefix, make a dummy call to that function
    SetPrefix(prefix);
}

void wxXml2Namespace::SetPrefix(const wxString &p)
{
    xmlFree((void *)m_ns->prefix);
    m_ns->prefix = xmlStrdup(WX2XML(p));

    // if prefix changed, maybe we must update the main
    // node which uses this namespace
    /*if (m_owner != NULL) {
        if (!p.IsEmpty())
            m_owner->SetNamespace(*this);
        else
            m_owner->SetNamespace(wxXml2EmptyNamespace);
    }*/
}

void wxXml2Namespace::SetURI(const wxString &href)
{
    xmlFree((void *)m_ns->href);
    m_ns->href = xmlStrdup(WX2XML(href));
}




//-----------------------------------------------------------------------------
//  wxXml2Node - creation functions
//-----------------------------------------------------------------------------

void wxXml2Node::CreateRoot(wxXml2Document &doc,
                           const wxString &name,
                           wxXml2Namespace &ns,
                           wxXml2Property &props)
{
    Build(wxXML2_ELEMENT_NODE, wxXml2EmptyNode, doc, ns, name,
        wxEmptyString, props, wxXml2EmptyNode);

    // a last touch
    xmlDocSetRootElement(doc.GetObj(), GetObj());
}

void wxXml2Node::CreateChild(wxXml2NodeType type,
                            wxXml2Node &parent,
                            const wxString &name,
                            const wxString &content,
                            wxXml2Namespace &ns,
                            wxXml2Property &props,
                            wxXml2Node &next)
{
    Build(type, parent, wxXml2EmptyDoc, ns, name, content, props, next);
}

void wxXml2Node::CreateTemp(wxXml2NodeType type,
                           wxXml2Document &doc,
                           const wxString &name,
                           const wxString &content,
                           wxXml2Namespace &ns,
                           wxXml2Property &props)
{
    Build(type, wxXml2EmptyNode, doc, ns, name, content, props, wxXml2EmptyNode);
}


void wxXml2Node::Build(const wxXml2NodeType type, wxXml2Node &parent,
                      wxXml2Document &doc, wxXml2Namespace &ns,
                      const wxString &name, const wxString &content,
                      wxXml2Property &prop, wxXml2Node &next,
                      wxXml2Node &previous, wxXml2Node &children)
{
    // if this node was already build, first destroy it
    UnwrappingOld();

    // then, build the node; this passage should set:
    // - the type of the node
    // - the parent and/or the document which owns this node
    // - the name and/or content
    // - the namespace associated with this node
    switch (type) {
    case wxXML2_TEXT_NODE:

        // is the content of this text child an entity ?
        if (content.StartsWith(wxS("&")) && content.Last() == ';') {

            // yes, it is; just check if the caller
            // also wants to create a tag which wraps the entity
            if (!name.IsEmpty()) {

                // first create an element node....
                Build(wxXML2_ELEMENT_NODE, parent, doc, ns, name, wxEmptyString, prop, next, previous);

                // and then adds the entity into a new text child...
                wxXml2Node entity;
                entity.Build(wxXML2_TEXT_NODE, parent, doc, ns, wxEmptyString, content);
                AddChild(entity);

            } else {

                // yes, it is.... we just need to create a wxXML2_ENTITY_REF_NODE nodetype
                m_obj = (wxXml2BaseNodeObj *)xmlNewReference(doc.GetObj(), WX2XML(content));
                JustWrappedNew();
                SetNamespace(ns);
            }
            break;
        }

        // not, it wasn't an entity node
        if (!name.IsEmpty()) {

            if (wxXml2EmptyNode != parent) {
                m_obj = (wxXml2BaseNodeObj *)xmlNewTextChild(parent.GetObj(),
                                    ns.GetObj(), WX2XML(name), WX2XML(content));
                JustWrappedNew();

            } else {
                // this function can accept NULL as first argument
                m_obj = (wxXml2BaseNodeObj *)xmlNewDocRawNode(doc.GetObj(), ns.GetObj(),
                                    WX2XML(name), WX2XML(content));
                JustWrappedNew();
            }

        } else {

            // do not use libxml2 xmlNewTextChild or xmlNewDocRawNode functions:
            // they would create first a wxXML2_ELEMENT_NODE with a wxXML2_TEXT_NODE
            // as child; instead we want to create directly a wxXML2_TEXT_NODE...
            if (wxXml2EmptyNode != parent) {
                m_obj = (wxXml2BaseNodeObj *)xmlNewText(WX2XML(content));
                parent.AddChild(*this);
                JustWrappedNew();

            } else {

                // this function can accept NULL as first argument
                m_obj = (wxXml2BaseNodeObj *)xmlNewDocText(doc.GetObj(), WX2XML(content));
                JustWrappedNew();
            }
        }
        break;

    case wxXML2_ELEMENT_NODE:
        if (wxXml2EmptyNode != parent) {

            wxASSERT_MSG(parent.GetType() == wxXML2_ELEMENT_NODE,
                wxS("Cannot add children to a non-container node. If you want to add ")
                wxS("tags after special nodes like wxXML2_PI_NODE or wxXML2_CDATA_NODE, ")
                wxS("use one of wxXml2Node::Add***Child() on the parent of the special ")
                wxS("node. Special nodes cannot have children."));
            m_obj = (wxXml2BaseNodeObj *)xmlNewChild(parent.GetObj(), ns.GetObj(), WX2XML(name), NULL);
            JustWrappedNew();

        } else {

            // this function can accept NULL as first argument
            m_obj = (wxXml2BaseNodeObj *)xmlNewDocRawNode(doc.GetObj(), ns.GetObj(), WX2XML(name), NULL);
            JustWrappedNew();
        }
        break;

    case wxXML2_COMMENT_NODE:
        if (wxXml2EmptyNode != parent) {
            m_obj = (wxXml2BaseNodeObj *)xmlNewComment(WX2XML(content));
            JustWrappedNew();

            // set this comment as child of the given parent
            parent.AddChild(*this);
        } else {

            // this function can accept NULL as first argument
            m_obj = (wxXml2BaseNodeObj *)xmlNewDocComment(doc.GetObj(), WX2XML(content));
            JustWrappedNew();
        }
        break;

    case wxXML2_CDATA_SECTION_NODE:
        m_obj = (wxXml2BaseNodeObj *)xmlNewChild(parent.GetObj(), ns.GetObj(), WX2XML(name), NULL);
        JustWrappedNew();

        // change the type of this node & its contents
        m_obj->type = (xmlElementType)wxXML2_CDATA_SECTION_NODE;
        xmlNodeSetContent((xmlNode *)m_obj, WX2XML(content));
        break;

    case wxXML2_PI_NODE:
        m_obj = (wxXml2BaseNodeObj *)xmlNewPI(WX2XML(name), WX2XML(content));
        JustWrappedNew();

        wxASSERT_MSG(doc != wxXml2EmptyDoc,
            wxS("A PI node must have no parents: it must be attached directly to a ")
            wxS("document since it must be placed before any other type of node"));

        // prepend the node to the children list of the document
        xmlAddPrevSibling(doc.GetObj()->children, (xmlNode *)m_obj);
        break;

    case wxXML2_ENTITY_REF_NODE:
        wxASSERT_MSG(0, wxS("Such node is not supported directly: you should create ")
            wxS("a text node (wxXML2_TEXT_NODE) whose content is an entity. In this ")
            wxS("way, a wxXML2_ENTITY_REF_NODE node will be created automatically"));

    case wxXML2_DTD_NODE:
    case wxXML2_DOCUMENT_NODE:
    case wxXML2_HTML_DOCUMENT_NODE:
    case wxXML2_NAMESPACE_DECL:
        wxASSERT_MSG(0, wxS("Such type of node cannot be created !!\n")
                        wxS("You must use wxXml2Document, wxXml2DTD or wxXml2Namespace ")
                        wxS("to create such types of 'nodes'..."));

    case wxXML2_ELEMENT_DECL:
    case wxXML2_ATTRIBUTE_DECL:
    case wxXML2_ENTITY_DECL:
        wxASSERT_MSG(0, wxS("still not wrapped"));
        break;
    }

    // now, set up:
    // - prop
    // - next, previous & children
    if (wxXml2EmptyProperty != prop) AddProperty(prop);
    if (wxXml2EmptyNode != previous) AddPrevious(previous);
    if (wxXml2EmptyNode != next) AddNext(previous);
    if (wxXml2EmptyNode != children) AddChild(children);
}

wxXml2Node wxXml2Node::AddContainerChild(const wxString &name)
{
    wxXml2Node child;
    child.CreateChild(wxXML2_ELEMENT_NODE, *this, name);
    return child;
}

wxXml2Node wxXml2Node::AddTextChild(const wxString &name, const wxString &content)
{
    wxXml2Node child;
    child.CreateChild(wxXML2_TEXT_NODE, *this, name, content);
    return child;
}

wxXml2Node wxXml2Node::AddCommentChild(const wxString &content)
{
    wxXml2Node child;
    child.CreateChild(wxXML2_COMMENT_NODE, *this, wxS(""), content);
    return child;
}

wxXml2Node wxXml2Node::AddCDATAChild(const wxString &content)
{
    wxXml2Node child;
    child.CreateChild(wxXML2_CDATA_SECTION_NODE, *this, wxS(""), content);
    return child;
}

wxXml2Node wxXml2Node::AddPIChild(const wxString &name, const wxString &content)
{
    // this must be a valid node.
    wxASSERT(m_obj != NULL && m_obj->doc != NULL);
    wxXml2Node child;
    wxXml2Document doc(GetDoc());

    // use CreateTemp even if the created node won't be really temporaneous...
    child.CreateTemp(wxXML2_PI_NODE, doc, name, content);
    doc.DestroyIfUnlinked();

    return child;
}

wxXml2Node wxXml2Node::AddBreakLineChild(int breaklines)
{
    // a "break line child" is just a text node with a new line.
    return AddTextChild(wxEmptyString, wxString(wxS('\n'), breaklines));
}

wxXml2Property wxXml2Node::AddProperty(const wxString &name, const wxString &value)
{
    wxXml2Property prop;
    prop.Create(name, value, *this);
    return prop;
}

wxXml2Namespace wxXml2Node::AddNamespace(const wxString &prefix, const wxString &href)
{
    wxXml2Namespace ns;
    ns.Create(prefix, href, *this);
    return ns;
}

void wxXml2Node::AddChild(wxXml2Node &child)
{
    // child will be added at the end of the children list of this
    // object. Adding a child requires also to update "last" pointer
    // in the xmlNode structure and to set child->parent relation.
    // This function will do everything.
    xmlAddChild(GetObj(), child.GetObj());

    // do we have namespaces to update ?
    if (GetNamespace() == wxXml2EmptyNamespace &&
        GetNamespaceDecl() == wxXml2EmptyNamespace)
        return;

    wxXml2Namespace ns;
    if (GetNamespace() == wxXml2EmptyNamespace)
        ns = GetNamespaceDecl();
    else
        ns = GetNamespace();

    // doing something like:
    //    child.GetObj()->ns = GetObj()->ns
    //                    or
    //    xmlSetNs(child.GetObj(), GetObj()->ns);
    //
    // won't work because all the children's namespaces of 'child' must be
    // updated; use a recursive function !!!
    child.SetNamespace(ns);
}

void wxXml2Node::AddProperty(wxXml2Property &prop)
{
    // this function will change the eventually preexisting property with
    // the same name of the given one, or will add it if this node hasn't
    // got any property with that name...
    xmlSetProp(GetObj(), WX2XML(prop.GetName()), WX2XML(prop.GetValue()));
}

void wxXml2Node::AddPrevious(wxXml2Node &node)
{
    // unlinks given node and links it as previous sibling of this node
    xmlAddPrevSibling(GetObj(), node.GetObj());
}

void wxXml2Node::AddNext(wxXml2Node &node)
{
    xmlAddNextSibling(GetObj(), node.GetObj());
}




//-----------------------------------------------------------------------------
//  wxXml2Node - set functions
//-----------------------------------------------------------------------------

void wxXml2Node::SetProperties(const wxXml2Property &prop)
{
    GetObj()->properties = prop.GetObj();
    prop.GetObj()->parent = (xmlNode *)m_obj;
}

void wxXml2Node::SetNamespace(wxXml2Namespace &ns)
{
    //ns.SetOwner(*this);
    GetObj()->ns = ns.GetObj();

    // set namespace for all the children of this node
    wxXml2Node p = this->GetChildren();
    while (p != wxXml2EmptyNode) {
        p.SetNamespace(ns);
        p = p.GetNext();
    }
}




//-----------------------------------------------------------------------------
//  wxXml2Node - miscellaneous functions
//-----------------------------------------------------------------------------

bool wxXml2Node::Cmp(const wxXml2Node &node) const
{
    // check contained data
    if (CmpNoNs(node)) {
        wxCHECK_NULL_POINTERS(node.GetObj(), GetObj());

        // also check namespace
        if (GetNamespace() == node.GetNamespace()) {

            // everything was identic (but we didn't check on purpose
            //  parents & children nor properties)
            return TRUE;
        }
    }

    return FALSE;
}

bool wxXml2Node::CmpNoNs(const wxXml2Node &node) const
{
    wxCHECK_NULL_POINTERS(node.GetObj(), GetObj());

    // check contained data
    if (GetName().CmpNoCase(node.GetName()) == 0 &&
        GetContent() == node.GetContent() &&
        GetType() == node.GetType()) {

        // everything was identic (but we didn't check parents & children
        // nor properties)
        return TRUE;
    }

    return FALSE;
}

bool wxXml2Node::HasProp(const wxString &propName) const
{
    wxXml2Property prop = GetProperties();

    while (prop != wxXml2EmptyProperty) {
        if (prop.GetName() == propName)
             return TRUE;
        prop = prop.GetNext();
    }

    return FALSE;
}

bool wxXml2Node::GetPropVal(const wxString &propName, wxString *value) const
{
    wxXml2Property prop = GetProperties();

    while (prop != wxXml2EmptyProperty) {
        if (prop.GetName() == propName) {
            *value = prop.GetValue();
            return TRUE;
        }
        prop = prop.GetNext();
    }

    return FALSE;
}

wxString wxXml2Node::GetPropVal(const wxString &propName,
                                const wxString &defaultVal) const
{
    wxString tmp;
    if (GetPropVal(propName, &tmp))
        return tmp;
    else
        return defaultVal;
}

void wxXml2Node::MakeUpper()
{
    SetName(GetName().MakeUpper());

    // set namespace for all the children of this node
    wxXml2Node p = this->GetChildren();
    while (p != wxXml2EmptyNode) {
        p.MakeUpper();
        p = p.GetNext();
    }
}

void wxXml2Node::MakeLower()
{
    SetName(GetName().MakeLower());

    // set namespace for all the children of this node
    wxXml2Node p = this->GetChildren();
    while (p != wxXml2EmptyNode) {
        p.MakeLower();
        p = p.GetNext();
    }
}

wxXml2Node wxXml2Node::Find(const wxString &name, const wxString &content,
                            int occ, bool bNS, bool recurse) const
{
    wxXml2Node tmp;    // this will be automatically removed
    wxXml2Document doc(GetDoc());

    // create a temporary node so that we can use the other overload of Find()
    tmp.CreateTemp(wxXML2_ELEMENT_NODE, doc, name, content);
    doc.DestroyIfUnlinked();

    return Find(tmp, occ, bNS, recurse);
}

wxXml2Node wxXml2Node::Find(const wxXml2Node &tofind, int occ, bool bNS, bool recurse) const
{
    // check pointer is okay
    wxASSERT_MSG(tofind != NULL, wxS("Invalid pointer"));

    // declare some variables
    wxXml2Node r = wxXml2EmptyNode, n = GetChildren();

    // search among the children of the given node
    while (n != wxXml2EmptyNode)
    {
        // check this node
        bool found = FALSE;
        if (bNS)
            found = n.Cmp(tofind);
        else
            found = n.CmpNoNs(tofind);

        if (found) {

            // found an occurrence of the tag
            occ--;
            if (occ == -1)
                return n;
        }

        if (recurse) {
            // search among the children of this node...
            if ((r = n.Find(tofind, occ, bNS, recurse)) != NULL)
                return r;
        }

        // search in next child
        n = n.GetNext();
    }

    // we didn't found it
    return wxXml2EmptyNode;
}

wxXml2Node wxXml2Node::Replace(const wxXml2Node &newnode)
{
    // we've got a perfect API for this...
    xmlReplaceNode(GetObj(), newnode.GetObj());

    UnwrappingOld();
    m_obj = (wxXml2BaseNodeObj*)newnode.GetObj();
    JustWrappedNew();

    return *this;
}

wxXml2Node wxXml2Node::Encapsulate(const wxString &nodename,
                                    const wxString &content,
                                    wxXml2Namespace &ns,
                                    wxXml2Property &prop)
{
    wxXml2Node copy(*this);

    // create a node of the given name
    wxXml2Node tmp;
    tmp.CreateTemp(wxXML2_ELEMENT_NODE, wxXml2EmptyDoc, nodename,
                    content, ns, prop);

    // insert "tmp" to the
    xmlNode *parent = GetObj()->parent;
    xmlNode *prev = GetObj()->prev;
    xmlNode *next = GetObj()->next;

    // change the links of this node's SIBLINGS and PARENT/CHILDREN
    if (parent && parent->children == tmp.GetObj())
        parent->children = tmp.GetObj();
    if (prev) prev->next = tmp.GetObj();
    if (next) next->prev = tmp.GetObj();

    // change the links of the TMP node
    tmp.GetObj()->prev = prev;
    tmp.GetObj()->next = next;
    tmp.GetObj()->parent = parent;
    tmp.GetObj()->children = GetObj();

    // change the links of THIS node
    GetObj()->parent = tmp.GetObj();


    UnwrappingOld();
    m_obj = (wxXml2BaseNodeObj*)tmp.GetObj();
    JustWrappedNew();

    // we should now be linked with "tmp"
    return *this;
}

bool wxXml2Node::operator==(const wxXml2Node &node) const
{
    // just use the case-sensitive comparison function
    return Cmp(node);
}





//-----------------------------------------------------------------------------
//  wxXml2Document
//-----------------------------------------------------------------------------

bool wxXml2Document::Create(const wxString &version)
{
    // create a new empty doc
    m_doc = xmlNewDoc(WX2XML(version));
    if (!m_doc) return FALSE;

    // and set its refcount to 1
    JustWrappedNew();

    return IsOk();
}

bool wxXml2Document::operator==(const wxXml2Document &doc) const
{
    // check for null pointers
    if ((long)doc.GetObj() ^ (long)GetObj())
        return FALSE;
    if (GetRoot() == doc.GetRoot())
        return TRUE;
    return FALSE;
}

// helper function for wxXml2Document::Load
static void XMLDocumentMsg(void *ctx, const char *pszFormat, ...)
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

bool wxXml2Document::Load(wxInputStream &stream, wxString *pErr)
{
    size_t l = stream.GetSize();
    if (l <= 0) {
        if (pErr) *pErr = wxS("Invalid size");
        return FALSE;
    }

    // load everything in a buffer
    char *buf = new char[l+1];
    if (!buf || stream.Read(buf, l).LastRead() != l) {
        if (pErr) *pErr = wxS("Couldn't read the input stream");
        if (buf) delete [] buf;
        return FALSE;
    }

    // set the error & warning handlers
    xmlSAXHandler h;
    memset(&h, 0, sizeof(h));
#if wxXML2CHECK_VERSION(2, 6, 10)
    xmlSAX2InitDefaultSAXHandler(&h, 0);
#else
    {
        if ((h == NULL) || (h->initialized != 0))
        return;

        xmlSAXVersion(h, xmlSAX2DefaultVersionValue);
        if (0 == 0)
            h->warning = NULL;
        else
            h->warning = xmlParserWarning;
    }
#endif
    h.error = XMLDocumentMsg;
    h.warning = XMLDocumentMsg;

    // xmlSAXHandler::fatalError is unused (in LibXML2 2.5.x),
    // however, we set it anyway to support future versions...
    h.fatalError = XMLDocumentMsg;

    // parse from buffer
    wxString error;
    UnwrappingOld();
    m_doc = xmlSAXParseMemoryWithData(&h, buf, l, 1, &error);
    JustWrappedNew();
    
    delete [] buf;

    // copy error string
    if (pErr) *pErr = error;

    return IsOk();
}

// helper function for wxXml2Document::Save
static int XMLDocumentWrite(void *context, const char *buffer, int len)
{
    wxOutputStream *stream = (wxOutputStream *)context;


    stream->Write(buffer, len);
    return stream->LastWrite();
}

int wxXml2Document::Save(wxOutputStream &stream, const wxString &encoding,
                         long flags, int indentstep) const
{
    xmlCharEncodingHandler *encoder = NULL;
    wxNativeNewlinesFilterStream *filter = NULL;        // created only if required
    wxOutputStream *mystream = &stream;

    // use one of the libxml2 encoders if required...
    if (!encoding.IsEmpty())
        encoder = xmlFindCharEncodingHandler((const char *)WX2XML(encoding));

    // will we use the native newline mode ?
    if (flags & wxXML2DOC_USE_NATIVE_NEWLINES) {

        // use a filter stream to change libxml2 newlines to native newlines
        filter = new wxNativeNewlinesFilterStream(stream);
        mystream = filter;
    }

    // create an ouput buffer
    xmlOutputBuffer *ob = xmlOutputBufferCreateIO(XMLDocumentWrite, NULL,
                                (void *)(mystream), encoder);

    // set up indentation mode
    wxXml2::SetIndentMode((flags & wxXML2DOC_USE_INDENTATION) != 0, indentstep);

    // save file & return (the output buffer will be deleted by libxml2)
    xmlSubstituteEntitiesDefault(0);
    int res = xmlSaveFormatFileTo(ob, m_doc, (const char *)WX2XML(encoding), 1);

    // restore old indentation mode
    wxXml2::RestoreLastIndentMode();

    // eventually cleanup the filter
    if (filter) delete filter;

    return res;
}

void wxXml2Document::SetRoot(wxXml2Node &node)    // not const because its object is modified...
{
    // and the old root should be deleted or it is automatically
    // deleted by libxml2 ?
    xmlDocSetRootElement(m_doc, node.GetObj());
}

void wxXml2Document::SetDTD(wxXml2DTD &dtd)
{
    // avoid unhandled exceptions
    if (GetObj() == NULL) return;

    xmlDtd *cur = dtd.GetObj();
    xmlDoc *doc = GetObj();

    // link the document with the given DTD: this section of code
    // has been copied from the xmlCreateIntSubset() libxml2's function.
    // Libxml2 in fact, misses a function like xmlSetDocDTD and thus
    // the only other way to implement wxXml2Document::SetDTD would
    // be to create on the fly another DTD with xmlCreateIntSubset():
    // but in such way the DTD of this document wouldn't be the DTD
    // given to this function but a copy of it...
    doc->intSubset = cur;
    if (cur == NULL) return;        // this was a little add by me
    cur->parent = doc;
    cur->doc = doc;

    if (doc->children == NULL) {
        doc->children = (xmlNodePtr) cur;
        doc->last = (xmlNodePtr) cur;
    } else {
        if (doc->type == XML_HTML_DOCUMENT_NODE) {
            xmlNodePtr prev;

            prev = doc->children;
            prev->prev = (xmlNodePtr) cur;
            cur->next = prev;
            doc->children = (xmlNodePtr) cur;
        } else {
            xmlNodePtr next;

            next = doc->children;
            while ((next != NULL) && (next->type != XML_ELEMENT_NODE))
                next = next->next;
            if (next == NULL) {
                cur->prev = doc->last;
                cur->prev->next = (xmlNodePtr) cur;
                cur->next = NULL;
                doc->last = (xmlNodePtr) cur;
            } else {
                cur->next = next;
                cur->prev = next->prev;
                if (cur->prev == NULL)
                    doc->children = (xmlNodePtr) cur;
                else
                    cur->prev->next = (xmlNodePtr) cur;
                next->prev = (xmlNodePtr) cur;
            }
        }
    }
}

wxXml2Node wxXml2Document::GetRoot() const
{
    // wrap our root in a wxXml2Node object...
    return wxXml2Node(xmlDocGetRootElement(m_doc));
}

wxXml2DTD wxXml2Document::GetDTD() const
{
    // wrap our DTD into a wxXml2DTD
    return wxXml2DTD(m_doc->intSubset);
}

void wxXml2Document::SetMathMLDTD()
{
    // this is the standard DTD reference for MathML 2.0 documents
    wxXml2DTD mathml(*this, wxS("math"), wxS("-//W3C//DTD MathML 2.0//EN"),
          wxS("http://www.w3.org/TR/MathML2/dtd/mathml2.dtd"));

    SetDTD(mathml);
}

void wxXml2Document::SetXHTMLStrictDTD()
{
    // this is a standard DTD reference for XHTML 1.0 documents
    wxXml2DTD xhtml(*this, wxS("html"), wxS("-//W3C//DTD XHTML 1.0 Strict//EN"),
                    wxS("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

    SetDTD(xhtml);
}

void wxXml2Document::SetXHTMLTransitionalDTD()
{
    // this is a standard DTD reference for XHTML 1.0 documents
    wxXml2DTD xhtml(*this, wxS("html"), wxS("-//W3C//DTD XHTML 1.0 Transitional//EN"),
                    wxS("http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"));

    SetDTD(xhtml);
}

void wxXml2Document::SetXHTMLFrameSetDTD()
{
    // this is a standard DTD reference for XHTML 1.0 documents
    wxXml2DTD xhtml(*this, wxS("html"), wxS("-//W3C//DTD XHTML 1.0 Frameset//EN"),
                    wxS("http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"));

    SetDTD(xhtml);
}

void wxXml2Document::SetStyleSheet(const wxString &xslfile)
{
    wxString content(wxS("type=\"text/xsl\" href=\"") + xslfile + wxS("\""));
    wxXml2Node root(GetRoot());

    if (root == wxXml2EmptyNode) {

        // we must create a "root" which is then
        // transformed into a PI node
        root.CreateRoot(*this, wxS("xml-stylesheet"));
        root.SetType(wxXML2_PI_NODE);
        root.SetContent(content);

    } else {

        // we can directly use the pre-existing root
        root.AddPIChild(wxS("xml-stylesheet"), content);
    }
}

bool wxXml2Document::Load(const wxString &filename, wxString *pErr)
{
    wxFileInputStream stream(filename);
    if (!stream.IsOk() || !wxFileName::FileExists(filename)) return FALSE;
    return Load(stream, pErr);
}

bool wxXml2Document::Save(const wxString &filename, const wxString &encoding,
                          long flags, int indentstep) const
{
    wxFileOutputStream stream(filename);
    if (!stream.IsOk()) return FALSE;
    return (Save(stream, encoding, flags, indentstep) != -1);
}

// helper function for wxXml2Document::IsDTDValid
static void XMLValidationMsg(void *out, const char *pszFormat, ...)
{
    // get the variable argument list
    va_list argptr;
    va_start(argptr, pszFormat);
    wxString str = wxString::FormatV(wxString(pszFormat, wxConvUTF8), argptr);
    va_end(argptr);

    // append the error/warning string to the void pointer
    // which is the "userData" member of the xmlValidCtxt
    // used for validation, which was set to a pointer to
    // a wxString by wxXml2Document::IsDTDValid
    wxString *err = (wxString *)out;
    if (err != NULL) *err += str;
}

bool wxXml2Document::IsDTDValid(wxString *err, int bUseInternal) const
{
    // create a new validation context
    xmlValidCtxt *ctxt = xmlNewValidCtxt();

    // set some useful callbacks
    ctxt->userData = err;
    ctxt->error = XMLValidationMsg;
    ctxt->warning = XMLValidationMsg;

    // try to validate the document...
    bool ret = FALSE;

    if (bUseInternal == 1) {

        // use the internal subset; do nothing if it's not available.
        if (!m_doc->intSubset) return FALSE;
        ret = xmlValidateDtd(ctxt, m_doc, m_doc->intSubset) != 0;

    } else if (bUseInternal == 0) {

        // use the external subset; do nothing if it's not available.
        if (!m_doc->extSubset) return FALSE;
        ret = xmlValidateDtd(ctxt, m_doc, m_doc->extSubset) != 0;

    } else {

        // use subset which is not NULL
        if (!m_doc->intSubset && !m_doc->extSubset) return FALSE;
        ret = xmlValidateDtd(ctxt, m_doc,
            (m_doc->intSubset ? m_doc->intSubset : m_doc->extSubset)) != 0;
    }

    xmlFreeValidCtxt(ctxt);
    return ret;
}





//-----------------------------------------------------------------------------
//  wxNativeNewlinesFilterStream
//-----------------------------------------------------------------------------

// DO NOT USE THE wxT MACRO IN THE DEFINES BELOW !!!
// THIS IS UNICODE-SAFE EVEN WITHOUT IT

#define wxXML2_NEWLINE            "\n"

#define wxDOS_NEWLINE            "\r\n"
#define wxUNIX_NEWLINE            "\n"
#define wxMAC_NEWLINE            "\r"
#define wxOS2_NEWLINE            "\r\n"


size_t wxNativeNewlinesFilterStream::OnSysWrite(const void *buffer, size_t bufsize)
{
    const char *newline = NULL;

#if defined(__WINDOWS__)
    newline = wxDOS_NEWLINE;
#elif defined(__APPLE__)
    newline = wxMAC_NEWLINE;
#elif defined(__OS2__)
    newline = wxOS2_NEWLINE;
#elif defined(__UNIX__)
    newline = wxUNIX_NEWLINE;
#endif

    wxASSERT_MSG(newline != NULL, wxS("Cannot recognize this OS newline endings..."));
    int newlinelenght = strlen(newline);

    // do not slow everything if we are on systems
    // which use the same line ending which is used directly by libxml2
    if (strcmp(wxXML2_NEWLINE, newline) == 0)
        return GetFilterOutputStream()->Write(buffer, bufsize).LastWrite();

    // we need to scan all the buffer replacing wxXML2_NEWLINEs...
    const char *buf = (const char *)buffer;
    size_t written = 0;

    for (unsigned int i=0; i<bufsize; i++) {
        if (buf[i] == wxXML2_NEWLINE[0])
            GetFilterOutputStream()->Write(newline, newlinelenght);
        else
            GetFilterOutputStream()->Write(&buf[i], 1);

        written += GetFilterOutputStream()->LastWrite();
    }

    // BE CAREFUL: "written" could be smaller or bigger than "bufsize"...
    //             but we need to return "bufsize" since otherwise libxml2
    //             could think that we missed to write something...
    return bufsize;//written;
}




//-----------------------------------------------------------------------------
//  wxStringOutputStream
//-----------------------------------------------------------------------------

#if !wxCHECK_VERSION(2,7,0)

size_t wxStringOutputStream::OnSysWrite(const void *buffer, size_t bufsize)
{
    // we will append the given buffer to our wxString,
    // assuming it is encoded in UTF8 format
    // (the wxString constructor we use will call the wxConvUTF8.MB2WC
    //  function to do the encode conversion).
    m_str += wxString((const char *)buffer, wxConvUTF8, bufsize);
    return bufsize;
}

#endif
