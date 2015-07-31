targets = split

all: $(targets)

# On largefile-aware systems you might want to use these instead:
#MPG123_CFLAGS  := $(shell pkg-config --cflags libmpg123_64)
#MPG123_LDFLAGS := $(shell pkg-config --libs   libmpg123_64)
# This works on sane setups where off_t is off_t, and just that.
ifeq ($(MPG123_PREFIX),)
  MPG123_CFLAGS  := $(shell pkg-config --cflags libmpg123)
  MPG123_LDFLAGS := $(shell pkg-config --libs   libmpg123)
else # Yeah, that's with GNU/Linux in mind, at least GNU ld ...
  MPG123_CFLAGS  := -I$(MPG123_PREFIX)/include
  MPG123_LDFLAGS := -L$(MPG123_PREFIX)/lib -Wl,-rpath $(MPG123_PREFIX)/lib -lmpg123
endif
SND_CFLAGS     := $(shell pkg-config --cflags sndfile)
SND_LDFLAGS    := $(shell pkg-config --libs   sndfile)

CC := g++

# Oder of libs not that important here...
compile = $(CC) $(CPPFLAGS) $(CFLAGS) $(MPG123_CFLAGS)
linkflags = $(MPG123_LDFLAGS) $(LDFLAGS)

split: main.cpp
	$(compile) -o split main.cpp $(linkflags)