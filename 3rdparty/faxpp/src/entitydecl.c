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

SINGLE_CHAR_STATE(entitydecl_initial_state1, 'T', 0, entitydecl_initial_state2, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(entitydecl_initial_state2, 'I', 0, entitydecl_initial_state3, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(entitydecl_initial_state3, 'T', 0, entitydecl_initial_state4, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(entitydecl_initial_state4, 'Y', entitydecl_param_or_general_state, ws_plus_state, INVALID_DTD_DECL)

FAXPP_Error
entitydecl_param_or_general_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '%':
    env->stored_state = paramentitydecl_name_state1;
    env->state = ws_plus_state;
    next_char(env);
    break;
  LINE_ENDINGS
  default:
    env->state = entitydecl_name_state;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ENTITYDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FAXPP_Error
entitydecl_name_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
      env->stored_state = entitydecl_content_state;
      env->state = ws_state;
      token_end_position(env);
      report_token(ENTITYDECL_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ENTITYDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
entitydecl_content_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '"':
    env->state = entitydecl_value_quot_state;
    next_char(env);
    token_start_position(env);
    return NO_ERROR;
  case '\'':
    env->state = entitydecl_value_apos_state;
    next_char(env);
    token_start_position(env);
    return NO_ERROR;
  case 'S':
    env->state = entitydecl_system_id_initial_state1;
    break;
  case 'P':
    env->state = entitydecl_public_id_initial_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_ENTITYDECL;
  }
  next_char(env);
  return NO_ERROR;
}

#define PREFIX(name) entitydecl_ ## name
#define END_STATE entitydecl_ws_state
#define ALLOW_PARAMETER_ENTITIES

#include "system_public_states.h"

#undef ALLOW_PARAMETER_ENTITIES
#undef END_STATE
#undef PREFIX

FAXPP_Error
entitydecl_value_apos_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ENTITYDECL_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '\'':
      env->state = entitydecl_end_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '%':
      store_state(env);
      env->state = parameter_entity_reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    LINE_ENDINGS
    default:
      if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
        next_char(env);
        return RESTRICTED_CHAR;
      }
      break;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
entitydecl_value_quot_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ENTITYDECL_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '"':
      env->state = entitydecl_end_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '%':
      store_state(env);
      env->state = parameter_entity_reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    LINE_ENDINGS
    default:
      if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
        next_char(env);
        return RESTRICTED_CHAR;
      }
      break;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
entitydecl_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = entitydecl_ndata_or_end_state;;
    next_char(env);
    break;
  case '>':
    env->state = entitydecl_end_state;
    break;
  default:
    env->state = entitydecl_ndata_or_end_state;
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
entitydecl_ndata_or_end_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '>':
    env->state = entitydecl_end_state;
    break;
  case 'N':
    env->state = entitydecl_ndata_state1;
    next_char(env);
    break;
  default:
    next_char(env);
    return INVALID_ENTITYDECL;
  }
  return NO_ERROR;
}

SINGLE_CHAR_STATE(entitydecl_ndata_state1, 'D', 0, entitydecl_ndata_state2, INVALID_ENTITYDECL)
SINGLE_CHAR_STATE(entitydecl_ndata_state2, 'A', 0, entitydecl_ndata_state3, INVALID_ENTITYDECL)
SINGLE_CHAR_STATE(entitydecl_ndata_state3, 'T', 0, entitydecl_ndata_state4, INVALID_ENTITYDECL)
SINGLE_CHAR_STATE(entitydecl_ndata_state4, 'A', entitydecl_ndata_name_state1, ws_plus_state, INVALID_ENTITYDECL)

FAXPP_Error
entitydecl_ndata_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = entitydecl_ndata_name_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_ENTITYDECL;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
entitydecl_ndata_name_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
      env->state = entitydecl_end_state;
      token_end_position(env);
      report_token(NDATA_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '>':
      env->state = entitydecl_end_state;
      token_end_position(env);
      report_token(NDATA_NAME_TOKEN, env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_ENTITYDECL;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
entitydecl_end_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '>':
    base_state(env);
    report_empty_token(ENTITYDECL_END_TOKEN, env);
    next_char(env);
    token_start_position(env);
    break;
  default:
    next_char(env);
    return INVALID_ENTITYDECL;
  }
  return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FAXPP_Error
paramentitydecl_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = paramentitydecl_name_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ENTITYDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
paramentitydecl_name_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
      env->stored_state = paramentitydecl_content_state;
      env->state = ws_state;
      token_end_position(env);
      report_token(PARAMENTITYDECL_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ENTITYDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
paramentitydecl_content_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '"':
    env->state = paramentitydecl_value_quot_state;
    next_char(env);
    token_start_position(env);
    return NO_ERROR;
  case '\'':
    env->state = paramentitydecl_value_apos_state;
    next_char(env);
    token_start_position(env);
    return NO_ERROR;
  case 'S':
    env->state = paramentitydecl_system_id_initial_state1;
    break;
  case 'P':
    env->state = paramentitydecl_public_id_initial_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_ENTITYDECL;
  }
  next_char(env);
  return NO_ERROR;
}

#define PREFIX(name) paramentitydecl_ ## name
#define END_STATE paramentitydecl_end_state
#define ALLOW_PARAMETER_ENTITIES

#include "system_public_states.h"

#undef ALLOW_PARAMETER_ENTITIES
#undef END_STATE
#undef PREFIX

FAXPP_Error
paramentitydecl_value_apos_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ENTITYDECL_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '\'':
      env->state = paramentitydecl_end_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '%':
      store_state(env);
      env->state = parameter_entity_reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    LINE_ENDINGS
    default:
      if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
        next_char(env);
        return RESTRICTED_CHAR;
      }
      break;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
paramentitydecl_value_quot_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ENTITYDECL_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '"':
      env->state = paramentitydecl_end_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '%':
      store_state(env);
      env->state = parameter_entity_reference_state;
      token_end_position(env);
      report_token(ENTITYDECL_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    LINE_ENDINGS
    default:
      if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
        next_char(env);
        return RESTRICTED_CHAR;
      }
      break;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
paramentitydecl_end_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '>':
    base_state(env);
    report_empty_token(ENTITYDECL_END_TOKEN, env);
    next_char(env);
    token_start_position(env);
    break;
  default:
    next_char(env);
    return INVALID_ENTITYDECL;
  }
  return NO_ERROR;
}

