#!/bin/sh
# prepare.sh - process bakefile "myrulib.bkl" and patch it.

bakefile -f autoconf myrulib.bkl 
sed -i 's|$(srcdir)/sources/wxsqlite3.cpp|wxsqlite3.cpp|g' Makefile.in
