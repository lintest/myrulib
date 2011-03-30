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

#include "xml_parser.h"
#include "char_classes.h"
#include "tokenizer_states.h"
#include "config.h"

#define INITIAL_ATTRS_SIZE 4
#define INITIAL_NS_SIZE 6

/// Must be at least 4 for encoding sniffing
/// Must be a multiple of 4
#define READ_BUFFER_SIZE 16 * 1024

#define INITIAL_EVENT_BUFFER_SIZE 256
#define INITIAL_ENTITY_BUFFER_SIZE 256
#define INITIAL_ELEMENT_INFO_BUFFER_SIZE 256

static FAXPP_Error nc_start_document_next_event(FAXPP_ParserEnv *env);
static FAXPP_Error nc_dtd_next_event(FAXPP_ParserEnv *env);
static FAXPP_Error nc_next_event(FAXPP_ParserEnv *env);
static FAXPP_Error nc_unsupported_encoding_next_event(FAXPP_ParserEnv *env);

static FAXPP_Error wf_next_event(FAXPP_ParserEnv *env);

static void p_change_event_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer);
static void p_change_entity_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer);

FAXPP_Parser *FAXPP_create_parser(FAXPP_ParseMode mode, FAXPP_Transcoder encode)
{
  FAXPP_ParserEnv *env = malloc(sizeof(FAXPP_ParserEnv));
  memset(env, 0, sizeof(FAXPP_ParserEnv));

  env->max_attr_count = INITIAL_ATTRS_SIZE;
  env->attrs = (FAXPP_Attribute*)malloc(sizeof(FAXPP_Attribute) * INITIAL_ATTRS_SIZE);
  if(!env->attrs) {
    FAXPP_free_parser(env);
    return 0;
  }

  if(FAXPP_init_buffer(&env->event_buffer, INITIAL_EVENT_BUFFER_SIZE, p_change_event_buffer, env) == OUT_OF_MEMORY) {
    FAXPP_free_parser(env);
    return 0;
  }

  if(FAXPP_init_buffer(&env->entity_buffer, INITIAL_ENTITY_BUFFER_SIZE, p_change_entity_buffer, env) == OUT_OF_MEMORY) {
    FAXPP_free_parser(env);
    return 0;
  }

  env->tenv = FAXPP_create_tokenizer(encode);
  if(!env->tenv) {
    FAXPP_free_parser(env);
    return 0;
  }

  switch(mode) {
  case NO_CHECKS_PARSE_MODE:
    env->main_next_event = nc_next_event;
    FAXPP_set_normalize_attrs(env, 0);
    break;
  case WELL_FORMED_PARSE_MODE:
    env->main_next_event = wf_next_event;
    FAXPP_set_normalize_attrs(env, 1);
    break;
  }

  env->next_event = nc_start_document_next_event;

  return env;
}

static void p_free_entity_list(FAXPP_EntityInfo *list)
{
  FAXPP_EntityInfo *ent;
  FAXPP_EntityValue *entv;

  while(list) {
    ent = list;
    list = ent->next;

    while(ent->value) {
      entv = ent->value;
      ent->value = entv->prev;
      free(entv);
    }

    free(ent);
  }
}

void FAXPP_free_parser(FAXPP_Parser *env)
{
  FAXPP_AttrValue *at;
  FAXPP_ElementInfo *el;
  FAXPP_NamespaceInfo *ns;
  FAXPP_ContentSpec *cs;

  if(env->attrs) free(env->attrs);

  while(env->current_elementdecl) {
    cs = env->current_elementdecl;
    env->current_elementdecl = cs->parent;
    free(cs);
  }

  while(env->av_dealloc) {
    at = env->av_dealloc;
    env->av_dealloc = at->dealloc_next;
    free(at);
  }

  while(env->element_info_stack) {
    el = env->element_info_stack;
    env->element_info_stack = el->prev;
    FAXPP_free_buffer(&el->buffer);
    free(el);
  }

  while(env->namespace_stack) {
    ns = env->namespace_stack;
    env->namespace_stack = ns->prev;
    free(ns);
  }

  while(env->element_info_pool) {
    el = env->element_info_pool;
    env->element_info_pool = el->prev;
    FAXPP_free_buffer(&el->buffer);
    free(el);
  }

  p_free_entity_list(env->general_entities);
  p_free_entity_list(env->parameter_entities);

  while(env->namespace_pool) {
    ns = env->namespace_pool;
    env->namespace_pool = ns->prev;
    free(ns);
  }

  FAXPP_free_buffer(&env->entity_buffer);
  FAXPP_free_buffer(&env->event_buffer);

  FAXPP_free_tokenizer(env->tenv);
  free(env);
}

void FAXPP_set_null_terminate(FAXPP_Parser *parser, unsigned int boolean)
{
  parser->tenv->null_terminate = boolean != 0;
}

void FAXPP_set_normalize_attrs(FAXPP_Parser *parser, unsigned int boolean)
{
  parser->tenv->normalize_attrs = boolean != 0;
}

void FAXPP_set_encode(FAXPP_Parser *parser, FAXPP_Transcoder encode)
{
  parser->tenv->transcoder = encode;
}


FAXPP_DecodeFunction FAXPP_get_decode(const FAXPP_Parser *parser)
{
  return parser->tenv->decode;
}

void FAXPP_set_decode(FAXPP_Parser *parser, FAXPP_DecodeFunction decode)
{
  FAXPP_set_tokenizer_decode(parser->tenv, decode);
  parser->tenv->user_provided_decode = 1;
  if(parser->next_event == nc_unsupported_encoding_next_event) {
    if(parser->tenv->external_subset)
      parser->next_event = nc_dtd_next_event;
    else
      parser->next_event = parser->main_next_event;
  }
}

const FAXPP_Text *FAXPP_get_base_uri(const FAXPP_Parser *env)
{
  FAXPP_TokenizerEnv *tokenizer = env->tenv;
  while(tokenizer->prev) {
    if(tokenizer->start_of_file)
      break;
    tokenizer = tokenizer->prev;
  }

  return tokenizer->base_uri.ptr == 0 ? 0 : &tokenizer->base_uri;
}

FAXPP_Error FAXPP_set_base_uri(FAXPP_Parser *env, const FAXPP_Text *base_uri)
{
  FAXPP_Error err;

  FAXPP_TokenizerEnv *tokenizer = env->tenv;
  while(tokenizer->prev) {
    if(tokenizer->start_of_file)
      break;
    tokenizer = tokenizer->prev;
  }

  tokenizer->base_uri.ptr = env->entity_buffer.cursor;

  err = FAXPP_buffer_append(&env->entity_buffer, base_uri->ptr, base_uri->len);
  if(err) return err;

  tokenizer->base_uri.len = env->entity_buffer.cursor - tokenizer->base_uri.ptr;

  return NO_ERROR;
}

FAXPP_Error FAXPP_set_base_uri_str(FAXPP_Parser *parser, const char *base_uri)
{
  FAXPP_Text text = { (void*)base_uri, strlen(base_uri) };
  return FAXPP_set_base_uri(parser, &text);
}

void FAXPP_set_external_entity_callback(FAXPP_Parser *parser, FAXPP_ExternalEntityCallback callback, void *userData)
{
  parser->external_entity_callback = callback;
  parser->external_entity_user_data = userData;
}

static FAXPP_Error p_allocate_buffer(FAXPP_ParserEnv *env)
{
  if(!env->tenv->read_buffer) {
    env->tenv->read_buffer = malloc(READ_BUFFER_SIZE);
    if(!env->tenv->read_buffer) return OUT_OF_MEMORY;
    env->tenv->read_buffer_length = READ_BUFFER_SIZE;
  }
  return NO_ERROR;
}

static FAXPP_Error p_reset_parser(FAXPP_ParserEnv *env)
{
  FAXPP_ElementInfo *el;
  FAXPP_NamespaceInfo *ns;
  FAXPP_ContentSpec *cs;

  env->tenv->buffered_token = 0;
  env->tenv->user_provided_decode = 0;

  // Free the elementdecl stack
  while(env->current_elementdecl) {
    cs = env->current_elementdecl;
    env->current_elementdecl = cs->parent;
    free(cs);
  }

  env->current_attr = 0;
  env->current_entity = 0;
  env->current_attlist = 0;
  env->current_notation = 0;

  env->standalone = 0;
  env->event_returned = 0;
  env->xml_version = XML_VERSION_NOT_KNOWN;

  // Put the element info objects back in the pool
  while(env->element_info_stack) {
    el = env->element_info_stack;
    env->element_info_stack = el->prev;
    el->prev = env->element_info_pool;
    env->element_info_pool = el;
  }

  // Put the namespace info objects back in the pool
  while(env->namespace_stack) {
    ns = env->namespace_stack;
    env->namespace_stack = ns->prev;
    ns->prev = env->namespace_pool;
    env->namespace_pool = ns;
  }

  p_free_entity_list(env->general_entities);
  env->general_entities = 0;

  p_free_entity_list(env->parameter_entities);
  env->parameter_entities = 0;

  FAXPP_reset_buffer(&env->entity_buffer);

  env->next_event = nc_start_document_next_event;

  return NO_ERROR;
}

FAXPP_Error FAXPP_init_parse(FAXPP_Parser *env, void *buffer, unsigned int length, unsigned int done)
{
  FAXPP_Error err = p_reset_parser(env);
  if(err != 0) return err;

  env->tenv->read = 0;
  env->tenv->read_user_data = 0;

  return FAXPP_init_tokenize(env->tenv, buffer, length, done);
}

static unsigned int p_file_read_callback(void *userData, void *buffer, unsigned int length)
{
  return fread(buffer, 1, length, (FILE*)userData);
}

FAXPP_Error FAXPP_init_parse_file(FAXPP_Parser *env, FILE *file)
{
  return FAXPP_init_parse_callback(env, p_file_read_callback, (void*)file);
}

FAXPP_Error FAXPP_init_parse_callback(FAXPP_Parser *env, FAXPP_ReadCallback callback, void *userData)
{
  FAXPP_Error err = p_reset_parser(env);
  if(err != 0) return err;
  err = p_allocate_buffer(env);
  if(err != 0) return err;

  env->tenv->read = callback;
  env->tenv->read_user_data = userData;

  unsigned int len = env->tenv->read(env->tenv->read_user_data, env->tenv->read_buffer, env->tenv->read_buffer_length);

  return FAXPP_init_tokenize(env->tenv, env->tenv->read_buffer, len, /*done*/len != env->tenv->read_buffer_length);
}

