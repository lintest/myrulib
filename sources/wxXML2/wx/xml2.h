/////////////////////////////////////////////////////////////////////////////
// Name:        xml2.h
// Purpose:     libxml2 wrappers
// Author:      Francesco Montorsi (original design based on EXPAT by Vaclav Slavik)
// Created:     2000/03/05
// RCS-ID:      $Id: xml2.h 1235 2010-03-10 19:47:01Z frm $
// Copyright:   (c) 2000 Vaclav Slavik and (c) 2003 Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_XML2_H_
#define _WX_XML2_H_

// wxWidgets headers
#include "wx/string.h"
#include "wx/object.h"
#include "wx/wfstream.h"

// for shared builds
#include "wx/xml2def.h"

// When using mingw, we must define the LIBXML_STATIC symbol, otherwise
// we will have problems when linking about the "xmlFree" function.
#if defined(__GNUG__) && defined(__WXMSW__)
    #define LIBXML_STATIC
#endif

// Libxml2 headers: these classes wraps only libxml2 elements. You cannot
// use them without libxml2 !!!!!
//
// To add libxml2 to your project:
//   1) install libxml2 library binaries or compile them
//      (you can find all the info at http://xmlsoft.org)
//   2) add the include and library paths to the compiler
//   3) links your progrma with the 'libxml2.lib'
//      (and 'iconv.lib' under Win32)
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>



// -------------------------------
// Miscellaneous defines/macros
// -------------------------------

//! A macro used in the overloaded operator==; this is the return table:
//!
//!         x            y          returns
//!       NULL          NULL          TRUE        (they are equal)
//!       NULL        non-NULL        FALSE        (they are different)
//!     non-NULL        NULL          FALSE        (they are different)
//!     non-NULL      non-NULL      nothing: no 'return' statement is executed:
//!                                 x and y could be equal but they could also
//!                                 be different. The code following the macro
//!                                 must care about this possibility
//!
#define wxCHECK_NULL_POINTERS(x, y)        \
    if (x == NULL && y == NULL)            \
        return TRUE;                       \
    if ((x == NULL && y != NULL) ||        \
        (x != NULL && y == NULL))          \
        return FALSE;

//! Converts from wxStrings to xmlChars.
//! Libxml2 takes sequences of xmlChar (which is defined to be *always* unsigned char)
//! which are asupposed to be always in UTF8: thus WX2XML converts wxStrings to UTF8.
#if wxUSE_UNICODE

    // we must use the data() function of the wxCharBuffer which is returned
    // by wxString::mb_str
    #define WX2XML(str)        ((xmlChar *)(str.mb_str(wxConvUTF8).data()))
#else

    // wxString::mb_str will return an array of char*
    #define WX2XML(str)        ((xmlChar *)(str.mb_str(wxConvUTF8)))
#endif


//! Converts from xmlChars to wxStrings.
//! Libxml2 always outputs a sequence of xmlChar which are encoded in UTF8:
//! this macro creates a wxString which converts the given string from UTF8
//! to the format internally used by wxString (whatever it is).
#define XML2WX(str)        (wxString((const char *)str, wxConvUTF8))


//! A wxXml2Document::Save flag.
//! Tells wxXml2Document to save the document using the native newline
//! format; if not specified all newlines will be encoded in unix format
//! (that is, as simple '\n').
#define wxXML2DOC_USE_NATIVE_NEWLINES        1

//! A wxXml2Document::Save flag.
//! Tells wxXml2Document to save the document using an indentation step.
#define wxXML2DOC_USE_INDENTATION            2





// -----------------------------
// Basic types (enums, structs)
// -----------------------------

//! Represents an XML node type. LibXML2 allows a lot of different element types,
//! but the following are the types you can use with the wxXml2 wrappers...
enum wxXml2NodeType
{
    // note: values are synchronized with xmlElementType from libxml2 2.5.x
    //

    //! Container nodes; they are characterized by the NAME, the NAMESPACE
    //! and the CHILDRENs. They create something like:
    //!                  <NAME>children</NAME>
    //! or (if namespace != wxXml2EmptyNamespace),
    //!                  <NS:NAME>children</NS:NAME>
    //! Examples:
    //!     wxXml2Node container(wxXML2_ELEMENT_NODE, parentnode, "MYTAG");
    //!     container.AddChild(....);
    //!
    wxXML2_ELEMENT_NODE =           1,

    //! Nodes containing text; they are characterized by VALUE (or CONTENT).
    wxXML2_TEXT_NODE =              3,

    //! Creates something like: <![CDATA[content]]>
    //! To use this type of node, create a container node and then use the
    //! SetType(wxXML2_CDATA_SECTION_NODE) function.
    //! To set content use wxXml2Node::SetContent.
    wxXML2_CDATA_SECTION_NODE =     4,

    //! Like a text node, but this node contains only an "entity". Entities
    //! are strings like: &amp; or &quot; or &lt; ....
    //! To create them, use
    //!    wxXml2Node entityref(wxXML2_TEXT_NODE, parentnode, "&amp;");
    //!    containernode.AddChild(entityref);
    wxXML2_ENTITY_REF_NODE =        5,

    //! Creates a Processing Instruction (PI) node.
    //! Such type of nodes looks like: <?name content ?>
    //! To use this type of node, create a container node and then use SetType().
    //! To add 'properties', you cannot use wxXml2Property; instead use
    //! wxXml2Node::SetContent("property=\"value\"").
    wxXML2_PI_NODE =                7,

    //! Creates a comment node: <!-- content -->.
    wxXML2_COMMENT_NODE =           8,

    //! The value of the "type" member of an xmlDoc structure.
    //! This value is used to identify a node as a wxXml2Document.
    //! Never use it directly: use wxXml2Document instead.
    wxXML2_DOCUMENT_NODE =          9,
    wxXML2_HTML_DOCUMENT_NODE =    13,        //!< Like #wxXML2_DOCUMENT_NODE.


    //! The value of the "type" member of an xmlDtd structure.
    //! This value is used to identify a node as a wxXml2DTD.
    //! Never use it directly: use wxXml2DTD instead.
    wxXML2_DTD_NODE =              14,

    //! A DTD node which declares an element.
    //! This value is used to identify a node as a wxXml2ElemDecl.
    //! Looks like:     <!ELEMENT mynode (#PCDATA)>
    wxXML2_ELEMENT_DECL =          15,

    //! A DTD node which declares an attribute.
    //! This value is used to identify a node as a wxXml2AttrDecl.
    //! Looks like:     <!ATTLIST myattr type #PCDATA "defaultvalue">
    wxXML2_ATTRIBUTE_DECL =        16,

    //! A DTD node which declares an entity.
    //! This value is used to identify a node as a wxXml2EntityDecl.
    //! Looks like:     <!ENTITY myentity "entity's replacement">
    wxXML2_ENTITY_DECL =           17,


