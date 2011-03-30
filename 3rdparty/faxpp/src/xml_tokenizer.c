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

#include <string.h>
#include <stdlib.h>

#include "xml_tokenizer.h"
#include "xml_parser.h"
#include "tokenizer_states.h"
#include "char_classes.h"
#include "config.h"
#include <faxpp/token.h>

/*********************
 *
 *  Tokenizer Functions
 *
 *********************/

#define INITIAL_TOKEN_BUFFER_SIZE 64

#define SNIFF_NEXT_CHAR(buf) (((buf) < (unsigned char*)env->buffer_end) ? *(buf)++ : 0x100)

FAXPP_Error
FAXPP_sniff_encoding(FAXPP_Tokenizer *env)
{
  // Make initial judgement on the encoding
  unsigned char *buf = (unsigned char*)env->position;

/*   printf("First bytes: %02X %02X %02X %02X\n", *buf, *(buf + 1), */
/*          *(buf + 2), *(buf + 3)); */

  switch(SNIFF_NEXT_CHAR(buf)) {
  case 0x00:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0x00:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x00:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x3C:
          /* 00 00 00 3C  UCS-4, big-endian machine (1234 order) */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_native_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_be_decode);
#endif
          break;
        }
        break;
      case 0x3C:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* 00 00 3C 00  UCS-4, unusual octet order (2143) */
          return UNSUPPORTED_ENCODING;
        }
        break;
      case 0xFE:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0xFF:
          /* 00 00 FE FF  UCS-4, big-endian machine (1234 order) */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_native_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_be_decode);
#endif
          // Skip BOM
          env->position += 4;
          break;
        }
        break;
      case 0xFF:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0xFE:
          /* 00 00 FF FE  UCS-4, unusual octet order (2143) */
          return UNSUPPORTED_ENCODING;
        }
        break;
      }
      break;
    case 0x3C:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x00:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* 00 3C 00 00  UCS-4, unusual octet order (3412) */
          return UNSUPPORTED_ENCODING;
        case 0x3F:
          /* 00 3C 00 3F  UTF-16, big-endian */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_be_decode);
#endif
          break;
        }
        break;
      }
      break;
    }
    break;
  case 0x3C:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0x00:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x00:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* 3C 00 00 00  UCS-4, little-endian machine (4321 order) */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_le_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_native_decode);
#endif
          break;
        }
        break;
      case 0x3F:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* 3C 00 3F 00  UTF-16, little-endian */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_le_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#endif
          break;
        }
        break;
      }
      break;
    case 0x3F:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x78:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x6D:
          /* 3C 3F 78 6D  UTF-8, ISO 646, ASCII, some part of ISO 8859, Shift-JIS, EUC, etc. */
          FAXPP_set_tokenizer_decode(env, FAXPP_utf8_decode);
          break;
        }
        break;
      }
      break;
    }
    break;
  case 0x4C:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0x6F:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0xA7:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x94:
          /* 4C 6F A7 94 EBCDIC */
          return UNSUPPORTED_ENCODING;
        }
        break;
      }
      break;
    }
    break;
  case 0xEF:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0xBB:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0xBF:
        /* EF BB BF  UTF-8 with byte order mark */
        FAXPP_set_tokenizer_decode(env, FAXPP_utf8_decode);
        // Skip BOM
        env->position += 3;
      }
      break;
    }
    break;
  case 0xFE:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0xFF:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x00:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* FE FF 00 00  UCS-4, unusual octet order (3412) */
          return UNSUPPORTED_ENCODING;
        default:
          /* FE FF ## ##  UTF-16, big-endian */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_be_decode);
#endif
          // Skip BOM
          env->position += 2;
          break;
        }
        break;
      default:
        /* FE FF ## ##  UTF-16, big-endian */
#ifdef WORDS_BIGENDIAN
        FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#else
        FAXPP_set_tokenizer_decode(env, FAXPP_utf16_be_decode);
#endif
        // Skip BOM
        env->position += 2;
        break;
      }
      break;
    }
    break;
  case 0xFF:
    switch(SNIFF_NEXT_CHAR(buf)) {
    case 0xFE:
      switch(SNIFF_NEXT_CHAR(buf)) {
      case 0x00:
        switch(SNIFF_NEXT_CHAR(buf)) {
        case 0x00:
          /* FF FE 00 00  UCS-4, little-endian machine (4321 order) */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_le_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_ucs4_native_decode);
#endif
          // Skip BOM
          env->position += 4;
          break;
        default:
          /* FF FE ## ##  UTF-16, little-endian */
#ifdef WORDS_BIGENDIAN
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_le_decode);
#else
          FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#endif
          // Skip BOM
          env->position += 2;
          break;
        }
        break;
      default:
        /* FF FE ## ##  UTF-16, little-endian */
