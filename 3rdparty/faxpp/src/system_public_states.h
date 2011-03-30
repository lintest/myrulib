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

// This file needs to have a number of macros defined before it is included
// PREFIX(name)
// END_STATE
// SKIP_SYSTEM_LITERAL
// ALLOW_PARAMETER_ENTITIES

#define SP_SINGLE_CHAR_STATE(name, ch, next_stored_state, next_state, error) \
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

SP_SINGLE_CHAR_STATE(PREFIX(system_id_initial_state1), 'Y', 0, PREFIX(system_id_initial_state2), INVALID_SYSTEM_ID)
SP_SINGLE_CHAR_STATE(PREFIX(system_id_initial_state2), 'S', 0, PREFIX(system_id_initial_state3), INVALID_SYSTEM_ID)
SP_SINGLE_CHAR_STATE(PREFIX(system_id_initial_state3), 'T', 0, PREFIX(system_id_initial_state4), INVALID_SYSTEM_ID)
SP_SINGLE_CHAR_STATE(PREFIX(system_id_initial_state4), 'E', 0, PREFIX(system_id_initial_state5), INVALID_SYSTEM_ID)
SP_SINGLE_CHAR_STATE(PREFIX(system_id_initial_state5), 'M', 0, PREFIX(system_id_ws_state), INVALID_SYSTEM_ID)

FAXPP_Error
PREFIX(system_id_ws_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = PREFIX(system_literal_start_state);
    next_char(env);
    break;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
  default:
    env->state = PREFIX(system_literal_start_state);
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
PREFIX(system_literal_start_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    return NO_ERROR;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
  case '"':
    env->state = PREFIX(system_literal_quot_state);
    break;
  case '\'':
    env->state = PREFIX(system_literal_apos_state);
    break;
  default:
    next_char(env);
    return EXPECTING_SYSTEM_LITERAL;
  }
  next_char(env);
  token_start_position(env);
  return NO_ERROR;
}

FAXPP_Error
PREFIX(system_literal_apos_state)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    case '\'':
      env->state = END_STATE;
      token_end_position(env);
      report_token(SYSTEM_LITERAL_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '#':
      next_char(env);
      return INVALID_SYSTEM_ID;
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
PREFIX(system_literal_quot_state)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    case '"':
      env->state = END_STATE;
      token_end_position(env);
      report_token(SYSTEM_LITERAL_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '#':
      next_char(env);
      return INVALID_SYSTEM_ID;
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

SP_SINGLE_CHAR_STATE(PREFIX(public_id_initial_state1), 'U', 0, PREFIX(public_id_initial_state2), INVALID_PUBLIC_ID)
SP_SINGLE_CHAR_STATE(PREFIX(public_id_initial_state2), 'B', 0, PREFIX(public_id_initial_state3), INVALID_PUBLIC_ID)
SP_SINGLE_CHAR_STATE(PREFIX(public_id_initial_state3), 'L', 0, PREFIX(public_id_initial_state4), INVALID_PUBLIC_ID)
SP_SINGLE_CHAR_STATE(PREFIX(public_id_initial_state4), 'I', 0, PREFIX(public_id_initial_state5), INVALID_PUBLIC_ID)
SP_SINGLE_CHAR_STATE(PREFIX(public_id_initial_state5), 'C', 0, PREFIX(public_id_ws_state), INVALID_PUBLIC_ID)

FAXPP_Error
PREFIX(public_id_ws_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = PREFIX(pubid_literal_start_state);
    next_char(env);
    break;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
  default:
    env->state = PREFIX(pubid_literal_start_state);
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
PREFIX(pubid_literal_start_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    return NO_ERROR;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
  case '"':
    env->state = PREFIX(pubid_literal_quot_state);
    break;
  case '\'':
    env->state = PREFIX(pubid_literal_apos_state);
    break;
  default:
    next_char(env);
    return EXPECTING_PUBID_LITERAL;
  }
  next_char(env);
  token_start_position(env);
  return NO_ERROR;
}

FAXPP_Error
PREFIX(pubid_literal_apos_state)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    case '\'':
      env->state = PREFIX(public_id_ws_state2);
      token_end_position(env);
      report_token(PUBID_LITERAL_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    // [13]   	PubidChar	   ::=   	#x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
    LINE_ENDINGS
    // A-Z
               case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
    case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
    case 0x58: case 0x59: case 0x5A:
    // a-z
               case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
    case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
    case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
    case 0x78: case 0x79: case 0x7A:
    // 0-9
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
    case '9': 
    case ' ': case '-': case '(': case ')': case '+': case ',': case '.': case '/': case ':':
    case '=': case '?': case ';': case '!': case '*': case '#': case '@': case '$': case '_':
    case '%':
      // Valid PubidChar
      break;
    default:
      next_char(env);
      return INVALID_CHAR_IN_PUBID_LITERAL;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
PREFIX(pubid_literal_quot_state)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    case '"':
      env->state = PREFIX(public_id_ws_state2);
      token_end_position(env);
      report_token(PUBID_LITERAL_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    // [13]   	PubidChar	   ::=   	#x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
    LINE_ENDINGS
    // A-Z
               case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
    case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
    case 0x58: case 0x59: case 0x5A:
    // a-z
               case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
    case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
    case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
    case 0x78: case 0x79: case 0x7A:
    // 0-9
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
    case '9': 
    case ' ': case '-': case '(': case ')': case '+': case ',': case '.': case '/': case ':':
    case '=': case '?': case ';': case '!': case '*': case '#': case '@': case '$': case '_':
    case '%': case '\'':
      // Valid PubidChar
      break;
    default:
      next_char(env);
      return INVALID_CHAR_IN_PUBID_LITERAL;
    }
    next_char(env);
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
PREFIX(public_id_ws_state2)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = PREFIX(public_id_ws_state3);
    next_char(env);
    break;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
#ifdef SKIP_SYSTEM_LITERAL
  case '>':
    // Notation decls can skip the system literal
    env->state = END_STATE;
    return NO_ERROR;
#endif
  default:
    env->state = PREFIX(system_literal_start_state);
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
PREFIX(public_id_ws_state3)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
#ifdef ALLOW_PARAMETER_ENTITIES
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
#endif
#ifdef SKIP_SYSTEM_LITERAL
  case '>':
    // Notation decls can skip the system literal
    env->state = END_STATE;
    return NO_ERROR;
#endif
  default:
    env->state = PREFIX(system_literal_start_state);
    break;
  }
  return NO_ERROR;
}

#undef SP_SINGLE_CHAR_STATE