    //! The value of the "type" member of an xmlNs structure.
    //! This value is used to identify a node as a wxXml2Namespace.
    //! Never use it directly: use wxXml2Namespace instead.
    wxXML2_NAMESPACE_DECL =        18

    // I don't even know when these one are used... sorry
    //wxXML2_XINCLUDE_START = 19,
    //wxXML2_XINCLUDE_END = 20
};


//! The common part of all XML nodes.
//! This structure has been copied-and-pasted from line 440 of tree.h
//! of the libxml2 xmlNode structure.
typedef struct tagXml2BaseNode {

    void           *_private;    /* application data */
    xmlElementType   type;    /* type number, must be second ! */
    const xmlChar   *name;      /* the name of the node, or the entity */
    struct tagXml2BaseNode *children;    /* parent->childs link */
    struct tagXml2BaseNode *last;    /* last child link */
    struct tagXml2BaseNode *parent;    /* child->parent link */
    struct tagXml2BaseNode *next;    /* next sibling link  */
    struct tagXml2BaseNode *prev;    /* previous sibling link  */
    struct _xmlDoc  *doc;    /* the containing document */

} wxXml2BaseNodeObj;

// these are required because the compiler must know about their existence
class WXDLLIMPEXP_WXXML2 wxXml2BaseNode;
class WXDLLIMPEXP_WXXML2 wxXml2Node;
class WXDLLIMPEXP_WXXML2 wxXml2Property;
class WXDLLIMPEXP_WXXML2 wxXml2Namespace;
class WXDLLIMPEXP_WXXML2 wxXml2Document;
class WXDLLIMPEXP_WXXML2 wxXml2DTD;            // defined in "dtd.h"

// global instances of empty objects
extern WXDLLIMPEXP_DATA_WXXML2(wxXml2Node) wxXml2EmptyNode;
extern WXDLLIMPEXP_DATA_WXXML2(wxXml2Property) wxXml2EmptyProperty;
extern WXDLLIMPEXP_DATA_WXXML2(wxXml2Namespace) wxXml2EmptyNamespace;
extern WXDLLIMPEXP_DATA_WXXML2(wxXml2Document) wxXml2EmptyDoc;
extern WXDLLIMPEXP_DATA_WXXML2(wxXml2BaseNode) wxXml2EmptyBaseNode;




// --------------
// Classes
// --------------


//! Checks if the libxml2 version currently used (when compiling) is
//! newer or match the given version numbers.
//! The version numbers must be: 1 digit for major & minor; always
//! 2 digits for release:
//! \code
//! #if wxXML2CHECK_VERSION(2, 6, 10)
//!     put here the code for libxml2 >= 2.6.10
//! #else
//!     put here the code for libxml2 < 2.6.10
//! #endif
//! \endcode
//! \note LIBXML_VERSION is in the format
//!       MAJOR|MAJOR|MINOR|RELEASE|RELEASE
//!
#define wxXML2CHECK_VERSION(major,minor,release) \
    (LIBXML_VERSION >= major##0##minor##release)

// wxXml2 cannot be compiled with libxml2 < 2.6.1 since SAX2 API was not
// present prior that version...
#if !wxXML2CHECK_VERSION(2, 6, 10)
    #error wxXml2 requires at least libxml2 2.6.1; please download it from http://xmlsoft.org
#endif

//! A wrapper for some libxml2 static functions regarding the entire
//! parser. In particular, you should call the #Init() function before
//! using any of the wxXml2Wrapper and you should call the #Cleanup()
//! function before exiting to avoid memory leaks.
class WXDLLIMPEXP_WXXML2 wxXml2 : public wxObject
{
    static char m_strIndent[32];        // 32 = max indentation level.
    static const char *m_strOld;
    static int m_nOld;

public:
    wxXml2() {}
    virtual ~wxXml2() {}

    //! Initializes the libxml2 parser.
    //! Call this function before using a wxXml2Wrapper.
    static void Init()
        { xmlInitMemory(); xmlInitParser(); }

    //! Frees some internal libxml2 structures.
    //! Call this function after you've finished to work with all
    //! wxXml2Wrapper objects to avoid memory leaks.
    static void Cleanup()
        { xmlCleanupParser(); }

    //! Returns a string with the libxml2 library version used when
    //! this source was compiled.
    static wxString GetLibxml2Version()
        { return wxS(LIBXML_DOTTED_VERSION); }

    //! Checks the compiled library version against the include one.
    //! If something is wrong, a warning or a fatal error is generated.
    static void TestLibxml2Version()
        { LIBXML_TEST_VERSION; }

    //! Returns the global state of the libxml2 library.
    static xmlGlobalState *GetGlobalState()
        { return xmlGetGlobalState(); }

    //! Enables/disable the indentation mode for the libxml2 library.
    //! The \c indentstep is the number of spaces used when indenting.
    static void SetIndentMode(bool benable = TRUE, int indentstep = 4);

    //! Restores the indentation mode which was used before last
    //! call to #SetIndentMode() function.
    static void RestoreLastIndentMode();
};



//! Provides some standard methods for all wxXml2 wrappers.
//! This class is used as base for all those classes which wrap libxml2
//! structure with a "_private" member which can be used to hold the
//! reference count.
//! For more info, please read the specific \link wxxml2 page on this topic
//! \endlink (VERY IMPORTANT !).
class WXDLLIMPEXP_WXXML2 wxXml2Wrapper : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxXml2Wrapper)

protected:

    //! Destroys the data wrapped by this class freeing its memory.
    //! This function should not try to update the reference count
    //! since it should have been already updated by the caller.
    virtual void Destroy() = 0;

    //! Sets this element as empty: that is wrapping a NULL pointer.
    //! This function *must not* free the memory of the currently
    //! wrapped pointer: it must just reset that pointer to NULL.
    //! This function is used by #DestroyIfUnlinked().
    virtual void SetAsEmpty() = 0;

    //! Copies the given object. This function should be used to implement
    //! copy constructor and assignment operators.
    /*virtual void Copy(const wxXml2Wrapper &tocopy) = 0;*/

    //! Returns the private member of the wrapped structure.
    virtual int &GetPrivate() const = 0;