FAXPP_Error FAXPP_parse_external_entity(FAXPP_Parser *env, FAXPP_EntityType type, void *buffer, unsigned int length, unsigned int done)
{
  FAXPP_Error err = FAXPP_push_entity_tokenizer(&env->tenv, type, /*internal_buffer*/0, buffer, length, done);
  if(err != 0) return err;

  // Associate it with the relevent FAXPP_EntityInfo object
  env->tenv->entity = env->event_entity;

  env->tenv->start_of_entity = 1;
  env->tenv->start_of_file = 1;

  env->next_event = nc_start_document_next_event;

  return FAXPP_sniff_encoding(env->tenv);
}

FAXPP_Error FAXPP_parse_external_entity_file(FAXPP_Parser *env, FAXPP_EntityType type, FILE *file)
{
  return FAXPP_parse_external_entity_callback(env, type, p_file_read_callback, (void*)file);
}

FAXPP_Error FAXPP_parse_external_entity_callback(FAXPP_Parser *env, FAXPP_EntityType type, FAXPP_ReadCallback callback, void *userData)
{
  FAXPP_Error err = FAXPP_push_entity_tokenizer(&env->tenv, type, /*internal_buffer*/0, 0, 0, 0);
  if(err != 0) return err;

  err = p_allocate_buffer(env);
  if(err != 0) return err;

  // Associate it with the relevent FAXPP_EntityInfo object
  env->tenv->entity = env->event_entity;

  env->tenv->read = callback;
  env->tenv->read_user_data = userData;
  env->tenv->start_of_entity = 1;
  env->tenv->start_of_file = 1;

  env->next_event = nc_start_document_next_event;

  unsigned int len = env->tenv->read(env->tenv->read_user_data, env->tenv->read_buffer, env->tenv->read_buffer_length);

  err = FAXPP_continue_tokenize(env->tenv, env->tenv->read_buffer, len, /*done*/len != env->tenv->read_buffer_length);
  if(err != 0) return err;

  return FAXPP_sniff_encoding(env->tenv);
}

FAXPP_Error FAXPP_next_event(FAXPP_Parser *env)
{
  FAXPP_Error err;
  do {
    err = env->next_event(env);
    if(err != NO_ERROR) return err;
  } while(env->event.type == NO_EVENT);

  env->event_returned = 1;
  return err;
}

const FAXPP_Event *FAXPP_get_current_event(const FAXPP_Parser *parser)
{
  return &parser->event;
}

#define p_find_ns_info(env, prefix, uri) (((env)->namespace_stack) ? p_find_ns_info_impl((env), (prefix), (uri)) : (((prefix)->len == 0) ? NO_ERROR : NO_URI_FOR_PREFIX))

static FAXPP_Error p_find_ns_info_impl(const FAXPP_ParserEnv *env, const FAXPP_Text *prefix, FAXPP_Text *uri);

FAXPP_Error FAXPP_lookup_namespace_uri(const FAXPP_Parser *parser, const FAXPP_Text *prefix, FAXPP_Text *uri)
{
  uri->ptr = 0;
  uri->len = 0;
  return p_find_ns_info_impl(parser, prefix, uri);
}

unsigned int FAXPP_get_nesting_level(const FAXPP_Parser *parser)
{
  unsigned int level = 0;
  FAXPP_TokenizerEnv *env = parser->tenv;
  while(env) {
    level += env->nesting_level;
    env = env->prev;
  }
  return level;
}

unsigned int FAXPP_get_error_line(const FAXPP_Parser *parser)
{
  return parser->err_line;
}

unsigned int FAXPP_get_error_column(const FAXPP_Parser *parser)
{
  return parser->err_column;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void p_change_buffer(FAXPP_Buffer *buffer, void *newBuffer, void **text)
{
  if(*text >= buffer->buffer && *text <= (buffer->buffer + buffer->length)) {
    *text += newBuffer - buffer->buffer;
  }
}

#define p_text_change_buffer(buffer, newBuffer, text) p_change_buffer((buffer), (newBuffer), &(text)->ptr)

static void p_change_event_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer)
{
  unsigned int i;
  FAXPP_AttrValue *atval;

  FAXPP_ParserEnv *env = (FAXPP_ParserEnv*)userData;

  p_text_change_buffer(buffer, newBuffer, &env->event.prefix);
  p_text_change_buffer(buffer, newBuffer, &env->event.uri);
  p_text_change_buffer(buffer, newBuffer, &env->event.name);
  p_text_change_buffer(buffer, newBuffer, &env->event.value);
  p_text_change_buffer(buffer, newBuffer, &env->event.version);
  p_text_change_buffer(buffer, newBuffer, &env->event.encoding);
  p_text_change_buffer(buffer, newBuffer, &env->event.standalone);
  p_text_change_buffer(buffer, newBuffer, &env->event.system_id);
  p_text_change_buffer(buffer, newBuffer, &env->event.public_id);

  for(i = 0; i < env->event.attr_count; ++i) {
    p_text_change_buffer(buffer, newBuffer, &env->event.attrs[i].prefix);
    p_text_change_buffer(buffer, newBuffer, &env->event.attrs[i].uri);
    p_text_change_buffer(buffer, newBuffer, &env->event.attrs[i].name);

    atval = &env->event.attrs[i].value;
    while(atval) {
      p_text_change_buffer(buffer, newBuffer, &atval->value);
      atval = atval->next;
    }
  }
}

static void p_change_entity_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer)
{
  FAXPP_EntityInfo *ent;
  FAXPP_EntityValue *entv;
  FAXPP_TokenizerEnv *tokenizer;

  FAXPP_ParserEnv *env = (FAXPP_ParserEnv*)userData;

  // Change the general entities
  ent = env->general_entities;
  while(ent) {
    p_text_change_buffer(buffer, newBuffer, &ent->name);
    p_text_change_buffer(buffer, newBuffer, &ent->base_uri);

    entv = ent->value;
    while(entv) {
      p_text_change_buffer(buffer, newBuffer, &entv->value);
      entv = entv->prev;
    }

    ent = ent->next;
  }

  // Change the parameter entities
  ent = env->parameter_entities;
  while(ent) {
    p_text_change_buffer(buffer, newBuffer, &ent->name);
    p_text_change_buffer(buffer, newBuffer, &ent->base_uri);

    entv = ent->value;
    while(entv) {
      p_text_change_buffer(buffer, newBuffer, &entv->value);
      entv = entv->prev;
    }

    ent = ent->next;
  }

  tokenizer = env->tenv;
  while(tokenizer) {
    p_text_change_buffer(buffer, newBuffer, &tokenizer->base_uri);

    // The tokenizer buffer can also point into the entity_buffer, so change that too
    p_change_buffer(buffer, newBuffer, &tokenizer->buffer);
    p_change_buffer(buffer, newBuffer, &tokenizer->buffer_end);
    p_change_buffer(buffer, newBuffer, &tokenizer->position);

    p_change_buffer(buffer, newBuffer, &tokenizer->result_token.value.ptr);
    p_change_buffer(buffer, newBuffer, &tokenizer->token.value.ptr);
    p_change_buffer(buffer, newBuffer, &tokenizer->token_position1);
    p_change_buffer(buffer, newBuffer, &tokenizer->token_position2);

    tokenizer = tokenizer->prev;
  }
}

#define p_move_text_to_buffer(env, text, buf) \
{ \
  if((text)->ptr >= (env)->tenv->buffer && (text)->ptr < (env)->tenv->buffer_end && \
     ((text)->ptr < (buf)->buffer || (text)->ptr >= ((buf)->buffer + (buf)->length))) { \
    void *newPtr = (buf)->cursor; \
    FAXPP_Error err = FAXPP_buffer_append((buf), (text)->ptr, (text)->len); \
    if((env)->tenv->null_terminate && err == 0) \
      err = FAXPP_buffer_append_ch((buf), (env)->tenv->transcoder.encode, 0); \
    if(err != 0) return err; \
    (text)->ptr = newPtr; \
  } \
}

FAXPP_Error FAXPP_release_buffer(FAXPP_Parser *env, void **buffer_position)
{
  unsigned int i;
  FAXPP_AttrValue *atval;
  FAXPP_Error err;

  err = FAXPP_tokenizer_release_buffer(env->tenv, buffer_position);
  if(err != 0) return err;

  // Copy any strings in the event which point to the old buffer
  // into the event_buffer
  p_move_text_to_buffer(env, &env->event.prefix, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.uri, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.name, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.value, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.version, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.encoding, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.standalone, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.system_id, &env->event_buffer);
  p_move_text_to_buffer(env, &env->event.public_id, &env->event_buffer);

  for(i = 0; i < env->event.attr_count; ++i) {
    p_move_text_to_buffer(env, &env->event.attrs[i].prefix, &env->event_buffer);
    p_move_text_to_buffer(env, &env->event.attrs[i].uri, &env->event_buffer);
    p_move_text_to_buffer(env, &env->event.attrs[i].name, &env->event_buffer);

    atval = &env->event.attrs[i].value;
    while(atval) {
      p_move_text_to_buffer(env, &atval->name, &env->event_buffer);
      p_move_text_to_buffer(env, &atval->value, &env->event_buffer);
      atval = atval->next;
    }
  }

  // Copy any strings in the element and namespace stack which point to the old buffer
  // into the appropriate element info buffer
  FAXPP_NamespaceInfo *ns;
  FAXPP_ElementInfo *el = env->element_info_stack;
  while(el) {
    p_move_text_to_buffer(env, &el->prefix, &el->buffer);
    p_move_text_to_buffer(env, &el->uri, &el->buffer);
    p_move_text_to_buffer(env, &el->name, &el->buffer);

    ns = el->ns;
    while(ns != el->prev_ns) {
      p_move_text_to_buffer(env, &ns->prefix, &el->buffer);
      p_move_text_to_buffer(env, &ns->uri, &el->buffer);

      ns = ns->prev;
    }

    el = el->prev;
  }

  return NO_ERROR;
}

FAXPP_Error FAXPP_continue_parse(FAXPP_Parser *env, void *buffer,
                                 unsigned int length, unsigned int done)
{
  return FAXPP_continue_tokenize(env->tenv, buffer, length, done);
}

#define p_check_err(err, env) \
{ \
  if((err) != NO_ERROR) { \
    if((err) == PREMATURE_END_OF_BUFFER) { \
      (env)->event.type = NO_EVENT; \
      (err) = p_read_more((env)); \
      if((err) == NO_ERROR) return NO_ERROR; \
    } \
    goto error; \
  } \
}

/* #define BUF_SIZE 50 */

