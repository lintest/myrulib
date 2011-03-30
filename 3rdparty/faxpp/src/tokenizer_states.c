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

#ifdef DEBUG
const char *FAXPP_state_to_string(FAXPP_StateFunction state)
{
  if(state == initial_state)
    return "initial_state";
  else if(state == initial_misc_state)
    return "initial_misc_state";
  else if(state == parsed_entity_state)
    return "parsed_entity_state";
  else if(state == initial_markup_state)
    return "initial_markup_state";
  else if(state == end_of_buffer_state)
    return "end_of_buffer_state";
  else if(state == comment_start_state1)
    return "comment_start_state1";
  else if(state == comment_start_state2)
    return "comment_start_state2";
  else if(state == comment_content_state)
    return "comment_content_state";
  else if(state == comment_content_seen_dash_state)
    return "comment_content_seen_dash_state";
  else if(state == comment_content_seen_dash_twice_state)
    return "comment_content_seen_dash_twice_state";
  else if(state == pi_name_start_state)
    return "pi_name_start_state";
  else if(state == pi_name_x_state)
    return "pi_name_x_state";
  else if(state == pi_name_m_state)
    return "pi_name_m_state";
  else if(state == pi_name_l_state)
    return "pi_name_l_state";
  else if(state == pi_name_state)
    return "pi_name_state";
  else if(state == pi_ws_state)
    return "pi_ws_state";
  else if(state == pi_content_state)
    return "pi_content_state";
  else if(state == pi_content_seen_question_state)
    return "pi_content_seen_question_state";

  else if(state == default_start_element_name_state)
    return "default_start_element_name_state";
  else if(state == default_start_element_name_seen_colon_state)
    return "default_start_element_name_seen_colon_state";
  else if(state == default_start_element_name_seen_colon_state2)
    return "default_start_element_name_seen_colon_state2";
  else if(state == default_start_element_mandatory_ws_state)
    return "default_start_element_mandatory_ws_state";
  else if(state == default_start_element_ws_state)
    return "default_start_element_ws_state";
  else if(state == default_element_content_markup_state)
    return "default_element_content_markup_state";
  else if(state == default_attr_name_state)
    return "default_attr_name_state";
  else if(state == default_attr_name_seen_colon_state)
    return "default_attr_name_seen_colon_state";
  else if(state == default_attr_name_seen_colon_state2)
    return "default_attr_name_seen_colon_state2";
  else if(state == default_attr_equals_state)
    return "default_attr_equals_state";
  else if(state == default_attr_value_start_state)
    return "default_attr_value_start_state";
  else if(state == default_attr_value_apos_state)
    return "default_attr_value_apos_state";
  else if(state == default_attr_value_quot_state)
    return "default_attr_value_quot_state";
  else if(state == default_attr_value_state_en)
    return "default_attr_value_state_en";
  else if(state == default_element_content_state)
    return "default_element_content_state";
  else if(state == default_element_content_rsquare_state1)
    return "default_element_content_rsquare_state1";
  else if(state == default_element_content_rsquare_state2)
    return "default_element_content_rsquare_state2";
  else if(state == default_end_element_name_state)
    return "default_end_element_name_state";
  else if(state == default_end_element_name_state2)
    return "default_end_element_name_state2";
  else if(state == default_end_element_name_seen_colon_state)
    return "default_end_element_name_seen_colon_state";
  else if(state == default_end_element_name_seen_colon_state2)
    return "default_end_element_name_seen_colon_state2";
  else if(state == default_ns_name_state1)
    return "default_ns_name_state1";
  else if(state == default_ns_name_state2)
    return "default_ns_name_state2";
  else if(state == default_ns_name_state3)
    return "default_ns_name_state3";
  else if(state == default_ns_name_state4)
    return "default_ns_name_state4";
  else if(state == default_ns_name_state5)
    return "default_ns_name_state5";

  else if(state == utf8_start_element_name_state)
    return "utf8_start_element_name_state";
  else if(state == utf8_start_element_name_seen_colon_state)
    return "utf8_start_element_name_seen_colon_state";
  else if(state == utf8_start_element_name_seen_colon_state2)
    return "utf8_start_element_name_seen_colon_state2";
  else if(state == utf8_start_element_mandatory_ws_state)
    return "utf8_start_element_mandatory_ws_state";
  else if(state == utf8_start_element_ws_state)
    return "utf8_start_element_ws_state";
  else if(state == utf8_attr_name_state)
    return "utf8_attr_name_state";
  else if(state == utf8_attr_name_seen_colon_state)
    return "utf8_attr_name_seen_colon_state";
  else if(state == utf8_attr_name_seen_colon_state2)
    return "utf8_attr_name_seen_colon_state2";
  else if(state == utf8_attr_equals_state)
    return "utf8_attr_equals_state";
  else if(state == utf8_attr_value_start_state)
    return "utf8_attr_value_start_state";
  else if(state == utf8_attr_value_apos_state)
    return "utf8_attr_value_apos_state";
  else if(state == utf8_attr_value_quot_state)
    return "utf8_attr_value_quot_state";
  else if(state == utf8_attr_value_state_en)
    return "utf8_attr_value_state_en";
  else if(state == utf8_element_content_state)
    return "utf8_element_content_state";
  else if(state == utf8_element_content_rsquare_state1)
    return "utf8_element_content_rsquare_state1";
  else if(state == utf8_element_content_rsquare_state2)
    return "utf8_element_content_rsquare_state2";
  else if(state == utf8_element_content_markup_state)
    return "utf8_element_content_markup_state";
  else if(state == utf8_end_element_name_state)
    return "utf8_end_element_name_state";
  else if(state == utf8_end_element_name_state2)
    return "utf8_end_element_name_state2";
  else if(state == utf8_end_element_name_seen_colon_state)
    return "utf8_end_element_name_seen_colon_state";
  else if(state == utf8_end_element_name_seen_colon_state2)
    return "utf8_end_element_name_seen_colon_state2";
  else if(state == utf8_ns_name_state1)
    return "utf8_ns_name_state1";
  else if(state == utf8_ns_name_state2)
    return "utf8_ns_name_state2";
  else if(state == utf8_ns_name_state3)
    return "utf8_ns_name_state3";
  else if(state == utf8_ns_name_state4)
    return "utf8_ns_name_state4";
  else if(state == utf8_ns_name_state5)
    return "utf8_ns_name_state5";

  else if(state == utf16_start_element_name_state)
    return "utf16_start_element_name_state";
  else if(state == utf16_start_element_name_seen_colon_state)
    return "utf16_start_element_name_seen_colon_state";
  else if(state == utf16_start_element_name_seen_colon_state2)
    return "utf16_start_element_name_seen_colon_state2";
  else if(state == utf16_start_element_mandatory_ws_state)
    return "utf16_start_element_mandatory_ws_state";
  else if(state == utf16_start_element_ws_state)
    return "utf16_start_element_ws_state";
  else if(state == utf16_attr_name_state)
    return "utf16_attr_name_state";
  else if(state == utf16_attr_name_seen_colon_state)
    return "utf16_attr_name_seen_colon_state";
  else if(state == utf16_attr_name_seen_colon_state2)
    return "utf16_attr_name_seen_colon_state2";
  else if(state == utf16_attr_equals_state)
    return "utf16_attr_equals_state";
  else if(state == utf16_attr_value_start_state)
    return "utf16_attr_value_start_state";
  else if(state == utf16_attr_value_apos_state)
    return "utf16_attr_value_apos_state";
  else if(state == utf16_attr_value_quot_state)
    return "utf16_attr_value_quot_state";
  else if(state == utf16_attr_value_state_en)
    return "utf16_attr_value_state_en";
  else if(state == utf16_element_content_state)
    return "utf16_element_content_state";
  else if(state == utf16_element_content_rsquare_state1)
    return "utf16_element_content_rsquare_state1";
  else if(state == utf16_element_content_rsquare_state2)
    return "utf16_element_content_rsquare_state2";
  else if(state == utf16_element_content_markup_state)
    return "utf16_element_content_markup_state";
  else if(state == utf16_end_element_name_state)
    return "utf16_end_element_name_state";
  else if(state == utf16_end_element_name_state2)
    return "utf16_end_element_name_state2";
  else if(state == utf16_end_element_name_seen_colon_state)
    return "utf16_end_element_name_seen_colon_state";
  else if(state == utf16_end_element_name_seen_colon_state2)
    return "utf16_end_element_name_seen_colon_state2";
  else if(state == utf16_ns_name_state1)
    return "utf16_ns_name_state1";
  else if(state == utf16_ns_name_state2)
    return "utf16_ns_name_state2";
  else if(state == utf16_ns_name_state3)
    return "utf16_ns_name_state3";
  else if(state == utf16_ns_name_state4)
    return "utf16_ns_name_state4";
  else if(state == utf16_ns_name_state5)
    return "utf16_ns_name_state5";

  else if(state == start_element_end_state)
    return "start_element_end_state";
  else if(state == self_closing_element_state)
    return "self_closing_element_state";
  else if(state == end_element_ws_state)
    return "end_element_ws_state";
  else if(state == reference_state)
    return "reference_state";
  else if(state == a_entity_reference_state)
    return "a_entity_reference_state";
  else if(state == amp_entity_reference_state1)
    return "amp_entity_reference_state1";
  else if(state == amp_entity_reference_state2)
    return "amp_entity_reference_state2";
  else if(state == apos_entity_reference_state1)
    return "apos_entity_reference_state1";
  else if(state == apos_entity_reference_state2)
    return "apos_entity_reference_state2";
  else if(state == apos_entity_reference_state3)
    return "apos_entity_reference_state3";
  else if(state == gt_entity_reference_state1)
    return "gt_entity_reference_state1";
  else if(state == gt_entity_reference_state2)
    return "gt_entity_reference_state2";
  else if(state == lt_entity_reference_state1)
    return "lt_entity_reference_state1";
  else if(state == lt_entity_reference_state2)
    return "lt_entity_reference_state2";
  else if(state == quot_entity_reference_state1)
    return "quot_entity_reference_state1";
  else if(state == quot_entity_reference_state2)
    return "quot_entity_reference_state2";
  else if(state == quot_entity_reference_state3)
    return "quot_entity_reference_state3";
  else if(state == quot_entity_reference_state4)
    return "quot_entity_reference_state4";
  else if(state == entity_reference_state)
    return "entity_reference_state";
  else if(state == parameter_entity_reference_state)
    return "parameter_entity_reference_state";
  else if(state == parameter_entity_reference_state2)
    return "parameter_entity_reference_state2";
  else if(state == parameter_entity_reference_in_markup_state)
    return "parameter_entity_reference_in_markup_state";
  else if(state == parameter_entity_reference_in_markup_state2)
    return "parameter_entity_reference_in_markup_state2";
  else if(state == char_reference_state)
    return "char_reference_state";
  else if(state == dec_char_reference_state)
    return "dec_char_reference_state";
  else if(state == hex_char_reference_state1)
    return "hex_char_reference_state1";
  else if(state == hex_char_reference_state2)
    return "hex_char_reference_state2";
  else if(state == cdata_or_comment_state)
    return "cdata_or_comment_state";
  else if(state == cdata_start_state1)
    return "cdata_start_state1";
  else if(state == cdata_start_state2)
    return "cdata_start_state2";
  else if(state == cdata_start_state3)
    return "cdata_start_state3";
  else if(state == cdata_start_state4)
    return "cdata_start_state4";
  else if(state == cdata_start_state5)
    return "cdata_start_state5";
  else if(state == cdata_start_state6)
    return "cdata_start_state6";
  else if(state == cdata_content_state)
    return "cdata_content_state";
  else if(state == cdata_end_state1)
    return "cdata_end_state1";
  else if(state == cdata_end_state2)
    return "cdata_end_state2";
  else if(state == ws_plus_state)
    return "ws_plus_state";
  else if(state == ws_state)
    return "ws_state";
  else if(state == equals_state)
    return "equals_state";
  else if(state == xml_decl_or_markup_state)
    return "xml_decl_or_markup_state";
  else if(state == xml_decl_or_pi_state1)
    return "xml_decl_or_pi_state1";
  else if(state == xml_decl_or_pi_state2)
    return "xml_decl_or_pi_state2";
  else if(state == xml_decl_or_pi_state3)
    return "xml_decl_or_pi_state3";
  else if(state == xml_decl_or_pi_state4)
    return "xml_decl_or_pi_state4";
  else if(state == xml_decl_version_state1)
    return "xml_decl_version_state1";
  else if(state == xml_decl_version_state2)
    return "xml_decl_version_state2";
  else if(state == xml_decl_version_state3)
    return "xml_decl_version_state3";
  else if(state == xml_decl_version_state4)
    return "xml_decl_version_state4";
  else if(state == xml_decl_version_state5)
    return "xml_decl_version_state5";
  else if(state == xml_decl_version_state6)
    return "xml_decl_version_state6";
  else if(state == xml_decl_version_state7)
    return "xml_decl_version_state7";
  else if(state == xml_decl_version_value_state1)
    return "xml_decl_version_value_state1";
  else if(state == xml_decl_version_value_state2)
    return "xml_decl_version_value_state2";
  else if(state == xml_decl_version_value_state3)
    return "xml_decl_version_value_state3";
  else if(state == xml_decl_version_value_state4)
    return "xml_decl_version_value_state4";
  else if(state == xml_decl_version_value_quot_state)
    return "xml_decl_version_value_quot_state";
  else if(state == xml_decl_version_value_apos_state)
    return "xml_decl_version_value_apos_state";
  else if(state == xml_decl_encoding_ws_state)
    return "xml_decl_encoding_ws_state";
  else if(state == xml_decl_encoding_state1)
    return "xml_decl_encoding_state1";
  else if(state == xml_decl_encoding_state2)
    return "xml_decl_encoding_state2";
  else if(state == xml_decl_encoding_state3)
    return "xml_decl_encoding_state3";
  else if(state == xml_decl_encoding_state4)
    return "xml_decl_encoding_state4";
  else if(state == xml_decl_encoding_state5)
    return "xml_decl_encoding_state5";
  else if(state == xml_decl_encoding_state6)
    return "xml_decl_encoding_state6";
  else if(state == xml_decl_encoding_state7)
    return "xml_decl_encoding_state7";
  else if(state == xml_decl_encoding_state8)
    return "xml_decl_encoding_state8";
  else if(state == xml_decl_encoding_value_state)
    return "xml_decl_encoding_value_state";
  else if(state == xml_decl_encoding_value_quot_state1)
    return "xml_decl_encoding_value_quot_state1";
  else if(state == xml_decl_encoding_value_quot_state2)
    return "xml_decl_encoding_value_quot_state2";
  else if(state == xml_decl_encoding_value_apos_state1)
    return "xml_decl_encoding_value_apos_state1";
  else if(state == xml_decl_encoding_value_apos_state2)
    return "xml_decl_encoding_value_apos_state2";
  else if(state == xml_decl_standalone_ws_state)
    return "xml_decl_standalone_ws_state";
  else if(state == xml_decl_seen_question_state)
    return "xml_decl_seen_question_state";
  else if(state == xml_decl_standalone_state1)
    return "xml_decl_standalone_state1";
  else if(state == xml_decl_standalone_state2)
    return "xml_decl_standalone_state2";
  else if(state == xml_decl_standalone_state3)
    return "xml_decl_standalone_state3";
  else if(state == xml_decl_standalone_state4)
    return "xml_decl_standalone_state4";
  else if(state == xml_decl_standalone_state5)
    return "xml_decl_standalone_state5";
  else if(state == xml_decl_standalone_state6)
    return "xml_decl_standalone_state6";
  else if(state == xml_decl_standalone_state7)
    return "xml_decl_standalone_state7";
  else if(state == xml_decl_standalone_state8)
    return "xml_decl_standalone_state8";
  else if(state == xml_decl_standalone_state9)
    return "xml_decl_standalone_state9";
  else if(state == xml_decl_standalone_state10)
    return "xml_decl_standalone_state10";
  else if(state == xml_decl_standalone_value_state1)
    return "xml_decl_standalone_value_state1";
  else if(state == xml_decl_standalone_value_state2)
    return "xml_decl_standalone_value_state2";
  else if(state == xml_decl_standalone_yes_state1)
    return "xml_decl_standalone_yes_state1";
  else if(state == xml_decl_standalone_yes_state2)
    return "xml_decl_standalone_yes_state2";
  else if(state == xml_decl_standalone_no_state)
    return "xml_decl_standalone_no_state";
  else if(state == xml_decl_standalone_value_quot_state)
    return "xml_decl_standalone_value_quot_state";
  else if(state == xml_decl_standalone_value_apos_state)
    return "xml_decl_standalone_value_apos_state";
  else if(state == xml_decl_question_state)
    return "xml_decl_question_state";

  else if(state == doctype_or_comment_state)
    return "doctype_or_comment_state";
  else if(state == doctype_initial_state1)
    return "doctype_initial_state1";
  else if(state == doctype_initial_state2)
    return "doctype_initial_state2";
  else if(state == doctype_initial_state3)
    return "doctype_initial_state3";
  else if(state == doctype_initial_state4)
    return "doctype_initial_state4";
  else if(state == doctype_initial_state5)
    return "doctype_initial_state5";
  else if(state == doctype_initial_state6)
    return "doctype_initial_state6";
  else if(state == doctype_name_state1)
    return "doctype_name_state1";
  else if(state == doctype_name_state2)
    return "doctype_name_state2";
  else if(state == doctype_name_seen_colon_state1)
    return "doctype_name_seen_colon_state1";
  else if(state == doctype_name_seen_colon_state2)
    return "doctype_name_seen_colon_state2";
  else if(state == doctype_after_name_state)
    return "doctype_after_name_state";
  else if(state == doctype_internal_subset_start_state)
    return "doctype_internal_subset_start_state";
  else if(state == doctype_end_state)
    return "doctype_end_state";

  else if(state == internal_subset_state)
    return "internal_subset_state";
  else if(state == internal_subset_state_en)
    return "internal_subset_state_en";
  else if(state == internal_subset_markup_state)
    return "internal_subset_markup_state";
  else if(state == internal_subset_decl_state)
    return "internal_subset_decl_state";

  else if(state == external_subset_state)
    return "external_subset_state";
  else if(state == external_subset_seen_rsquare_state1)
    return "external_subset_seen_rsquare_state1";
  else if(state == external_subset_seen_rsquare_state2)
    return "external_subset_seen_rsquare_state2";
  else if(state == external_subset_markup_state)
    return "external_subset_markup_state";
  else if(state == external_subset_decl_state)
    return "external_subset_decl_state";

  else if(state == doctype_system_id_initial_state1)
    return "doctype_system_id_initial_state1";
  else if(state == doctype_system_id_initial_state2)
    return "doctype_system_id_initial_state2";
  else if(state == doctype_system_id_initial_state3)
    return "doctype_system_id_initial_state3";
  else if(state == doctype_system_id_initial_state4)
    return "doctype_system_id_initial_state4";
  else if(state == doctype_system_id_initial_state5)
    return "doctype_system_id_initial_state5";
  else if(state == doctype_system_id_ws_state)
    return "doctype_system_id_ws_state";
  else if(state == doctype_system_literal_start_state)
    return "doctype_system_literal_start_state";
  else if(state == doctype_system_literal_apos_state)
    return "doctype_system_literal_apos_state";
  else if(state == doctype_system_literal_quot_state)
    return "doctype_system_literal_quot_state";

  else if(state == doctype_public_id_initial_state1)
    return "doctype_public_id_initial_state1";
  else if(state == doctype_public_id_initial_state2)
    return "doctype_public_id_initial_state2";
  else if(state == doctype_public_id_initial_state3)
    return "doctype_public_id_initial_state3";
  else if(state == doctype_public_id_initial_state4)
    return "doctype_public_id_initial_state4";
  else if(state == doctype_public_id_initial_state5)
    return "doctype_public_id_initial_state5";
  else if(state == doctype_public_id_ws_state)
    return "doctype_public_id_ws_state";
  else if(state == doctype_pubid_literal_start_state)
    return "doctype_pubid_literal_start_state";
  else if(state == doctype_pubid_literal_apos_state)
    return "doctype_pubid_literal_apos_state";
  else if(state == doctype_pubid_literal_quot_state)
    return "doctype_pubid_literal_quot_state";
  else if(state == doctype_public_id_ws_state2)
    return "doctype_public_id_ws_state2";
  else if(state == doctype_public_id_ws_state3)
    return "doctype_public_id_ws_state3";

  else if(state == elementdecl_or_entitydecl_state)
    return "elementdecl_or_entitydecl_state";
  else if(state == elementdecl_initial_state1)
    return "elementdecl_initial_state1";
  else if(state == elementdecl_initial_state2)
    return "elementdecl_initial_state2";
  else if(state == elementdecl_initial_state3)
    return "elementdecl_initial_state3";
  else if(state == elementdecl_initial_state4)
    return "elementdecl_initial_state4";
  else if(state == elementdecl_initial_state5)
    return "elementdecl_initial_state5";
  else if(state == elementdecl_name_ws_state1)
    return "elementdecl_name_ws_state1";
  else if(state == elementdecl_name_ws_state2)
    return "elementdecl_name_ws_state2";
  else if(state == elementdecl_name_state1)
    return "elementdecl_name_state1";
  else if(state == elementdecl_name_state2)
    return "elementdecl_name_state2";
  else if(state == elementdecl_name_seen_colon_state1)
    return "elementdecl_name_seen_colon_state1";
  else if(state == elementdecl_name_seen_colon_state2)
    return "elementdecl_name_seen_colon_state2";
  else if(state == elementdecl_content_ws_state1)
    return "elementdecl_content_ws_state1";
  else if(state == elementdecl_content_ws_state2)
    return "elementdecl_content_ws_state2";
  else if(state == elementdecl_content_state)
    return "elementdecl_content_state";
  else if(state == elementdecl_empty_state1)
    return "elementdecl_empty_state1";
  else if(state == elementdecl_empty_state2)
    return "elementdecl_empty_state2";
  else if(state == elementdecl_empty_state3)
    return "elementdecl_empty_state3";
  else if(state == elementdecl_empty_state4)
    return "elementdecl_empty_state4";
  else if(state == elementdecl_any_state1)
    return "elementdecl_any_state1";
  else if(state == elementdecl_any_state2)
    return "elementdecl_any_state2";
  else if(state == elementdecl_mixed_or_children_ws_state)
    return "elementdecl_mixed_or_children_ws_state";
  else if(state == elementdecl_mixed_or_children_state)
    return "elementdecl_mixed_or_children_state";
  else if(state == elementdecl_cp_name_ws_state)
    return "elementdecl_cp_name_ws_state";
  else if(state == elementdecl_cp_name_state1)
    return "elementdecl_cp_name_state1";
  else if(state == elementdecl_cp_name_state2)
    return "elementdecl_cp_name_state2";
  else if(state == elementdecl_cp_name_seen_colon_state1)
    return "elementdecl_cp_name_seen_colon_state1";
  else if(state == elementdecl_cp_name_seen_colon_state2)
    return "elementdecl_cp_name_seen_colon_state2";
  else if(state == elementdecl_cp_cardinality_state)
    return "elementdecl_cp_cardinality_state";
  else if(state == elementdecl_cp_separator_or_end_ws_state)
    return "elementdecl_cp_separator_or_end_ws_state";
  else if(state == elementdecl_cp_separator_or_end_state)
    return "elementdecl_cp_separator_or_end_state";
  else if(state == elementdecl_pcdata_state1)
    return "elementdecl_pcdata_state1";
  else if(state == elementdecl_pcdata_state2)
    return "elementdecl_pcdata_state2";
  else if(state == elementdecl_pcdata_state3)
    return "elementdecl_pcdata_state3";
  else if(state == elementdecl_pcdata_state4)
    return "elementdecl_pcdata_state4";
  else if(state == elementdecl_pcdata_state5)
    return "elementdecl_pcdata_state5";
  else if(state == elementdecl_pcdata_state6)
    return "elementdecl_pcdata_state6";
  else if(state == elementdecl_pcdata_end_or_names_ws_state1)
    return "elementdecl_pcdata_end_or_names_ws_state1";
  else if(state == elementdecl_pcdata_end_or_names_state1)
    return "elementdecl_pcdata_end_or_names_state1";
  else if(state == elementdecl_pcdata_optional_star_state)
    return "elementdecl_pcdata_optional_star_state";
  else if(state == elementdecl_pcdata_end_or_names_ws_state2)
    return "elementdecl_pcdata_end_or_names_ws_state2";
  else if(state == elementdecl_pcdata_end_or_names_state2)
    return "elementdecl_pcdata_end_or_names_state2";
  else if(state == elementdecl_pcdata_star_state)
    return "elementdecl_pcdata_star_state";
  else if(state == elementdecl_pcdata_name_ws_state)
    return "elementdecl_pcdata_name_ws_state";
  else if(state == elementdecl_pcdata_name_state1)
    return "elementdecl_pcdata_name_state1";
  else if(state == elementdecl_pcdata_name_state2)
    return "elementdecl_pcdata_name_state2";
  else if(state == elementdecl_pcdata_name_seen_colon_state1)
    return "elementdecl_pcdata_name_seen_colon_state1";
  else if(state == elementdecl_pcdata_name_seen_colon_state2)
    return "elementdecl_pcdata_name_seen_colon_state2";
  else if(state == elementdecl_end_ws_state)
    return "elementdecl_end_ws_state";
  else if(state == elementdecl_end_state)
    return "elementdecl_end_state";

  else if(state == attlistdecl_initial_state1)
    return "attlistdecl_initial_state1";
  else if(state == attlistdecl_initial_state1)
    return "attlistdecl_initial_state1";
  else if(state == attlistdecl_initial_state2)
    return "attlistdecl_initial_state2";
  else if(state == attlistdecl_initial_state3)
    return "attlistdecl_initial_state3";
  else if(state == attlistdecl_initial_state4)
    return "attlistdecl_initial_state4";
  else if(state == attlistdecl_initial_state5)
    return "attlistdecl_initial_state5";
  else if(state == attlistdecl_initial_state6)
    return "attlistdecl_initial_state6";
  else if(state == attlistdecl_name_ws_state1)
    return "attlistdecl_name_ws_state1";
  else if(state == attlistdecl_name_ws_state2)
    return "attlistdecl_name_ws_state2";
  else if(state == attlistdecl_name_state1)
    return "attlistdecl_name_state1";
  else if(state == attlistdecl_name_state2)
    return "attlistdecl_name_state2";
  else if(state == attlistdecl_name_seen_colon_state1)
    return "attlistdecl_name_seen_colon_state1";
  else if(state == attlistdecl_name_seen_colon_state2)
    return "attlistdecl_name_seen_colon_state2";
  else if(state == attlistdecl_attdef_name_ws_state1)
    return "attlistdecl_attdef_name_ws_state1";
  else if(state == attlistdecl_attdef_name_ws_state2)
    return "attlistdecl_attdef_name_ws_state2";
  else if(state == attlistdecl_attdef_name_state1)
    return "attlistdecl_attdef_name_state1";
  else if(state == attlistdecl_attdef_name_state2)
    return "attlistdecl_attdef_name_state2";
  else if(state == attlistdecl_attdef_name_seen_colon_state1)
    return "attlistdecl_attdef_name_seen_colon_state1";
  else if(state == attlistdecl_attdef_name_seen_colon_state2)
    return "attlistdecl_attdef_name_seen_colon_state2";

  else if(state == attlistdecl_atttype_ws_state1)
    return "attlistdecl_atttype_ws_state1";
  else if(state == attlistdecl_atttype_ws_state2)
    return "attlistdecl_atttype_ws_state2";
  else if(state == attlistdecl_atttype_state)
    return "attlistdecl_atttype_state";
  else if(state == attlistdecl_atttype_cdata_state1)
    return "attlistdecl_atttype_cdata_state1";
  else if(state == attlistdecl_atttype_cdata_state2)
    return "attlistdecl_atttype_cdata_state2";
  else if(state == attlistdecl_atttype_cdata_state3)
    return "attlistdecl_atttype_cdata_state3";
  else if(state == attlistdecl_atttype_cdata_state4)
    return "attlistdecl_atttype_cdata_state4";
  else if(state == attlistdecl_atttype_id_state1)
    return "attlistdecl_atttype_id_state1";
  else if(state == attlistdecl_atttype_id_state2)
    return "attlistdecl_atttype_id_state2";
  else if(state == attlistdecl_atttype_idref_state1)
    return "attlistdecl_atttype_idref_state1";
  else if(state == attlistdecl_atttype_idref_state2)
    return "attlistdecl_atttype_idref_state2";
  else if(state == attlistdecl_atttype_idref_state3)
    return "attlistdecl_atttype_idref_state3";
  else if(state == attlistdecl_atttype_entity_state1)
    return "attlistdecl_atttype_entity_state1";
  else if(state == attlistdecl_atttype_entity_state2)
    return "attlistdecl_atttype_entity_state2";
  else if(state == attlistdecl_atttype_entity_state3)
    return "attlistdecl_atttype_entity_state3";
  else if(state == attlistdecl_atttype_entity_state4)
    return "attlistdecl_atttype_entity_state4";
  else if(state == attlistdecl_atttype_entity_state5)
    return "attlistdecl_atttype_entity_state5";
  else if(state == attlistdecl_atttype_entities_state1)
    return "attlistdecl_atttype_entities_state1";
  else if(state == attlistdecl_atttype_entities_state2)
    return "attlistdecl_atttype_entities_state2";
  else if(state == attlistdecl_atttype_nmtoken_state1)
    return "attlistdecl_atttype_nmtoken_state1";
  else if(state == attlistdecl_atttype_nmtoken_state2)
    return "attlistdecl_atttype_nmtoken_state2";
  else if(state == attlistdecl_atttype_nmtoken_state3)
    return "attlistdecl_atttype_nmtoken_state3";
  else if(state == attlistdecl_atttype_nmtoken_state4)
    return "attlistdecl_atttype_nmtoken_state4";
  else if(state == attlistdecl_atttype_nmtoken_state5)
    return "attlistdecl_atttype_nmtoken_state5";
  else if(state == attlistdecl_atttype_nmtoken_state6)
    return "attlistdecl_atttype_nmtoken_state6";
  else if(state == attlistdecl_atttype_nmtoken_state7)
    return "attlistdecl_atttype_nmtoken_state7";
  else if(state == attlistdecl_atttype_notation_state1)
    return "attlistdecl_atttype_notation_state1";
  else if(state == attlistdecl_atttype_notation_state2)
    return "attlistdecl_atttype_notation_state2";
  else if(state == attlistdecl_atttype_notation_state3)
    return "attlistdecl_atttype_notation_state3";
  else if(state == attlistdecl_atttype_notation_state4)
    return "attlistdecl_atttype_notation_state4";
  else if(state == attlistdecl_atttype_notation_state5)
    return "attlistdecl_atttype_notation_state5";
  else if(state == attlistdecl_atttype_notation_state6)
    return "attlistdecl_atttype_notation_state6";
  else if(state == attlistdecl_atttype_notation_ws_state1)
    return "attlistdecl_atttype_notation_ws_state1";
  else if(state == attlistdecl_atttype_notation_ws_state2)
    return "attlistdecl_atttype_notation_ws_state2";
  else if(state == attlistdecl_atttype_notation_lpar_state)
    return "attlistdecl_atttype_notation_lpar_state";
  else if(state == attlistdecl_atttype_notation_name_ws_state)
    return "attlistdecl_atttype_notation_name_ws_state";
  else if(state == attlistdecl_atttype_notation_name_state1)
    return "attlistdecl_atttype_notation_name_state1";
  else if(state == attlistdecl_atttype_notation_name_state2)
    return "attlistdecl_atttype_notation_name_state2";
  else if(state == attlistdecl_atttype_notation_separator_ws_state)
    return "attlistdecl_atttype_notation_separator_ws_state";
  else if(state == attlistdecl_atttype_notation_separator_state)
    return "attlistdecl_atttype_notation_separator_state";
  else if(state == attlistdecl_atttype_enumeration_name_ws_state)
    return "attlistdecl_atttype_enumeration_name_ws_state";
  else if(state == attlistdecl_atttype_enumeration_name_state1)
    return "attlistdecl_atttype_enumeration_name_state1";
  else if(state == attlistdecl_atttype_enumeration_name_state2)
    return "attlistdecl_atttype_enumeration_name_state2";
  else if(state == attlistdecl_atttype_enumeration_separator_ws_state)
    return "attlistdecl_atttype_enumeration_separator_ws_state";
  else if(state == attlistdecl_atttype_enumeration_separator_state)
    return "attlistdecl_atttype_enumeration_separator_state";

  else if(state == attlistdecl_default_ws_state1)
    return "attlistdecl_default_ws_state1";
  else if(state == attlistdecl_default_ws_state2)
    return "attlistdecl_default_ws_state2";
  else if(state == attlistdecl_default_state1)
    return "attlistdecl_default_state1";
  else if(state == attlistdecl_default_state2)
    return "attlistdecl_default_state2";
  else if(state == attlistdecl_default_implied_state1)
    return "attlistdecl_default_implied_state1";
  else if(state == attlistdecl_default_implied_state2)
    return "attlistdecl_default_implied_state2";
  else if(state == attlistdecl_default_implied_state3)
    return "attlistdecl_default_implied_state3";
  else if(state == attlistdecl_default_implied_state4)
    return "attlistdecl_default_implied_state4";
  else if(state == attlistdecl_default_implied_state5)
    return "attlistdecl_default_implied_state5";
  else if(state == attlistdecl_default_implied_state6)
    return "attlistdecl_default_implied_state6";
  else if(state == attlistdecl_default_required_state1)
    return "attlistdecl_default_required_state1";
  else if(state == attlistdecl_default_required_state2)
    return "attlistdecl_default_required_state2";
  else if(state == attlistdecl_default_required_state3)
    return "attlistdecl_default_required_state3";
  else if(state == attlistdecl_default_required_state4)
    return "attlistdecl_default_required_state4";
  else if(state == attlistdecl_default_required_state5)
    return "attlistdecl_default_required_state5";
  else if(state == attlistdecl_default_required_state6)
    return "attlistdecl_default_required_state6";
  else if(state == attlistdecl_default_required_state7)
    return "attlistdecl_default_required_state7";
  else if(state == attlistdecl_default_fixed_state1)
    return "attlistdecl_default_fixed_state1";
  else if(state == attlistdecl_default_fixed_state2)
    return "attlistdecl_default_fixed_state2";
  else if(state == attlistdecl_default_fixed_state3)
    return "attlistdecl_default_fixed_state3";
  else if(state == attlistdecl_default_fixed_state4)
    return "attlistdecl_default_fixed_state4";
  else if(state == attlistdecl_attvalue_start_state)
    return "attlistdecl_attvalue_start_state";
  else if(state == attlistdecl_attvalue_apos_state)
    return "attlistdecl_attvalue_apos_state";
  else if(state == attlistdecl_attvalue_quot_state)
    return "attlistdecl_attvalue_quot_state";

  else if(state == notationdecl_initial_state1)
    return "notationdecl_initial_state1";
  else if(state == notationdecl_initial_state1)
    return "notationdecl_initial_state1";
  else if(state == notationdecl_initial_state2)
    return "notationdecl_initial_state2";
  else if(state == notationdecl_initial_state3)
    return "notationdecl_initial_state3";
  else if(state == notationdecl_initial_state4)
    return "notationdecl_initial_state4";
  else if(state == notationdecl_initial_state5)
    return "notationdecl_initial_state5";
  else if(state == notationdecl_initial_state6)
    return "notationdecl_initial_state6";
  else if(state == notationdecl_initial_state7)
    return "notationdecl_initial_state7";
  else if(state == notationdecl_name_state1)
    return "notationdecl_name_state1";
  else if(state == notationdecl_name_state2)
    return "notationdecl_name_state2";
  else if(state == notationdecl_content_state)
    return "notationdecl_content_state";
  else if(state == notationdecl_end_state)
    return "notationdecl_end_state";

  else if(state == notationdecl_system_id_initial_state1)
    return "notationdecl_system_id_initial_state1";
  else if(state == notationdecl_system_id_initial_state2)
    return "notationdecl_system_id_initial_state2";
  else if(state == notationdecl_system_id_initial_state3)
    return "notationdecl_system_id_initial_state3";
  else if(state == notationdecl_system_id_initial_state4)
    return "notationdecl_system_id_initial_state4";
  else if(state == notationdecl_system_id_initial_state5)
    return "notationdecl_system_id_initial_state5";
  else if(state == notationdecl_system_id_ws_state)
    return "notationdecl_system_id_ws_state";
  else if(state == notationdecl_system_literal_start_state)
    return "notationdecl_system_literal_start_state";
  else if(state == notationdecl_system_literal_apos_state)
    return "notationdecl_system_literal_apos_state";
  else if(state == notationdecl_system_literal_quot_state)
    return "notationdecl_system_literal_quot_state";
  else if(state == notationdecl_public_id_initial_state1)
    return "notationdecl_public_id_initial_state1";
  else if(state == notationdecl_public_id_initial_state2)
    return "notationdecl_public_id_initial_state2";
  else if(state == notationdecl_public_id_initial_state3)
    return "notationdecl_public_id_initial_state3";
  else if(state == notationdecl_public_id_initial_state4)
    return "notationdecl_public_id_initial_state4";
  else if(state == notationdecl_public_id_initial_state5)
    return "notationdecl_public_id_initial_state5";
  else if(state == notationdecl_public_id_ws_state)
    return "notationdecl_public_id_ws_state";
  else if(state == notationdecl_pubid_literal_start_state)
    return "notationdecl_pubid_literal_start_state";
  else if(state == notationdecl_pubid_literal_apos_state)
    return "notationdecl_pubid_literal_apos_state";
  else if(state == notationdecl_pubid_literal_quot_state)
    return "notationdecl_pubid_literal_quot_state";
  else if(state == notationdecl_public_id_ws_state2)
    return "notationdecl_public_id_ws_state2";
  else if(state == notationdecl_public_id_ws_state3)
    return "notationdecl_public_id_ws_state3";

  else if(state == entitydecl_initial_state1)
    return "entitydecl_initial_state1";
  else if(state == entitydecl_initial_state1)
    return "entitydecl_initial_state1";
  else if(state == entitydecl_initial_state2)
    return "entitydecl_initial_state2";
  else if(state == entitydecl_initial_state3)
    return "entitydecl_initial_state3";
  else if(state == entitydecl_initial_state4)
    return "entitydecl_initial_state4";
  else if(state == entitydecl_param_or_general_state)
    return "entitydecl_param_or_general_state";

  else if(state == entitydecl_name_state)
    return "entitydecl_name_state";
  else if(state == entitydecl_content_state)
    return "entitydecl_content_state";
  else if(state == entitydecl_value_apos_state)
    return "entitydecl_value_apos_state";
  else if(state == entitydecl_value_quot_state)
    return "entitydecl_value_quot_state";
  else if(state == entitydecl_ws_state)
    return "entitydecl_ws_state";
  else if(state == entitydecl_ndata_or_end_state)
    return "entitydecl_ndata_or_end_state";
  else if(state == entitydecl_ndata_state1)
    return "entitydecl_ndata_state1";
  else if(state == entitydecl_ndata_state2)
    return "entitydecl_ndata_state2";
  else if(state == entitydecl_ndata_state3)
    return "entitydecl_ndata_state3";
  else if(state == entitydecl_ndata_state4)
    return "entitydecl_ndata_state4";
  else if(state == entitydecl_ndata_name_state1)
    return "entitydecl_ndata_name_state1";
  else if(state == entitydecl_ndata_name_state2)
    return "entitydecl_ndata_name_state2";
  else if(state == entitydecl_end_state)
    return "entitydecl_end_state";

  else if(state == entitydecl_system_id_initial_state1)
    return "entitydecl_system_id_initial_state1";
  else if(state == entitydecl_system_id_initial_state2)
    return "entitydecl_system_id_initial_state2";
  else if(state == entitydecl_system_id_initial_state3)
    return "entitydecl_system_id_initial_state3";
  else if(state == entitydecl_system_id_initial_state4)
    return "entitydecl_system_id_initial_state4";
  else if(state == entitydecl_system_id_initial_state5)
    return "entitydecl_system_id_initial_state5";
  else if(state == entitydecl_system_id_ws_state)
    return "entitydecl_system_id_ws_state";
  else if(state == entitydecl_system_literal_start_state)
    return "entitydecl_system_literal_start_state";
  else if(state == entitydecl_system_literal_apos_state)
    return "entitydecl_system_literal_apos_state";
  else if(state == entitydecl_system_literal_quot_state)
    return "entitydecl_system_literal_quot_state";
  else if(state == entitydecl_public_id_initial_state1)
    return "entitydecl_public_id_initial_state1";
  else if(state == entitydecl_public_id_initial_state2)
    return "entitydecl_public_id_initial_state2";
  else if(state == entitydecl_public_id_initial_state3)
    return "entitydecl_public_id_initial_state3";
  else if(state == entitydecl_public_id_initial_state4)
    return "entitydecl_public_id_initial_state4";
  else if(state == entitydecl_public_id_initial_state5)
    return "entitydecl_public_id_initial_state5";
  else if(state == entitydecl_public_id_ws_state)
    return "entitydecl_public_id_ws_state";
  else if(state == entitydecl_pubid_literal_start_state)
    return "entitydecl_pubid_literal_start_state";
  else if(state == entitydecl_pubid_literal_apos_state)
    return "entitydecl_pubid_literal_apos_state";
  else if(state == entitydecl_pubid_literal_quot_state)
    return "entitydecl_pubid_literal_quot_state";
  else if(state == entitydecl_public_id_ws_state2)
    return "entitydecl_public_id_ws_state2";
  else if(state == entitydecl_public_id_ws_state3)
    return "entitydecl_public_id_ws_state3";

  else if(state == paramentitydecl_name_state1)
    return "paramentitydecl_name_state1";
  else if(state == paramentitydecl_name_state2)
    return "paramentitydecl_name_state2";
  else if(state == paramentitydecl_content_state)
    return "paramentitydecl_content_state";
  else if(state == paramentitydecl_value_apos_state)
    return "paramentitydecl_value_apos_state";
  else if(state == paramentitydecl_value_quot_state)
    return "paramentitydecl_value_quot_state";
  else if(state == paramentitydecl_end_state)
    return "paramentitydecl_end_state";

  else if(state == paramentitydecl_system_id_initial_state1)
    return "paramentitydecl_system_id_initial_state1";
  else if(state == paramentitydecl_system_id_initial_state2)
    return "paramentitydecl_system_id_initial_state2";
  else if(state == paramentitydecl_system_id_initial_state3)
    return "paramentitydecl_system_id_initial_state3";
  else if(state == paramentitydecl_system_id_initial_state4)
    return "paramentitydecl_system_id_initial_state4";
  else if(state == paramentitydecl_system_id_initial_state5)
    return "paramentitydecl_system_id_initial_state5";
  else if(state == paramentitydecl_system_id_ws_state)
    return "paramentitydecl_system_id_ws_state";
  else if(state == paramentitydecl_system_literal_start_state)
    return "paramentitydecl_system_literal_start_state";
  else if(state == paramentitydecl_system_literal_apos_state)
    return "paramentitydecl_system_literal_apos_state";
  else if(state == paramentitydecl_system_literal_quot_state)
    return "paramentitydecl_system_literal_quot_state";
  else if(state == paramentitydecl_public_id_initial_state1)
    return "paramentitydecl_public_id_initial_state1";
  else if(state == paramentitydecl_public_id_initial_state2)
    return "paramentitydecl_public_id_initial_state2";
  else if(state == paramentitydecl_public_id_initial_state3)
    return "paramentitydecl_public_id_initial_state3";
  else if(state == paramentitydecl_public_id_initial_state4)
    return "paramentitydecl_public_id_initial_state4";
  else if(state == paramentitydecl_public_id_initial_state5)
    return "paramentitydecl_public_id_initial_state5";
  else if(state == paramentitydecl_public_id_ws_state)
    return "paramentitydecl_public_id_ws_state";
  else if(state == paramentitydecl_pubid_literal_start_state)
    return "paramentitydecl_pubid_literal_start_state";
  else if(state == paramentitydecl_pubid_literal_apos_state)
    return "paramentitydecl_pubid_literal_apos_state";
  else if(state == paramentitydecl_pubid_literal_quot_state)
    return "paramentitydecl_pubid_literal_quot_state";
  else if(state == paramentitydecl_public_id_ws_state2)
    return "paramentitydecl_public_id_ws_state2";
  else if(state == paramentitydecl_public_id_ws_state3)
    return "paramentitydecl_public_id_ws_state3";

  else if(state == conditional_ws_state)
    return "conditional_ws_state";
  else if(state == conditional_state1)
    return "conditional_state1";
  else if(state == conditional_state2)
    return "conditional_state2";
  else if(state == ignore_state1)
    return "ignore_state1";
  else if(state == ignore_state2)
    return "ignore_state2";
  else if(state == ignore_state3)
    return "ignore_state3";
  else if(state == ignore_state4)
    return "ignore_state4";
  else if(state == ignore_state5)
    return "ignore_state5";
  else if(state == ignore_content_state)
    return "ignore_content_state";
  else if(state == ignore_content_seen_lt_state)
    return "ignore_content_seen_lt_state";
  else if(state == ignore_content_seen_bang_state)
    return "ignore_content_seen_bang_state";
  else if(state == ignore_content_seen_rsquare_state1)
    return "ignore_content_seen_rsquare_state1";
  else if(state == ignore_content_seen_rsquare_state2)
    return "ignore_content_seen_rsquare_state2";
  else if(state == include_state1)
    return "include_state1";
  else if(state == include_state2)
    return "include_state2";
  else if(state == include_state3)
    return "include_state3";
  else if(state == include_state4)
    return "include_state4";
  else if(state == include_state5)
    return "include_state5";
  else if(state == include_state6)
    return "include_state6";

  return "unknown";
}
#endif

