/*
 * bin2c: A Program to convert binary data into C source code
 * Copyright 2004 by Adrian Prantl <adrian@f4z.org>
 *
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.
 *
 */

#include <stdio.h>
#include <unistd.h>

char* self = 0;

void usage()
{
  printf("Usage:\n%s input_binary_file output_c_file symbol_name\n\n", self);
}

void bail_out(const char* s1, const char* s2)
{
  fprintf(stderr, "%s: FATAL ERROR:\n%s%s\n", self, s1, s2);
}

int main(int argc, char** argv)
{
  FILE *fi, *fo;
  int c, i;

  self = argv[0];

  if (argc != 4) {
    usage();
    return 1;
  }

  if ((fi = fopen(argv[1], "rb")) == 0) {
    bail_out("Cannot open input file ", argv[1]);
    return 1;
  }

  if ((fo = fopen(argv[2], "w")) == 0) {
    bail_out("Cannot open output file ", argv[2]);
    return 1;
  }

  fprintf(fo, "const unsigned char locale_binary_%s[] = {\n", argv[3]);

  i = 0;
  while ((c = fgetc(fi)) != EOF) {
    fprintf(fo, " 0x%02x,", (unsigned char)c);
    if (++i == 12) {
      fprintf(fo, "\n");
      i = 0;
    }
  }
  fprintf(fo, " };\n");

  printf("converted %s\n", argv[1]);

  return 0;
}