protected:        // reference counting utilities

    //! \page refcount wxXml2Wrappers and reference counting
    //! wxXml2Wrapper-derived classes does not use a full Copy-On-Write
    //! technique: when copying a wxXml2Wrapper you just create a new
    //! wxXml2Wrapper (which requires few bytes and thus this is a very
    //! fast operation) which wraps the same
    //! libxml2 structure of the original wxXml2Wrapper.
    //! Consider the following code:
    //! \code
    //!     wxXml2Document doc;
    //!     wxXml2Node root(doc, "root_of_doc");
    //!
    //!     // copy the root node
    //!     wxXml2Node copy(root);
    //!     copy.SetName("myroot");        // modify the copy
    //!
    //!     // save the document
    //!     doc.Save(....);
    //! \endcode
    //! The result won't be a document with a root named "root_of_doc"
    //! but a document with "myroot" as root's name.
    //! This is because deep copies are never performed, unlike, for example,
    //! in wxString's COW system where the string is shared until a non-const
    //! function is called.
    //! The libxml2 structures are always shared with wxXml2Wrapper.
    //! This is required since the wrappers around libxml2 structures are
    //! created on the fly by the wrapper's getters: this is why you'll see
    //! only functions returning objects and not references to objects.
    //!
    //! Since the underlying libxml2 structures are always shared,
    //! a reference count system is required to \b avoid things like:
    //! \code
    //!     wxXml2Node mynode;
    //!     mynode.Create(...);
    //!     {
    //!         // copy and mynode are now sharing the same libxml2 structure
    //!         wxXml2Node copy(mynode);
    //!         [...]
    //!     } // "copy" is destroyed: it destroys the libxml2 structure
    //!
    //!     // mynode references an invalid memory address: CRASH !
    //!     mynode.AddChild(...);
    //! \endcode
    //! Without reference counting, the destructor of an xml2 wrapper cannot
    //! know if other instances of xml wrappers are sharing the same structure.
    //! Performing a deep-copy in the copy constructor would be a problem:
    //! XML nodes are all linked together and unlinking a node and then inserting
    //! a new one is not so easy (especially for all types of nodes).
    //!
    //! The only solution is to use a reference count; but
    //! the reference count must be stored in the wrapped structure.
    //! How wxXml2Wrapper handle this problem ? Well, all libxml2 structure has
    //! a VOID* field called "_private" which can be safely used by external
    //! functions to hold user contents. The #GetPrivate() function casts that
    //! pointer to a reference to a integer so that it can be used as an int
    //! instead of a pointer to void.
    //!
    //! With reference counting the destructor of a wxXml2Wrapper knows if it
    //! is the only instance of that structure (and in this case it should
    //! free the structure) or if other wxXml2Wrappers still own that structure
    //! (and in this case it only decreases the refcount):
    //! \code
    //!     wxXml2Node mynode;
    //!     mynode.Create(...);
    //!     {
    //!         // copy and mynode are now sharing the same libxml2 structure
    //!         // and the structure's refcount is set to 2
    //!         wxXml2Node copy(mynode);
    //!         [...]
    //!     } // "copy" is destroyed: it decreases the refcount
    //!
    //!     // mynode references a valid structure (with refcount = 1)
    //!     mynode.AddChild(...);
    //! \endcode
    //!
    //! Anyway there is another thing to consider: libxml2 implements
    //! "recursive destruction".
    //! When a node is destroyed, all its children are too.
    //! This means that we must also be careful not to break libxml2 memory
    //! representation destroying a node's child when it's still linked
    //! to its parent. Precisely, when a node (but also a property, a dtd,
    //! a namespace...) is part of a wider XML tree, we must *never* delete
    //! the node when the relative wxXml2Wrapper is destroyed.
    //! This is why all wxXml2Wrappers must implement the #IsUnlinked()
    //! function: the #DestroyIfUnlinked() function uses it to find when
    //! nodes are unlinked from a wider tree and thus must be destroyed by
    //! xml2 wrapper.
    //! However, this rises another problem !
    //! Consider the example below:
    //! \code
    //!     // we'll now build an XML tree
    //!     {
    //!         wxXml2Node root;
    //!         root.Create(...);
    //!
    //!         // we are going to create a child of the "root" element...
    //!         wxXml2Node child(root, ...);
    //!
    //!     } // which one of the "root" and "child" element will be destroyed
    //!       // first ? If the "child" destructor is called before the "root"'s one
    //!       // then there are no problems: child::IsUnlinked() will return FALSE
    //!       // and "child" destructor will do nothing. Then the "root" destructor
    //!       // will delete everything.
    //!
    //!       // What happens if the "root" destructor is called first ?
    //!       // the root::IsUnlinked() function will return TRUE, root::refcount
    //!       // is set to one; the "root" destructor will destroy the structure
    //!       // and libxml2 will free the memory of all its children too.
    //!       // then, "child" destructor is called and it references an invalid
    //!       // memory address: CRASH !
    //! \endcode
    //!
    //! How can we solve this problem ?
    //! One solution could be not only to check if the node is unlinked but also
    //! to check if all children refcounts are set to zero, in each node destructor.
    //! However, I did not implement this solution because it can make all
    //! wxxml2 wrappers very slow: wxXml2Nodes are continuosly created & destroyed.
    //! The solution I adopted is simply to force the user (you!) to call the
    //! wxXml2Wrapper::DestroyIfUnlinked() function in the right order.
    //! This approach is not more restrictive of the approach used, for example,
    //! by libxml++ which uses pointers and thus require the DELETE calls to be
    //! in the right order.
    //! The example above must thus be rewritten as:
    //! \code
    //!     // we'll now build an XML tree
    //!     {
    //!         wxXml2Node root;
    //!         root.Create(...);
    //!
    //!         // we are going to create a child of the "root" element...
    //!         wxXml2Node child(root, ...);
    //!
    //!         // destroy the nodes in the right order:
    //!         // first children and then the root element
    //!         // (which will be the only element to do a real call
    //!         //  to the xmlFreeXXXX function)
    //!         child.DestroyIfUnlinked();    // this sets "child" as empty
    //!         root.DestroyIfUnlinked();    // this destroys the tree
    //!
    //!         // if we try to access the "child" or "root" element
    //!         // we'll find that they are set to empty elements: in this
    //!         // way the order used by the compiler to call the destructors
    //!         // of the nodes does not care...
    //!     }
    //! \endcode
    //! So, as general rule, when using wxXml2Wrapper-derived classes, you
    //! <B>must always call the wxXmlWrapper::DestroyIfUnlinked functions
    //! in the right order: first the DestroyIfUnlinked functions of all
    //! children and then, as last, the DestroyIfUnlinked function of the
    //! node containing all the others.</B>

    void ResetRefCount()
        { if (IsNonEmpty()) GetPrivate() = 0; }

    void IncRefCount()
        { if (IsNonEmpty()) GetPrivate()++; }

    void DecRefCount()
        { if (IsNonEmpty()) GetPrivate()--; }

    int GetRefCount() const
        { if (IsNonEmpty()) return (int)(GetPrivate()); else return 0; }

    //! This function should be called each time the internal pointer
    //! of the class is going to be replaced by another one.
    void UnwrappingOld()
        { DestroyIfUnlinked(); }

    //! This function should be called each time the internal pointer
    //! of the class has been replaced by another one.
    void JustWrappedNew()
        { IncRefCount(); }

