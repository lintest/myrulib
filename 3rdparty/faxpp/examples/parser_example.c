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
#include <sys/time.h>

#include <faxpp/parser.h>
#include "entity_resolver.h"
#include "output_event.h"

#define BUFFER_SIZE 10
#define MSECS_IN_SECS 1000000

unsigned long getTime()
{
  struct timeval timev;
  gettimeofday(&timev, 0);

  return (timev.tv_sec * MSECS_IN_SECS) + timev.tv_usec;
}

int
main(int argc, char **argv)
{
  FAXPP_Error err;
  int i;
  unsigned long startTime;
  FILE *file;

  if(argc < 2) {
    printf("Too few arguments\n");
    exit(-1);
  }

  FAXPP_Parser *parser = FAXPP_create_parser(WELL_FORMED_PARSE_MODE, FAXPP_utf8_transcoder);
  if(parser == 0) {
    printf("ERROR: out of memory\n");
    exit(1);
  }

  for(i = 1; i < argc; ++i) {

    FAXPP_set_external_entity_callback(parser, entity_callback, 0);

    startTime = getTime();

    file = fopen(argv[i], "r");
    if(file == 0) {
      printf("Open of '%s' failed: %s\n", argv[i], strerror(errno));
      exit(1);
    }

    err = FAXPP_init_parse_callback(parser, file_read_callback, file);
    if(err == NO_ERROR)
      err = FAXPP_set_base_uri_str(parser, argv[i]);

    if(err != NO_ERROR) {
      printf("ERROR: %s\n", FAXPP_err_to_string(err));
      exit(1);
    }

    while((err = FAXPP_next_event(parser)) == 0) {
      output_event(FAXPP_get_current_event(parser), stdout);

      if(FAXPP_get_current_event(parser)->type == END_DOCUMENT_EVENT)
        break;
    }

    if(err != NO_ERROR) {
      output_text(FAXPP_get_base_uri(parser), stdout);
      printf(":%d:%d ERROR: %s\n", FAXPP_get_error_line(parser),
             FAXPP_get_error_column(parser), FAXPP_err_to_string(err));
    }

    printf("Time taken: %gms\n", ((double)(getTime() - startTime) / MSECS_IN_SECS * 1000));
  }

  FAXPP_free_parser(parser);

  return 0;
}
