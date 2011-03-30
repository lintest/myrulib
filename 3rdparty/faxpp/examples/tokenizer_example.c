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

#include <faxpp/tokenizer.h>

#define MSECS_IN_SECS 1000000

unsigned long getTime()
{
  struct timeval timev;
  gettimeofday(&timev, 0);

  return (timev.tv_sec * MSECS_IN_SECS) + timev.tv_usec;
}

#define OUTPUT_BUFFER_SIZE 50
#define INPUT_BUFFER_SIZE 4 * 1024

int
main(int argc, char **argv)
{
  FAXPP_Error err;
  const FAXPP_Token *token;
  int i;
  char buf[OUTPUT_BUFFER_SIZE + 1];
  unsigned long startTime;
  FILE *file;
  char xml[INPUT_BUFFER_SIZE];
  long length;

  if(argc < 2) {
    printf("Too few arguments\n");
    exit(-1);
  }

  FAXPP_Tokenizer *tokenizer = FAXPP_create_tokenizer(FAXPP_utf8_transcoder);
  if(tokenizer == 0) {
    printf("ERROR: out of memory\n");
    exit(1);
  }

  for(i = 1; i < argc; ++i) {

    startTime = getTime();

    file = fopen(argv[i], "r");
    if(file == 0) {
      printf("Open failed: %s\n", strerror(errno));
      exit(1);
    }

    length = fread(xml, 1, sizeof(xml), file);

    err = FAXPP_init_tokenize(tokenizer, xml, length, length != sizeof(xml));
    if(err != NO_ERROR) {
      printf("ERROR: %s\n", FAXPP_err_to_string(err));
      exit(1);
    }

    err = FAXPP_next_token(tokenizer);
    token = FAXPP_get_current_token(tokenizer);
    while(token->type != END_OF_BUFFER_TOKEN) {
      if(err == PREMATURE_END_OF_BUFFER && length == sizeof(xml)) {
        // Repopulate the buffer
        void *buffer_position;
        err = FAXPP_tokenizer_release_buffer(tokenizer, &buffer_position);
        if(err != NO_ERROR) {
          printf("ERROR: %s\n", FAXPP_err_to_string(err));
          exit(1);
        }

        if(buffer_position < (void*)xml + sizeof(xml)) {
          length = (void*)(xml + sizeof(xml)) - buffer_position;
          memmove(xml, buffer_position, length);
        }
        else length = 0;

        length += fread(xml + length, 1, sizeof(xml) - length, file);

        err = FAXPP_continue_tokenize(tokenizer, xml, length, length != sizeof(xml));
        if(err != NO_ERROR) {
          printf("ERROR: %s\n", FAXPP_err_to_string(err));
          exit(1);
        }
      }
      else if(err != NO_ERROR) {
        printf("%03d:%03d ERROR: %s\n", FAXPP_get_tokenizer_error_line(tokenizer),
               FAXPP_get_tokenizer_error_column(tokenizer), FAXPP_err_to_string(err));
        if(err == PREMATURE_END_OF_BUFFER ||
           err == BAD_ENCODING ||
           err == OUT_OF_MEMORY) break;
      }
      else if(token->value.len != 0) {
        if(token->value.len > OUTPUT_BUFFER_SIZE) {
          strncpy(buf, token->value.ptr, OUTPUT_BUFFER_SIZE - 3);
          buf[OUTPUT_BUFFER_SIZE - 3] = '.';
          buf[OUTPUT_BUFFER_SIZE - 2] = '.';
          buf[OUTPUT_BUFFER_SIZE - 1] = '.';
          buf[OUTPUT_BUFFER_SIZE] = 0;
        }
        else {
          strncpy(buf, token->value.ptr, token->value.len);
          buf[token->value.len] = 0;
        }
        printf("%03d:%03d Token ID: %s, Token: \"%s\"\n", token->line, token->column, FAXPP_token_to_string(token->type), buf);
      }
      else
        printf("%03d:%03d Token ID: %s\n", token->line, token->column, FAXPP_token_to_string(token->type));

      err = FAXPP_next_token(tokenizer);
    }

    printf("Time taken: %gms\n", ((double)(getTime() - startTime) / MSECS_IN_SECS * 1000));
  }

  FAXPP_free_tokenizer(tokenizer);

  return 0;
}