/* static void p_print_token(FAXPP_ParserEnv *env) */
/* { */
/*   char buf[BUF_SIZE + 1]; */
/*   if(env->tenv->result_token.value.ptr != 0) { */
/*     if(env->tenv->result_token.value.len > BUF_SIZE) { */
/*       strncpy(buf, env->tenv->result_token.value.ptr, BUF_SIZE - 3); */
/*       buf[BUF_SIZE - 3] = '.'; */
/*       buf[BUF_SIZE - 2] = '.'; */
/*       buf[BUF_SIZE - 1] = '.'; */
/*       buf[BUF_SIZE] = 0; */
/*     } */
/*     else { */
/*       strncpy(buf, env->tenv->result_token.value.ptr, env->tenv->result_token.value.len); */
/*       buf[env->tenv->result_token.value.len] = 0; */
/*     } */
/*     printf("%03d:%03d Token ID: %s, Token: \"%s\"\n", env->tenv->result_token.line, */
/*            env->tenv->result_token.column, FAXPP_token_to_string(env->tenv->result_token.type), buf); */
/*   } */
/*   else { */
/*     printf("%03d:%03d Token ID: %s\n", env->tenv->result_token.line, env->tenv->result_token.column, */
/*            FAXPP_token_to_string(env->tenv->result_token.type)); */
/*   } */
/* } */

#define p_next_token(err, env) \
{ \
  if((env)->tenv->buffered_token) { \
    (env)->tenv->buffered_token = 0; \
  } \
  else { \
    (env)->tenv->result_token.type = NO_TOKEN; \
    while((env)->tenv->result_token.type == NO_TOKEN) { \
      (err) = (env)->tenv->state((env)->tenv); \
      p_check_err((err), (env)); \
    } \
/*     p_print_token(env); */ \
  } \
}

#define p_set_text_from_text(text, o) \
{ \
  (text)->ptr = (o)->ptr; \
  (text)->len = (o)->len; \
}

#define p_copy_text_from_event(text, o, env, buf) \
{ \
  if(((o)->ptr >= (env)->event_buffer.buffer && \
      (o)->ptr < ((env)->event_buffer.buffer + (env)->event_buffer.length)) || \
     (env)->tenv->null_terminate) { \
    (text)->ptr = (buf)->cursor; \
    (text)->len = (o)->len; \
    FAXPP_Error err = FAXPP_buffer_append((buf), (o)->ptr, (o)->len); \
    if((env)->tenv->null_terminate && err == 0) \
      err = FAXPP_buffer_append_ch((buf), (env)->tenv->transcoder.encode, 0); \
    if(err != 0) return err; \
  } else { \
    p_set_text_from_text((text), (o)); \
  } \
}

FAXPP_Error p_normalize_attr_value(FAXPP_Text *text, FAXPP_Buffer *buffer, const FAXPP_AttrValue *value, const FAXPP_ParserEnv *env)
{
  FAXPP_Error err;

  text->len = 0;

  while(value) {
    err = FAXPP_buffer_append_text(buffer, &value->value);
    if(err != NO_ERROR) return err;

    text->len += value->value.len;
    value = value->next;
  }

  // Work out the pointer from the calculated length - since a buffer
  // reallocation won't necessarily update text->ptr
  text->ptr = buffer->cursor - text->len;

  if(env->tenv->null_terminate)
    return FAXPP_buffer_append_ch(buffer, env->tenv->transcoder.encode, 0);

  return NO_ERROR;
}

#define p_force_copy_text_from_token(text, env, buf) \
{ \
  (text)->len = (env)->tenv->result_token.value.len; \
  (text)->ptr = (buf)->cursor; \
  FAXPP_Error err = FAXPP_buffer_append((buf), (env)->tenv->result_token.value.ptr, (env)->tenv->result_token.value.len); \
  if((env)->tenv->null_terminate && err == 0) \
    err = FAXPP_buffer_append_ch((buf), (env)->tenv->transcoder.encode, 0); \
  if(err != 0) return err; \
}

#define p_copy_text_from_token_with_buffer(text, env, buf, useTokenBuffer) \
{ \
  (text)->len = (env)->tenv->result_token.value.len; \
  if(((useTokenBuffer) || (env)->tenv->result_token.value.ptr != (env)->tenv->token_buffer.buffer) && !(env)->tenv->null_terminate) { \
    (text)->ptr = (env)->tenv->result_token.value.ptr; \
  } else { \
    (text)->ptr = (buf)->cursor; \
    FAXPP_Error err = FAXPP_buffer_append((buf), (env)->tenv->result_token.value.ptr, (env)->tenv->result_token.value.len); \
    if((env)->tenv->null_terminate && err == 0) \
      err = FAXPP_buffer_append_ch((buf), (env)->tenv->transcoder.encode, 0); \
    if(err != 0) return err; \
  } \
}

#define p_copy_text_from_token(text, env, useTokenBuffer) p_copy_text_from_token_with_buffer((text), (env), &(env)->event_buffer, (useTokenBuffer))

#define p_set_location(o, source) \
{ \
  if((o)->line == 0) { \
    (o)->line = (source)->line; \
    (o)->column = (source)->column; \
  } \
}

#define p_set_location_from_token(o, env) p_set_location(o, &(env)->tenv->result_token)
#define p_set_event_location_from_token(env) p_set_location_from_token(&(env)->event, (env))

#define p_copy_text_from_str(text, buffer, env, str) \
{ \
  (text)->ptr = (buffer)->cursor; \
  FAXPP_Error err = 0; \
  const char *chars = (str); \
  for(; err == 0 && *chars; ++chars) { \
    err = FAXPP_buffer_append_ch((buffer), (env)->tenv->transcoder.encode, *chars); \
  } \
  (text)->len = (buffer)->cursor - (text)->ptr; \
  if((env)->tenv->null_terminate && err == 0) \
    err = FAXPP_buffer_append_ch((buffer), (env)->tenv->transcoder.encode, 0); \
  if(err != 0) return err; \
}

static FAXPP_Error p_next_attr(FAXPP_ParserEnv *env)
{
  if(env->event.attr_count == env->max_attr_count) {
    env->max_attr_count = env->max_attr_count << 1;
    env->attrs = (FAXPP_Attribute*)realloc(env->attrs, sizeof(FAXPP_Attribute) * env->max_attr_count);
    if(!env->attrs) return OUT_OF_MEMORY;
  }
  env->event.attrs = env->attrs;
  env->current_attr = env->event.attrs + env->event.attr_count;
  env->event.attr_count += 1;

  memset(env->current_attr, 0, sizeof(FAXPP_Attribute));

  return NO_ERROR;
}

static FAXPP_AttrValue *p_next_attr_value(FAXPP_ParserEnv *env)
{
  FAXPP_AttrValue *atval;

  if(env->av_ptr) {
    atval = env->av_ptr;
    env->av_ptr = atval->dealloc_next;

    memset(atval, 0, sizeof(FAXPP_AttrValue));
    atval->dealloc_next = env->av_ptr;
  } else {
    atval = (FAXPP_AttrValue*)malloc(sizeof(FAXPP_AttrValue));
    if(!atval) return 0;

    memset(atval, 0, sizeof(FAXPP_AttrValue));
    atval->dealloc_next = env->av_dealloc;
    env->av_dealloc = atval;
  }

  return atval;
}

static FAXPP_Error p_set_attr_value(FAXPP_Attribute *attr, FAXPP_ParserEnv *env, FAXPP_EventType type)
{
  FAXPP_AttrValue *newatval;

  if(attr->value.type == NO_EVENT) {
    newatval = &attr->value;
  }
  else {
    newatval = p_next_attr_value(env);
    if(!newatval) return OUT_OF_MEMORY;

    /* Add newatval to the end of the linked list */
    FAXPP_AttrValue *atval = &attr->value;
    while(atval->next) atval = atval->next;
    atval->next = newatval;
  }

  p_copy_text_from_token(&newatval->value, env, /*useTokenBuffer*/0);
  newatval->type = type;
  newatval->line = env->tenv->result_token.line;
  newatval->column = env->tenv->result_token.column;

  return NO_ERROR;
}

#define p_set_text_to_char(text, buffer, env, ch) \
{ \
  (text)->ptr = (buffer)->cursor; \
  FAXPP_Error err = FAXPP_buffer_append_ch((buffer), (env)->tenv->transcoder.encode, (ch)); \
  (text)->len = (buffer)->cursor - (text)->ptr; \
  if((env)->tenv->null_terminate && err == 0) \
    err = FAXPP_buffer_append_ch((buffer), (env)->tenv->transcoder.encode, 0); \
  if(err != 0) return err; \
}

static FAXPP_Error p_set_attr_value_name(FAXPP_Attribute *attr, FAXPP_ParserEnv *env, FAXPP_EventType type, Char32 ch)
{
  FAXPP_AttrValue *newatval;

  if(attr->value.type == NO_EVENT) {
    newatval = &attr->value;
  }
  else {
    newatval = p_next_attr_value(env);
    if(!newatval) return OUT_OF_MEMORY;

    /* Add newatval to the end of the linked list */
    FAXPP_AttrValue *atval = &attr->value;
    while(atval->next) atval = atval->next;
    atval->next = newatval;
  }

  p_copy_text_from_token(&newatval->name, env, /*useTokenBuffer*/0);
  newatval->type = type;
  newatval->line = env->tenv->result_token.line;
  newatval->column = env->tenv->result_token.column;

  if(ch != 0) {
    p_set_text_to_char(&newatval->value, &env->event_buffer, env, ch);
  }

  return NO_ERROR;
}

static FAXPP_Error p_set_attr_value_name_from_entity(FAXPP_Attribute *attr, FAXPP_ParserEnv *env, FAXPP_EventType type, FAXPP_EntityInfo *ent)
{
  FAXPP_AttrValue *newatval;

  if(attr->value.type == NO_EVENT) {
    newatval = &attr->value;
  }
  else {
    newatval = p_next_attr_value(env);
    if(!newatval) return OUT_OF_MEMORY;

    /* Add newatval to the end of the linked list */
    FAXPP_AttrValue *atval = &attr->value;
    while(atval->next) atval = atval->next;
    atval->next = newatval;
  }

  p_set_text_from_text(&newatval->name, &ent->name);
  newatval->type = type;
  newatval->line = ent->line;
  newatval->column = ent->column;

  return NO_ERROR;
}

static void p_reset_event(FAXPP_ParserEnv *env)
{
  // Reset the attribute value store
  env->av_ptr = env->av_dealloc;

  // Reset the event buffer cursor
  FAXPP_reset_buffer(&env->event_buffer);

  // This event has not been returned yet
  env->event_returned = 0;

  // Clear the event
  env->event.type = NO_EVENT;

  env->event.prefix.ptr = 0;
  env->event.prefix.len = 0;
  env->event.uri.ptr = 0;
  env->event.uri.len = 0;
  env->event.name.ptr = 0;
  env->event.name.len = 0;
  env->event.value.ptr = 0;
  env->event.value.len = 0;

  env->event.attr_count = 0;

  env->event.version.ptr = 0;
  env->event.version.len = 0;
  env->event.encoding.ptr = 0;
  env->event.encoding.len = 0;
  env->event.standalone.ptr = 0;
  env->event.standalone.len = 0;

  env->event.system_id.ptr = 0;
  env->event.system_id.len = 0;
  env->event.public_id.ptr = 0;
  env->event.public_id.len = 0;

  env->event.line = 0;

  env->event_entity = 0;
}

