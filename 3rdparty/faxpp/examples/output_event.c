/*
 * Copyright 2007 Doxological Ltd.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* #include <stdio.h> */
/* #include <stdlib.h> */
/* #include <errno.h> */
/* #include <string.h> */

#include "output_event.h"

#define SHOW_URIS 0
#define SHOW_ENTITIES 0

void
output_text(const FAXPP_Text *text, FILE *stream)
{
  char *buffer = (char*)text->ptr;
  char *buffer_end = buffer + text->len;

  while(buffer < buffer_end) {
    putc(*buffer++, stream);
  }
}

void
output_escaped_attr_text(const FAXPP_Text *text, FILE *stream)
{
  char *buffer = (char*)text->ptr;
  char *buffer_end = buffer + text->len;

  while(buffer < buffer_end) {
    if(*buffer == '&') {
      fprintf(stream, "&amp;");
    }
    else if(*buffer == '<') {
      fprintf(stream, "&lt;");
    }
    else if(*buffer == '"') {
      fprintf(stream, "&quot;");
    }
    else {
      putc(*buffer, stream);
    }
    ++buffer;
  }
}

void output_attr_value(const FAXPP_AttrValue *atval, FILE *stream)
{
  while(atval) {
    switch(atval->type) {
    case CHARACTERS_EVENT:
      output_escaped_attr_text(&atval->value, stream);
      break;
    case ENTITY_REFERENCE_EVENT:
      fprintf(stream, "&");
      output_text(&atval->name, stream);
      fprintf(stream, ";");
      break;
    case DEC_CHAR_REFERENCE_EVENT:
      fprintf(stream, "&#");
      output_text(&atval->name, stream);
      fprintf(stream, ";");
      break;
    case HEX_CHAR_REFERENCE_EVENT:
      fprintf(stream, "&#x");
      output_text(&atval->name, stream);
      fprintf(stream, ";");
      break;
    case ENTITY_REFERENCE_START_EVENT:
#if SHOW_ENTITIES
      fprintf(stream, "&");
      output_text(&atval->name, stream);
      fprintf(stream, ";(");
#endif
      break;
    case ENTITY_REFERENCE_END_EVENT:
#if SHOW_ENTITIES
      fprintf(stream, ")");
#endif
      break;
    default:
      break;
    }
    atval = atval->next;
  }
}

void
output_event(const FAXPP_Event *event, FILE *stream)
{
  int i;

  switch(event->type) {
  case START_DOCUMENT_EVENT:
    if(event->version.ptr != 0) {
      fprintf(stream, "<?xml version=\"");
      output_text(&event->version, stream);
      if(event->encoding.ptr != 0) {
        fprintf(stream, "\" encoding=\"");
        output_text(&event->encoding, stream);
      }
      if(event->standalone.ptr != 0) {
        fprintf(stream, "\" standalone=\"");
        output_text(&event->standalone, stream);
      }
      fprintf(stream, "\"?>");
    }
    break;
  case END_DOCUMENT_EVENT:
    break;
  case DOCTYPE_EVENT:
    fprintf(stream, "<!DOCTYPE ");

    if(event->prefix.ptr != 0) {
      output_text(&event->prefix, stream);
      fprintf(stream, ":");
    }
    output_text(&event->name, stream);

    if(event->system_id.ptr != 0) {
      if(event->public_id.ptr != 0) {
        fprintf(stream, " PUBLIC \"");
        output_text(&event->public_id, stream);
        fprintf(stream, "\" \"");
        output_text(&event->system_id, stream);
        fprintf(stream, "\"");
      }
      else {
        fprintf(stream, " SYSTEM \"");
        output_text(&event->system_id, stream);
        fprintf(stream, "\"");
      }
    }
    fprintf(stream, ">");
    break;
  case START_ELEMENT_EVENT:
  case SELF_CLOSING_ELEMENT_EVENT:
    fprintf(stream, "<");
#if SHOW_URIS
    if(event->uri.ptr != 0) {
      fprintf(stream, "{");
      output_text(&event->uri, stream);
      fprintf(stream, "}");
    } else
#endif
    if(event->prefix.ptr != 0) {
      output_text(&event->prefix, stream);
      fprintf(stream, ":");
    }
    output_text(&event->name, stream);

    for(i = 0; i < event->attr_count; ++i) {
      fprintf(stream, " ");
#if SHOW_URIS
      if(event->attrs[i].uri.ptr != 0) {
        fprintf(stream, "{");
        output_text(&event->attrs[i].uri, stream);
        fprintf(stream, "}");
      } else
#endif
      if(event->attrs[i].prefix.ptr != 0) {
        output_text(&event->attrs[i].prefix, stream);
        fprintf(stream, ":");
      }
      output_text(&event->attrs[i].name, stream);
      fprintf(stream, "=\"");
      output_attr_value(&event->attrs[i].value, stream);
      fprintf(stream, "\"");
    }

    if(event->type == SELF_CLOSING_ELEMENT_EVENT)
      fprintf(stream, "/>");
    else
      fprintf(stream, ">");
    break;
  case END_ELEMENT_EVENT:
    fprintf(stream, "</");
#if SHOW_URIS
    if(event->uri.ptr != 0) {
      fprintf(stream, "{");
      output_text(&event->uri, stream);
      fprintf(stream, "}");
    } else
#endif
    if(event->prefix.ptr != 0) {
      output_text(&event->prefix, stream);
      fprintf(stream, ":");
    }
    output_text(&event->name, stream);
    fprintf(stream, ">");
    break;
  case CHARACTERS_EVENT:
    output_text(&event->value, stream);
    break;
  case CDATA_EVENT:
    fprintf(stream, "<![CDATA[");
    output_text(&event->value, stream);
    fprintf(stream, "]]>");
    break;
  case IGNORABLE_WHITESPACE_EVENT:
    output_text(&event->value, stream);
    break;
  case COMMENT_EVENT:
    fprintf(stream, "<!--");
    output_text(&event->value, stream);
    fprintf(stream, "-->");
    break;
  case PI_EVENT:
    fprintf(stream, "<?");
    output_text(&event->name, stream);
    if(event->value.ptr != 0) {
      fprintf(stream, " ");
      output_text(&event->value, stream);
    }
    fprintf(stream, "?>");
    break;
  case ENTITY_REFERENCE_EVENT:
    fprintf(stream, "&");
    output_text(&event->name, stream);
    fprintf(stream, ";");
    break;
  case DEC_CHAR_REFERENCE_EVENT:
    fprintf(stream, "&#");
    output_text(&event->name, stream);
    fprintf(stream, ";");
    break;
  case HEX_CHAR_REFERENCE_EVENT:
    fprintf(stream, "&#x");
    output_text(&event->name, stream);
    fprintf(stream, ";");
    break;
  case ENTITY_REFERENCE_START_EVENT:
#if SHOW_ENTITIES
    fprintf(stream, "&");
    output_text(&event->name, stream);
    fprintf(stream, ";(");
#endif
    break;
  case ENTITY_REFERENCE_END_EVENT:
#if SHOW_ENTITIES
    fprintf(stream, ")");
#endif
    break;
  case START_EXTERNAL_ENTITY_EVENT:
  case END_EXTERNAL_ENTITY_EVENT:
  case NO_EVENT:
    break;
  }
}

