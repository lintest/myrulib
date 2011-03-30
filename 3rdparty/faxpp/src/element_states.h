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

FAXPP_Error
PREFIX(start_element_name_state)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    END_CHECK;

    READ_CHAR;

    switch(env->current_char) {
    WHITESPACE:
      env->state = PREFIX(start_element_ws_state);
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
 next_char_no_error:
      next_char(env);
      return NO_ERROR;
    case '/':
      env->state = self_closing_element_state;
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
      goto next_char_no_error;
    case '>':
      env->state = start_element_end_state;
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
 next_char_tok_start_no_error:
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case ':':
      env->state = PREFIX(start_element_name_seen_colon_state);
      token_end_position(env);
      report_token(START_ELEMENT_PREFIX_TOKEN, env);
      goto next_char_tok_start_no_error;
    default:
      DEFAULT_CASE;
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0) {
      return INVALID_CHAR_IN_ELEMENT_NAME;
    }
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
PREFIX(start_element_name_seen_colon_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default: 
    env->state = PREFIX(start_element_name_seen_colon_state2);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
PREFIX(start_element_name_seen_colon_state2)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    END_CHECK;

    READ_CHAR;

    switch(env->current_char) {
    WHITESPACE:
      env->state = PREFIX(start_element_ws_state);
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
 next_char_no_error:
      next_char(env);
      return NO_ERROR;
    case '/':
      env->state = self_closing_element_state;
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
      goto next_char_no_error;
    case '>':
      env->state = start_element_end_state;
      token_end_position(env);
      report_token(START_ELEMENT_NAME_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    default:
      DEFAULT_CASE;
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

FAXPP_Error
PREFIX(start_element_mandatory_ws_state)(FAXPP_TokenizerEnv *env)
{
  END_CHECK;

  READ_CHAR;

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    // Fall through
  case '/':
  case '>':
    env->state = PREFIX(start_element_ws_state);
    break;
  default:
    env->state = PREFIX(start_element_ws_state);
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;  
}

FAXPP_Error
PREFIX(start_element_ws_state)(FAXPP_TokenizerEnv *env)
{
  END_CHECK;

  READ_CHAR;

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  case '/':
    env->state = self_closing_element_state;
    next_char(env);
    break;
  case '>':
    env->state = PREFIX(element_content_state);
    env->nesting_level += 1;
    report_empty_token(START_ELEMENT_END_TOKEN, env);
    next_char(env);
    token_start_position(env);
    break;
  case 'x':
    env->state = PREFIX(ns_name_state1);
    token_start_position(env);
    next_char(env);
    break;
  default:
    DEFAULT_CASE;

    env->state = PREFIX(attr_name_state);
    token_start_position(env);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ATTRIBUTE_NAME;
    break;
  }
  return NO_ERROR;  
}

FAXPP_Error
PREFIX(element_content_state)(FAXPP_TokenizerEnv *env)
{
  if((env)->token_buffer.cursor) {

    while(1) {
      END_CHECK_IF {
        if(env->token.value.ptr) {
          token_end_position(env);
          if(env->token.value.len != 0) {
            report_token(CHARACTERS_TOKEN, env);
            return NO_ERROR;
          }
        }
        token_start_position(env);
        return PREMATURE_END_OF_BUFFER;
      }

      READ_CHAR;

      switch(env->current_char) {
      case '<':
        env->state = PREFIX(element_content_markup_state);
        token_end_position(env);
        report_token(CHARACTERS_TOKEN, env);
        goto next_char_no_error;
      case '&':
        store_state(env);
        env->state = reference_state;
        token_end_position(env);
        report_token(CHARACTERS_TOKEN, env);
        next_char(env);
        token_start_position(env);
        return NO_ERROR;
      case ']':
        env->state = PREFIX(element_content_rsquare_state1);
        goto next_char_no_error;
      LINE_ENDINGS_LABEL(0)
        break;

      // 0x0A, 0x0D, 0x26, 0x3C, 0x5D - Done above

      case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
      case 0x08:                       case 0x0B: case 0x0C:            case 0x0E: case 0x0F:
      case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
      case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
        goto restricted_char_error;

      case 0x09:
      case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:            case 0x27:
      case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2E: case 0x2F:
      case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3A: case 0x3B:            case 0x3D: case 0x3E: case 0x3F:
      case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
      case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
      case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
      case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C:            case 0x5E: case 0x5F:
      case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
      case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
      case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
      case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
        // The char is a valid one byte char
        break;

      default:
        DEFAULT_CASE;

        if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0)
          goto restricted_char_error;
        break;
      }

      next_char_append(env);
      next_char_position(env);
    }

  } else {

    while(1) {
      END_CHECK_IF {
        if(env->token.value.ptr) {
          token_end_position(env);
          if(env->token.value.len != 0) {
            report_token(CHARACTERS_TOKEN, env);
            return NO_ERROR;
          }
        }
        token_start_position(env);
        return PREMATURE_END_OF_BUFFER;
      }

      READ_CHAR;

      switch(env->current_char) {
      case '<':
        env->state = PREFIX(element_content_markup_state);
        token_end_position(env);
        report_token(CHARACTERS_TOKEN, env);
        goto next_char_no_error;
      case '&':
        store_state(env);
        env->state = reference_state;
        token_end_position(env);
        report_token(CHARACTERS_TOKEN, env);
        next_char(env);
        token_start_position(env);
        return NO_ERROR;
      case ']':
        env->state = PREFIX(element_content_rsquare_state1);
        goto next_char_no_error;
      LINE_ENDINGS_LABEL(1)
        break;

      // 0x0A, 0x0D, 0x26, 0x3C, 0x5D - Done above

      case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
      case 0x08:                       case 0x0B: case 0x0C:            case 0x0E: case 0x0F:
      case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
      case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
        goto restricted_char_error;

      case 0x09:
      case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:            case 0x27:
      case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2E: case 0x2F:
      case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3A: case 0x3B:            case 0x3D: case 0x3E: case 0x3F:
      case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
      case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
      case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
      case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C:            case 0x5E: case 0x5F:
      case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
      case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
      case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
      case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
        // The char is a valid one byte char
        break;

      default:
        DEFAULT_CASE;

        if((FAXPP_char_flags(env->current_char) & env->non_restricted_char) == 0)
          goto restricted_char_error;
        break;
      }

      next_char_position(env);
    }

  }

restricted_char_error:
  next_char(env);
  return RESTRICTED_CHAR;
next_char_no_error:
  next_char(env);
  return NO_ERROR;
}

FAXPP_Error
PREFIX(element_content_markup_state)(FAXPP_TokenizerEnv *env)
{
  END_CHECK;

  READ_CHAR;

  switch(env->current_char) {
  case '?':
    env->state = pi_name_start_state;
    next_char(env);
    token_start_position(env);
    break;
  case '!':
    env->state = cdata_or_comment_state;
    next_char(env);
    token_start_position(env);
    break;
  case '/':
    env->state = PREFIX(end_element_name_state);
    next_char(env);
    token_start_position(env);
    break;
  LINE_ENDINGS
  default:
    DEFAULT_CASE;

    env->state = PREFIX(start_element_name_state);
    token_start_position(env);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
PREFIX(element_content_rsquare_state1)(FAXPP_TokenizerEnv *env)
{
  END_CHECK_IF {
    if(env->token.value.ptr) {
      token_end_position(env);
      if(env->token.value.len != 0) {
        report_token(CHARACTERS_TOKEN, env);
        return NO_ERROR;
      }
    }
    token_start_position(env);
    return PREMATURE_END_OF_BUFFER;
  }

  READ_CHAR;

  switch(env->current_char) {
  case ']':
    env->state = PREFIX(element_content_rsquare_state2);
    next_char(env);
    break;
  default:
    base_state(env);
    break;
  }

  return NO_ERROR;
}

FAXPP_Error
PREFIX(element_content_rsquare_state2)(FAXPP_TokenizerEnv *env)
{
  END_CHECK_IF {
    if(env->token.value.ptr) {
      token_end_position(env);
      if(env->token.value.len != 0) {
        report_token(CHARACTERS_TOKEN, env);
        return NO_ERROR;
      }
    }
    token_start_position(env);
    return PREMATURE_END_OF_BUFFER;
  }

  READ_CHAR;

  switch(env->current_char) {
  case '>':
    base_state(env);
    next_char(env);
    return CDATA_END_IN_ELEMENT_CONTENT;
  case ']':
    next_char(env);
    break;
  default:
    base_state(env);
    break;;
  }

  return NO_ERROR;
}

FAXPP_Error
PREFIX(end_element_name_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = PREFIX(end_element_name_state2);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
  }
  return NO_ERROR;  
}

FAXPP_Error
PREFIX(end_element_name_state2)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    END_CHECK;

    READ_CHAR;

    switch(env->current_char) {
    WHITESPACE:
      env->state = end_element_ws_state;
      token_end_position(env);
      report_token(END_ELEMENT_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '>':
      env->nesting_level -= 1;
      base_state(env);
      token_end_position(env);
      report_token(END_ELEMENT_NAME_TOKEN, env);
 next_char_tok_start_no_error:
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    case ':':
      env->state = PREFIX(end_element_name_seen_colon_state);
      token_end_position(env);
      report_token(END_ELEMENT_PREFIX_TOKEN, env);
      goto next_char_tok_start_no_error;
    default:
      DEFAULT_CASE;
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0) {
      return INVALID_CHAR_IN_ELEMENT_NAME;
    }
  }

  // Never happens
  return NO_ERROR;
}

FAXPP_Error
PREFIX(end_element_name_seen_colon_state)(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  LINE_ENDINGS
  default:
    env->state = PREFIX(end_element_name_seen_colon_state2);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
    break;
  }

  return NO_ERROR;  
}

FAXPP_Error
PREFIX(end_element_name_seen_colon_state2)(FAXPP_TokenizerEnv *env)
{
  while(1) {
    END_CHECK;

    READ_CHAR;

    switch(env->current_char) {
    WHITESPACE:
      env->state = end_element_ws_state;
      token_end_position(env);
      report_token(END_ELEMENT_NAME_TOKEN, env);
      next_char(env);
      return NO_ERROR;
    case '>':
      env->nesting_level -= 1;
      base_state(env);
      token_end_position(env);
      report_token(END_ELEMENT_NAME_TOKEN, env);
      next_char(env);
      token_start_position(env);
      return NO_ERROR;
    default:
      DEFAULT_CASE;
      break;
    }

    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
  }

  // Never happens
  return NO_ERROR;  
}