#ifdef WORDS_BIGENDIAN
        FAXPP_set_tokenizer_decode(env, FAXPP_utf16_le_decode);
#else
        FAXPP_set_tokenizer_decode(env, FAXPP_utf16_native_decode);
#endif
        // Skip BOM
        env->position += 2;
        break;
      }
      break;
    }
    break;
  }

  if(env->decode == 0) {
    // Default encoding is UTF-8
    FAXPP_set_tokenizer_decode(env, FAXPP_utf8_decode);
  }

  token_start_position(env);
  return NO_ERROR;
}

FAXPP_DecodeFunction
FAXPP_get_tokenizer_decode(const FAXPP_Tokenizer *tokenizer)
{
  return tokenizer->decode;
}

void
FAXPP_set_tokenizer_decode(FAXPP_Tokenizer *tokenizer, FAXPP_DecodeFunction decode)
{
  tokenizer->do_encode = 1;

  if(decode == FAXPP_utf16_native_decode ||
#ifdef WORDS_BIGENDIAN
     decode == FAXPP_utf16_be_decode
#else
     decode == FAXPP_utf16_le_decode
#endif
     ) {
    tokenizer->decode = FAXPP_utf16_native_decode;

    if(tokenizer->transcoder.encode == FAXPP_utf16_native_encode)
      tokenizer->do_encode = 0;

    tokenizer->start_element_name_state = utf16_start_element_name_state;
    tokenizer->element_content_state = utf16_element_content_state;
  }
  else if(decode == FAXPP_utf8_decode) {
    tokenizer->decode = FAXPP_utf8_decode;

    if(tokenizer->transcoder.encode == FAXPP_utf8_encode)
      tokenizer->do_encode = 0;

    tokenizer->start_element_name_state = utf8_start_element_name_state;
    tokenizer->element_content_state = utf8_element_content_state;
  }
  else if(decode == FAXPP_iso_8859_1_decode) {
    tokenizer->decode = FAXPP_iso_8859_1_decode;

    // Latin1 can use the UTF-8 states, since the first 128 values are the same as UTF-8
    tokenizer->start_element_name_state = utf8_start_element_name_state;
    tokenizer->element_content_state = utf8_element_content_state;
  }
  else if(decode == FAXPP_ucs4_native_decode ||
#ifdef WORDS_BIGENDIAN
     decode == FAXPP_ucs4_be_decode
#else
     decode == FAXPP_ucs4_le_decode
#endif
     ) {
    tokenizer->decode = FAXPP_ucs4_native_decode;

    tokenizer->start_element_name_state = default_start_element_name_state;
    tokenizer->element_content_state = default_element_content_state;
  }
  else {
    tokenizer->decode = decode;

    tokenizer->start_element_name_state = default_start_element_name_state;
    tokenizer->element_content_state = default_element_content_state;
  }
}

void change_token_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer)
{
  FAXPP_TokenizerEnv *env = (FAXPP_TokenizerEnv*)userData;

  env->token_position1 += newBuffer - buffer->buffer;
  env->token_position2 += newBuffer - buffer->buffer;

  env->token.value.ptr = newBuffer;
  env->result_token.value.ptr = newBuffer;
}

FAXPP_Tokenizer *
FAXPP_create_tokenizer(FAXPP_Transcoder encode)
{
  FAXPP_TokenizerEnv *result = malloc(sizeof(FAXPP_TokenizerEnv));
  if(result == 0) return 0;

  memset(result, 0, sizeof(FAXPP_TokenizerEnv));
  result->transcoder = encode;
  if(FAXPP_init_buffer(&result->token_buffer, INITIAL_TOKEN_BUFFER_SIZE,
                       change_token_buffer, result) == OUT_OF_MEMORY) {
    free(result);
    return 0;
  }

  return result;
}

static void free_tokenizer_internal(FAXPP_Tokenizer *tokenizer)
{
  if(tokenizer->read_buffer) free(tokenizer->read_buffer);
  FAXPP_free_buffer(&tokenizer->token_buffer);
  free(tokenizer);
}

void
FAXPP_free_tokenizer(FAXPP_Tokenizer *tokenizer)
{
  FAXPP_Tokenizer *tmp;

  while(tokenizer) {
    tmp = tokenizer;
    tokenizer = tmp->prev;

    if(tmp->read_buffer) free(tmp->read_buffer);
    FAXPP_free_buffer(&tmp->token_buffer);
    free(tmp);
  }
}

