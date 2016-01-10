

  CC = gcc -g


  CFLAGS = -std=gnu99 -Wall -pedantic -I/usr/include -I/usr/local/include -I/opt/local/include

  ifeq "x$(MSYSTEM)" "x"
   LIBS = -L/opt/local/lib -lpthread -lreadline -lcurses -lncurses 
  #TODO WINDOWS SUPPORT
  else
   LIBS = -lwsock32 -lpthreadGC2 
  endif
  
  ifeq "x$(PREFIX)" "x"
   PREFIX = $(PS4SDK)
  endif

  all: bin/ps4sh

  clean:
	rm -f obj/*.o bin/*ps4sh*

  install: bin/ps4sh
	strip bin/*ps4sh*
	cp bin/*ps4sh* $(PREFIX)/bin

 ####################
 ## CLIENT MODULES ##
 ####################

  OFILES += obj/network.o
  obj/network.o: src/network.c src/network.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/network.c -o obj/network.o

  OFILES += obj/ps4link.o
  obj/ps4link.o: src/ps4link.c src/ps4link.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/ps4link.c -o obj/ps4link.o

  OFILES += obj/utility.o
  obj/utility.o: src/utility.c src/utility.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/utility.c -o obj/utility.o
	
  OFILES += obj/rl_common.o
  obj/rl_common.o: src/rl_common.c src/rl_common.h 
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/rl_common.c -o obj/rl_common.o

  OFILES += obj/common.o
  obj/common.o: src/common.c src/common.h 
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/common.c -o obj/common.o
	
  OFILES += obj/debugnet.o
  obj/debugnet.o: src/debugnet.c src/debugnet.h 
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/debugnet.c -o obj/debugnet.o

 #####################
 ## CLIENT PROGRAMS ##
 #####################


  bin/ps4sh: $(OFILES) src/ps4sh.c src/ps4sh.h
	@mkdir -p bin
	$(CC) $(CFLAGS) $(OFILES) src/ps4sh.c -o bin/ps4sh $(LIBS)
