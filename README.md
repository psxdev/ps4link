PS4LINK FOR PS4
=================
 
===================
 What does this do?
===================
 
  PS4Link is a group of tools (libps4link,libdebugnet,ps4sh) for PS4 and host. It is the same method that we used in ps2dev days, so basically it is the same protocol than ps2link and ps2client have been using since 2003.
  
  Functions availables are defined like native sce functions so it is easy for homebrew developer to use these new functions:
  
  FILEIO functions
  ```
  int ps4LinkOpen(const char *file, int flags, int mode);
  int ps4LinkClose(int fd);
  int ps4LinkRead(int fd, void *data, size_t size);
  int ps4LinkWrite(int fd, const void *data, size_t size);
  int ps4LinkLseek(int fd, int offset, int whence);
  int ps4LinkRemove(const char *file);
  int ps4LinkMkdir(const char *dirname, int mode);
  int ps4LinkRmdir(const char *dirname);
  int ps4LinkDopen(const char *dirname);
  int ps4LinkDread(int fd, struct dirent *dir);
  int ps4LinkDclose(int fd);
  ```
  
  Commands functions (Not documented yet)
  check ps4link_internal.h and commands.c to see how can you implements new commands
 
==================
  How do I use it?
==================

 1) Configure your environment: 
 
  You will need:
  
  
  * [clang] 3.7 or upper i tested it on freebsd and osx. For osx :
  
  For osx only (you will not need this for freebsd):
  
  Downloading clang
  
  ```
  cd /usr/local/ps4dev/git
  mkdir crossllvm
  cd crossllvm
  git clone http://llvm.org/git/llvm.git
  cd llvm/tools
  git clone http://llvm.org/git/clang.git
  git clone http://llvm.org/git/lld.git
  cd ../projects
  git clone http://llvm.org/git/compiler-rt.git
  cd ../..
  ```
  
  You must create build directory outside of llvm
  
  ```
  mkdir build
  cd build
  ```
  
  Now prepare configuration cmake and python needed
  
  ```
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ps4dev/toolchain -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_DEFAULT_TARGET_TRIPLE=x86_64-scei-ps4 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCOMPILER_RT_BUILD_BUILTINS:BOOL=OFF -DCOMPILER_RT_BUILD_SANITIZERS:BOOL=OFF -DCOMPILER_RT_CAN_EXECUTE_TESTS:BOOL=OFF -DCOMPILER_RT_INCLUDE_TESTS:BOOL=OFF -DLLVM_TOOL_COMPILER_RT_BUILD:BOOL=OFF  -DCLANG_BUILD_EXAMPLES:BOOL=ON -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_C_FLAGS="-Wdocumentation -Wno-documentation-deprecated-sync" -DCMAKE_CXX_FLAGS="-std=c++11 -Wdocumentation -Wno-documentation-deprecated-sync" -DLLVM_LIT_ARGS="-v" ../llvm
  ```
  
  Compiling and install
  
  ```
  cmake --build .
  cmake --build . --target install
  ```
  
  Fine a fresh clang 3.8 with the same compiling option from Sony. However Sony is using propietary linker so we need a valid linker for osx(freebsd has not this problem already has one).
  
  i downloaded binutils 2.25 and compile it with:
  ```
  ./configure --prefix="$PS4DEV/host-osx" --target="x86_64-pc-freebsd9" \
  	--disable-nls \
  	--disable-dependency-tracking \
  	--disable-werror \
  	--enable-ld \
  	--enable-lto \
  	--enable-plugins \
  	--enable-poison-system-directories
  make
  make install
  ```
  
  to avoid use osx native tools
  add this to your ps4dev.sh environment script only for osx using my configuration:
  PATH=$PS4DEV/host-osx/x86_64-pc-freebsd9/bin:$PS4DEV/toolchain/bin:$PATH
  
  Also if clang is searching for ps4-ld:
  
  ```
  cd /usr/local/ps4dev/host-osx/x86_64-pc-freebsd9/bin
  cp ld ps4-ld
  ```
  
  final thing for osx you will need to add theses symbols to symbol.mk for libps4 inside libSceLibcInternal:
  
  ```
  __stack_chk_fail_local __stack_chk_fail __stack_chk_guard 
  ```
  
  
  now we can compile valid elf for ps4 from osx :)
  
  
  
  * [libps4 library](http://github.com/ps4dev/libps4) It is the base sdk for ps4dev
  
  * [elfldr basic loader](http://github.com/ps4dev/elfldr) It is the basic loader for ps4dev
  
  
 2) Declare variables and install libps4
  
  PS4DEV=/usr/local/ps4dev;export PS4DEV
  libps4=$PS4DEV/libps4;export libps4
  cd /usr/local/ps4dev
  git clone http://github.com/ps4dev/libps4
  cd libps4
  make
  cp -frv include $PS4DEV/libps4
  cp -frv make $PS4DEV/libps4
  cp -frv lib $PS4DEV/libps4
  cp crt0.s $PS4DEV/libps4
  
 3) PS4link
  
  Customize your ip configuration if you need
  
  ```
  cd /usr/local/ps4dev/git
  git clone http://github.com/psxdev/ps4link
  cd ps4link
  ```
  compile and instal libdebugnet it will let udp logging to your elfs:
  
  ```
  cd libdebugnet
  make
  make install
  cd ../..
  ```
  
  compile and install libps4link it will let fio host and commands support(elf loader soon inside)
  
  ```
  cd libps4link
  make 
  make install
  ```
  compile our PS4Link.elf using our libraries: libps4.a libdebugnet.a and libps4link.a
  
  ```
  cd ../..
  cd ps4link
  make
  make install
  ```
  
 4) Compile ps4sh
 
  ps4sh will let you speak with ps4link 
  
  ```
  cd /usr/local/ps4dev/git/ps4link/ps4sh
  make
  ```

 5) Run PS4Link.elf with elfldr tools from ps4dev repo
   
  Check instructions at [elfldr basic loader](http://github.com/ps4dev/elfldr)
  
 6) Session example
 
 First to see initial logs
 ```  
 socat udp-recv:18194 stdout
 ```
 then load PS4Link.elf to ps4 and we will see log from debugnet library 
 
 ```
 [PS4][INFO]: debugnet initialized
 [PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
 [PS4][INFO]: ready to have a lot of fun...
 [PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x80ABD120
 [PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 89
 [PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
 [PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x80AC17E0
 [PS4][DEBUG]: [PS4LINK] Ready for connection 1
 [PS4][DEBUG]: [PS4LINK] Command Thread Started.
 [PS4][DEBUG]: [PS4LINK] Waiting for connection
 [PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 90
 [PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
 ```
 Now it's time to use ps4sh tool from mac/pc
 
 ```
 $bin/ps4sh
 ps4sh version 1.0
 /Users/bigboss/.pkshrc: No such file or directory
  Connecting to fio ps4link ip 192.168.1.17
 log: [PS4][DEBUG]: [PS4LINK] Client connected from 192.168.1.3 port: 23235

 log: [PS4][DEBUG]: [PS4LINK] sock ps4link_fileio set 91 connected 1
 log: [PS4][DEBUG]: [PS4LINK] Initialized and connected from pc/mac ready to receive commands
 log: [PS4][DEBUG]: [PS4LINK] Waiting for connection
 ps4sh> help
 ?           ? :: Synonym for `help'..
 cd          cd [dir] :: Change pksh directory to [dir]..
 debug       debug :: Show pksh debug messages. ( alt-d ).
 exit        exit :: Exits pksh ( alt-q ).
 help        help :: Display this text..
 list        list [dir] :: List files in [dir]..
 log         log [file] :: Log messages from PS2 to [file]..
 ls          ls [dir] :: Synonym for list.
 make        make [argn] ... :: Execute make [argn] ....
 gmake       gmake [argn] ... :: Execute gmake [argn] ....
 pwd         pwd :: Print the current working directory ( alt-p ).
 quit        quit :: Quit pksh ( alt-q ).
 setroot     setroot [dir] :: Sets [dir] to be root dir..
 status      status :: Display some pksh information. ( alt-s ).
 execelf     execelf :: Load and exec elf. ....
 execsprx    execsprx :: Load and exec sprx. ....
 exitps4     exitps4 :: Finish ps4link in ps4 side. ....
 verbose     verbose :: Show verbose pksh messages. ( alt-v ).
 ps4sh> execelf sample_file.txt
 argc=1 argv=host0:sample_file.txt
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execelf argc=1 argv=host0:sample_file.txt
 log: [PS4][DEBUG]: [PS4LINK] file open req (host0:sample_file.txt, 0 0)
 Opening sample_file.txt flags 0
 Open return 7
 log: [PS4][DEBUG]: [PS4LINK] file open reply received (ret 7)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 20 result of lseek 0 offset 2 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 20)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 0 result of lseek 0 offset 0 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 0)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: Reply said there's 20 bytes to read (wanted 20)
 log: [PS4][DEBUG]: [PS4LINK] Content of file: Hello world ps4dev


 log: [PS4][DEBUG]: [PS4LINK] ps4link_file: file close req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_close_file: close reply received (ret 0)
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> exitps4
 argc=0 argv=
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command exit. Closing PS4Link...
 log: [PS4][DEBUG]: [PS4LINK] Aborting ps4link_requests_sock
 log: [PS4][DEBUG]: [PS4LINK] sceNetAccept error (0x80410104)
 log: [PS4][DEBUG]: [PS4LINK] exit thread requests
 log: [PS4][DEBUG]: [PS4LINK] closing fileio_sock
 ps4sh>
 ```
 
 
 7) ready to have a lot of fun :P
 
===================
 What next?
===================
  
 Add elf loader code inside PS4Link and begin research for kernel exploit
  
===========================
  Credits
===========================
  
  Special thanks goes to:
  
  - ps2dev old comrades. 
  - xerpi, hitodama, CTurt
  - All people who collaborated in ps4dev
  
  