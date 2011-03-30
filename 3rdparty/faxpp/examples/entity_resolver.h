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

#ifndef ENTITY_RESOLVER_H
#define ENTITY_RESOLVER_H

#include <faxpp/parser.h>

char *resolve_paths(const char *base, unsigned int base_len, const char *path, unsigned int path_len);
unsigned int file_read_callback(void *userData, void *buffer, unsigned int length);
FAXPP_Error entity_callback(void *userData, FAXPP_Parser *parser, FAXPP_EntityType type,
                            const FAXPP_Text *base_uri, const FAXPP_Text *system, const FAXPP_Text *public);

#endif
