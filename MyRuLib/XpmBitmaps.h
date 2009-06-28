#ifndef __XPMBITMAPS_H__
#define __XPMBITMAPS_H__

#include "res/new.xpm"
#include "res/find.xpm"
#include "res/dir_down.xpm"
#include "res/new_dir.xpm"
#include "res/htmbook.xpm"
#include "res/nocheck.xpm"
#include "res/checked.xpm"
#include "res/checkout.xpm"

#include "res/folder_open.xpm"
#include "res/exefile.xpm"

class NoWarnings
{
    NoWarnings() {
        wxUnusedVar(new_xpm);
        wxUnusedVar(find_xpm);
        wxUnusedVar(dir_down_xpm);
        wxUnusedVar(new_dir_xpm);
        wxUnusedVar(htmbook_xpm);
        wxUnusedVar(nocheck_xpm);
        wxUnusedVar(checked_xpm);
        wxUnusedVar(checkout_xpm);

        wxUnusedVar(folder_open_xpm);
        wxUnusedVar(exefile_xpm);
    };
};

#endif // __XPMBITMAPS_H__