static FAXPP_Error p_read_more(FAXPP_ParserEnv *env)
{
  unsigned int len = 0;
  unsigned int readlen;
  FAXPP_Error err;

  err = FAXPP_release_buffer(env, 0);
  if(err != 0) return err;

  if(env->tenv->read && !env->tenv->buffer_done) {
    if(env->tenv->position < env->tenv->buffer_end) {
      // We're half way through a charcter, so we need to copy
      // the partial char to the begining of the buffer to keep
      // it for the next parse
      len = env->tenv->buffer_end - env->tenv->position;
      memmove(env->tenv->read_buffer, env->tenv->position, len);
    }

    readlen = env->tenv->read(env->tenv->read_user_data, env->tenv->read_buffer + len, env->tenv->read_buffer_length - len);
    len += readlen;
    if(len != 0 || !env->tenv->buffer_done)
      return FAXPP_continue_parse(env, env->tenv->read_buffer, len, /*done*/len != env->tenv->read_buffer_length);
  }

  // See if we can pop the tokenizer stack
  if(!env->tenv->prev)
    return PREMATURE_END_OF_BUFFER;

  // Output the ENTITY_REFERENCE_END_EVENT
  if(env->tenv->start_of_entity) {
    if(env->tenv->attr_entity) {
      // TBD default attr values - jpcs
      if(!env->tenv->prev->internal_subset && !env->tenv->prev->external_subset &&
         !env->tenv->prev->internal_dtd_entity && !env->tenv->prev->external_dtd_entity) {
        err = p_set_attr_value_name_from_entity(env->current_attr, env, ENTITY_REFERENCE_END_EVENT, env->tenv->entity);
        if(err) return err;
      }
    }
    else if(env->tenv->element_entity) {
      p_reset_event(env);
      p_set_text_from_text(&env->event.name, &env->tenv->entity->name);
      p_set_location(&env->event, env->tenv->entity);
      env->event.type = ENTITY_REFERENCE_END_EVENT;
    }
    else if(env->tenv->external_parsed_entity) {
      p_reset_event(env);
      env->event.type = END_EXTERNAL_ENTITY_EVENT;
    }
    else if(env->tenv->external_subset) {
      if(env->tenv->prev->internal_subset) {
        env->next_event = nc_dtd_next_event;
      }
      else {
        // Go back to parsing the document content
        env->next_event = env->main_next_event;
      }
    }
  }

  // Also checks that the tokenizer is in the correct state
  err = FAXPP_pop_tokenizer(&env->tenv);
  if(err) return err;

  return NO_ERROR;
}

#define set_err_info(env, o) \
{ \
  (env)->err_line = (o)->line; \
  (env)->err_column = (o)->column; \
}

#define set_err_info_from_tokenizer(env) set_err_info(env, env->tenv)
#define set_err_info_from_event(env) set_err_info(env, &env->event)
#define set_err_info_from_attr(env, attr) set_err_info(env, attr)

// "str" should be upper case
static int p_case_insensitive_equals(const char *str, FAXPP_EncodeFunction encode, const FAXPP_Text *text)
{
  // No encoding represents a character with as many as 10 bytes
  uint8_t encode_buffer[10];
  unsigned int encode_len;

  void *text_ptr = text->ptr;
  void *text_end = text_ptr + text->len;

  while(*str != 0) {
    if(text_ptr >= text_end) return 0;

    encode_len = encode(encode_buffer, encode_buffer + sizeof(encode_buffer), *str);
    if((text_end - text_ptr) < encode_len || memcmp(encode_buffer, text_ptr, encode_len) != 0) {
      if(*str >= 'A' && *str <= 'Z') {
        // Try the lower case letter as well
        encode_len = encode(encode_buffer, encode_buffer + sizeof(encode_buffer), (*str) - 'A' + 'a');
        if((text_end - text_ptr) < encode_len || memcmp(encode_buffer, text_ptr, encode_len) != 0)
          return 0;
      }
      else return 0;
    }

    text_ptr += encode_len;
    ++str;
  }

  return text_ptr == text_end;
}

static int p_equals(const char *str, FAXPP_EncodeFunction encode, const FAXPP_Text *text)
{
  // No encoding represents a character with as many as 10 bytes
  uint8_t encode_buffer[10];
  unsigned int encode_len;

  void *text_ptr = text->ptr;
  void *text_end = text_ptr + text->len;

  while(*str != 0) {
    if(text_ptr >= text_end) return 0;

    encode_len = encode(encode_buffer, encode_buffer + sizeof(encode_buffer), *str);
    if((text_end - text_ptr) < encode_len || memcmp(encode_buffer, text_ptr, encode_len) != 0) {
      return 0;
    }

    text_ptr += encode_len;
    ++str;
  }

  return text_ptr == text_end;
}

static FAXPP_Error nc_start_document_next_event(FAXPP_ParserEnv *env)
{
  FAXPP_NextEvent next;
  FAXPP_Error err = 0;

  env->event.type = NO_EVENT;

  while(1) {
    p_next_token(err, env);

    switch(env->tenv->result_token.type) {
    case XML_DECL_VERSION_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.version, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);

      if(p_equals("1.1", env->tenv->transcoder.encode, &env->event.version)) {
        if(env->xml_version == XML_VERSION_NOT_KNOWN) {
          env->xml_version = XML_VERSION_1_1;
        }
        else if(env->xml_version == XML_VERSION_1_0) {
          err = UNKNOWN_XML_VERSION;
          goto error;
        }
      }
      else {
        if(env->xml_version == XML_VERSION_NOT_KNOWN) {
          env->xml_version = XML_VERSION_1_0;
        }
      }
      break;
    case XML_DECL_ENCODING_TOKEN:
      p_copy_text_from_token(&env->event.encoding, env, /*useTokenBuffer*/0);
      break;
    case XML_DECL_STANDALONE_TOKEN:
      p_copy_text_from_token(&env->event.standalone, env, /*useTokenBuffer*/0);

      if(p_equals("yes", env->tenv->transcoder.encode, &env->event.standalone)) {
        env->standalone = 1;
      }
      break;
    case XML_DECL_END_TOKEN:
      env->next_event = nc_unsupported_encoding_next_event;

      if(env->tenv->external_subset || env->tenv->external_in_markup_entity) {
        // TBD event for start of external subset - jpcs
        next = nc_dtd_next_event;
      }
      else if(env->tenv->external_parsed_entity) {
        env->event.type = START_EXTERNAL_ENTITY_EVENT;
        next = env->main_next_event;
      }
      else {
        env->event.type = START_DOCUMENT_EVENT;
        next = env->main_next_event;
      }

      // Check the encoding string against our internally supported encodings
      if(env->tenv->user_provided_decode || env->event.encoding.ptr == 0) {
          env->next_event = next;
      }
      else if(p_case_insensitive_equals("UTF-8", env->tenv->transcoder.encode, &env->event.encoding)) {
        env->next_event = next;
        if(env->tenv->decode != FAXPP_utf8_decode)
          return BAD_ENCODING;
      }
      else if(p_case_insensitive_equals("UTF-16", env->tenv->transcoder.encode, &env->event.encoding)) {
        env->next_event = next;
        if(env->tenv->decode != FAXPP_utf16_le_decode &&
           env->tenv->decode != FAXPP_utf16_be_decode &&
           env->tenv->decode != FAXPP_utf16_native_decode)
          return BAD_ENCODING;
      }
      else if(p_case_insensitive_equals("UTF-16LE", env->tenv->transcoder.encode, &env->event.encoding)) {
        env->next_event = next;
        if(env->tenv->decode != FAXPP_utf16_le_decode
#ifndef WORDS_BIGENDIAN
           && env->tenv->decode != FAXPP_utf16_native_decode
#endif
           )
          return BAD_ENCODING;
      }
      else if(p_case_insensitive_equals("UTF-16BE", env->tenv->transcoder.encode, &env->event.encoding)) {
        env->next_event = next;
        if(env->tenv->decode != FAXPP_utf16_be_decode
#ifdef WORDS_BIGENDIAN
           && env->tenv->decode != FAXPP_utf16_native_decode
#endif
           )
          return BAD_ENCODING;
      }
      else if(p_case_insensitive_equals("ISO-10646-UCS-4", env->tenv->transcoder.encode, &env->event.encoding)) {
        env->next_event = next;
        if(env->tenv->decode != FAXPP_ucs4_le_decode &&
           env->tenv->decode != FAXPP_ucs4_be_decode &&
           env->tenv->decode != FAXPP_ucs4_native_decode)
          return BAD_ENCODING;
      }
      else if(p_case_insensitive_equals("ISO-8859-1", env->tenv->transcoder.encode, &env->event.encoding)) {
        FAXPP_set_decode(env, FAXPP_iso_8859_1_decode);
      }
    
      return NO_ERROR;
    default:
      env->tenv->buffered_token = 1;
      p_reset_event(env);

      if(env->tenv->external_subset || env->tenv->external_in_markup_entity) {
        // TBD event for start of external subset - jpcs
        env->next_event = nc_dtd_next_event;
      }
      else if(env->tenv->external_parsed_entity) {
        env->event.type = START_EXTERNAL_ENTITY_EVENT;
        env->next_event = env->main_next_event;
      }
      else {
        env->event.type = START_DOCUMENT_EVENT;
        env->next_event = env->main_next_event;
      }

      if(env->xml_version == XML_VERSION_NOT_KNOWN) {
        env->xml_version = XML_VERSION_1_0;
      }

      return NO_ERROR;
    }
  }

  // Never happens
  return NO_ERROR;

 error:
  set_err_info_from_tokenizer(env);
  return err;
}

static FAXPP_Error nc_unsupported_encoding_next_event(FAXPP_ParserEnv *env)
{
  return UNSUPPORTED_ENCODING;
}

static FAXPP_Error p_create_entity_info(FAXPP_ParserEnv *env, FAXPP_EntityInfo **list)
{
  const FAXPP_Text *text;

  FAXPP_EntityInfo *ent = (FAXPP_EntityInfo*)malloc(sizeof(FAXPP_EntityInfo));
  if(!ent) return OUT_OF_MEMORY;

  memset(ent, 0, sizeof(FAXPP_EntityInfo));

  env->current_entity = ent;

  while(*list) {
    list = &(*list)->next;
  }
  *list = ent;

  text  = FAXPP_get_base_uri(env);
  if(text != 0) {
    p_set_text_from_text(&ent->base_uri, text);
  }
  p_force_copy_text_from_token(&ent->name, env, &env->entity_buffer);
  p_set_location_from_token(ent, env);

  ent->from_internal_subset = env->tenv->internal_subset;

  return NO_ERROR;
}

