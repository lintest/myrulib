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

FAXPP_Error
start_element_end_state(FAXPP_TokenizerEnv *env)
{
  env->state = (env)->element_content_state;
  env->nesting_level += 1;
  report_empty_token(START_ELEMENT_END_TOKEN, env);
  // no next_char
  return 0;
}

FAXPP_Error
self_closing_element_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '>':
    base_state(env);
    token_end_position(env);
    report_empty_token(SELF_CLOSING_ELEMENT_TOKEN, env);
    next_char(env);
    token_start_position(env);
    break;
  LINE_ENDINGS
  default:
    next_char(env);
    return INVALID_CHAR_IN_START_ELEMENT;
  }
  return NO_ERROR;  
}

FAXPP_Error
end_element_ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '>':
    env->nesting_level -= 1;
    base_state(env);
    next_char(env);
    token_start_position(env);
    break;
  default:
    next_char(env);
    return INVALID_CHAR_IN_END_ELEMENT;
  }
  return NO_ERROR;  
}

