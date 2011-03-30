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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <faxpp/parser.h>
#include "../examples/entity_resolver.h"
#include "../examples/output_event.h"

void error(const FAXPP_Parser *parser, FAXPP_Error err)
{
  unsigned int line = FAXPP_get_error_line(parser);

  if(line != 0) {
    output_text(FAXPP_get_base_uri(parser), stderr);
    fprintf(stderr, ":%d:%d FAXPP_Error: %s\n", line, FAXPP_get_error_column(parser), FAXPP_err_to_string(err));
  } else {
    fprintf(stderr, "FAXPP_Error: %s\n", FAXPP_err_to_string(err));
  }
  exit(1);
}

void printIndent(unsigned int indent)
{
  while(indent--) printf("  ");
}

#define text_equal(text, str) (((text).len == strlen((str))) ? memcmp((text).ptr, (str), (text).len) == 0 : 0)

const FAXPP_Attribute *find_attribute(const FAXPP_Event *event, const char *name)
{
  int i;

  for(i = 0; i < event->attr_count; ++i) {
    if(text_equal(event->attrs[i].name, name))
      return &event->attrs[i];
  }

  return 0;
}

void calculateBase(const char *testFile, const FAXPP_AttrValue *atval, char *out)
{
  char *ptr = out;

  int len = strlen(testFile);
  memcpy(ptr, testFile, len);
  ptr += len;

  // Search backwards to the last "/", or the begining of the string
  while(ptr > out && *(ptr - 1) != '/') --ptr;

  while(atval) {
    memcpy(ptr, atval->value.ptr, atval->value.len);
    ptr += atval->value.len;
    atval = atval->next;
  }

  // null terminate
  *ptr = 0;
}

FAXPP_Error run_test_case(const char *filename, char *errFileBuffer, unsigned int bufLen, unsigned int *errLine, unsigned int *errColumn)
{
  const FAXPP_Text *text;
  unsigned int len;

  FAXPP_Parser *testparser = FAXPP_create_parser(WELL_FORMED_PARSE_MODE, FAXPP_utf8_transcoder);

  FILE *file = fopen(filename, "r");
  if(file == 0) {
    printf("Could not open xml file '%s': %s\n", filename, strerror(errno));
    exit(1);
  }

  FAXPP_set_external_entity_callback(testparser, entity_callback, 0);

  FAXPP_Error err = FAXPP_init_parse_file(testparser, file);
  if(err == NO_ERROR)
    err = FAXPP_set_base_uri_str(testparser, filename);

  if(err == NO_ERROR) {
    while((err = FAXPP_next_event(testparser)) == 0) {
      if(FAXPP_get_current_event(testparser)->type == END_DOCUMENT_EVENT)
        break;
    }
  }

  if(err != NO_ERROR) {
    text = FAXPP_get_base_uri(testparser);
    len = text->len < bufLen - 1 ? text->len : bufLen - 1;
    memcpy(errFileBuffer, text->ptr, len);
    errFileBuffer[len] = 0;

    *errLine = FAXPP_get_error_line(testparser);
    *errColumn = FAXPP_get_error_column(testparser);
  }

  fclose(file);
  FAXPP_free_parser(testparser);

  return err;
}