static FAXPP_EntityValue *p_add_entity_value(FAXPP_ParserEnv *env)
{
  FAXPP_EntityInfo *ent = env->current_entity;

  FAXPP_EntityValue *entv = (FAXPP_EntityValue*)malloc(sizeof(FAXPP_EntityValue));
  if(!entv) return 0;

  memset(entv, 0, sizeof(FAXPP_EntityValue));

  entv->prev = ent->value;
  ent->value = entv;

  entv->type = env->tenv->result_token.type;
  p_set_location_from_token(entv, env);

  return entv;
}

#define p_compare_text(a, b) (((a)->len == (b)->len) ? memcmp((a)->ptr, (b)->ptr, (a)->len) : ((a)->len - (b)->len))

static FAXPP_EntityInfo *p_find_entity_info(const FAXPP_Text *name, FAXPP_EntityInfo *list)
{
  while(list) {
    if(p_compare_text(name, &list->name) == 0)
      break;
    list = list->next;
  }
  return list;
}

static FAXPP_Error p_parse_entity_impl(FAXPP_ParserEnv *env, FAXPP_EntityInfo *ent, FAXPP_EntityParseState state, FAXPP_EntityInfo **initial_entity);

static FAXPP_Error p_parse_internal_entity(FAXPP_ParserEnv *env, FAXPP_EntityInfo *ent, FAXPP_EntityParseState state, FAXPP_EntityInfo **initial_entity)
{
  FAXPP_EntityValue *entv;
  FAXPP_Error err;

  // Add the entity values to the tokenizer stack
  entv = ent->value;
  while(entv) {
    if(entv->entity_ref) {
      err = p_parse_entity_impl(env, entv->entity_ref, state, initial_entity);
      if(err) return err;
    }
    else {
      err = FAXPP_push_entity_tokenizer(&env->tenv, state, /*internal_buffer*/1, entv->value.ptr, entv->value.len, /*done*/1);
      if(err) return err;

      env->tenv->line = entv->line;
      env->tenv->column = entv->column;

      // Set the entity on the first new tokenizer
      if(*initial_entity) {
        env->tenv->start_of_entity = 1;
        env->tenv->entity = *initial_entity;
        *initial_entity = 0;
      }
    }

    entv = entv->prev;
  }

  return NO_ERROR;
}

static FAXPP_Error p_parse_external_entity(FAXPP_ParserEnv *env, FAXPP_EntityInfo *ent, FAXPP_EntityType type)
{
  FAXPP_EntityValue *entv;
  FAXPP_Error err;

  env->event_entity = ent;

  entv = ent->value;
  while(entv) {
    switch(entv->type) {
    case SYSTEM_LITERAL_TOKEN: p_set_text_from_text(&env->event.system_id, &entv->value); break;
    case PUBID_LITERAL_TOKEN: p_set_text_from_text(&env->event.public_id, &entv->value); break;
    default: break;
    }
    entv = entv->prev;
  }

  err = DONT_PARSE_EXTERNAL_ENTITY;
  if(env->external_entity_callback) {
    err = env->external_entity_callback(env->external_entity_user_data, env, type,
                                        ent->base_uri.ptr == 0 ? 0 : &ent->base_uri,
                                        &env->event.system_id, &env->event.public_id);
  }

  return err;
}

static FAXPP_Error p_parse_entity_impl(FAXPP_ParserEnv *env, FAXPP_EntityInfo *ent, FAXPP_EntityParseState state, FAXPP_EntityInfo **initial_entity)
{
  FAXPP_Error err;

  if(ent->external) {
    switch(state) {
    case ELEMENT_CONTENT_ENTITY: state = EXTERNAL_PARSED_ENTITY; break;
    case INTERNAL_DTD_ENTITY: state = EXTERNAL_SUBSET_ENTITY; break;
    case EXTERNAL_DTD_ENTITY: state = EXTERNAL_SUBSET_ENTITY; break;
    case IN_MARKUP_ENTITY: state = EXTERNAL_IN_MARKUP_ENTITY; break;
    default: break;
    }

    err = p_parse_external_entity(env, ent, state);
    if(err) return err;

    // Set the entity on the first new tokenizer
    if(*initial_entity) {
      env->tenv->start_of_entity = 1;
      env->tenv->entity = *initial_entity;
      *initial_entity = 0;
    }
    else {
      env->tenv->start_of_entity = 0;
    }
  }
  else {
    err = p_parse_internal_entity(env, ent, state, initial_entity);
    if(err) return err;
  }

  return NO_ERROR;
}

static const char single_space[] = {' '};

static FAXPP_Error p_parse_entity(FAXPP_ParserEnv *env, FAXPP_EntityInfo *ent, FAXPP_EntityParseState state)
{
  FAXPP_Error err;
  FAXPP_EntityInfo *tmp;

  // Check for a recursive entity
  // TBD Need a better method for doing this - jpcs
  FAXPP_TokenizerEnv *tokenizer = env->tenv;
  while(tokenizer) {
    if(tokenizer->entity == ent)
      return RECURSIVE_ENTITY;

    tokenizer = tokenizer->prev;
  }

  if(state == IN_MARKUP_ENTITY || state == EXTERNAL_IN_MARKUP_ENTITY) {
    // Add a space after the entity inside DTD markup
    err = FAXPP_push_entity_tokenizer(&env->tenv, IN_MARKUP_ENTITY, /*internal_buffer*/0, (void*)single_space, 1, /*done*/1);
    if(err) return err;

    env->tenv->line = ent->line;
    env->tenv->column = ent->column;

    FAXPP_set_tokenizer_decode(env->tenv, FAXPP_utf8_decode);
  }

  if(ent->external) {
    switch(state) {
    case ELEMENT_CONTENT_ENTITY: state = EXTERNAL_PARSED_ENTITY; break;
    case INTERNAL_DTD_ENTITY: state = EXTERNAL_SUBSET_ENTITY; break;
    case EXTERNAL_DTD_ENTITY: state = EXTERNAL_SUBSET_ENTITY; break;
    case IN_MARKUP_ENTITY: state = EXTERNAL_IN_MARKUP_ENTITY; break;
    default: break;
    }

    err = p_parse_external_entity(env, ent, state);
    if(err) return err;
  }
  else {
    tmp = ent;
    err = p_parse_internal_entity(env, ent, state, &tmp);
    if(err) return err;

    if(state == IN_MARKUP_ENTITY || state == EXTERNAL_IN_MARKUP_ENTITY) {
      // Add a space before the entity inside DTD markup
      err = FAXPP_push_entity_tokenizer(&env->tenv, IN_MARKUP_ENTITY, /*internal_buffer*/0, (void*)single_space, 1, /*done*/1);
      if(err) return err;

      env->tenv->line = ent->line;
      env->tenv->column = ent->column;

      FAXPP_set_tokenizer_decode(env->tenv, FAXPP_utf8_decode);
    }
  }

  return NO_ERROR;
}

static Char32 p_dec_char_ref_value(const FAXPP_Text *text, FAXPP_ParserEnv *env)
{
  Char32 ch, result = 0;

  void *text_ptr = text->ptr;
  void *text_end = text_ptr + text->len;

  while(text_ptr < text_end) {
    text_ptr += env->tenv->transcoder.decode(text_ptr, text_end, &ch);
    result *= 10;
    if(ch <= '9' && ch >= '0')
      result += ch - '0';
  }

  return result;
}

static Char32 p_hex_char_ref_value(const FAXPP_Text *text, FAXPP_ParserEnv *env)
{
  Char32 ch, result = 0;

  void *text_ptr = text->ptr;
  void *text_end = text_ptr + text->len;

  while(text_ptr < text_end) {
    text_ptr += env->tenv->transcoder.decode(text_ptr, text_end, &ch);
    result <<= 4;
    if(ch <= '9' && ch >= '0')
      result += ch - '0';
    else if(ch <= 'F' && ch >= 'A')
      result += ch - 'A' + 10;
    else if(ch <= 'f' && ch >= 'a')
      result += ch - 'a' + 10;
  }

  return result;
}