public:

    //! Constructs this wrapper but not its wrapped data.
    wxXml2Wrapper() {}

    //! The wxXml2Wrapper destructor will free the memory of the underlying
    //! libxml2 structure only if it is not linked to any XML tree.
    //! To perform this behaviour, the derived class MUST always call the
    //! #DestroyIfUnlinked function in its destructor (it cannot be called here).
    virtual ~wxXml2Wrapper() {}


    //! Returns TRUE if this object is unlinked from an XML tree.
    virtual bool IsUnlinked() const = 0;

    //! Returns TRUE if the wrapped element is not NULL.
    virtual bool IsNonEmpty() const = 0;

    //! Destroys the wrapped data if it is unlinked.
    virtual void DestroyIfUnlinked();
};



//! Represents a node property.
//! Example: in
//!                    <img src="hello.gif" id="3"/>
//! "src" is property with value "hello.gif" and "id" is a property with value "3".
class WXDLLIMPEXP_WXXML2 wxXml2Property : public wxXml2Wrapper
{
    DECLARE_DYNAMIC_CLASS(wxXml2Property)

    //! The XML property wrapped by this class.
    xmlAttr *m_attr;

protected:

    void Destroy() {
        if (m_attr) xmlRemoveProp(m_attr);
        SetAsEmpty();
    }

    void SetAsEmpty()
        { m_attr = NULL; }

    void Copy(const wxXml2Property &prop)
        { UnwrappingOld(); m_attr = prop.m_attr; JustWrappedNew(); }

    int &GetPrivate() const
        { return (int &)(m_attr->_private); }

public:

    //! Constructs an empty property; it cannot be used until you #Create() it.
    wxXml2Property() : m_attr(NULL) {}

    //! Constructs a property associated with the given node.
    wxXml2Property(const wxString &name,
                    const wxString &value,
                    wxXml2Node &owner = wxXml2EmptyNode) : m_attr(NULL)
        { Create(name, value, owner); }

    //! Wraps the given XML structure.
    wxXml2Property(xmlAttr *pattr) : m_attr(pattr)
        { JustWrappedNew(); }

    //! Copies the given wrapper (does not imply the XML structure copy).
    wxXml2Property(const wxXml2Property &prop) : wxXml2Wrapper(), m_attr(NULL)
        { Copy(prop); }

    //! Destructor.
    virtual ~wxXml2Property() { DestroyIfUnlinked(); }


public:        // operators

    //! Returns TRUE if the two wxXml2Properties have the same name (case insensitive
    //! comparison is performed) and same values (case sensitive comparison).
    bool operator==(const wxXml2Property &p) const;

    //! Returns NOT operator==.
    bool operator!=(const wxXml2Property &p) const        { return !(*this == p); }

    //! Assignment operator.
    wxXml2Property &operator=(const wxXml2Property &p)
        { Copy(p); return *this; }

public:        // miscellaneous

    //! Builds this XML property.
    void Create(const wxString &name,
                const wxString &value,
                wxXml2Node &owner = wxXml2EmptyNode);

    //! Returns TRUE if this property is not linked to any wider XML tree.
    bool IsUnlinked() const;

    //! Returns TRUE if this object is wrapping a non-NULL object.
    bool IsNonEmpty() const
        { return m_attr != NULL; }


public:        // getters

    wxString GetName() const
        { if (m_attr) return XML2WX(m_attr->name); return wxEmptyString; }
    wxString GetValue() const
        { if (m_attr->children) return XML2WX(m_attr->children->content); return wxEmptyString; }

    wxXml2Property GetNext() const
        { if (m_attr) return wxXml2Property(m_attr->next); return wxXml2EmptyProperty; }
    wxXml2Property GetPrevious() const
        { if (m_attr) return wxXml2Property(m_attr->prev); return wxXml2EmptyProperty; }

    wxXml2Node GetOwner() const;    // cannot be implemented here: wxXml2Node not defined yet

    //! Returns the XML structure wrapped by this class
    xmlAttr *GetObj() const
        { return m_attr; }


public:        // setters

    void SetValue(const wxString &value);
    void SetName(const wxString &name);

    void SetNext(wxXml2Property &next);
    void SetPrevious(wxXml2Property &prev);

    //! Links this class with a wxXml2Node: it will have the duty of destroy this object.
    void SetOwner(wxXml2Node &owner);
};



//! Represents an XML namespace. In this example:
//! \code
//!   <root xmlns="http://!www.w3.org/1999/xhtml">
//!     <e:child xmlns:e="http://!mysite.com/mynamespaces/e/">
//!       <e:subchild>content</e:subchild>
//!     </e:child>
//!     <child2 prop="value"/>
//!   </root>
//! \endcode
//! The 'e' declared on node 'child' is a namespace with an explicit prefix
//! (the 'e') and with hreference = http://!mysite.com/mynamespaces/e/.
//! The element 'root' instead defines a namespace, without any explicit prefix,
//! which has href==http://!www.w3.org/1999/xhtml.
//! The namespace of 'root' is then overridden by namespace 'e' in element
//! 'child' and all its subelements.
class WXDLLIMPEXP_WXXML2 wxXml2Namespace : public wxXml2Wrapper
{
    DECLARE_DYNAMIC_CLASS(wxXml2Namespace)

    //! The XML namespace wrapped by this class.
    xmlNs *m_ns;

    //! The node which owns this structure.
    //! This variable is requird since the xmlNs structure
    //! does not contain anything which can be used to determine
    //! if the namespace is linked or unlinked.
    xmlNode *m_owner;

protected:

    void Destroy() {
        if (m_ns) xmlFreeNs(m_ns);
        SetAsEmpty();
    }

    void SetAsEmpty()
        { m_ns = NULL; }

    void Copy(const wxXml2Namespace &ns) {
        UnwrappingOld();
        m_ns = ns.m_ns;
        m_owner = ns.m_owner;
        JustWrappedNew();
    }

    int &GetPrivate() const
        { return (int &)(m_ns->_private); }

public:

    //! Constructs an empty namespace. It cannot be used until you Create() it.
    wxXml2Namespace() : m_ns(NULL), m_owner(NULL) {}

    //! Constructs a namespaces and eventually set it as the namespace used
    //! by 'owner'. If 'owner' == wxXml2EmptyNode, then the namespace won't be
    //! attached to any XML tree. Use SetOwner to attach it to a specific node.
    wxXml2Namespace(const wxString &prefix, const wxString &uri,
                    wxXml2Node &owner = wxXml2EmptyNode) : m_ns(NULL), m_owner(NULL)
        { Create(prefix, uri, owner); }

    //! Wraps a libxml2 namespace structure.
    wxXml2Namespace(xmlNs *ns, wxXml2Node &owner);