int
main(int argc, char **argv)
{
  FAXPP_Error err;
  const FAXPP_Event *event;
  const FAXPP_Attribute *attr;
  char base_buffer[1024];
  char file_buffer[1024];
  FAXPP_Error result;
  char errFileBuffer[1024];
  unsigned int errLine;
  unsigned int errColumn;
  int output_events = 0;

  int test_failures = 0;
  int test_passes = 0;
  int test_skips = 0;

  if(argc < 2) {
    printf("Too few arguments\n");
    exit(-1);
  }

  FAXPP_Parser *parser = FAXPP_create_parser(WELL_FORMED_PARSE_MODE, FAXPP_utf8_transcoder);

  const char *testFile = argv[1];

  FILE *file = fopen(testFile, "r");
  if(file == 0) {
    printf("Could not open test file: %s\n", strerror(errno));
    exit(1);
  }

  FAXPP_set_external_entity_callback(parser, entity_callback, 0);

  err = FAXPP_init_parse_file(parser, file);
  if(err != NO_ERROR) error(parser, err);

  err = FAXPP_set_base_uri_str(parser, testFile);
  if(err != NO_ERROR) error(parser, err);

  while((err = FAXPP_next_event(parser)) == 0) {
    event = FAXPP_get_current_event(parser);

    switch(event->type) {
    case START_ELEMENT_EVENT:
    case SELF_CLOSING_ELEMENT_EVENT:
      if(text_equal(event->name, "TESTSUITE")) {
        attr = find_attribute(event, "PROFILE");
        if(attr) {
          printf("\n");
          printIndent(FAXPP_get_nesting_level(parser) - 1);
          output_attr_value(&attr->value, stdout);
          printf(": ");
        }
      }

      else if(text_equal(event->name, "TESTCASES")) {
        attr = find_attribute(event, "PROFILE");
        if(attr) {
          printf("\n");
          printIndent(FAXPP_get_nesting_level(parser) - 1);
          output_attr_value(&attr->value, stdout);
          printf(": ");
        }

        attr = find_attribute(event, "base");
        if(attr && attr->xml_attr) {
          calculateBase(testFile, &attr->value, base_buffer);
        }
      }

      else if(text_equal(event->name, "TEST")) {
        // TBD Check output - jpcs
/*         attr = find_attribute(event, "OUTPUT"); */
/*         if(attr) { */
/*           calculateBase(base_buffer, &attr->value, file_buffer); */

/*           printf("^"); */
/*           printf("\n%s\n", file_buffer); */
/*           fflush(stdout); */
/*           ++test_skips; */
/*           exit(-1); */
/*           break; */
/*         } */

        attr = find_attribute(event, "URI");
        calculateBase(base_buffer, &attr->value, file_buffer);

        result = run_test_case(file_buffer, errFileBuffer, sizeof(errFileBuffer), &errLine, &errColumn);

        // Skip tests that require no namespaces
        attr = find_attribute(event, "NAMESPACE");
        if(attr && text_equal(attr->value.value, "no")) {
          printf("^");
          fflush(stdout);
          ++test_skips;
          break;
        }

        attr = find_attribute(event, "TYPE");

        // Skip "error" type tests at the moment - since they
        // probably need detailed inspection to see which ones
        // ought to pass or fail
        // TBD enable these tests - jpcs
        if(text_equal(attr->value.value, "error")) {
          printf("^");
          fflush(stdout);
          ++test_skips;
          break;
        }

        // @TYPE is not-wf, error, invalid, or valid
        if(text_equal(attr->value.value, "not-wf") ||
           text_equal(attr->value.value, "error")) {
          if(result != NO_ERROR) {
            printf(".");
            fflush(stdout);
            ++test_passes;
            break;
          }
        }
        else if(result == NO_ERROR) {
          printf(".");
          fflush(stdout);
          ++test_passes;
          break;
        }

        printf("!");
        fflush(stdout);
        ++test_failures;

        fprintf(stderr, "File: %s\nType: ", file_buffer);
        output_attr_value(&attr->value, stderr);

        attr = find_attribute(event, "ENTITIES");
        if(attr) {
          fprintf(stderr, "\nEntities: ");
          output_attr_value(&attr->value, stderr);
        }

        attr = find_attribute(event, "SECTIONS");
        if(attr) {
          fprintf(stderr, "\nSections: ");
          output_attr_value(&attr->value, stderr);
        }

        if(result != NO_ERROR) {
          fprintf(stderr, "\nError: %s:%d:%d %s\n", errFileBuffer, errLine, errColumn, FAXPP_err_to_string(result));
        }

        fprintf(stderr, "\n");
        if(event->type == SELF_CLOSING_ELEMENT_EVENT) {
          fprintf(stderr, "\n");
        }
        else {
          output_events = 1;
        }
      }

      break;
    case END_ELEMENT_EVENT:
      if(output_events) {
        output_events = 0;
        fprintf(stderr, "\n\n");
      }
      break; 
    default:
      if(output_events) output_event(event, stderr);
      break;
    case END_DOCUMENT_EVENT:
      goto cleanup;
    }
  }

  if(err != NO_ERROR) error(parser, err);

 cleanup:
  printf("\n\nTests run: %d, Tests passed: %d, Tests skipped: %d, Tests failed: %d (%.3f%%)\n", test_passes + test_failures + test_skips,
         test_passes, test_skips, test_failures, ((double)test_passes / (double)(test_passes + test_failures)) * 100);


  fclose(file);

  FAXPP_free_parser(parser);

  return 0;
}
