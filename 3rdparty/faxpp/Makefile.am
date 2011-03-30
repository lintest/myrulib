AUTOMAKE_OPTIONS = foreign dist-zip
SUBDIRS = include

INCLUDES = -I$(top_srcdir)/include/ 

noinst_PROGRAMS = tokenizer_example parser_example xmlconf_runner
lib_LTLIBRARIES = libfaxpp.la

# Library version, see http://sourceware.org/autobook/autobook/autobook_91.html
libfaxpp_la_LDFLAGS = -version-info 2:1:0 -no-undefined

libfaxpp_la_SOURCES = \
src/attr_states.h \
src/buffer.c \
src/buffer.h \
src/cdata.c \
src/char_classes.c \
src/char_classes.h \
src/comment.c \
src/element.c \
src/element_states.h \
src/error.c \
src/event.c \
src/pi.c \
src/reference.c \
src/token.c \
src/tokenizer_states.c \
src/tokenizer_states.h \
src/transcode.c \
src/xmldecl.c \
src/xml_parser.c \
src/xml_parser.h \
src/xml_tokenizer.c \
src/xml_tokenizer.h \
src/doctype.c \
src/elementdecl.c \
src/attlistdecl.c \
src/notationdecl.c \
src/entitydecl.c \
src/conditional.c \
src/system_public_states.h

tokenizer_example_LDADD = libfaxpp.la
tokenizer_example_SOURCES = examples/tokenizer_example.c

parser_example_LDADD = libfaxpp.la
parser_example_SOURCES = \
examples/parser_example.c \
examples/entity_resolver.c \
examples/entity_resolver.h \
examples/output_event.c \
examples/output_event.h

xmlconf_runner_LDADD = libfaxpp.la
xmlconf_runner_SOURCES = \
tests/xmlconf_runner.c \
examples/entity_resolver.c \
examples/entity_resolver.h \
examples/output_event.c \
examples/output_event.h

EXTRA_DIST = \
docs/Doxyfile.api \
docs/header.html \
docs/api \
LICENSE \
TODO \
ChangeLog

.PHONY: docs

docs:
	$(MAKE) -C include docs