    //! Copies the given wrapper (does not imply the XML structure copy).
    wxXml2Namespace(const wxXml2Namespace &ns) : wxXml2Wrapper(), m_ns(NULL), m_owner(NULL)
        { Copy(ns); }

    //! Destroys this namespace if it is completely unlinked from a greater
    //! XML tree.
    virtual ~wxXml2Namespace() { DestroyIfUnlinked(); }


public:        // operators

    bool operator==(const wxXml2Namespace &ns) const;
    bool operator!=(const wxXml2Namespace &p) const        { return !(*this == p); }

    wxXml2Namespace &operator=(const wxXml2Namespace &n)
        { Copy(n); return *this; }

public:        // miscellaneous

    //! Creates a new namespace and eventually attach it to the given node.
    void Create(const wxString &prefix, const wxString &uri, wxXml2Node &owner);

    //! Returns TRUE if this element is unlinked.
    //! Since namespace structures does not contain a parent pointer by
    //! themselves, a wxXml2Namespace also keeps trace of the linked nodes
    //! using an additional variable #m_owner.
    bool IsUnlinked() const
        { return (m_owner == NULL) || (m_ns == NULL); }

    //! Returns TRUE if this object is wrapping a non-NULL object.
    bool IsNonEmpty() const
        { return m_ns != NULL; }


public:        // getters

    wxString GetPrefix() const
        { if (m_ns) return XML2WX(m_ns->prefix); return wxEmptyString; }
    wxString GetURI() const
        { if (m_ns) return XML2WX(m_ns->href); return wxEmptyString; }

    //! Returns the associated XML structure
    xmlNs *GetObj() const
        { return m_ns; }

public:        // setters

    void SetPrefix(const wxString &p);
    void SetURI(const wxString &u);
};




//! This class holds XML data/document as parsed by the libxml2 parser.
//! Use #Load and #Save to perform IO on wxStream or on wxFile objects.
//! Use the #GetRoot() function to edit the contents of the XML
//! document through wxXml2Node functions.
class WXDLLIMPEXP_WXXML2 wxXml2Document : public wxXml2Wrapper
{
    DECLARE_DYNAMIC_CLASS(wxXml2Document)

    //! The parsed document & the associated tree.
    xmlDoc *m_doc;

protected:

    void Destroy() {
        if (m_doc) xmlFreeDoc(m_doc);
        SetAsEmpty();
    }

    void SetAsEmpty()
        { m_doc = NULL; }

    void Copy(const wxXml2Document &doc) {
        UnwrappingOld();
        m_doc = doc.m_doc;
        JustWrappedNew();
    }

    int &GetPrivate() const
        { return (int &)(m_doc->_private); }

public:

    //! Creates an empty document. Use the #Create function
    //! before using it.
    wxXml2Document()
        { m_doc = NULL; }

    //! Constructs a document and sets its root to the given node.
    wxXml2Document(wxXml2Node &root);

    //! Loads the given filename and parse it.
    wxXml2Document(const wxString &filename)
        { m_doc = xmlParseFile((const char *)WX2XML(filename)); JustWrappedNew(); }

    //! Wraps the given libxml2 structure.
    wxXml2Document(xmlDoc *doc) : m_doc(doc)
        { JustWrappedNew(); }

    //! Copies the given wrapper (does not imply the XML structure copy).
    wxXml2Document(const wxXml2Document &doc) : wxXml2Wrapper(), m_doc(NULL)
        { Copy(doc); }

    //! Destructor.
    virtual ~wxXml2Document()
        { DestroyIfUnlinked(); }


public:        // operators

    bool operator==(const wxXml2Document &doc) const;
    bool operator!=(const wxXml2Document &doc) const     { return !(*this == doc); }

    wxXml2Document &operator=(const wxXml2Document &doc)
        { Copy(doc); return *this; }


public:        // create, load & save

    //! Creates a new empty XML document with the given version.
    bool Create(const wxString &version = wxS("1.0"));

    //! Parses the data from the given wxInputStream. See #Load().
    bool Load(wxInputStream &stream, wxString *pErr = NULL);

    //! Parses XML & XHTML file and loads data. Returns TRUE on success.
    //! Returns FALSE otherwise & provide an error description if the given
    //! pointer is not NULL.
    bool Load(const wxString &filename, wxString *pErr = NULL);

    //! Saves the XML data in the given stream with the given encoding.
    //! Returns the number of bytes written: -1 if there were errors.
    int Save(wxOutputStream &stream, const wxString &encoding = wxS("UTF-8"),
                long flags = wxXML2DOC_USE_NATIVE_NEWLINES, int indentstep = 4) const;

    //! Saves the document as XML or XHTML file in the given encoding format.
    //! Returns TRUE on success.
    bool Save(const wxString &filename, const wxString &encoding = wxS("UTF-8"),
                long flags = wxXML2DOC_USE_NATIVE_NEWLINES, int indentstep = 4) const;


public:        // miscellaneous

    //! Returns TRUE if this object is wrapping a non-NULL object.
    bool IsNonEmpty() const
        { return m_doc != NULL; }

    //! Returns TRUE if everything is okay in the XML tree structure.
    bool IsOk() const
        { return xmlDocGetRootElement(m_doc) != NULL; }

    //! Returns always TRUE since documents are entities which contain
    //! links to other structures (wxXml2DTD, wxXml2Node...) but a doc
    //! is never linked with structures that are not own by itself.
    bool IsUnlinked() const
        { return TRUE; }

    //! Checks if this document is conform to the DTD which it holds.
    //! \param bUseInternal If this parameter is set to 1, then the
    //!        internal subset (m_doc->intSubset) will be used.
    //!        If this parameter is set to 0, then the external subset
    //!        (m_doc->extSubset) will be used.
    //!        If this parameter is set to -1, then the subset will be
    //!        chosen automatically (the first valid one).
    //! \note Libxml2 does not specify if a document can hold both
    //!       an internal and an external subset. Anyway, the internal
    //!       one should be the preferred choice; besides, it is the
    //!       DTD returned by #GetDTD() function.
    bool IsDTDValid(wxString *err = NULL, int bUseInternal = 0) const;

public:        // getters

    //! Returns the root node of the document.
    wxXml2Node GetRoot() const;

    //! Returns the internal DTD associated with this document.
    //! If no internal DTDs are associated then wxXml2EmptyDTD is returned.
    //! \note The DTD associated with this document could be only a
    //!       simple line like:
    //!            <!DOCTYPE mydtd SYSTEM "mydtd.dtd">
    //!       to intercept this case, you should use the
    //!       wxXml2DTD::IsExternalReference() function.
    wxXml2DTD GetDTD() const;

    //! Returns version of document (may be empty).
    wxString GetVersion() const
        { if (m_doc) return XML2WX(m_doc->version); return wxEmptyString; }