static void init_tokenize_internal(FAXPP_Tokenizer *env)
{
  env->current_char = 0;
  env->char_len = 0;

  env->line = 1;
  env->column = 0;

  env->nesting_level = 0;
  env->elemdecl_content_level = 0;
  env->ignore_start_level = 0;
  env->do_encode = 1;

  env->seen_doctype = 0;
  env->internal_subset = 0;
  env->external_subset = 0;
  env->seen_doc_element = 0;
  env->element_entity = 0;
  env->attr_entity = 0;
  env->internal_dtd_entity = 0;
  env->external_dtd_entity = 0;
  env->external_parsed_entity = 0;
  env->in_markup_entity = 0;
  env->external_in_markup_entity = 0;

  env->start_of_entity = 0;
  env->start_of_file = 0;

  env->decode = 0;

  env->token_buffer.cursor = 0;

  env->token_position1 = 0;
  env->token_position2 = 0;

  env->state = initial_state;
  env->stored_state = 0;

  env->start_element_name_state = default_start_element_name_state;
  env->element_content_state = default_element_content_state;

  env->ncname_start_char = NCNAME_START_CHAR10;
  env->ncname_char = NCNAME_CHAR10;
  env->non_restricted_char = NON_RESTRICTED_CHAR10;
  env->xml_char = CHAR10;
}

FAXPP_Error
FAXPP_init_tokenize(FAXPP_Tokenizer *env, void *buffer, unsigned int length, unsigned int done)
{
  init_tokenize_internal(env);

  env->buffer = buffer;
  env->buffer_end = buffer + length;
  env->position = buffer;

  env->buffer_done = done;

  FAXPP_Error err = FAXPP_sniff_encoding(env);
  if(err) return err;

  return NO_ERROR;
}

FAXPP_Error
FAXPP_push_entity_tokenizer(FAXPP_Tokenizer **list, FAXPP_EntityParseState state, unsigned int internal_buffer,
                            void *buffer, unsigned int length, unsigned int done)
{
  FAXPP_Tokenizer *env = FAXPP_create_tokenizer((*list)->transcoder);
  if(!env) return OUT_OF_MEMORY;

  env->prev = *list;
  *list = env;

  init_tokenize_internal(env);

  env->buffer = buffer;
  env->buffer_end = buffer + length;
  env->position = buffer;

  if(state == IN_MARKUP_ENTITY || state == EXTERNAL_IN_MARKUP_ENTITY) {
    env->elemdecl_content_start_level = env->prev->elemdecl_content_level;
    env->elemdecl_content_level = env->prev->elemdecl_content_level;
  }

  env->buffer_done = done;

  env->normalize_attrs = env->prev->normalize_attrs;
  env->buffered_token = env->prev->buffered_token;
  env->null_terminate = env->prev->null_terminate;

  env->element_entity = state == ELEMENT_CONTENT_ENTITY;
  env->attr_entity = state == ATTRIBUTE_VALUE_ENTITY;
  env->internal_dtd_entity = state == INTERNAL_DTD_ENTITY;
  env->external_dtd_entity = state == EXTERNAL_DTD_ENTITY;
  env->external_parsed_entity = state == EXTERNAL_PARSED_ENTITY;
  env->external_subset = state == EXTERNAL_SUBSET_ENTITY;
  env->in_markup_entity = state == IN_MARKUP_ENTITY;
  env->external_in_markup_entity = state == EXTERNAL_IN_MARKUP_ENTITY;

  if(internal_buffer) {
    FAXPP_set_tokenizer_decode(env, env->prev->transcoder.decode);
  }

  switch(state) {
  case ELEMENT_CONTENT_ENTITY:
    env->state = parsed_entity_state;
    break;
  case ATTRIBUTE_VALUE_ENTITY:
    if(env->transcoder.encode == FAXPP_utf8_encode)
      env->state = utf8_attr_value_state_en;
    else if(env->transcoder.encode == FAXPP_utf16_native_encode)
      env->state = utf16_attr_value_state_en;
    else
      env->state = default_attr_value_state_en;
    break;
  case INTERNAL_DTD_ENTITY:
    env->state = internal_subset_state_en;
    break;
  case EXTERNAL_DTD_ENTITY:
    env->state = external_subset_state;
    break;
  case IN_MARKUP_ENTITY:
    env->state = env->prev->state;
    break;
  case EXTERNAL_PARSED_ENTITY:
  case EXTERNAL_SUBSET_ENTITY:
  case EXTERNAL_IN_MARKUP_ENTITY:
    env->state = initial_state;
    break;
  }

  env->ncname_start_char = env->prev->ncname_start_char;
  env->ncname_char = env->prev->ncname_char;
  env->non_restricted_char = env->prev->non_restricted_char;
  env->xml_char = env->prev->xml_char;

  token_start_position(env);

  return NO_ERROR;
}