static FAXPP_Error nc_dtd_next_event(FAXPP_ParserEnv *env)
{
  FAXPP_EntityInfo *ent;
  FAXPP_EntityValue *entv;
  FAXPP_ContentSpec *cs;
  FAXPP_Text bkup_system, bkup_public;
  Char32 ch;
  FAXPP_Error err = 0;

  while(1) {
    p_next_token(err, env);

    switch(env->tenv->result_token.type) {
    case ENTITYDECL_NAME_TOKEN:
      err = p_create_entity_info(env, &env->general_entities);
      if(err) goto error;
      break;
    case PARAMENTITYDECL_NAME_TOKEN:
      err = p_create_entity_info(env, &env->parameter_entities);
      if(err) goto error;
      break;
    case ENTITYDECL_END_TOKEN:
      env->current_entity = 0;
      break;

    case NDATA_NAME_TOKEN:
      env->current_entity->unparsed = 1;
      // Fall through
    case ENTITYDECL_VALUE_TOKEN:
      entv = p_add_entity_value(env);
      if(!entv) goto out_of_memory;

      p_force_copy_text_from_token(&entv->value, env, &env->entity_buffer);
      break;
    case SYSTEM_LITERAL_TOKEN:
      if(env->current_entity) {
        env->current_entity->external = 1;

        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_force_copy_text_from_token(&entv->value, env, &env->entity_buffer);
      }
      else if(env->current_notation) {
        // We're in a notation decl - which we don't store yet
      }
      else {
        p_copy_text_from_token(&env->event.system_id, env, /*useTokenBuffer*/0);
      }
      break;
    case PUBID_LITERAL_TOKEN:
      if(env->current_entity) {
        env->current_entity->external = 1;

        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_force_copy_text_from_token(&entv->value, env, &env->entity_buffer);
      }
      else if(env->current_notation) {
        // We're in a notation decl - which we don't store yet
      }
      else {
        p_copy_text_from_token(&env->event.public_id, env, /*useTokenBuffer*/0);
      }
      break;

    case AMP_ENTITY_REFERENCE_TOKEN:
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_copy_text_from_str(&entv->value, &env->entity_buffer, env, "&amp;");
      }
      break;
    case APOS_ENTITY_REFERENCE_TOKEN:
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_copy_text_from_str(&entv->value, &env->entity_buffer, env, "&apos;");
      }
      break;
    case GT_ENTITY_REFERENCE_TOKEN:
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_copy_text_from_str(&entv->value, &env->entity_buffer, env, "&gt;");
      }
      break;
    case LT_ENTITY_REFERENCE_TOKEN:
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_copy_text_from_str(&entv->value, &env->entity_buffer, env, "&lt;");
      }
      break;
    case QUOT_ENTITY_REFERENCE_TOKEN:
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_copy_text_from_str(&entv->value, &env->entity_buffer, env, "&quot;");
      }
      break;

    case DEC_CHAR_REFERENCE_TOKEN:
      /* [WFC: Legal Character] */
      ch = p_dec_char_ref_value(&env->tenv->result_token.value, env);
      if((FAXPP_char_flags(ch) & env->tenv->xml_char) == 0) {
        err = ILLEGAL_CHARACTER_REFERENCE;
        goto error;
      }

      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_set_text_to_char(&entv->value, &env->entity_buffer, env, ch);
      }
      break;
    case HEX_CHAR_REFERENCE_TOKEN:
      /* [WFC: Legal Character] */
      ch = p_hex_char_ref_value(&env->tenv->result_token.value, env);
      if((FAXPP_char_flags(ch) & env->tenv->xml_char) == 0) {
        err = ILLEGAL_CHARACTER_REFERENCE;
        goto error;
      }

      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        p_set_text_to_char(&entv->value, &env->entity_buffer, env, ch);
      }
      break;

    case ENTITY_REFERENCE_TOKEN:
      // General entity references can be forward references -
      // so we don't look them up yet
      if(env->current_entity) {
        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        entv->value.ptr = env->entity_buffer.cursor;

        err = FAXPP_buffer_append_ch(&env->entity_buffer, env->tenv->transcoder.encode, '&');
        if(err) goto error;
        err = FAXPP_buffer_append(&env->entity_buffer, env->tenv->result_token.value.ptr, env->tenv->result_token.value.len);
        if(err) goto error;
        err = FAXPP_buffer_append_ch(&env->entity_buffer, env->tenv->transcoder.encode, ';');
        if(err) goto error;

        entv->value.len = env->entity_buffer.cursor - entv->value.ptr;
      }
      else if(env->current_attlist) {
        // General entities in ATTLIST values should be looked up straight away
        ent = p_find_entity_info(&env->tenv->result_token.value, env->general_entities);
        if(ent == 0) {
          err = UNDEFINED_ENTITY;
          goto error;
        }
        if(env->standalone && !ent->from_internal_subset) {
          err = UNDEFINED_ENTITY;
          goto error;
        }
        // [WFC: Parsed Entity]
        if(ent->unparsed) {
          err = REFERENCE_TO_UNPARSED_ENTITY;
          goto error;
        }

        // [WFC: No External Entity References]
        if(ent->external) {
          err = REFERENCE_TO_EXTERNAL_ENTITY;
          goto error;
        }

/*         err = p_set_attr_value_name_from_entity(env->current_attr, env, ENTITY_REFERENCE_START_EVENT, ent); */
/*         if(err) goto error; */

        err = p_parse_entity(env, ent, ATTRIBUTE_VALUE_ENTITY);
        if(err) goto error;
        return NO_ERROR;
      }
      break;
    case PE_REFERENCE_TOKEN:
      // Parameter entity references cannot be forward references -
      // so we go ahead and look them up straight away
      ent = p_find_entity_info(&env->tenv->result_token.value, env->parameter_entities);
      // [VC: Entity Declared]
      if(ent == 0) {
        err = UNDEFINED_ENTITY;
        goto error;
      }

      if(env->current_entity) {
        if(env->tenv->internal_subset) {
          // [WFC: PEs in Internal Subset]
          err = ILLEGAL_PARAMETER_ENTITY;
          goto error;
        }

        entv = p_add_entity_value(env);
        if(!entv) goto out_of_memory;

        entv->entity_ref = ent;
      }
      else {
        p_set_text_from_text(&bkup_system, &env->event.system_id);
        p_set_text_from_text(&bkup_public, &env->event.public_id);

        err = p_parse_entity(env, ent, env->tenv->internal_subset ? INTERNAL_DTD_ENTITY : EXTERNAL_DTD_ENTITY);

        p_set_text_from_text(&env->event.system_id, &bkup_system);
        p_set_text_from_text(&env->event.public_id, &bkup_public);

        if(err) goto error;
        return NO_ERROR;
      }
      break;
    case PE_REFERENCE_IN_MARKUP_TOKEN:
      // Parameter entity references cannot be forward references -
      // so we go ahead and look them up straight away
      ent = p_find_entity_info(&env->tenv->result_token.value, env->parameter_entities);
      // [VC: Entity Declared]
      if(ent == 0) {
        err = UNDEFINED_ENTITY;
        goto error;
      }

      p_set_text_from_text(&bkup_system, &env->event.system_id);
      p_set_text_from_text(&bkup_public, &env->event.public_id);

      err = p_parse_entity(env, ent, IN_MARKUP_ENTITY);

      p_set_text_from_text(&env->event.system_id, &bkup_system);
      p_set_text_from_text(&env->event.public_id, &bkup_public);

      if(err) goto error;
      return NO_ERROR;

    case ELEMENTDECL_LPAR_TOKEN:
      cs = (FAXPP_ContentSpec*)malloc(sizeof(FAXPP_ContentSpec));
      memset(cs, 0, sizeof(FAXPP_ContentSpec));
      cs->parent = env->current_elementdecl;
      env->current_elementdecl = cs;
      break;
    case ELEMENTDECL_RPAR_TOKEN:
      cs = env->current_elementdecl;
      env->current_elementdecl = cs->parent;
      free(cs);
      break;
    case ELEMENTDECL_BAR_TOKEN:
      if(env->current_elementdecl->type == CONTENTSPEC_NONE) {
        env->current_elementdecl->type = CONTENTSPEC_CHOICE;
      }
      else if(env->current_elementdecl->type != CONTENTSPEC_CHOICE) {
        err = INVALID_ELEMENTDECL_CONTENT;
        goto error;
      }
      break;
    case ELEMENTDECL_COMMA_TOKEN:
      if(env->current_elementdecl->type == CONTENTSPEC_NONE) {
        env->current_elementdecl->type = CONTENTSPEC_SEQUENCE;
      }
      else if(env->current_elementdecl->type != CONTENTSPEC_SEQUENCE) {
        err = INVALID_ELEMENTDECL_CONTENT;
        goto error;
      }
      break;

    case ATTLISTDECL_PREFIX_TOKEN:
    case ATTLISTDECL_NAME_TOKEN:
      env->current_attlist = 1;
      break;
    case ATTLISTDECL_END_TOKEN:
      env->current_attlist = 0;
      break;
    case NOTATIONDECL_NAME_TOKEN:
      env->current_notation = 1;
      break;
    case NOTATIONDECL_END_TOKEN:
      env->current_notation = 0;
      break;
    case DOCTYPE_END_TOKEN:
      env->next_event = env->main_next_event;

      if((env->event.system_id.ptr || env->event.public_id.ptr) && env->external_entity_callback) {
        err = env->external_entity_callback(env->external_entity_user_data, env, EXTERNAL_SUBSET_ENTITY,
                                            FAXPP_get_base_uri(env),
                                            &env->event.system_id, &env->event.public_id);
        if(err == DONT_PARSE_EXTERNAL_ENTITY) err = 0;
        if(err) goto error;
      }

      env->event.type = DOCTYPE_EVENT;
      return NO_ERROR;

    case DOCTYPE_PREFIX_TOKEN:
      p_copy_text_from_token(&env->event.prefix, env, /*useTokenBuffer*/0);
      break;
    case DOCTYPE_NAME_TOKEN:
      p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/0);
      break;

    case ELEMENTDECL_PREFIX_TOKEN:
    case ELEMENTDECL_NAME_TOKEN:
    case ELEMENTDECL_EMPTY_TOKEN:
    case ELEMENTDECL_ANY_TOKEN:
    case ELEMENTDECL_PCDATA_TOKEN:
    case ELEMENTDECL_QUESTION_TOKEN:
    case ELEMENTDECL_STAR_TOKEN:
    case ELEMENTDECL_PLUS_TOKEN:
    case ELEMENTDECL_END_TOKEN:

    case ATTLISTDECL_ATTDEF_PREFIX_TOKEN:
    case ATTLISTDECL_ATTDEF_NAME_TOKEN:
    case ATTLISTDECL_ATTTYPE_ENTITY_TOKEN:
    case ATTLISTDECL_ATTTYPE_ENTITIES_TOKEN:
    case ATTLISTDECL_ATTTYPE_NMTOKEN_TOKEN:
    case ATTLISTDECL_ATTTYPE_NMTOKENS_TOKEN:
    case ATTLISTDECL_ATTTYPE_ID_TOKEN:
    case ATTLISTDECL_ATTTYPE_IDREF_TOKEN:
    case ATTLISTDECL_ATTTYPE_IDREFS_TOKEN:
    case ATTLISTDECL_ATTTYPE_CDATA_TOKEN:
    case ATTLISTDECL_NOTATION_NAME_TOKEN:
    case ATTLISTDECL_ENUMERATION_NAME_TOKEN:
    case ATTLISTDECL_DEFAULT_IMPLIED_TOKEN:
    case ATTLISTDECL_DEFAULT_REQUIRED_TOKEN:
    case ATTLISTDECL_DEFAULT_FIXED_TOKEN:
      // Ignore for now
      break;
    case COMMENT_TOKEN:
    case PI_NAME_TOKEN:
    case PI_VALUE_TOKEN:
      // Ignore for now
      break;

    default:
      // Should never happen
      break;
    }
  }

  // Never happens
  return NO_ERROR;

 out_of_memory:
  err = OUT_OF_MEMORY;
 error:
  set_err_info_from_tokenizer(env);
  return err;
}