    //! Returns encoding of document (may be empty).
    //! Note: this is the encoding original file was saved in, *not* the
    //! encoding of in-memory representation!
    wxString GetFileEncoding() const
        { if (m_doc) return XML2WX(m_doc->encoding); return wxEmptyString; }

    //! Returns the libxml2 underlying object.
    xmlDoc *GetObj() const
        { return m_doc; }


public:        // setters

    //! Changes the root of the document (deleting old one).
    void SetRoot(wxXml2Node &node);

    //! Sets the DTD for this document.
    void SetDTD(wxXml2DTD &dtd);

    //! Creates a PI node of the type:
    //!   <?xml-stylesheet type="text/xsl" href="xslfile"?>
    //! where "xslfile" is the given URI to an XSL file.
    void SetStyleSheet(const wxString &xslfile);

public:        // some common DTDs

    //! Sets the MathML DTD for this document.
    void SetMathMLDTD();

    //! Sets the XHTML DTD for this document.
    void SetXHTMLStrictDTD();
    void SetXHTMLTransitionalDTD();
    void SetXHTMLFrameSetDTD();
};



//! A generic XML node.
//! This is the base class for more specific nodes like wxXml2Node,
//! wxXml2ElemDecl, wxXml2AttrDecl, wxXml2EntityDecl.
//! This class provides the basic functions used to modify an XML
//! tree. Libxml2 in fact, uses different structures for the different
//! types of nodes but all of them have some parts in common:
//! wxXml2BaseNodeObj is the common part of all XML nodes handled by
//! libxml2. So, when you have an XML node, you can always cast it,
//! regardless of its specific type, to a wxXml2BaseNodeObj and then
//! change its NEXT, PREVIOUS, CHILDREN, PARENT pointers...
class WXDLLIMPEXP_WXXML2 wxXml2BaseNode : public wxXml2Wrapper
{
    DECLARE_DYNAMIC_CLASS(wxXml2BaseNode)

    //! The libxml2 structure.
    wxXml2BaseNodeObj *m_obj;

protected:

    void Destroy() {
        if (m_obj) xmlFree(m_obj);
        SetAsEmpty();
    }

    void SetAsEmpty()
        { m_obj = NULL; }

    void  Copy(const wxXml2BaseNode &n) {
        UnwrappingOld();
        m_obj = n.m_obj;
        JustWrappedNew();
    }

    int &GetPrivate() const
        { return (int &)(m_obj->_private); }

public:

    wxXml2BaseNode() : m_obj(NULL) {}
    wxXml2BaseNode(wxXml2BaseNodeObj *p) : m_obj(p) { JustWrappedNew(); }

    virtual ~wxXml2BaseNode() {}


    //! Is this node linked to a wider XML tree ?
    bool IsUnlinked() const;

    //! Returns TRUE if this object is wrapping a non-NULL object.
    bool IsNonEmpty() const
        { return m_obj != NULL; }


public:        // operators

    bool operator==(const wxXml2BaseNode &decl) const;
    bool operator!=(const wxXml2BaseNode &decl) const     { return !(*this == decl); }

    wxXml2BaseNode &operator=(const wxXml2BaseNode &decl)
        { Copy(decl); return *this; }


public:        // getters

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //! \name Getters
    //! These functions wrap on the fly the internal pointers of the libxml2
    //! structure and then return them (this is why they return objects and not
    //! references to objects). @{

    wxXml2NodeType GetType() const
        { return (wxXml2NodeType)((int)m_obj->type); }
    wxXml2BaseNode GetParent() const
        { if (m_obj) return wxXml2BaseNode(m_obj->parent); return wxXml2EmptyBaseNode; }
    wxXml2BaseNode GetChildren() const
        { if (m_obj) return wxXml2BaseNode(m_obj->children); return wxXml2EmptyBaseNode; }
    wxXml2BaseNode GetFirstChild() const
        { return GetChildren(); }
    wxXml2BaseNode GetNext() const
        { if (m_obj) return wxXml2BaseNode(m_obj->next); return wxXml2EmptyBaseNode; }
    wxXml2BaseNode GetPrevious() const
        { if (m_obj) return wxXml2BaseNode(m_obj->prev); return wxXml2EmptyBaseNode; }

    wxXml2BaseNodeObj *GetObj() const
        { return m_obj; }

    //@}


public:        // setters

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //! \name Setters: still not tested all & unstable.
    //! These functions are dangereous since they can break the libxml2 internal
    //! tree if they are not used carefully. Try to avoid them. @{

    void SetType(wxXml2NodeType type)
        { m_obj->type = (xmlElementType)((int)type); }

    void SetChildren(const wxXml2BaseNode &child);
    void SetNext(const wxXml2BaseNode &next);
    void SetPrevious(const wxXml2BaseNode &prev);

    //@}
};



//! Represents a node in XML documents. Node has name and may have content
//! and properties. Most common node types are wxXML2_TEXT_NODE (name and props
//! are irrelevant) and wxXML2_ELEMENT_NODE (e.g. in <title>hi</title> there is
//! element with name="title" and with irrelevant content and one child
//! (of type=wxXML2_TEXT_NODE with content="hi").
//!
class WXDLLIMPEXP_WXXML2 wxXml2Node : public wxXml2BaseNode
{
    DECLARE_DYNAMIC_CLASS(wxXml2Node)

protected:

    //! Builds a new XML node settings ALL its possible attributes:
    //! the type (wxXml2NodeType), the parent, the children, the
    //! next & previous nodes (wxXml2Node), the properties (wxXml2Property),
    //! the parent document (wxXml2Document), the name & content (wxString).
    void Build(const wxXml2NodeType type,
                wxXml2Node &parent,
                wxXml2Document &document = wxXml2EmptyDoc,
                wxXml2Namespace &ns = wxXml2EmptyNamespace,
                const wxString &name = wxEmptyString,
                const wxString &content = wxEmptyString,
                wxXml2Property &props = wxXml2EmptyProperty,
                wxXml2Node &next = wxXml2EmptyNode,
                wxXml2Node &previous = wxXml2EmptyNode,
                wxXml2Node &children = wxXml2EmptyNode);

    //! Uses functions specific to xmlNode.
    void Destroy() {
        xmlUnlinkNode((xmlNode *)m_obj);
        xmlFreeNode((xmlNode *)m_obj);
        SetAsEmpty();
    }


public:

    //! Builds an empty node. If you need an empty node in a "read-only" operation
    //! (comparison, for example), you can use the global instance wxXml2EmptyNode.
    wxXml2Node() {
        // before this node can be used it must be constructed through
        // Create****() functions. See below.
        m_obj = NULL;
    }

