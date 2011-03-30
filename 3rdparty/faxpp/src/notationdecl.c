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

#include "tokenizer_states.h"
#include "char_classes.h"

#define SINGLE_CHAR_STATE(name, ch, next_stored_state, next_state, error) \
FAXPP_Error \
name(FAXPP_TokenizerEnv *env) \
{ \
  read_char(env); \
\
  switch(env->current_char) { \
  case (ch): \
    if((next_stored_state) != 0) env->stored_state = (next_stored_state); \
    env->state = (next_state); \
    next_char(env); \
    break; \
  LINE_ENDINGS \
  default: \
    next_char(env); \
    return (error); \
  } \
  return NO_ERROR; \
}

SINGLE_CHAR_STATE(notationdecl_initial_state1, 'O', 0, notationdecl_initial_state2, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state2, 'T', 0, notationdecl_initial_state3, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state3, 'A', 0, notationdecl_initial_state4, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state4, 'T', 0, notationdecl_initial_state5, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state5, 'I', 0, notationdecl_initial_state6, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state6, 'O', 0, notationdecl_initial_state7, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(notationdecl_initial_state7, 'N', notationdecl_name_state1, ws_plus_state, INVALID_DTD_DECL)

FAXPP_Error
notationdecl_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = notationdecl_name_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_NOTATIONDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
notationdecl_name_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
      env->stored_state = notationdecl_content_state;
      env->state = ws_state;
      token_end_position(env);
      report_token(NOTATIONDECL_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_NOTATIONDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
notationdecl_content_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'S':
    env->state = notationdecl_system_id_initial_state1;
    break;
  case 'P':
    env->state = notationdecl_public_id_initial_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_DTD_DECL;
  }
  next_char(env);
  return NO_ERROR;
}

#define PREFIX(name) notationdecl_ ## name
#define END_STATE notationdecl_end_state
#define SKIP_SYSTEM_LITERAL
#define ALLOW_PARAMETER_ENTITIES

#include "system_public_states.h"

#undef ALLOW_PARAMETER_ENTITIES
#undef SKIP_SYSTEM_LITERAL
#undef END_STATE
#undef PREFIX

FAXPP_Error
notationdecl_end_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '>':
    base_state(env);
    report_empty_token(NOTATIONDECL_END_TOKEN, env);
    next_char(env);
    token_start_position(env);
    break;
  default:
    next_char(env);
    return INVALID_DTD_DECL;
  }
  return NO_ERROR;
}

