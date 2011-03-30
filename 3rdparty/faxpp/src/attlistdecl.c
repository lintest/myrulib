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

#define SINGLE_CHAR_STATE_RETURN(name, ch, next_stored_state, next_state, error, return_token) \
FAXPP_Error \
name(FAXPP_TokenizerEnv *env) \
{ \
  read_char(env); \
\
  switch(env->current_char) { \
  case (ch): \
    if((next_stored_state) != 0) env->stored_state = (next_stored_state); \
    env->state = (next_state); \
    if((return_token) != NO_TOKEN) { report_empty_token((return_token), env); } \
    next_char(env); \
    break; \
  LINE_ENDINGS \
  default: \
    next_char(env); \
    return (error); \
  } \
  return NO_ERROR; \
}

#define SINGLE_CHAR_STATE(name, ch, next_stored_state, next_state, error) SINGLE_CHAR_STATE_RETURN(name, ch, next_stored_state, next_state, error, NO_TOKEN)

SINGLE_CHAR_STATE(attlistdecl_initial_state1, 'T', 0, attlistdecl_initial_state2, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(attlistdecl_initial_state2, 'T', 0, attlistdecl_initial_state3, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(attlistdecl_initial_state3, 'L', 0, attlistdecl_initial_state4, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(attlistdecl_initial_state4, 'I', 0, attlistdecl_initial_state5, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(attlistdecl_initial_state5, 'S', 0, attlistdecl_initial_state6, INVALID_DTD_DECL)
SINGLE_CHAR_STATE(attlistdecl_initial_state6, 'T', 0, attlistdecl_name_ws_state1, INVALID_DTD_DECL)

FAXPP_Error
attlistdecl_name_ws_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = attlistdecl_name_ws_state2;
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_name_state1;
    token_start_position(env);
    // No next_char
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_name_ws_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_name_state1;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = attlistdecl_name_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_name_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
    case '%':
      env->state = attlistdecl_attdef_name_ws_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_NAME_TOKEN, env);
      // No next_char
      return NO_ERROR;
    case '>':
      env->state = attlistdecl_attdef_name_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_NAME_TOKEN, env);
      // no next char
      return NO_ERROR;
    case ':':
      env->state = attlistdecl_name_seen_colon_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_PREFIX_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_name_seen_colon_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = attlistdecl_name_seen_colon_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_name_seen_colon_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
    case '%':
      env->state = attlistdecl_attdef_name_ws_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_NAME_TOKEN, env);
      // No next_char
      return NO_ERROR;
    case '>':
      env->state = attlistdecl_attdef_name_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_NAME_TOKEN, env);
      // no next char
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_attdef_name_ws_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = attlistdecl_attdef_name_ws_state2;
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  case '>':
    base_state(env);
    report_empty_token(ATTLISTDECL_END_TOKEN, env);
    break;
  default:
    env->state = attlistdecl_attdef_name_state1;
    token_start_position(env);
    // No next_char
    return EXPECTING_WHITESPACE;
  }
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_attdef_name_ws_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_attdef_name_state1;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_attdef_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '>':
    base_state(env);
    report_empty_token(ATTLISTDECL_END_TOKEN, env);
    break;
  LINE_ENDINGS
  default:
    env->state = attlistdecl_attdef_name_state2;
    token_start_position(env);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
    return NO_ERROR;
  }

  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_attdef_name_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
    case '%':
      env->state = attlistdecl_atttype_ws_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_ATTDEF_NAME_TOKEN, env);
      // No next_char
      return NO_ERROR;
    case ':':
      env->state = attlistdecl_attdef_name_seen_colon_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_ATTDEF_PREFIX_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_attdef_name_seen_colon_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = attlistdecl_attdef_name_seen_colon_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_attdef_name_seen_colon_state2(FAXPP_TokenizerEnv *env)
{
  while(1) {
    read_char(env);

    switch(env->current_char) {
    WHITESPACE:
    case '%':
      env->state = attlistdecl_atttype_ws_state1;
      token_end_position(env);
      report_token(ATTLISTDECL_ATTDEF_NAME_TOKEN, env);
      // No next_char
      return NO_ERROR;
    default:
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ATTLISTDECL_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_ws_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = attlistdecl_atttype_ws_state2;
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_state;
    token_start_position(env);
    // No next_char
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_ws_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_state;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

/* [54]    AttType    ::=    StringType | TokenizedType | EnumeratedType */
/* [55]    StringType     ::=    'CDATA' */
/* [56]    TokenizedType    ::=    'ID'  [VC: ID] [VC: One ID per Element Type] [VC: ID Attribute Default] */
/*       | 'IDREF' [VC: IDREF] */
/*       | 'IDREFS'  [VC: IDREF] */
/*       | 'ENTITY'  [VC: Entity Name] */
/*       | 'ENTITIES'  [VC: Entity Name] */
/*       | 'NMTOKEN' [VC: Name Token] */
/*       | 'NMTOKENS'  [VC: Name Token] */

/* [57]    EnumeratedType     ::=    NotationType | Enumeration */
/* [58]    NotationType     ::=    'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'   [VC: Notation Attributes] */
/*         [VC: One Notation Per Element Type] */
/*         [VC: No Notation on Empty Element] */
/*         [VC: No Duplicate Tokens] */
/* [59]    Enumeration    ::=    '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'  [VC: Enumeration] */
/*         [VC: No Duplicate Tokens] */
FAXPP_Error
attlistdecl_atttype_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'C':
    env->state = attlistdecl_atttype_cdata_state1;
    break;
  case 'I':
    env->state = attlistdecl_atttype_id_state1;
    break;
  case 'E':
    env->state = attlistdecl_atttype_entity_state1;
    break;
  case 'N':
    env->state = attlistdecl_atttype_nmtoken_state1;
    break;
  case '(':
    env->state = attlistdecl_atttype_enumeration_name_ws_state;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_entity_state1, 'N', 0, attlistdecl_atttype_entity_state2, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_entity_state2, 'T', 0, attlistdecl_atttype_entity_state3, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_entity_state3, 'I', 0, attlistdecl_atttype_entity_state4, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_entity_state4, 'T', 0, attlistdecl_atttype_entity_state5, INVALID_ATTRIBUTE_TYPE)

FAXPP_Error
attlistdecl_atttype_entity_state5(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'Y':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_ENTITY_TOKEN, env);
    break;
  case 'I':
    env->state = attlistdecl_atttype_entities_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_entities_state1, 'E', 0, attlistdecl_atttype_entities_state2, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE_RETURN(attlistdecl_atttype_entities_state2, 'S', 0, attlistdecl_default_ws_state1, INVALID_ATTRIBUTE_TYPE, ATTLISTDECL_ATTTYPE_ENTITIES_TOKEN)

FAXPP_Error
attlistdecl_atttype_nmtoken_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'O':
    env->state = attlistdecl_atttype_notation_state1;
    break;
  case 'M':
    env->state = attlistdecl_atttype_nmtoken_state2;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_nmtoken_state2, 'T', 0, attlistdecl_atttype_nmtoken_state3, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_nmtoken_state3, 'O', 0, attlistdecl_atttype_nmtoken_state4, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_nmtoken_state4, 'K', 0, attlistdecl_atttype_nmtoken_state5, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_nmtoken_state5, 'E', 0, attlistdecl_atttype_nmtoken_state6, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_nmtoken_state6, 'N', 0, attlistdecl_atttype_nmtoken_state7, INVALID_ATTRIBUTE_TYPE)

FAXPP_Error
attlistdecl_atttype_nmtoken_state7(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
  case '%':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_NMTOKEN_TOKEN, env);
    // No next_char
    return NO_ERROR;
    break;
  case 'S':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_NMTOKENS_TOKEN, env);
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state1, 'T', 0, attlistdecl_atttype_notation_state2, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state2, 'A', 0, attlistdecl_atttype_notation_state3, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state3, 'T', 0, attlistdecl_atttype_notation_state4, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state4, 'I', 0, attlistdecl_atttype_notation_state5, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state5, 'O', 0, attlistdecl_atttype_notation_state6, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_notation_state6, 'N', 0, attlistdecl_atttype_notation_ws_state1, INVALID_ATTRIBUTE_TYPE)