    //! Builds a new node as root element of the given document. The node is
    //! automatically set as wxXML2_ELEMENT_NODE and will replace any other
    //! root node eventually set in the given document (the old node is
    //! destroyed). The root element of a document should have parent,
    //! next, previous == NULL.
    wxXml2Node(wxXml2Document &doc,
                const wxString &name = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace)
        { CreateRoot(doc, name, ns); }

    //! Builds a new node as child of the given node.
    wxXml2Node(wxXml2NodeType type,
                wxXml2Node &parent,
                const wxString &name,
                const wxString &content = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace)
        { CreateChild(type, parent, name, content, ns);    }

    //! Builds a new node unlinked from the main tree. This node must be considered
    //! temporary until it is linked to another node in the main tree.
    wxXml2Node(wxXml2NodeType type, wxXml2Document &doc,
                const wxString &name = wxEmptyString,
                const wxString &content = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace)
        { CreateTemp(type, doc, name, content, ns);    }

    //! Wraps the given XML node. This function doesn't copy the given
    //! structure, it just attach it to this object.
    wxXml2Node(xmlNode *node)
        { m_obj = (wxXml2BaseNodeObj *)node; JustWrappedNew(); }

    //! Copies the given wrapper (does not imply the XML structure copy).
    wxXml2Node(const wxXml2Node &n) : wxXml2BaseNode()
        { Copy(n); }

    //! Copies the given wrapper (does not imply the XML structure copy).
    wxXml2Node(const wxXml2BaseNode &n)
        { m_obj = n.GetObj(); JustWrappedNew(); }

    virtual ~wxXml2Node() { DestroyIfUnlinked(); }


public:        // operators

    bool operator==(const wxXml2Node &node) const;
    bool operator!=(const wxXml2Node &p) const                { return !(*this == p); }

    wxXml2Node &operator=(const wxXml2Node &n)
        { Copy(n); return *this; }


public:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //! \name User-friendly creation.
    //! These functions are safe & powerful. Use these when possible. @{

    //! Creates this node as the ROOT of the given document.
    //! The XML root of a document is the node containing all other nodes.
    //! It can be preceded only by some special nodes. Consider the following:
    //! \code
    //!
    //!   // this type of node is handled by wxXml2Document
    //!   <?xml version="1.0" encoding="UTF-8"?>
    //!
    //!   // this type of node is a wxXML2_PI_NODE
    //!   <?xml-stylesheet type="text/xsl" href="mathml.xsl"?>
    //!
    //!   // this type of node is handled by wxXml2DTD
    //!   <!DOCTYPE some PUBLIC "URI" "URI2">
    //!
    //!   // our ROOT node
    //!   <root> .... </root>
    //! \endcode
    //!
    //! It's valid since the ROOT is preceded only by wxXML2_PI_NODEs and a
    //! DTD declaration. If you see something like:
    //! \code
    //!
    //!   <?xml version="1.0" encoding="UTF-8"?>
    //!   <root>....</root>
    //!   <anothernode></anothernode>
    //! \endcode
    //!
    //! Then it's wrong: the ROOT node must contain all other container nodes.
    void CreateRoot(wxXml2Document &doc,
                const wxString &name = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace,
                wxXml2Property &props = wxXml2EmptyProperty);

    //! Creates a node which is attached to the given parent as one of its children.
    void CreateChild(wxXml2NodeType type,
                wxXml2Node &parent,
                const wxString &name,
                const wxString &content = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace,
                wxXml2Property &props = wxXml2EmptyProperty,
                wxXml2Node &next = wxXml2EmptyNode);

    //! Creates a temporary node unlinked from any XML tree (the wxXml2Document
    //! is required but it is not modified) which can be used to temporary
    //! store some data. Anyway, such node will be destroyed if you don't link
    //! it to a wider XML tree.
    void CreateTemp(wxXml2NodeType type,
                wxXml2Document &doc,
                const wxString &name = wxEmptyString,
                const wxString &content = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace,
                wxXml2Property &props = wxXml2EmptyProperty);

    //! Creates a child which can contain other children.
    wxXml2Node AddContainerChild(const wxString &name);

    //! Creates a container child (named 'name'), which
    //! contains a text child which contains the string 'content'.
    //! to avoid the creation of the container child and to create
    //! directly a text node, use wxEmptyString (or "") as name.
    wxXml2Node AddTextChild(const wxString &name, const wxString &content);

    //! Creates a comment node: <!-- content -->.
    wxXml2Node AddCommentChild(const wxString &content);

    //! Creates a CDATA node: <![CDATA[content]]>.
    wxXml2Node AddCDATAChild(const wxString &content);

    //! Creates a PI node: <?name content ?>.
    wxXml2Node AddPIChild(const wxString &name, const wxString &content);

    //! Creates a text node which contains one (or more) break line characters.
    //! In XML all characters in the content of the document are significant
    //! including blanks and formatting line breaks, so if you want to add
    //! a break line you MUST create a text child with '\n' as content.
    //! The problem is that, if you use this function even once, libxml2 will
    //! automatically turn off its format algorithm for this node and all
    //! its children...
    wxXml2Node AddBreakLineChild(int breaklines = 1);

    //! Creates a new wxXml2Property with the given name & value (NAME="value")
    //! and attaches it to the properties of this node.
    wxXml2Property AddProperty(const wxString &name, const wxString &value);

    //! Creates a new wxXml2Namespace with the given prefic & href and sets
    //! it as the namespace of this node.
    wxXml2Namespace AddNamespace(const wxString &prefix, const wxString &href);

    //@}


    ///////////////////////////////////////////////////////////////////////////////////////////////
    //! \name Raw-creation functions
    //! These functions should append the given objects at the end of the
    //! libxml2 node's list. Avoid them when possible. @{

    //! Links a new property to this node.
    void AddProperty(wxXml2Property &prop);

    //! Links a new child to this node.
    void AddChild(wxXml2Node &node);

    //! Links the given node as previous node.
    void AddPrevious(wxXml2Node &node);

    //! Links the given node as next node.
    void AddNext(wxXml2Node &node);

    //@}

public:        // getters

    wxXml2Node GetParent() const
        { return wxXml2Node(wxXml2BaseNode::GetParent()); }
    wxXml2Node GetChildren() const
        { return wxXml2Node(wxXml2BaseNode::GetChildren()); }
    wxXml2Node GetFirstChild() const
        { return wxXml2Node(wxXml2BaseNode::GetFirstChild()); }
    wxXml2Node GetNext() const
        { return wxXml2Node(wxXml2BaseNode::GetNext()); }
    wxXml2Node GetPrevious() const
        { return wxXml2Node(wxXml2BaseNode::GetPrevious()); }

    wxXml2Document GetDoc() const
        { if (GetObj()) return wxXml2Document(GetObj()->doc); return wxXml2EmptyDoc; }
    wxXml2Property GetProperties() const
        { if (GetObj()) return wxXml2Property(GetObj()->properties); return wxXml2EmptyProperty; }

