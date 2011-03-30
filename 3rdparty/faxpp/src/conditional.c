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

FAXPP_Error
conditional_ws_state(FAXPP_TokenizerEnv *env)
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
    return NO_ERROR;
  default:
    env->state = conditional_state1;
    // No next_char
    break;
  }
  return NO_ERROR;
}

SINGLE_CHAR_STATE(conditional_state1, 'I', 0, conditional_state2, INVALID_CONDITIONAL_SECTION)

FAXPP_Error
conditional_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case 'N':
    env->state = include_state1;
    break;
  case 'G':
    env->state = ignore_state1;
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_CONDITIONAL_SECTION;
  }
  next_char(env);
  return NO_ERROR;
}

SINGLE_CHAR_STATE(ignore_state1, 'N', 0, ignore_state2, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(ignore_state2, 'O', 0, ignore_state3, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(ignore_state3, 'R', 0, ignore_state4, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(ignore_state4, 'E', ignore_state5, ws_state, INVALID_CONDITIONAL_SECTION)

FAXPP_Error
ignore_state5(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '[':
    env->ignore_start_level = env->nesting_level;
    env->nesting_level += 1;
    env->state = ignore_content_state;
    next_char(env);
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_CONDITIONAL_SECTION;
  }
  return NO_ERROR;
}

FAXPP_Error
ignore_content_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '<':
    env->state = ignore_content_seen_lt_state;
    break;
  case ']':
    env->state = ignore_content_seen_rsquare_state1;
    break;
  LINE_ENDINGS
  default:
    if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
      next_char(env);
      return RESTRICTED_CHAR;
    }
    break;
  }
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
ignore_content_seen_lt_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '!':
    env->state = ignore_content_seen_bang_state;
    break;
  LINE_ENDINGS
  default:
    env->state = ignore_content_state;
    if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
      next_char(env);
      return RESTRICTED_CHAR;
    }
    break;
  }
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
ignore_content_seen_bang_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  env->state = ignore_content_state;

  switch(env->current_char) {
  case '[':
    env->nesting_level += 1;
    break;
  LINE_ENDINGS
  default:
    if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
      next_char(env);
      return RESTRICTED_CHAR;
    }
    break;
  }
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
ignore_content_seen_rsquare_state1(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case ']':
    env->state = ignore_content_seen_rsquare_state2;
    break;
  LINE_ENDINGS
  default:
    env->state = ignore_content_state;
    if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
      next_char(env);
      return RESTRICTED_CHAR;
    }
    break;
  }
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
ignore_content_seen_rsquare_state2(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case ']':
    break;
  case '>':
    env->nesting_level -= 1;
    if(env->nesting_level == env->ignore_start_level) {
      env->nesting_level -= 1;
      base_state(env);
    }
    else {
      env->state = ignore_content_state;
    }
    break;
  LINE_ENDINGS
  default:
    env->state = ignore_content_state;
    if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0) {
      next_char(env);
      return RESTRICTED_CHAR;
    }
    break;
  }
  next_char(env);
  return NO_ERROR;
}

SINGLE_CHAR_STATE(include_state1, 'C', 0, include_state2, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(include_state2, 'L', 0, include_state3, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(include_state3, 'U', 0, include_state4, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(include_state4, 'D', 0, include_state5, INVALID_CONDITIONAL_SECTION)
SINGLE_CHAR_STATE(include_state5, 'E', include_state6, ws_state, INVALID_CONDITIONAL_SECTION)

FAXPP_Error
include_state6(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '[':
    env->nesting_level += 1;
    env->state = external_subset_state;
    next_char(env);
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_CONDITIONAL_SECTION;
  }
  return NO_ERROR;
}