FAXPP_Error
attlistdecl_atttype_notation_ws_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = attlistdecl_atttype_notation_ws_state2;
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_notation_lpar_state;
    token_start_position(env);
    // No next_char
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_notation_ws_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_notation_lpar_state;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}
FAXPP_Error
attlistdecl_atttype_notation_lpar_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '(':
    env->state = attlistdecl_atttype_notation_name_ws_state;
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_notation_name_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_notation_name_state1;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_notation_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = attlistdecl_atttype_notation_name_state2;
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_ATTRIBUTE_TYPE;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_notation_name_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
  case '%':
    env->state = attlistdecl_atttype_notation_separator_ws_state;
    token_end_position(env);
    report_token(ATTLISTDECL_NOTATION_NAME_TOKEN, env);
    // No next_char
    return NO_ERROR;
  case '|':
    env->state = attlistdecl_atttype_notation_name_ws_state;
    token_end_position(env);
    report_token(ATTLISTDECL_NOTATION_NAME_TOKEN, env);
    break;
  case ')':
    env->state = attlistdecl_default_ws_state1;
    token_end_position(env);
    report_token(ATTLISTDECL_NOTATION_NAME_TOKEN, env);
    break;
  default:
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_ATTRIBUTE_TYPE;
    return NO_ERROR;
  }

  next_char(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_notation_separator_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_notation_separator_state;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_notation_separator_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '|':
    env->state = attlistdecl_atttype_notation_name_ws_state;
    break;
  case ')':
    env->state = attlistdecl_default_ws_state1;
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_id_state1, 'D', 0, attlistdecl_atttype_id_state2, INVALID_ATTRIBUTE_TYPE)