    wxXml2Namespace GetNamespace() const
        { if (GetObj()) return wxXml2Namespace(GetObj()->ns, (wxXml2Node &)(*this)); return wxXml2EmptyNamespace; }
    wxXml2Namespace GetNamespaceDecl() const
        { if (GetObj()) return wxXml2Namespace(GetObj()->nsDef, (wxXml2Node &)(*this)); return wxXml2EmptyNamespace; }

    //! Returns the name of this node.
    //! Note that for nodes of type wxXML2_TEXT_NODE, the name is meaningless;
    //! see #wxXml2NodeType for more info.
    wxString GetName() const
        { if (GetObj()) return XML2WX(GetObj()->name); return wxEmptyString; }

    //! Returns the content of this node.
    //! Note that for nodes of type wxXML2_ELEMENT_NODE, the content is meaningless;
    //! for element nodes use #GetNodeContent; see #wxXml2NodeType for more info.
    wxString GetContent() const
        { if (GetObj()) return XML2WX(GetObj()->content); return wxEmptyString; }

    //! Returns the content of this node; this can be either the text carried
    //! directly by this node if it's a TEXT node or the aggregate string
    //! of the values carried by this node child's (TEXT and ENTITY_REF).
    //! Entity references are substituted.
    //! This is especially useful for wxXML2_ELEMENT_NODE nodes.
    wxString GetNodeContent() const
    {
        if (GetObj())
        {
            xmlChar *str = xmlNodeGetContent(GetObj());
            wxString ret = XML2WX(str);
            xmlFree(str);
            return ret;
        }
        return wxEmptyString;
    }

    //! Returns the libxml2 node structure wrapped by this object. Use this function
    //! only if you know what to do with the returned structure.
    xmlNode *GetObj() const                    { return (xmlNode *)m_obj; }

    //! Returns TRUE if a property named 'propName' exists in this node and, in this
    //! case, returns its value in 'value'.
    bool GetPropVal(const wxString &propName, wxString *value) const;

    //! Returns the value of the property named 'propName' if it exists, otherwise
    //! returns 'defaultVal'.
    wxString GetPropVal(const wxString &propName, const wxString &defaultVal) const;

    //! Returns the n-th child named 'name', or wxXml2EmptyNode if it doesn't exist.
    wxXml2Node Get(const wxString &name, int n = 0)    const
        { return Find(name, wxS(""), n); }

    //! Sets the name of this node.
    void SetName(const wxString &name)
        { xmlNodeSetName(GetObj(), WX2XML(name)); }

    //! Sets the content of this node.
    void SetContent(const wxString &cont)
        { xmlNodeSetContent(GetObj(), WX2XML(cont)); }

    void SetProperties(const wxXml2Property &prop);
    void SetNamespace(wxXml2Namespace &ns);

public:        // miscellaneous

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //! \name Miscellaneous
    //! Some miscellaneous functions which work on wxXml2Nodes. @{

    //! Returns TRUE if this node contains the given one.
    //! Obviously the function performs a recursive check.
    bool Contains(const wxXml2Node &tofind) const
        { return (Find(tofind) != wxXml2EmptyNode); }

    //! Returns TRUE if this node has a property with the given name.
    bool HasProp(const wxString &propName) const;

    //! Makes the name of this tag all uppercase.
    //! Recursively updates the names of all children tags.
    void MakeUpper();

    //! Makes the name of this tag all lowercase.
    //! Recursively updates the names of all children tags.
    void MakeLower();

    //! Returns TRUE if the given node is identic to this one. Checks
    //! only content, name, namespace and type.
    bool Cmp(const wxXml2Node &node) const;

    //! Like Cmp() but this function checks only content, name and type.
    bool CmpNoNs(const wxXml2Node &node) const;

    //! Unlinks the old node from it's current context, prune the new one
    //! at the same place. If cur was already inserted in a document it is
    //! first unlinked from its existing context.
    //! \return The new node (that is, *this)
    wxXml2Node Replace(const wxXml2Node &newnode);

    //! Searches the occ-th occurence of a node with the given name & content,
    //! among the children of this node. If bNS == TRUE, this function also checks
    //! the namespace, otherwise the namespace will be irrelevant:
    //!
    //!      <myns:mynode>[...]</myns:mynode>      and    <mynode>[...]</mynode>
    //!
    //! will both match this query:
    //!
    //!      Find("mynode", wxEmptyString, 0, FALSE);
    //!
    //! This function does not check neither properties, children (except for
    //! the first text child which is tested if content != wxEmptyString)
    //! nor siblings.
    //! The \c recurse parameter can be used to disable the recursive behaviour
    //! (which is the default) of this function. In fact, if \c recurse == FALSE
    //! only the children of this node will be tested and the function won't
    //! descend into the child nodes.
    wxXml2Node Find(const wxString &name,
                    const wxString &content = wxEmptyString,
                    int occ = 0, bool bNS = TRUE, bool recurse = TRUE) const;

    //! Searches the occ-th occurrence of a node like the given one among
    //! all the children of this object. Also checks namespace if bNS == TRUE.
    //! The \c recurse parameter can be used to disable the recursive behaviour
    //! (which is the default) of this function. In fact, if \c recurse == FALSE
    //! only the children of this node will be tested and the function won't
    //! descend into the child nodes.
    wxXml2Node Find(const wxXml2Node &tofind, int occ = 0, bool bNS = TRUE, bool recurse = TRUE) const;

    //! Encapsulates this node in another node with the given name; if the
    //!
    //!      mynode.Encapsulate("newtag");
    //!
    //! is called, then, mynode is
    //!
    //! before:          <mynodetagname>[...]</mynodetagname>
    //! after:   <newtag><mynodetagname>[...]</mynodetagname></newtag>
    //!
    //! \return The new node (that is, *this)
    wxXml2Node Encapsulate(const wxString &nodename,
                 const wxString &content = wxEmptyString,
                wxXml2Namespace &ns = wxXml2EmptyNamespace,
                wxXml2Property &props = wxXml2EmptyProperty);

    //@}
};




// ----------------
// Utility classes
// ----------------

//! A little helper class used by wxXml2Document::Save and
//! wxXml2DTD::Save to convert \n to the native format of newlines.
class WXDLLIMPEXP_WXXML2 wxNativeNewlinesFilterStream : public wxFilterOutputStream
{
public:
    wxNativeNewlinesFilterStream(wxOutputStream &tofilter)
        : wxFilterOutputStream(tofilter) {}
    virtual ~wxNativeNewlinesFilterStream() {}


protected:

    //! Our overridden function. It replaces the libxml2 newlines
    //! with the native newlines for this system.
    virtual size_t OnSysWrite(const void *buffer, size_t bufsize);
};

#endif // _WX_XML2_H_