/*********************
 *
 *  Tokenizer State Functions
 *
 *********************/

FAXPP_Error
ws_plus_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    env->state = ws_state;
    next_char(env);
    break;
  default:
    retrieve_state(env);
    token_start_position(env);
    // don't call next_char()
    return EXPECTING_WHITESPACE;
  }
  return NO_ERROR;
}

FAXPP_Error
ws_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    next_char(env);
    break;
  default:
    retrieve_state(env);
    token_start_position(env);
    // don't call next_char()
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
equals_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  WHITESPACE:
    break;
  case '=':
    env->state = ws_state;
    break;
  default:
    next_char(env);
    return EXPECTING_EQUALS;
  }
  next_char(env);
  return NO_ERROR;  
}

FAXPP_Error
initial_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '<':
    env->state = xml_decl_or_markup_state;
    next_char(env);
    break;
  default:
    base_state(env);
    // No next_char
    break;
  }

  return NO_ERROR;
}

FAXPP_Error
initial_misc_state(FAXPP_TokenizerEnv *env)
{
  if(env->position >= env->buffer_end) {
    if(env->token.value.ptr) {
      token_end_position(env);
      if(env->token.value.len != 0) {
        report_token(IGNORABLE_WHITESPACE_TOKEN, env);
        return NO_ERROR;
      }
    }
    token_start_position(env);
    return PREMATURE_END_OF_BUFFER;
  }

  read_char_no_check(env);

  switch(env->current_char) {
  case '<':
    env->state = initial_markup_state;
    token_end_position(env);
    report_token(IGNORABLE_WHITESPACE_TOKEN, env);
    next_char(env);
    break;
  WHITESPACE:
    next_char(env);
    break;
  default:
    next_char(env);
    return NON_WHITESPACE_OUTSIDE_DOC_ELEMENT;
  }
  return NO_ERROR;
}