FAXPP_Error
attlistdecl_atttype_id_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
  case '%':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_ID_TOKEN, env);
    // No next_char
    return NO_ERROR;
  case 'R':
    env->state = attlistdecl_atttype_idref_state1;
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_idref_state1, 'E', 0, attlistdecl_atttype_idref_state2, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_idref_state2, 'F', 0, attlistdecl_atttype_idref_state3, INVALID_ATTRIBUTE_TYPE)

FAXPP_Error
attlistdecl_atttype_idref_state3(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
  case '%':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_IDREF_TOKEN, env);
    // No next_char
    return NO_ERROR;
  case 'S':
    env->state = attlistdecl_default_ws_state1;
    report_empty_token(ATTLISTDECL_ATTTYPE_IDREFS_TOKEN, env);
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_atttype_cdata_state1, 'D', 0, attlistdecl_atttype_cdata_state2, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_cdata_state2, 'A', 0, attlistdecl_atttype_cdata_state3, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE(attlistdecl_atttype_cdata_state3, 'T', 0, attlistdecl_atttype_cdata_state4, INVALID_ATTRIBUTE_TYPE)
SINGLE_CHAR_STATE_RETURN(attlistdecl_atttype_cdata_state4, 'A', 0, attlistdecl_default_ws_state1, INVALID_ATTRIBUTE_TYPE, ATTLISTDECL_ATTTYPE_CDATA_TOKEN)

FAXPP_Error
attlistdecl_atttype_enumeration_name_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_enumeration_name_state1;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_enumeration_name_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = attlistdecl_atttype_enumeration_name_state2;
    next_char(env);
    if(env->current_char != ':' && (FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_ATTRIBUTE_TYPE;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_enumeration_name_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
  case '%':
    env->state = attlistdecl_atttype_enumeration_separator_ws_state;
    token_end_position(env);
    report_token(ATTLISTDECL_ENUMERATION_NAME_TOKEN, env);
    // No next_char
    return NO_ERROR;
  case '|':
    env->state = attlistdecl_atttype_enumeration_name_ws_state;
    token_end_position(env);
    report_token(ATTLISTDECL_ENUMERATION_NAME_TOKEN, env);
    break;
  case ')':
    env->state = attlistdecl_default_ws_state1;
    token_end_position(env);
    report_token(ATTLISTDECL_ENUMERATION_NAME_TOKEN, env);
    break;
  case ':':
    break;
  default:
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_ATTRIBUTE_TYPE;
    return NO_ERROR;
  }

  next_char(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_atttype_enumeration_separator_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_atttype_enumeration_separator_state;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_atttype_enumeration_separator_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '|':
    env->state = attlistdecl_atttype_enumeration_name_ws_state;
    break;
  case ')':
    env->state = attlistdecl_default_ws_state1;
    break;
  default:
    next_char(env);
    return INVALID_ATTRIBUTE_TYPE;
  }

  next_char(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_default_ws_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = attlistdecl_default_ws_state2;
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_default_state1;
    token_start_position(env);
    // No next_char
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
attlistdecl_default_ws_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '%':
    store_state(env);
    env->state = parameter_entity_reference_in_markup_state;
    next_char(env);
    token_start_position(env);
    if(env->external_subset || env->external_dtd_entity || env->external_in_markup_entity)
      return NO_ERROR;
    return PARAMETER_ENTITY_IN_INTERNAL_SUBSET;
  default:
    env->state = attlistdecl_default_state1;
    token_start_position(env);
    // No next_char
    break;
  }
  return NO_ERROR;
}

/* [60]    DefaultDecl    ::=    '#REQUIRED' | '#IMPLIED' */
/*       | (('#FIXED' S)? AttValue)  [VC: Required Attribute] */
/*         [VC: Attribute Default Value Syntactically Correct] */
/*         [WFC: No < in Attribute Values] */
/*         [VC: Fixed Attribute Default] */
FAXPP_Error
attlistdecl_default_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '#':
    env->state = attlistdecl_default_state2;
    next_char(env);
    return NO_ERROR;  
  case '\'':
    env->state = attlistdecl_attvalue_apos_state;
    break;
  case '"':
    env->state = attlistdecl_attvalue_quot_state;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_DEFAULTDECL;
  }

  next_char(env);
  token_start_position(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_default_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'R':
    env->state = attlistdecl_default_required_state1;
    break;
  case 'I':
    env->state = attlistdecl_default_implied_state1;
    break;
  case 'F':
    env->state = attlistdecl_default_fixed_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_DEFAULTDECL;
  }

  next_char(env);
  return NO_ERROR;  
}