FAXPP_Error
FAXPP_pop_tokenizer(FAXPP_Tokenizer **list)
{
  FAXPP_Error err = NO_ERROR;
  FAXPP_TokenizerEnv *env = *list;
  *list = env->prev;

  if(env->start_of_entity) {
    if(env->in_markup_entity || env->external_in_markup_entity) {
      if(env->nesting_level != 0 || env->elemdecl_content_level != env->elemdecl_content_start_level)
        err = IMPROPER_NESTING_OF_ENTITY;
    }
    else if(env->stored_state != 0 || env->nesting_level != 0 || env->elemdecl_content_level != 0 ||
       (env->element_entity && env->state != parsed_entity_state &&
        env->state != default_element_content_rsquare_state1 &&
        env->state != default_element_content_rsquare_state2) ||
       (env->internal_dtd_entity && env->state != internal_subset_state_en) ||
       (env->external_dtd_entity && env->state != external_subset_state &&
        env->state != external_subset_seen_rsquare_state1 &&
        env->state != external_subset_seen_rsquare_state2)
       ) {
      err = INCOMPLETE_MARKUP_IN_ENTITY_VALUE;
    }
  }

  if(env->in_markup_entity || env->external_in_markup_entity || !env->start_of_entity) {
    // Force the old tokenizer token to point into the token buffer
    FAXPP_tokenizer_release_buffer(env, 0);

    (*list)->token = env->token;

    FAXPP_swap_buffer(&env->token_buffer, &(*list)->token_buffer);

    (*list)->token_position1 = env->token_position1;
    (*list)->token_position2 = env->token_position2;

    // If the token is empty, set it to point to the correct tokenizer buffer
    if((*list)->token.value.len == 0) {
      token_start_position(*list);
    }

    (*list)->nesting_level += env->nesting_level;

    (*list)->state = env->state;
    (*list)->stored_state = env->stored_state;
  }

  (*list)->result_token.type = NO_TOKEN;

  free_tokenizer_internal(env);

  return err;
}

FAXPP_Error
FAXPP_tokenizer_release_buffer(FAXPP_Tokenizer *tokenizer, void **buffer_position)
{
  if(buffer_position) *buffer_position = tokenizer->position;

  // Check if the partial token in the tokenizer needs copying to the token_buffer
  if(tokenizer->token.value.ptr >= tokenizer->buffer &&
     tokenizer->token.value.ptr < tokenizer->buffer_end) {
    void *token_start = tokenizer->token.value.ptr;

    // Find the length of the partial token
    unsigned int token_length = tokenizer->token.value.len;
    if(!token_length)
      token_length = tokenizer->position - tokenizer->token.value.ptr;

    // Re-position the token positions to point into the token_buffer
    FAXPP_reset_buffer(&tokenizer->token_buffer);
    tokenizer->token_position1 += tokenizer->token_buffer.cursor - token_start;
    tokenizer->token_position2 += tokenizer->token_buffer.cursor - token_start;
    tokenizer->token.value.ptr = tokenizer->token_buffer.cursor;
    tokenizer->token.value.len = token_length;

    return FAXPP_buffer_append(&tokenizer->token_buffer, token_start, token_length);
  }

  return NO_ERROR;
}

FAXPP_Error
FAXPP_continue_tokenize(FAXPP_Tokenizer *env, void *buffer, unsigned int length, unsigned int done)
{
  if(env->token.value.ptr == env->buffer_end)
    env->token.value.ptr = buffer;

  env->buffer = buffer;
  env->buffer_end = buffer + length;
  env->buffer_done = done;

  env->position = buffer;

  return NO_ERROR;
}

FAXPP_Error
FAXPP_next_token(FAXPP_Tokenizer *env)
{
  env->result_token.type = NO_TOKEN;

  FAXPP_Error err = 0;
  while(err == NO_ERROR && env->result_token.type == NO_TOKEN) {
    err = env->state(env);
  }

  return err;
}

const FAXPP_Token *
FAXPP_get_current_token(const FAXPP_Tokenizer *tokenizer)
{
  return &tokenizer->result_token;
}

unsigned int
FAXPP_get_tokenizer_nesting_level(const FAXPP_Tokenizer *tokenizer)
{
  return tokenizer->nesting_level;
}

unsigned int
FAXPP_get_tokenizer_error_line(const FAXPP_Tokenizer *tokenizer)
{
  return tokenizer->line;
}

unsigned int
FAXPP_get_tokenizer_error_column(const FAXPP_Tokenizer *tokenizer)
{
  return tokenizer->column;
}