FAXPP_Error
parsed_entity_state(FAXPP_TokenizerEnv *env)
{
  if(env->position >= env->buffer_end) {
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

  read_char_no_check(env);

  switch(env->current_char) {
  case '<':
    env->state = initial_markup_state;
    token_end_position(env);
    report_token(CHARACTERS_TOKEN, env);
    break;
  case '&':
    store_state(env);
    env->state = reference_state;
    token_end_position(env);
    report_token(CHARACTERS_TOKEN, env);
    next_char(env);
    token_start_position(env);
    return NO_ERROR;
  case ']':
    env->state = default_element_content_rsquare_state1;
    break;
  LINE_ENDINGS
    break;
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
initial_markup_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '?':
    env->state = pi_name_start_state;
    next_char(env);
    token_start_position(env);
    break;
  case '!':
    if(env->external_parsed_entity || env->element_entity)
      env->state = cdata_or_comment_state;
    else if(env->seen_doctype)
      env->state = comment_start_state1;
    else env->state = doctype_or_comment_state;
    next_char(env);
    token_start_position(env);
    break;
  LINE_ENDINGS
  default:
    env->state = (env)->start_element_name_state;
    env->seen_doc_element = 1;
    token_start_position(env);
    next_char(env);
    if((FAXPP_char_flags(env->current_char) & env->ncname_start_char) == 0)
      return INVALID_CHAR_IN_ELEMENT_NAME;
    break;
  }
  return NO_ERROR;
}

FAXPP_Error
final_state(FAXPP_TokenizerEnv *env)
{
  if(env->position >= env->buffer_end) {
    if(env->token.value.ptr) {
      token_end_position(env);
      if(env->token.value.len != 0) {
        report_token(IGNORABLE_WHITESPACE_TOKEN, env);
        if(env->buffer_done)
          env->state = end_of_buffer_state;
        return NO_ERROR;
      }
    }
    if(env->buffer_done) {
      report_empty_token(END_OF_BUFFER_TOKEN, env);
      return NO_ERROR;
    }
    token_start_position(env);
    return PREMATURE_END_OF_BUFFER;
  }

  read_char_no_check(env);

  switch(env->current_char) {
  case '<':
    env->state = final_markup_state;
    token_end_position(env);
    report_token(IGNORABLE_WHITESPACE_TOKEN, env);
    next_char(env);
    break;
  WHITESPACE:
    next_char(env);
    break;
  default:
    next_char(env);
    return NON_WHITESPACE_OUTSIDE_DOC_ELEMENT;
  }
  return NO_ERROR;
}

FAXPP_Error
final_markup_state(FAXPP_TokenizerEnv *env)
{
  read_char(env);

  switch(env->current_char) {
  case '?':
    env->state = pi_name_start_state;
    next_char(env);
    token_start_position(env);
    break;
  case '!':
    env->state = comment_start_state1;
    next_char(env);
    token_start_position(env);
    break;
  LINE_ENDINGS
  default:
    env->state = (env)->start_element_name_state;
    token_start_position(env);
    next_char(env);
    return ADDITIONAL_DOCUMENT_ELEMENT;
  }
  return NO_ERROR;
}

FAXPP_Error
end_of_buffer_state(FAXPP_TokenizerEnv *env)
{
    report_empty_token(END_OF_BUFFER_TOKEN, env);
    return NO_ERROR;
}

// Include the default states

#define PREFIX(name) default_ ## name
#define END_CHECK_IF if((env)->position >= (env)->buffer_end)
#define END_CHECK END_CHECK_IF { return PREMATURE_END_OF_BUFFER; }
#define READ_CHAR read_char_no_check(env)
#define DEFAULT_CASE (void)0

#include "element_states.h"
#include "attr_states.h"

#undef DEFAULT_CASE
#undef READ_CHAR
#undef END_CHECK
#undef END_CHECK_IF
#undef PREFIX

// Include the utf8 states

#define PREFIX(name) utf8_ ## name
#define END_CHECK_IF if((env)->position >= (env)->buffer_end)
#define END_CHECK END_CHECK_IF { return PREMATURE_END_OF_BUFFER; }
#define READ_CHAR \
  /* Assume it's a one byte character for now */ \
  env->current_char = *(uint8_t*)env->position; \
  env->char_len = 1
#define DEFAULT_CASE \
{ \
  /* Check if it really was a one byte char */ \
  if(env->current_char >= 0x80) { \
    /* Decode properly */ \
    env->char_len = (env)->decode(env->position, env->buffer_end, &env->current_char); \
    switch((env)->char_len) { \
    case TRANSCODE_PREMATURE_END_OF_BUFFER: \
      return PREMATURE_END_OF_BUFFER; \
    case TRANSCODE_BAD_ENCODING: \
      return BAD_ENCODING; \
    } \
  } \
}

#include "element_states.h"
#include "attr_states.h"

#undef DEFAULT_CASE
#undef READ_CHAR
#undef END_CHECK
#undef END_CHECK_IF
#undef PREFIX

// Include the utf16 states

#define PREFIX(name) utf16_ ## name
#define END_CHECK_IF if((env)->position + 1 >= (env)->buffer_end)
#define END_CHECK END_CHECK_IF { return PREMATURE_END_OF_BUFFER; }
#define READ_CHAR \
  /* Assume it's not a surrogate pair for now */ \
  env->current_char = *(uint16_t*)env->position; \
  env->char_len = 1 * sizeof(uint16_t)
#define DEFAULT_CASE \
{ \
  /* Check if it was actually a surrogate pair */ \
  if(env->current_char >= 0xD800 && env->current_char <= 0xDF00) { \
    /* Decode properly */ \
    env->char_len = FAXPP_utf16_native_decode(env->position, env->buffer_end, &env->current_char); \
    switch((env)->char_len) { \
    case TRANSCODE_PREMATURE_END_OF_BUFFER: \
      return PREMATURE_END_OF_BUFFER; \
    case TRANSCODE_BAD_ENCODING: \
      return BAD_ENCODING; \
    } \
  } \
}

#include "element_states.h"
#include "attr_states.h"

#undef DEFAULT_CASE
#undef READ_CHAR
#undef END_CHECK
#undef END_CHECK_IF
#undef PREFIX