SINGLE_CHAR_STATE(attlistdecl_default_implied_state1, 'M', 0, attlistdecl_default_implied_state2, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_implied_state2, 'P', 0, attlistdecl_default_implied_state3, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_implied_state3, 'L', 0, attlistdecl_default_implied_state4, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_implied_state4, 'I', 0, attlistdecl_default_implied_state5, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_implied_state5, 'E', 0, attlistdecl_default_implied_state6, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE_RETURN(attlistdecl_default_implied_state6, 'D', 0, attlistdecl_attdef_name_ws_state1, INVALID_DEFAULTDECL, ATTLISTDECL_DEFAULT_IMPLIED_TOKEN)

SINGLE_CHAR_STATE(attlistdecl_default_required_state1, 'E', 0, attlistdecl_default_required_state2, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_required_state2, 'Q', 0, attlistdecl_default_required_state3, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_required_state3, 'U', 0, attlistdecl_default_required_state4, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_required_state4, 'I', 0, attlistdecl_default_required_state5, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_required_state5, 'R', 0, attlistdecl_default_required_state6, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_required_state6, 'E', 0, attlistdecl_default_required_state7, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE_RETURN(attlistdecl_default_required_state7, 'D', 0, attlistdecl_attdef_name_ws_state1, INVALID_DEFAULTDECL, ATTLISTDECL_DEFAULT_REQUIRED_TOKEN)

SINGLE_CHAR_STATE(attlistdecl_default_fixed_state1, 'I', 0, attlistdecl_default_fixed_state2, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_fixed_state2, 'X', 0, attlistdecl_default_fixed_state3, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE(attlistdecl_default_fixed_state3, 'E', 0, attlistdecl_default_fixed_state4, INVALID_DEFAULTDECL)
SINGLE_CHAR_STATE_RETURN(attlistdecl_default_fixed_state4, 'D', attlistdecl_attvalue_start_state, ws_plus_state, INVALID_DEFAULTDECL, ATTLISTDECL_DEFAULT_FIXED_TOKEN)

FAXPP_Error
attlistdecl_attvalue_start_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '\'':
    env->state = attlistdecl_attvalue_apos_state;
    break;
  case '"':
    env->state = attlistdecl_attvalue_quot_state;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_DEFAULTDECL;
  }

  next_char(env);
  token_start_position(env);
  return NO_ERROR;  
}

FAXPP_Error
attlistdecl_attvalue_apos_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ATTRIBUTE_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '\'':
      env->state = attlistdecl_attdef_name_ws_state1;
      token_end_position(env);
      report_token(ATTRIBUTE_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ATTRIBUTE_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '<':
      next_char(env);
      return INVALID_CHAR_IN_ATTRIBUTE;
    LINE_ENDINGS
    case '\t':
      if(env->normalize_attrs) {
        // Move the token to the buffer, to normalize it
        FAXPP_Error err = FAXPP_tokenizer_release_buffer(env, 0);
        if(err != NO_ERROR) return err;
        env->current_char = ' ';
      }
      break;
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
attlistdecl_attvalue_quot_state(FAXPP_TokenizerEnv *env)
{
  while(1) {
    if(env->position >= env->buffer_end) {
      if(env->token.value.ptr) {
        token_end_position(env);
        if(env->token.value.len != 0) {
          report_token(ATTRIBUTE_VALUE_TOKEN, env);
          return NO_ERROR;
        }
      }
      token_start_position(env);
      return PREMATURE_END_OF_BUFFER;
    }

    read_char_no_check(env);

    switch(env->current_char) {
    case '"':
      env->state = attlistdecl_attdef_name_ws_state1;
      token_end_position(env);
      report_token(ATTRIBUTE_VALUE_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '&':
      store_state(env);
      env->state = reference_state;
      token_end_position(env);
      report_token(ATTRIBUTE_VALUE_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case '<':
      next_char(env);
      return INVALID_CHAR_IN_ATTRIBUTE;
    LINE_ENDINGS
    case '\t': {
      if(env->normalize_attrs) {
        // Move the token to the buffer, to normalize it
        FAXPP_Error err = FAXPP_tokenizer_release_buffer(env, 0);
        if(err != NO_ERROR) return err;
        env->current_char = ' ';
      }
      break;
    }
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


