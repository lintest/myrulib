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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "entity_resolver.h"

char *resolve_paths(const char *base, unsigned int base_len, const char *path, unsigned int path_len)
{
  char *result = malloc(base_len + path_len + 1);
  char *ptr = result;

  strncpy(ptr, base, base_len);
  ptr += base_len - 1;

  while(ptr >= result && *ptr != '/') {
    --ptr;
  }
  ++ptr;

  strncpy(ptr, path, path_len);
  ptr += path_len;
  *ptr = 0;

  return result;
}

unsigned int file_read_callback(void *userData, void *buffer, unsigned int length)
{
  unsigned int result = fread(buffer, 1, length, (FILE*)userData);
  if(result < length) {
    fclose((FILE*)userData);
  }
  return result;
}

FAXPP_Error entity_callback(void *userData, FAXPP_Parser *parser, FAXPP_EntityType type,
                            const FAXPP_Text *base_uri, const FAXPP_Text *system, const FAXPP_Text *public)
{
  FAXPP_Error err;
  FILE *file;
  char *path;

  path = resolve_paths((char*)base_uri->ptr, base_uri->len, (char*)system->ptr, system->len);

  file = fopen(path, "r");
  if(file == 0) {
/*     printf("Open of '%s' failed: %s\n", path, strerror(errno)); */
    return CANT_LOCATE_EXTERNAL_ENTITY;
  }

  err = FAXPP_parse_external_entity_callback(parser, type, file_read_callback, file);
  if(err == NO_ERROR)
    err = FAXPP_set_base_uri_str(parser, path);

  free(path);
  return err;
}
