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

#ifndef __FAXPP__XML_TOKENIZER_H
#define __FAXPP__XML_TOKENIZER_H

#include "buffer.h"
#include <faxpp/tokenizer.h>
#include <faxpp/parser.h>

/*********************
 *
 *  Tokenizer Environment
 *
 *********************/

typedef struct FAXPP_TokenizerEnv_s FAXPP_TokenizerEnv;
typedef FAXPP_Error (*FAXPP_StateFunction)(FAXPP_TokenizerEnv *env);

typedef struct FAXPP_EntityInfo_s FAXPP_EntityInfo;
typedef struct FAXPP_EntityValue_s FAXPP_EntityValue;

struct FAXPP_TokenizerEnv_s {
  FAXPP_ReadCallback read;
  void *read_user_data;

  void *read_buffer;
  unsigned int read_buffer_length;

  void *buffer;
  void *buffer_end;

  void *position;
  Char32 current_char;
  unsigned int char_len;

  unsigned int line;
  unsigned int column;

  unsigned int nesting_level;
  unsigned int ignore_start_level;
  unsigned int elemdecl_content_level;
  unsigned int elemdecl_content_start_level;
  unsigned int do_encode:1;
  unsigned int buffer_done:1;

  unsigned int seen_doctype:1;
  unsigned int internal_subset:1;
  unsigned int external_subset:1;
  unsigned int seen_doc_element:1;
  unsigned int element_entity:1;
  unsigned int attr_entity:1;
  unsigned int internal_dtd_entity:1;
  unsigned int external_dtd_entity:1;
  unsigned int external_parsed_entity:1;
  unsigned int in_markup_entity:1;
  unsigned int external_in_markup_entity:1;

  unsigned int normalize_attrs:1;
  unsigned int user_provided_decode:1;
  unsigned int buffered_token:1;
  unsigned int null_terminate:1;

  unsigned int start_of_entity:1;
  unsigned int start_of_file:1;

  FAXPP_DecodeFunction decode;
  FAXPP_Transcoder transcoder;

  FAXPP_Token result_token;
  FAXPP_Token token;

  FAXPP_Buffer token_buffer;
  void *token_position1;
  void *token_position2;

  FAXPP_StateFunction state;
  FAXPP_StateFunction stored_state;

  FAXPP_StateFunction start_element_name_state;
  FAXPP_StateFunction element_content_state;

  uint8_t ncname_start_char;
  uint8_t ncname_char;
  uint8_t non_restricted_char;
  uint8_t xml_char;

  FAXPP_Text base_uri;
  FAXPP_EntityInfo *entity;
  struct FAXPP_TokenizerEnv_s *prev;
};

// The first three values are the same as the values in FAXPP_EntityType
typedef enum {
  EXTERNAL_PARSED_ENTITY2 = EXTERNAL_PARSED_ENTITY,
  EXTERNAL_SUBSET_ENTITY2 = EXTERNAL_SUBSET_ENTITY,
  EXTERNAL_IN_MARKUP_ENTITY2 = EXTERNAL_IN_MARKUP_ENTITY,

  ELEMENT_CONTENT_ENTITY,
  INTERNAL_DTD_ENTITY,
  EXTERNAL_DTD_ENTITY,
  IN_MARKUP_ENTITY,
  ATTRIBUTE_VALUE_ENTITY
} FAXPP_EntityParseState;

FAXPP_Error FAXPP_sniff_encoding(FAXPP_Tokenizer *tokenizer);
FAXPP_Error FAXPP_push_entity_tokenizer(FAXPP_Tokenizer **list, FAXPP_EntityParseState state,
                                        unsigned int internal_buffer,
                                        void *buffer, unsigned int length, unsigned int done);
FAXPP_Error FAXPP_pop_tokenizer(FAXPP_Tokenizer **list);

#endif