static FAXPP_Error nc_next_event(FAXPP_ParserEnv *env)
{
  FAXPP_EntityInfo *ent;
  Char32 ch;
  FAXPP_Error err = 0;

  while(1) {
    p_next_token(err, env);

    switch(env->tenv->result_token.type) {
    case DOCTYPE_PREFIX_TOKEN:
    case DOCTYPE_NAME_TOKEN:
      p_reset_event(env);
      p_set_event_location_from_token(env);
      env->tenv->buffered_token = 1;
      env->next_event = nc_dtd_next_event;
      return NO_ERROR;
    case DOCTYPE_END_TOKEN:
    case SYSTEM_LITERAL_TOKEN:
    case PUBID_LITERAL_TOKEN:
    case NDATA_NAME_TOKEN:
    case PE_REFERENCE_TOKEN:
    case PE_REFERENCE_IN_MARKUP_TOKEN:
    case ELEMENTDECL_PREFIX_TOKEN:
    case ELEMENTDECL_NAME_TOKEN:
    case ELEMENTDECL_EMPTY_TOKEN:
    case ELEMENTDECL_ANY_TOKEN:
    case ELEMENTDECL_PCDATA_TOKEN:
    case ELEMENTDECL_LPAR_TOKEN:
    case ELEMENTDECL_RPAR_TOKEN:
    case ELEMENTDECL_QUESTION_TOKEN:
    case ELEMENTDECL_STAR_TOKEN:
    case ELEMENTDECL_PLUS_TOKEN:
    case ELEMENTDECL_BAR_TOKEN:
    case ELEMENTDECL_COMMA_TOKEN:
    case ELEMENTDECL_END_TOKEN:
    case ATTLISTDECL_PREFIX_TOKEN:
    case ATTLISTDECL_NAME_TOKEN:
    case ATTLISTDECL_ATTDEF_PREFIX_TOKEN:
    case ATTLISTDECL_ATTDEF_NAME_TOKEN:
    case ATTLISTDECL_ATTTYPE_ENTITY_TOKEN:
    case ATTLISTDECL_ATTTYPE_ENTITIES_TOKEN:
    case ATTLISTDECL_ATTTYPE_NMTOKEN_TOKEN:
    case ATTLISTDECL_ATTTYPE_NMTOKENS_TOKEN:
    case ATTLISTDECL_ATTTYPE_ID_TOKEN:
    case ATTLISTDECL_ATTTYPE_IDREF_TOKEN:
    case ATTLISTDECL_ATTTYPE_IDREFS_TOKEN:
    case ATTLISTDECL_ATTTYPE_CDATA_TOKEN:
    case ATTLISTDECL_NOTATION_NAME_TOKEN:
    case ATTLISTDECL_ENUMERATION_NAME_TOKEN:
    case ATTLISTDECL_DEFAULT_IMPLIED_TOKEN:
    case ATTLISTDECL_DEFAULT_REQUIRED_TOKEN:
    case ATTLISTDECL_DEFAULT_FIXED_TOKEN:
    case ATTLISTDECL_END_TOKEN:
    case NOTATIONDECL_NAME_TOKEN:
    case NOTATIONDECL_END_TOKEN:
    case ENTITYDECL_NAME_TOKEN:
    case ENTITYDECL_VALUE_TOKEN:
    case ENTITYDECL_END_TOKEN:
    case PARAMENTITYDECL_NAME_TOKEN:
      break;
    case XML_DECL_VERSION_TOKEN:
    case XML_DECL_ENCODING_TOKEN:
    case XML_DECL_STANDALONE_TOKEN:
    case XML_DECL_END_TOKEN:
      break;

    case START_ELEMENT_PREFIX_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.prefix, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);
      break;
    case START_ELEMENT_NAME_TOKEN:
      if(env->event_returned == 1) {
        p_reset_event(env);
      }
      p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);
      break;
    case XMLNS_PREFIX_TOKEN:
      err = p_next_attr(env);
      if(err != 0) goto error;

      p_copy_text_from_token(&env->current_attr->prefix, env, /*useTokenBuffer*/0);
      p_set_location_from_token(env->current_attr, env);
      env->current_attr->xmlns_attr = 1;
      break;
    case XMLNS_NAME_TOKEN:
      err = p_next_attr(env);
      if(err != 0) goto error;

      p_copy_text_from_token(&env->current_attr->name, env, /*useTokenBuffer*/0);
      p_set_location_from_token(env->current_attr, env);
      env->current_attr->xmlns_attr = 1;
      break;
    case XML_PREFIX_TOKEN:
      err = p_next_attr(env);
      if(err != 0) goto error;

      p_copy_text_from_token(&env->current_attr->prefix, env, /*useTokenBuffer*/0);
      p_set_location_from_token(env->current_attr, env);
      env->current_attr->xml_attr = 1;
      break;
    case ATTRIBUTE_PREFIX_TOKEN:
      err = p_next_attr(env);
      if(err != 0) goto error;

      p_copy_text_from_token(&env->current_attr->prefix, env, /*useTokenBuffer*/0);
      p_set_location_from_token(env->current_attr, env);
      break;
    case ATTRIBUTE_NAME_TOKEN:
      if(!env->current_attr || env->current_attr->name.ptr != 0) {
        err = p_next_attr(env);
        if(err != 0) goto error;
      }
      p_copy_text_from_token(&env->current_attr->name, env, /*useTokenBuffer*/0);
      p_set_location_from_token(env->current_attr, env);
      break;
    case ATTRIBUTE_VALUE_TOKEN:
      err = p_set_attr_value(env->current_attr, env, CHARACTERS_EVENT);
      if(err) goto error;
      break;
    case START_ELEMENT_END_TOKEN:
      env->event.type = START_ELEMENT_EVENT;
      env->current_attr = 0;
      return NO_ERROR;
    case SELF_CLOSING_ELEMENT_TOKEN:
      env->event.type = SELF_CLOSING_ELEMENT_EVENT;
      env->current_attr = 0;
      return NO_ERROR;
    case END_ELEMENT_PREFIX_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.prefix, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);
      break;
    case END_ELEMENT_NAME_TOKEN:
      if(env->event_returned == 1) {
        p_reset_event(env);
      }
      p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);
      env->event.type = END_ELEMENT_EVENT;
      return NO_ERROR;
    case CHARACTERS_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.value, env, /*useTokenBuffer*/1);
      p_set_event_location_from_token(env);
      env->event.type = CHARACTERS_EVENT;
      return NO_ERROR;
    case CDATA_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.value, env, /*useTokenBuffer*/1);
      p_set_event_location_from_token(env);
      env->event.type = CDATA_EVENT;
      return NO_ERROR;
    case IGNORABLE_WHITESPACE_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.value, env, /*useTokenBuffer*/1);
      p_set_event_location_from_token(env);
      env->event.type = IGNORABLE_WHITESPACE_EVENT;
      return NO_ERROR;
    case COMMENT_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.value, env, /*useTokenBuffer*/1);
      p_set_event_location_from_token(env);
      env->event.type = COMMENT_EVENT;
      return NO_ERROR;
    case PI_NAME_TOKEN:
      p_reset_event(env);
      p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/0);
      p_set_event_location_from_token(env);
      break;
    case PI_VALUE_TOKEN:
      p_copy_text_from_token(&env->event.value, env, /*useTokenBuffer*/0);
      env->event.type = PI_EVENT;
      return NO_ERROR;
    case AMP_ENTITY_REFERENCE_TOKEN:
      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, ENTITY_REFERENCE_EVENT, '&');
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, '&');
        p_set_event_location_from_token(env);
        env->event.type = ENTITY_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case APOS_ENTITY_REFERENCE_TOKEN:
      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, ENTITY_REFERENCE_EVENT, '\'');
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, '\'');
        p_set_event_location_from_token(env);
        env->event.type = ENTITY_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case GT_ENTITY_REFERENCE_TOKEN:
      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, ENTITY_REFERENCE_EVENT, '>');
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, '>');
        p_set_event_location_from_token(env);
        env->event.type = ENTITY_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case LT_ENTITY_REFERENCE_TOKEN:
      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, ENTITY_REFERENCE_EVENT, '<');
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, '<');
        p_set_event_location_from_token(env);
        env->event.type = ENTITY_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case QUOT_ENTITY_REFERENCE_TOKEN:
      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, ENTITY_REFERENCE_EVENT, '"');
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, '"');
        p_set_event_location_from_token(env);
        env->event.type = ENTITY_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;

    case ENTITY_REFERENCE_TOKEN:
      ent = p_find_entity_info(&env->tenv->result_token.value, env->general_entities);
      if(ent == 0) {
        err = UNDEFINED_ENTITY;
        goto error;
      }
      if(env->standalone && !ent->from_internal_subset) {
        err = UNDEFINED_ENTITY;
        goto error;
      }
      // [WFC: Parsed Entity]
      if(ent->unparsed) {
        err = REFERENCE_TO_UNPARSED_ENTITY;
        goto error;
      }

      if(env->current_attr) {
        // [WFC: No External Entity References]
        if(ent->external) {
          err = REFERENCE_TO_EXTERNAL_ENTITY;
          goto error;
        }

        err = p_set_attr_value_name_from_entity(env->current_attr, env, ENTITY_REFERENCE_START_EVENT, ent);
        if(err) goto error;

        err = p_parse_entity(env, ent, ATTRIBUTE_VALUE_ENTITY);
        if(err) goto error;
        return NO_ERROR;

      } else {
        p_reset_event(env);
        p_set_event_location_from_token(env);
        p_set_text_from_text(&env->event.name, &ent->name);

        err = p_parse_entity(env, ent, ELEMENT_CONTENT_ENTITY);
        if(!err)
          env->event.type = ENTITY_REFERENCE_START_EVENT;
        else if(err == DONT_PARSE_EXTERNAL_ENTITY)
          env->event.type = ENTITY_REFERENCE_EVENT;
        else goto error;

        return NO_ERROR;
      }
      break;
    case DEC_CHAR_REFERENCE_TOKEN:
      /* [WFC: Legal Character] */
      ch = p_dec_char_ref_value(&env->tenv->result_token.value, env);
      if((FAXPP_char_flags(ch) & env->tenv->xml_char) == 0) {
        err = ILLEGAL_CHARACTER_REFERENCE;
        goto error;
      }

      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, DEC_CHAR_REFERENCE_EVENT, ch);
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, ch);
        p_set_event_location_from_token(env);
        env->event.type = DEC_CHAR_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case HEX_CHAR_REFERENCE_TOKEN:
      /* [WFC: Legal Character] */
      ch = p_hex_char_ref_value(&env->tenv->result_token.value, env);
      if((FAXPP_char_flags(ch) & env->tenv->xml_char) == 0) {
        err = ILLEGAL_CHARACTER_REFERENCE;
        goto error;
      }

      if(env->current_attr) {
        err = p_set_attr_value_name(env->current_attr, env, HEX_CHAR_REFERENCE_EVENT, ch);
        if(err) goto error;
      } else {
        p_reset_event(env);
        p_copy_text_from_token(&env->event.name, env, /*useTokenBuffer*/1);
        p_set_text_to_char(&env->event.value, &env->event_buffer, env, ch);
        p_set_event_location_from_token(env);
        env->event.type = HEX_CHAR_REFERENCE_EVENT;
        return NO_ERROR;
      }
      break;
    case END_OF_BUFFER_TOKEN:
      p_reset_event(env);
      p_set_event_location_from_token(env);
      env->event.type = END_DOCUMENT_EVENT;
      return NO_ERROR;
    case NO_TOKEN:
      break;
    }
  }

  // Never happens
  return NO_ERROR;

 error:
  set_err_info_from_tokenizer(env);
  return err;
}

static const char *xml_prefix = "xml";
static const char *xmlns_prefix = "xmlns";
static const char *xml_uri = "http://www.w3.org/XML/1998/namespace";
static const char *xmlns_uri = "http://www.w3.org/2000/xmlns/";

