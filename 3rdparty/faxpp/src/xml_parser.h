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

#ifndef __FAXPP__XML_PARSER_H
#define __FAXPP__XML_PARSER_H

#include <stdio.h>

#include "xml_tokenizer.h"
#include "buffer.h"
#include <faxpp/parser.h>

/*********************
 *
 *  Parser Environment
 *
 *********************/

typedef struct FAXPP_NamespaceInfo_s {
  FAXPP_Text prefix;
  FAXPP_Text uri;

  struct FAXPP_NamespaceInfo_s *prev;
} FAXPP_NamespaceInfo;

typedef struct FAXPP_ElementInfo_s {
  FAXPP_Text prefix;
  FAXPP_Text uri;
  FAXPP_Text name;

  FAXPP_NamespaceInfo *ns;
  FAXPP_NamespaceInfo *prev_ns;

  FAXPP_Buffer buffer;

  struct FAXPP_ElementInfo_s *prev;
} FAXPP_ElementInfo;

struct FAXPP_EntityValue_s {
  FAXPP_TokenType type;
  FAXPP_Text value;
  FAXPP_EntityInfo *entity_ref;

  unsigned int line;
  unsigned int column;

  FAXPP_EntityValue *prev;
};

struct FAXPP_EntityInfo_s {
  FAXPP_Text name;
  FAXPP_Text base_uri;

  FAXPP_EntityValue *value;

  unsigned int external:1;
  unsigned int unparsed:1;
  unsigned int from_internal_subset:1;

  unsigned int line;
  unsigned int column;

  FAXPP_EntityInfo *next;
};

typedef enum {
  CONTENTSPEC_NONE = 0,
  CONTENTSPEC_SEQUENCE,
  CONTENTSPEC_CHOICE
} FAXPP_ContentSpecType;

typedef struct FAXPP_ContentSpec_s FAXPP_ContentSpec;

struct FAXPP_ContentSpec_s {
  FAXPP_ContentSpecType type;
  FAXPP_ContentSpec *parent;
};

typedef struct FAXPP_ParserEnv_s FAXPP_ParserEnv;

typedef FAXPP_Error (*FAXPP_NextEvent)(FAXPP_ParserEnv *env);

struct FAXPP_ParserEnv_s {
  FAXPP_NextEvent next_event;
  FAXPP_NextEvent main_next_event;

  FAXPP_ExternalEntityCallback external_entity_callback;
  void *external_entity_user_data;

  FAXPP_TokenizerEnv *tenv;

  unsigned int err_line;
  unsigned int err_column;

  FAXPP_Event event;
  FAXPP_EntityInfo *event_entity;

  unsigned int max_attr_count;
  FAXPP_Attribute *attrs;

  FAXPP_Attribute *current_attr;
  FAXPP_EntityInfo *current_entity;
  FAXPP_ContentSpec *current_elementdecl;
  unsigned int current_attlist:1;
  unsigned int current_notation:1;

  unsigned int standalone:1;
  unsigned int event_returned:1;

  enum {
    XML_VERSION_NOT_KNOWN = 0,
    XML_VERSION_1_0,
    XML_VERSION_1_1
  } xml_version;

  FAXPP_AttrValue *av_ptr;
  FAXPP_AttrValue *av_dealloc;

  FAXPP_ElementInfo *element_info_stack;
  FAXPP_NamespaceInfo *namespace_stack;

  FAXPP_ElementInfo *element_info_pool;
  FAXPP_NamespaceInfo *namespace_pool;

  FAXPP_EntityInfo *general_entities;
  FAXPP_EntityInfo *parameter_entities;
  FAXPP_Buffer entity_buffer;

  FAXPP_Buffer event_buffer;
};

#endif
