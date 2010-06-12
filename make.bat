bakefile -f autoconf myrulib.bkl
bakefile -f gnu myrulib.bkl
bakefile -f mingw myrulib.bkl
mingw32-make -f makefile.gcc