static FAXPP_Error p_add_ns_info(FAXPP_ParserEnv *env, const FAXPP_Attribute *attr)
{
  FAXPP_NamespaceInfo *nsinfo;

  // Check for invalid "xml" or "xmlns" namespace declarations
  if(attr->prefix.len != 0) {
    if(p_equals(xmlns_prefix, env->tenv->transcoder.encode, &attr->name))
      return INVALID_NAMESPACE_DECLARATION;
    if(p_equals(xml_prefix, env->tenv->transcoder.encode, &attr->name) &&
       !p_equals(xml_uri, env->tenv->transcoder.encode, &attr->value.value))
      return INVALID_NAMESPACE_DECLARATION;
  }

  if((attr->prefix.len == 0 || !p_equals(xml_prefix, env->tenv->transcoder.encode, &attr->name)) &&
     p_equals(xml_uri, env->tenv->transcoder.encode, &attr->value.value))
    return INVALID_NAMESPACE_DECLARATION;

  if(p_equals(xmlns_uri, env->tenv->transcoder.encode, &attr->value.value))
    return INVALID_NAMESPACE_DECLARATION;

  if(env->tenv->xml_char == CHAR10 && attr->prefix.len != 0 && attr->value.value.len == 0)
    return INVALID_NAMESPACE_DECLARATION;

  // Add the namespace binding
  nsinfo = env->namespace_pool;
  if(nsinfo == 0) {
    nsinfo = (FAXPP_NamespaceInfo*)malloc(sizeof(FAXPP_NamespaceInfo));
    if(!nsinfo) return OUT_OF_MEMORY;
  }
  else {
    env->namespace_pool = nsinfo->prev;
  }

  memset(nsinfo, 0, sizeof(FAXPP_NamespaceInfo));
  nsinfo->prev = env->namespace_stack;
  env->namespace_stack = nsinfo;
  env->element_info_stack->ns = nsinfo;

  p_copy_text_from_event(&nsinfo->uri, &attr->value.value, env, &env->element_info_stack->buffer);

  if(attr->prefix.len != 0) {
    p_copy_text_from_event(&nsinfo->prefix, &attr->name, env, &env->element_info_stack->buffer);
  }
  return NO_ERROR;
}

static FAXPP_Error p_find_ns_info_impl(const FAXPP_ParserEnv *env, const FAXPP_Text *prefix, FAXPP_Text *uri)
{
  const FAXPP_NamespaceInfo *nsinfo;

  nsinfo = env->namespace_stack;
  while(nsinfo != 0) {
    if(p_compare_text(prefix, &nsinfo->prefix) == 0) {
      if(nsinfo->prefix.len != 0 && nsinfo->uri.len == 0)
        return NO_URI_FOR_PREFIX;
      p_set_text_from_text(uri, &nsinfo->uri);
      return NO_ERROR;
    }
    nsinfo = nsinfo->prev;
  }

  if(prefix->len == 0) {
    /* The default namespace is implicitly set to no namespace */
    return NO_ERROR;
  }

  // The prefix "xml" is always bound to the namespace URI "http://www.w3.org/XML/1998/namespace"
  if(p_equals(xml_prefix, env->tenv->transcoder.encode, prefix)) {
    p_copy_text_from_str(uri, &((FAXPP_ParserEnv*)env)->event_buffer, (FAXPP_ParserEnv*)env, xml_uri);
    return NO_ERROR;
  }

  return NO_URI_FOR_PREFIX;
}

static void p_change_element_info_buffer(void *userData, FAXPP_Buffer *buffer, void *newBuffer)
{
  FAXPP_ElementInfo *el = (FAXPP_ElementInfo*)userData;

  p_text_change_buffer(buffer, newBuffer, &el->prefix);
  p_text_change_buffer(buffer, newBuffer, &el->uri);
  p_text_change_buffer(buffer, newBuffer, &el->name);

  FAXPP_NamespaceInfo *ns = el->ns;
  while(ns != el->prev_ns) {
    p_text_change_buffer(buffer, newBuffer, &ns->prefix);
    p_text_change_buffer(buffer, newBuffer, &ns->uri);

    ns = ns->prev;
  }
}

static FAXPP_Error p_push_element(FAXPP_ParserEnv *env, int selfClosing)
{
  FAXPP_ElementInfo *einfo = env->element_info_pool;

  if(einfo == 0) {
    einfo = (FAXPP_ElementInfo*)malloc(sizeof(FAXPP_ElementInfo));
    if(!einfo) return OUT_OF_MEMORY;

    if(FAXPP_init_buffer(&einfo->buffer, INITIAL_ELEMENT_INFO_BUFFER_SIZE,
                         p_change_element_info_buffer, einfo) == OUT_OF_MEMORY)
      return OUT_OF_MEMORY;
  }
  else {
    env->element_info_pool = einfo->prev;
    FAXPP_reset_buffer(&einfo->buffer);
  }

  einfo->prev = env->element_info_stack;
  env->element_info_stack = einfo;

  /* Store the current place in the namespace stack */
  einfo->ns = env->namespace_stack;
  einfo->prev_ns = env->namespace_stack;

  if(!selfClosing) {
    einfo->uri.ptr = 0; einfo->uri.len = 0;
    p_copy_text_from_event(&einfo->prefix, &env->event.prefix, env, &einfo->buffer);
    p_copy_text_from_event(&einfo->name, &env->event.name, env, &einfo->buffer);
  }

  return NO_ERROR;
}

static void p_pop_element(FAXPP_ParserEnv *env)
{
  FAXPP_ElementInfo *einfo;
  FAXPP_NamespaceInfo *ns;

  einfo = env->element_info_stack;

  /* Take the namespace_stack back to it's
     position before this element */
  while(env->namespace_stack != einfo->prev_ns) {
    ns = env->namespace_stack;
    env->namespace_stack = ns->prev;

    /* Put the FAXPP_NamespaceInfo object back in the pool */
    ns->prev = env->namespace_pool;
    env->namespace_pool = ns;
  }

  env->element_info_stack = einfo->prev;

  /* Put the FAXPP_ElementInfo object back in the pool */
  einfo->prev = env->element_info_pool;
  env->element_info_pool = einfo;
}

static FAXPP_Error wf_next_event(FAXPP_ParserEnv *env)
{
  int i, j;
  FAXPP_Attribute *attr, *attr2;
  FAXPP_Text tmpText;

  FAXPP_Error err = nc_next_event(env);
  if(err != 0) return err;

  switch(env->event.type) {
  case START_ELEMENT_EVENT:
  case SELF_CLOSING_ELEMENT_EVENT:
    /* Store the element name */
    err = p_push_element(env, env->event.type == SELF_CLOSING_ELEMENT_EVENT);
    if(err != 0) {
      set_err_info_from_event(env);
      return err;
    }

    for(i = 0; i < env->event.attr_count; ++i) {
      attr = &env->event.attrs[i];

      /* Normalize the attribute values if required */
      if(attr->xmlns_attr || attr->xml_attr ||
         (env->tenv->normalize_attrs &&
          (attr->value.type != CHARACTERS_EVENT || attr->value.next != 0))) {
        err = p_normalize_attr_value(&tmpText, &env->event_buffer, &attr->value, env);
        if(err != 0) return err;

        attr->value.type = CHARACTERS_EVENT;
        attr->value.name.ptr = 0;
        attr->value.name.len = 0;
        attr->value.value.ptr = tmpText.ptr;
        attr->value.value.len = tmpText.len;
        attr->value.next = 0;
      }

      /* Check for namespace attributes */
      if(attr->xmlns_attr) {
        err = p_add_ns_info(env, attr);
        if(err) {
          set_err_info_from_attr(env, attr);
          return err;
        }
      }
    }

    /* Resolve the element's URI */
    err = p_find_ns_info(env, &env->event.prefix, &env->event.uri);
    if(err != 0) {
      set_err_info_from_event(env);
      return err;
    }

    /* Copy the element's URI to the element stack */
    /* No need to use the element info buffer, since the value already comes from there */
    p_set_text_from_text(&env->element_info_stack->uri, &env->event.uri);

    for(i = 0; i < env->event.attr_count; ++i) {
      attr = &env->event.attrs[i];
      /* Resolve the attributes' URIs */
      if(attr->xmlns_attr) {
        p_copy_text_from_str(&attr->uri, &env->event_buffer, env, xmlns_uri);
      }
      else if(attr->xml_attr) {
        p_copy_text_from_str(&attr->uri, &env->event_buffer, env, xml_uri);

        if(p_equals("space", env->tenv->transcoder.encode, &attr->name) &&
           !p_equals("preserve", env->tenv->transcoder.encode, &attr->value.value) &&
           !p_equals("default", env->tenv->transcoder.encode, &attr->value.value)) {
          set_err_info_from_attr(env, attr);
          return INVALID_XMLSPACE_VALUE;
        }
      }
      else if(attr->prefix.len != 0) {
        err = p_find_ns_info(env, &attr->prefix, &attr->uri);
        if(err != 0) {
          set_err_info_from_attr(env, attr);
          return err;
        }
      }
    }

    for(i = 0; i < env->event.attr_count; ++i) {
      attr = &env->event.attrs[i];
      /* [WFC: Unique Att Spec] */
      /* Nested loop check for duplicate attributes */
      /* TBD make this a better algorithm? - jpcs */
      for(j = i + 1; j < env->event.attr_count; ++j) {
        attr2 = &env->event.attrs[j];
        if(attr2->xmlns_attr == attr->xmlns_attr &&
           p_compare_text(&attr2->name, &attr->name) == 0 &&
           p_compare_text(&attr2->uri, &attr->uri) == 0) {
          set_err_info_from_attr(env, attr);
          return DUPLICATE_ATTRIBUTES;
        }
      }
    }

    if(env->event.type == SELF_CLOSING_ELEMENT_EVENT) {
      /* Do this to remove the FAXPP_NamespaceInfo objects
         from the namespace stack */
      p_pop_element(env);
    }
    break;
  case END_ELEMENT_EVENT:
    /* [WFC: Element Type Match] */
    if(p_compare_text(&env->element_info_stack->name, &env->event.name) != 0 ||
       p_compare_text(&env->element_info_stack->prefix, &env->event.prefix) != 0) {
      set_err_info_from_event(env);
      err = ELEMENT_NAME_MISMATCH;
    }

    /* Copy the element's URI from the element stack */
    p_set_text_from_text(&env->event.uri, &env->element_info_stack->uri);

    p_pop_element(env);
    break;
  case START_DOCUMENT_EVENT:
  case END_DOCUMENT_EVENT:
  case DOCTYPE_EVENT:
  case CHARACTERS_EVENT:
  case CDATA_EVENT:
  case IGNORABLE_WHITESPACE_EVENT:
  case COMMENT_EVENT:
  case PI_EVENT:
  case ENTITY_REFERENCE_EVENT:
  case DEC_CHAR_REFERENCE_EVENT:
  case HEX_CHAR_REFERENCE_EVENT:
  case ENTITY_REFERENCE_START_EVENT:
  case ENTITY_REFERENCE_END_EVENT:
  case START_EXTERNAL_ENTITY_EVENT:
  case END_EXTERNAL_ENTITY_EVENT:
  case NO_EVENT:
    break;
  }

  return err;
}

