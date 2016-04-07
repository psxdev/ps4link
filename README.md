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
  
  Remote Commands functions 
  
  1) execelf
  
  This command let you load and exec elf files compiled with libps4. Check samples directory.
  
  2) execsprx
  
  Right now do nothing 
  
  3) execwhoami
  
  Show you uid and gid 
  
  4) execshowdir
  
  Let you list filenames in directory. With ftp server you will not need it 
  
  5) exitps4
  
  Try to close ps4link resources. If you try to run ps4sh again when all is released you will get a messager saying that it can't connect. Now you can leave ps4 browser.
  
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
  
  
  now we can compile valid elf for ps4 from osx :)
  
  
  
  * [libps4 library](http://github.com/ps4dev/libps4) It is the base sdk for ps4dev
  
  * [elfldr basic loader](http://github.com/ps4dev/elfldr) It is the basic loader for ps4dev
  
  
  
 2) Declare variables and install libps4
  
  You can use a script with your environment variables. I like to use /usr/local/ps4dev/ps4dev.sh
  its content is:
  
  ```
  PS4DEV=/usr/local/ps4dev;export PS4DEV
  libps4=$PS4DEV/libps4;export libps4
  COMPILER=clang37;export COMPILER
  ```
  
  
  ```
  cd /usr/local/ps4dev
  mkdir git
  mkdir libps4
  cd git
  git clone http://github.com/ps4dev/libps4
  cd libps4
  make
  cp -frv include $PS4DEV/libps4
  cp -frv make $PS4DEV/libps4
  cp -frv lib $PS4DEV/libps4
  cp crt0.s $PS4DEV/libps4
  ```
  
 3) PS4link
  
  
  ```
  cd /usr/local/ps4dev/git
  git clone http://github.com/psxdev/ps4link
  cd ps4link
  ```
  compile and instal libdebugnet it will let to use udp logging to your own elf files and libraries:
  
  ```
  cd libdebugnet
  make
  make install
  cd ../..
  ```
  
  compile and install libps4link it will let fio host requests and commands support
  
  ```
  cd libps4link
  make 
  make install
  ```
  
  Customize your pc/mac ip configuration if you need in /usr/local/ps4dev/git/ps4link/ps4link/source/main.c in ps4LinkInit call. You must use your pc/mac ip configuration in first parameter.
  
  
  create ps4link loader based on elfldr 
  
  ```
  cd ../
  ls
  LICENSE		elfldr		libps4link	ps4sh
  README.md	libdebugnet	ps4link		samples
  cd elfldr
  ./copy_ps4link_sources.sh
  make
  ```
  Now you have a ldr.js file in /usr/local/ps4dev/git/ps4link/elfldr/local/ldr with ps4link embed in it.
  
  To run webkit exploit you will need load index.html from directory local. Publish content from directory local in your web server or:
  
  ```
  cd /usr/local/ps4dev/git/ps4link/eldldr/local
  node server.js
  Serving directory /usr/local/ps4dev/git/ps4link/elfldr/local on port 5350
  ```
  
  Now you have ready to run ps4link from your PlayStation 4
  
 4) Compile ps4sh
 
  ps4sh will let you speak with ps4link. It is based on pksh tools that we used in ps2dev days and credit goes to all people involved in its developments, greets to all of them.
  
  change dst_ip for your PlayStation 4 ip at /usr/local/ps4dev/git/ps4link/ps4sh/src/ps4sh.c
  
  ```
  cd /usr/local/ps4dev/git/ps4link/ps4sh
  make
  ```

 5) Compile samples :)
  3 samples are included sample, payload and ps4ftp
  
  sample is a very basic example it will receive debugnet conf from our ps4link, display some messages and exit.
  
  payload is a dlclose poc, it will give you root privileges, prison break and full file access and exit. After load it ps4link will have uid and gid 0 , WARNING use it under your own risk
  
  ps4ftp is based on xerpi ftp code and it will be give you a ftp server in your PlayStation 4, it can run with your game running :P at the same time if you load after payload.elf
  
  let's go to compile our elf samples
  
  ```
  cd /usr/local/ps4dev/git/ps4link/samples
  cd sample
  make
  cp bin/sample.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
  cd ..
  cd payload
  make
  cp bin/payload.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
  cd ..
  cd ps4ftp
  make
  cp bin/ps4ftp.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
 
  ```
  
  Ok our samples elf files are ready to use in your PlayStation 4, switch on your console :)
  
 6) Webkit ps4link loader
 
  First to see initial logs execute in your pc/mac
  ```  
  socat udp-recv:18194 stdout
  ``` 
  Open your PlayStation 4 browser. Open you local directory content, for example if you use node server.js option use the following url:
 
  ```
  http://ipofyourserver:5350
  ```   
  at stage 5 you will have ps4link waiting for commands and you will see output in yout mac/pc
  
  ```
  [PS4][INFO]: debugnet initialized
  [PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
  [PS4][INFO]: ready to have a lot of fun...
  [PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x810CF440
  [PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x8111E640
  [PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 85
  [PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
  [PS4][DEBUG]: [PS4LINK] Ready for connection 1
  [PS4][DEBUG]: [PS4LINK] Waiting for connection
  [PS4][DEBUG]: [PS4LINK] Command Thread Started.
  [PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 87
  [PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
  ^C
  ```
  
  You are ready to load your elf files...
  
  Next step show you a full session executing commands and loading samples.
  
 7) Session example using ps4link 
 
 First to see initial logs before open url in PlayStation 4 browser
 
 ```  
 socat udp-recv:18194 stdout
 ```
 
 after load ps4link on ps4 we will see logs from debugnet library and information about threads created by ps4link
 
 ```
 [PS4][INFO]: debugnet initialized
 [PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
 [PS4][INFO]: ready to have a lot of fun...
 [PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x810CF440
 [PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x8111E640
 [PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 85
 [PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
 [PS4][DEBUG]: [PS4LINK] Ready for connection 1
 [PS4][DEBUG]: [PS4LINK] Waiting for connection
 [PS4][DEBUG]: [PS4LINK] Command Thread Started.
 [PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 87
 [PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
 ^C
 ```
 
 Now it's time to use ps4sh tool from mac/pc. Close socat with control-c
 
 
 
 ```
 cd /usr/local/ps4dev/git/ps4link/ps4sh/bin 
 ./ps4sh
 ps4sh version 1.0
 /Users/bigboss/.ps4shrc: No such file or directory
 Connecting to fio ps4link ip 192.168.1.17
 log: [HOST][INFO]: [PS4SH] Ready
 log: [PS4][DEBUG]: [PS4LINK] Client connected from 192.168.1.3 port: 25030
 log: [PS4][DEBUG]: [PS4LINK] sock ps4link_fileio set 86 connected 1
 log: [PS4][DEBUG]: [PS4LINK] Waiting for connection
 log: [PS4][DEBUG]: [PS4LINK] Initialized and connected from pc/mac ready to receive commands
 ps4sh> execwhoami
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=0 argv=�����������
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execwhoami
 log: [PS4][DEBUG]: [PS4LINK] UID: 1, GID: 1
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> help
 ?           ? :: Synonym for `help'..
 cd          cd [dir] :: Change ps4sh directory to [dir]..
 debug       debug :: Show ps4sh debug messages. ( alt-d ).
 exit        exit :: Exits ps4sh ( alt-q ).
 help        help :: Display this text..
 list        list [dir] :: List files in [dir]..
 log         log [file] :: Log messages from PS4 to [file]..
 ls          ls [dir] :: Synonym for list.
 make        make [argn] ... :: Execute make [argn] ....
 gmake       gmake [argn] ... :: Execute gmake [argn] ....
 pwd         pwd :: Print the current working directory ( alt-p ).
 quit        quit :: Quit pksh ( alt-q ).
 setroot     setroot [dir] :: Sets [dir] to be root dir..
 status      status :: Display some ps4sh information. ( alt-s ).
 execelf     execelf :: Load and exec elf. ....
 execsprx    execsprx :: Load and exec sprx. ....
 exitps4     exitps4 :: Finish ps4link in ps4 side. ....
 execwhoami  execwhoami :: show uid and gid in ps4 side. ....
 execshowdir  execshowdir :: list file from directory in ps4 side. ....
 verbose     verbose :: Show verbose pksh messages. ( alt-v ). 
 ps4sh> ls
 total 240
 -rwxr-xr-x  1 bigboss  staff  23524  3 abr 21:05 payload.elf
 -rwxr-xr-x  1 bigboss  staff  45402  3 abr 21:35 ps4ftp.elf
 -rwxr-xr-x  1 bigboss  staff  53252  3 abr 21:00 ps4sh
 ps4sh> execelf payload.elf
 log: [HOST][DEBUG]: [PS4SH] argc=1 argv=host0:payload.elf
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execelf argc=1 argv=host0:payload.elf
 log: [PS4][DEBUG]: [PS4LINK] file open req (host0:payload.elf, 0 0)
 log: [HOST][DEBUG]: [PS4SH] Opening payload.elf flags 0
 log: [HOST][DEBUG]: [PS4SH] Open return 7
 log: [PS4][DEBUG]: [PS4LINK] file open reply received (ret 7)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [HOST][DEBUG]: [PS4SH] 23524 result of lseek 0 offset 2 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 23524)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [HOST][DEBUG]: [PS4SH] 0 result of lseek 0 offset 0 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 0)
 log: [HOST][DEBUG]: [PS4SH] read 23524 bytes of file descritor 7
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: Reply said there's 23524 bytes to read (wanted 23524)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 0  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 1  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 2  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 3  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 4  readed 7140
 log: [PS4][DEBUG]: [PS4LINK] ps4link_file: file close req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_close_file: close reply received (ret 0)
 log: [PS4][DEBUG]: [PS4LINK] in elfCreate
 log: [PS4][DEBUG]: [PS4LINK] reserved memory for elf at 2014bc000
 log: [PS4][DEBUG]: [PS4LINK] ready to run elf
 log: [PS4][DEBUG]: [PS4LINK] protectedMemoryCreate(2115864) -> [PS4][DEBUG]: ps4ProtectedMemoryCreate(2115864) ->
 log: [PS4][DEBUG]: [PS4LINK] elfLoaderLoad(2014bc000, 2016c8000, 2014c0000) ->
 log: [PS4][DEBUG]: [PS4LINK] elfLoaderLoad return 0
 log: [PS4][DEBUG]: [PS4LINK] mm->main 2014c1b00
 log: [PS4][DEBUG]: PS4LINK run [2014c0000 + elfEntry = 2014c1b00]
 log: [PS4][DEBUG]: [PS4LINK] New elf thread UID: 0x8111F0A0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 log: [PS4][DEBUG]: [PS4LINK] Configuration pointer 8810855e0, pointer_conf string 8810855e0
 log: [PS4][DEBUG]: [PS4LINK] ps4LinkRunElfMain
 log: [PS4][INFO]: debugnet already initialized using configuration from ps4link
 log: [PS4][INFO]: debugnet_initialized=1 SocketFD=84 logLevel=3
 log: [PS4][INFO]: ready to have a lot of fun...
 log: [PS4][DEBUG]: [POC] argc=2 elfname=elf debugnetconf=8810855e0 8810855e0 84
 log: [PS4][DEBUG]: [POC] [+] Starting...
 log: [PS4][DEBUG]: [POC] [+] UID = 1
 log: [PS4][DEBUG]: [POC] Opening fisrt socket 89
 log: [PS4][DEBUG]: [POC] socket opened is now equeals fd 3840
 log: [PS4][DEBUG]: [POC] cleaning open sockets
 log: [PS4][DEBUG]: [POC] m event queue created  0x000000BD
 log: [PS4][DEBUG]: [POC] m2 event queue created  0x000000BE
 log: [PS4][DEBUG]: [POC] mapping pointer 2018d0000
 log: [PS4][DEBUG]: [POC] [+] UID: 1, GID: 1
 log: [PS4][DEBUG]: [POC] before SYS_dynlib_prepare_dlclose
 log: [PS4][DEBUG]: [POC] SYS_dynlib_prepare_dlclose: -1
 log: [PS4][DEBUG]: [POC] before sceKernelDeleteEqueue
 log: [POC] [+] Entered critical payload
 log: [POC] [+] cred
 log: [POC] [+] cred->cr_uid  cred->cr_ruid  cred->cr_rgid set to 0
 log: [POC] [+] set group0 to 0
 log: [POC] [+] set prison0
 log: [POC] [+] set rootnode to td_fdp_fd_rdir
 log: [POC] [+] set rootnode to td_fdp_fd_jdir
 log: [POC] [+] exit from payload
 log: [PS4][DEBUG]: [POC] cleaning spray queues
 log: [PS4][DEBUG]: [POC] [+] Kernel patch success!
 log: [PS4][DEBUG]: [PS4LINK] ps4LinkRunElfMain mm->main return 0
 ps4sh> execwhoami
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=0 argv=
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execwhoami
 log: [PS4][DEBUG]: [PS4LINK] UID: 0, GID: 0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> ls
 total 240
 -rwxr-xr-x  1 bigboss  staff  23524  3 abr 21:05 payload.elf
 -rwxr-xr-x  1 bigboss  staff  45402  3 abr 21:35 ps4ftp.elf
 -rwxr-xr-x  1 bigboss  staff  53252  3 abr 21:00 ps4sh
 ps4sh> execelf ps4ftp.elf
 log: [HOST][DEBUG]: [PS4SH] argc=1 argv=host0:ps4ftp.elf
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execelf argc=1 argv=host0:ps4ftp.elf
 log: [PS4][DEBUG]: [PS4LINK] file open req (host0:ps4ftp.elf, 0 0)
 log: [HOST][DEBUG]: [PS4SH] Opening ps4ftp.elf flags 0
 log: [HOST][DEBUG]: [PS4SH] Open return 7
 log: [PS4][DEBUG]: [PS4LINK] file open reply received (ret 7)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [HOST][DEBUG]: [PS4SH] 45402 result of lseek 0 offset 2 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 45402)
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [HOST][DEBUG]: [PS4SH] 0 result of lseek 0 offset 0 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 0)
 log: [HOST][DEBUG]: [PS4SH] read 45402 bytes of file descritor 7
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: Reply said there's 45402 bytes to read (wanted 45402)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 0  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 1  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 2  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 3  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 4  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 5  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 6  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 7  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 8  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 9  readed 4096
 log: [PS4][DEBUG]: [PS4LINK] ps4link_read_file: chunk 10  readed 4442
 log: [PS4][DEBUG]: [PS4LINK] ps4link_file: file close req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_close_file: close reply received (ret 0)
 log: [PS4][DEBUG]: [PS4LINK] in elfCreate
 log: [PS4][DEBUG]: [PS4LINK] reserved memory for elf at 2014cc000
 log: [PS4][DEBUG]: [PS4LINK] ready to run elf
 log: [PS4][DEBUG]: [PS4LINK] protectedMemoryCreate(2135064) -> [PS4][DEBUG]: ps4ProtectedMemoryCreate(2135064) ->
 log: [PS4][DEBUG]: [PS4LINK] elfLoaderLoad(2014cc000, 2018f0000, 2014d0000) ->
 log: [PS4][DEBUG]: [PS4LINK] elfLoaderLoad return 0
 log: [PS4][DEBUG]: [PS4LINK] mm->main 2014d3e10
 log: [PS4][DEBUG]: PS4LINK run [2014d0000 + elfEntry = 2014d3e10]
 log: [PS4][DEBUG]: [PS4LINK] New elf thread UID: 0x8111F9E0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 log: [PS4][DEBUG]: [PS4LINK] Configuration pointer 8810855e0, pointer_conf string 8810855e0
 log: [PS4][DEBUG]: [PS4LINK] ps4LinkRunElfMain
 log: [PS4][INFO]: debugnet already initialized using configuration from ps4link
 log: [PS4][INFO]: debugnet_initialized=1 SocketFD=84 logLevel=3
 log: [PS4][INFO]: ready to have a lot of fun...
 log: [PS4][DEBUG]: [PS4FTP] 2 elfname=elf debugnetconf=8810855e0 8810855e0 84
 log: [PS4][DEBUG]: [PS4FTP] Client list mutex UID: 0x81121BE0
 log: [PS4][DEBUG]: [PS4FTP] Server thread UID: 0x81120440
 log: [PS4][DEBUG]: [PS4FTP] Server thread started!
 log: [PS4][DEBUG]: [PS4FTP] Server socket fd: 89
 log: [PS4][DEBUG]: [PS4FTP] sceNetBind(): 0x00000000
 log: [PS4][DEBUG]: [PS4FTP] sceNetListen(): 0x00000000
 log: [PS4][DEBUG]: [PS4FTP] Waiting for incoming connections...
 log: [PS4][DEBUG]: [PS4FTP] New connection, client fd: 0x0000005A
 log: [PS4][INFO]: Client 0 connected, IP: 192.168.1.3 port: 25286
 log: [PS4][DEBUG]: [PS4FTP] Client 0 thread UID: 0x81110720
 log: [PS4][DEBUG]: [PS4FTP] Waiting for incoming connections...
 log: [PS4][DEBUG]: [PS4FTP] Client thread 0 started!
 log: [PS4][DEBUG]: [PS4FTP] Received 14 bytes from client number 0:
 log: [PS4][INFO]: 	0> USER bigboss
 log: [PS4][DEBUG]: [PS4FTP] Received 11 bytes from client number 0:
 log: [PS4][INFO]: 	0> PASS pass
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> SYST
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> FEAT
 log: [PS4][DEBUG]: [PS4FTP] Received 5 bytes from client number 0:
 log: [PS4][INFO]: 	0> PWD
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> EPSV
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> PASV
 log: [PS4][DEBUG]: [PS4FTP] PASV data socket fd: 92
 log: [PS4][DEBUG]: [PS4FTP] sceNetBind(): 0x00000000
 log: [PS4][DEBUG]: [PS4FTP] sceNetListen(): 0x00000000
 log: [PS4][DEBUG]: [PS4FTP] PASV mode port: 0xB9C7
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> LIST
 log: [PS4][DEBUG]: [PS4FTP] PASV client fd: 0x0000005E
 log: [PS4][DEBUG]: [PS4FTP] Done sending LIST
 log: [PS4][DEBUG]: [PS4FTP] Received 6 bytes from client number 0:
 log: [PS4][INFO]: 	0> QUIT
 log: [PS4][DEBUG]: [PS4FTP] Client thread 0 exiting!
 ps4sh> execwhoami
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=0 argv=
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execwhoami
 log: [PS4][DEBUG]: [PS4LINK] UID: 0, GID: 0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> execshowdir /
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=1 argv=/
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execshowdir
 log: [PS4][DEBUG]: [DIR]: .
 log: [PS4][DEBUG]: [DIR]: ..
 log: [PS4][DEBUG]: [DIR]: adm
 log: [PS4][DEBUG]: [DIR]: app_tmp
 log: [PS4][DEBUG]: [DIR]: data
 log: [PS4][DEBUG]: [DIR]: dev
 log: [PS4][DEBUG]: [DIR]: eap_user
 log: [PS4][DEBUG]: [DIR]: eap_vsh
 log: [PS4][DEBUG]: [DIR]: hdd
 log: [PS4][DEBUG]: [DIR]: host
 log: [PS4][DEBUG]: [DIR]: hostapp
 log: [PS4][DEBUG]: [FILE]: mini-syscore.elf
 log: [PS4][DEBUG]: [DIR]: mnt
 log: [PS4][DEBUG]: [DIR]: preinst
 log: [PS4][DEBUG]: [DIR]: preinst2
 log: [PS4][DEBUG]: [FILE]: safemode.elf
 log: [PS4][DEBUG]: [FILE]: SceBootSplash.elf
 log: [PS4][DEBUG]: [FILE]: SceSysAvControl.elf
 log: [PS4][DEBUG]: [DIR]: system
 log: [PS4][DEBUG]: [DIR]: system_data
 log: [PS4][DEBUG]: [DIR]: system_ex
 log: [PS4][DEBUG]: [DIR]: system_tmp
 log: [PS4][DEBUG]: [DIR]: update
 log: [PS4][DEBUG]: [DIR]: usb
 log: [PS4][DEBUG]: [DIR]: user
 log: [PS4][DEBUG]: [PS4LINK] closing dfd
 log: [PS4][DEBUG]: [PS4LINK] end command execshowdir
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 log: [PS4][DEBUG]: [PS4FTP] New connection, client fd: 0x0000005B
 log: [PS4][INFO]: Client 1 connected, IP: 192.168.1.3 port: 25798
 log: [PS4][DEBUG]: [PS4FTP] Client 1 thread UID: 0x81111180
 log: [PS4][DEBUG]: [PS4FTP] Server thread exiting!
 log: [PS4][DEBUG]: [PS4FTP] Client thread 1 started!
 log: [PS4][DEBUG]: [PS4FTP] Client thread 1 exiting!
 log: [PS4][DEBUG]: [PS4FTP] calling ftp_fini
 ps4sh> execwhoami
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=0 argv=
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execwhoami
 log: [PS4][DEBUG]: [PS4LINK] UID: 0, GID: 0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> exitps4
 #
 ```
 
 
 output from ftp client session :)
 
 ```
 ftp 192.168.1.17 1337
 Connected to 192.168.1.17.
 220 FTPS4 Server ready.
 Name (192.168.1.17:bigboss):
 331 Username OK, need password b0ss.
 Password:
 230 User logged in!
 Remote system type is UNIX.
 Using binary mode to transfer files.
 ftp> ls
 227 Entering Passive Mode (192,168,1,17,199,185)
 150 Opening ASCII mode data transfer for LIST.
 drwxr-xr-x 1 ps4 ps4 16384 Jan 1  03:00 .
 drwxr-xr-x 1 ps4 ps4 16384 Jan 1  03:00 ..
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 adm
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 app_tmp
 drwxr-xr-x 1 ps4 ps4 512 Feb 21 20:35 data
 drwxr-xr-x 1 ps4 ps4 512 Apr 3  21:41 dev
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 eap_user
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 eap_vsh
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 hdd
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 host
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 hostapp
 -rw-r--r-- 1 ps4 ps4 341886 Aug 20 03:14 mini-syscore.elf
 drwxr-xr-x 1 ps4 ps4 440 Apr 3  21:44 mnt
 drwxr-xr-x 1 ps4 ps4 4096 Jan 1  03:00 preinst
 drwxr-xr-x 1 ps4 ps4 4096 Jan 1  03:00 preinst2
 -rw-r--r-- 1 ps4 ps4 2738424 Aug 20 03:14 safemode.elf
 -rw-r--r-- 1 ps4 ps4 117164 Aug 20 03:14 SceBootSplash.elf
 -rw-r--r-- 1 ps4 ps4 538740 Aug 20 03:14 SceSysAvControl.elf
 drwxr-xr-x 1 ps4 ps4 4096 Jan 1  03:00 system
 drwxr-xr-x 1 ps4 ps4 512 Nov 29 21:05 system_data
 drwxr-xr-x 1 ps4 ps4 4096 Jan 1  03:00 system_ex
 drwxr-xr-x 1 ps4 ps4 28440 Apr 3  21:59 system_tmp
 drwxr-xr-x 1 ps4 ps4 32768 Jan 1  03:00 update
 drwxr-xr-x 1 ps4 ps4 4096 Dec 30 03:00 usb
 drwxr-xr-x 1 ps4 ps4 512 Sep 30 17:21 user
 226 Transfer complete.
 ftp> quit
 221 Goodbye senpai :'(
 $ ftp 192.168.1.17
 
 ```
 
 7) ready to have a lot of fun :P
 
===================
 What next?
===================
  
 Improve code, incoporate new features to libps4 and  pad and graphics will be next target.
  
===================
 Last Changes
===================
  - Added custom elfldr and fixed readme information
  - Added commands execwhoami and execshowdir
  - Added ftp server in sample ps4ftp. You can use execelf ps4ftp.elf
  - Added dlclose poc in sample payload. You can use execelf payload.elf
  - Fixed  elf loader problem with large files
  - Added elf loader support from host0
  - Added parameter passing to new elf to sharing fio and log facilities from ps4link
  - Added fine logs to ps4sh
  - Fix exitps4 bug
   
===========================
  Credits
===========================
  
  Special thanks goes to:
  
  - ps2dev old comrades. 
  - hitodama for libps4 and elfldr 
  - xerpi for ps4ftp code base :P
  - kr105 for valid return code to userland in his dlclose poc
  - qwertyoruiop and cturt for sharing dlclose information
  - All people who collaborated in ps4dev
  
  