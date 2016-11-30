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
  
  1) execuser name.elf
  
  This command let you load and exec elf files in user mode compiled with ps4sdk. Check samples directory.
  
  2) execkernel name.elf
  
  This command let you load and exec elf files in user mode compiled with ps4sdk. Check samples directory. I did not test it too much because PS4Link is running in superuser mode. 
  
  3) execwhoami
  
  Show you uid and gid 
  
  4) execshowdir ps4path
  
  Let you list filenames in directories from PlayStation 4 system. 
  
  5) execdecrypt ps4directorypath
  
  Let you decrypt all elf,self,sprx,prx,dll,sdll,exe,sexe and eboot.bin files from PlayStation 4 system and save it in your PC/Mac with the same name ps4 directory. You need first create in pc/mac where are you running ps4sh mkdir -p ps4/yourps4path
  
  For example for /system/sys decrypt
  ```
  ps4sh> mkdir -p ps4/system/sys
  ps4sh> execdecrypt /system/sys
  ```
  Check output at [link](https://gist.github.com/psxdev/cbbace3d73c6bd93250111842b61d1f4)
  5) exitps4
  
  Try to close ps4link resources. If you try to run ps4sh again when all is released you will get a messager saying that it can't connect. Now you can leave ps4 browser.
  
  check ps4link_internal.h and commands.c to see how can you implements new commands
 
==================
  How do I use it?
==================

 1) Configure your environment: 
 
  You will need:
  
  
  * [clang] 4.0 or upper i tested it on freebsd and macOS Sierra. For macOS :
  
  For macOS only (you will not need this for freebsd):
  
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
  
  After compiling and install you will have a fresh clang 4.0 with the same compiling options from Sony. However Sony is using propietary linker so we need a valid linker for macOS(freebsd has not this problem already has one).
  
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
  
  clang is searching for orbis-ld when you compile to fix that :
  
  ```
  cd /usr/local/ps4dev/host-osx/x86_64-pc-freebsd9/bin
  cp ld orbis-ld
  ```
  
  
  now we can compile valid elf for PlayStation 4 from macOS Sierra :)
  
  
  
  * [ps4sdk](http://github.com/ps4dev/ps4sdk) It is the base sdk for ps4dev
  
  * [elfldr basic loader](https://github.com/ps4dev/elf-loader) It is the basic loader for ps4dev however i use embed loader in PS4Link instead
  
  
  
 2) Declare variables and install ps4sdk
  
  You can use a script with your environment variables. I like to use /usr/local/ps4dev/ps4dev.sh
  Content for macOS:
  
  ```
  PS4DEV=/usr/local/ps4dev;export PS4DEV
  PATH=$PS4DEV/host-osx/x86_64-pc-freebsd9/bin:$PS4DEV/toolchain/bin:$PATH
  ps4sdk=$PS4DEV/ps4sdk;export ps4sdk
  ```
  
  
  ```
  cd /usr/local/ps4dev
  mkdir git
  mkdir ps4sdk
  cd git
  git clone http://github.com/ps4dev/ps4sdk
  cd ps4sdk
  make
  cp -frv include $PS4DEV/ps4sdk
  cp -frv make $PS4DEV/ps4sdk
  cp -frv lib $PS4DEV/ps4sdk
  cp crt0.s $PS4DEV/ps4sdk
  ```
  
  Ready we have now a valid toolchain, binutils and sdk :)
  
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
  cd ..
  ```
  
  compile and instal libelfloader it will let to use elf related function:
  
  ```
  cd libelfloader
  make
  make install
  cd ..
  ```
  
  compile and install libps4link it will let fio host requests and commands support
  
  ```
  cd libps4link
  make 
  make install
  cd ..
  ```
  
  Customize your pc/mac ip configuration if you need in /usr/local/ps4dev/git/ps4link/ps4link/source/main.c in ps4LinkInit call. You must use your pc/mac ip configuration in first parameter.
  
  
  create ps4link loader based on elf-loader 
  
  ```
  ls
  LICENSE		elf-loader		libps4link	ps4sh
  README.md	libdebugnet	ps4link		samples
  cd elf-loader
  ./copy_ps4link_sources.sh
  make
  ```
  Now you have a ldr.js file in /usr/local/ps4dev/git/ps4link/elf-loader/local/ldr with PS4Link embed in it.
  
  To run webkit exploit you will need load index.html from directory local. Publish content from directory local in your web server or:
  
  ```
  cd /usr/local/ps4dev/git/ps4link/elf-loader/local
  node server.js
  Serving directory /usr/local/ps4dev/git/ps4link/elf-loader/local on port 5350
  ```
  
  Now you are ready to run PS4Link from your PlayStation 4
  
 4) Compile ps4sh
 
  ps4sh will let you speak with PS4Link. It is based on pksh tools that we used in ps2dev days and credit goes to all people involved in its developments, greets to all of them.
  
  change dst_ip for your PlayStation 4 ip at /usr/local/ps4dev/git/ps4link/ps4sh/src/ps4sh.c
  
  ```
  cd /usr/local/ps4dev/git/ps4link/ps4sh
  make
  ```

 5) Compile samples :)
  4 samples are included sample, payload, ps4ftp and listproc
  
  sample is a very basic example it will receive debugnet conf from our PS4Link proccess, display some messages and exit.
  
  payload is a dlclose poc and it is deprecated you will not need it. It is here for historical/documentation purposes only so ignore it.
  
  ps4ftp is based on xerpi ftp code and it will be give you a ftp server in your PlayStation 4, it can run with your game running :P at the same time.
  
  listproc it is based on code from freebsd 9 [procstat](https://github.com/freebsd/freebsd/tree/release/9.0.0/usr.bin/procstat)
  It will give you information about all proccess and vmaps
  
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
  cd ..
  cd listproc
  make
  cp bin/listproc.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
  ```
  
  Ok our samples elf files are ready to use in your PlayStation 4, switch on your console :)
  
 6) Webkit ps4link loader
 
  First to see initial logs execute in your pc/mac your compiled ps4sh
  ```  
  ps4sh
  ps4sh version 1.0
  /Users/bigboss/.ps4shrc: No such file or directory

  log: [HOST][INFO]: [PS4SH] Ready
  ps4sh>
  ``` 
  Open your PlayStation 4 browser. Open you local directory content, for example if you use node server.js option use the following url:
 
  ```
  http://ipofyourserver:5350
  ```   
  at stage 5 you will have ps4link waiting for commands and you will see output in your mac/pc
  
  ```
  [PS4][INFO]: debugnet initialized
  [PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
  [PS4][INFO]: ready to have a lot of fun...
  [PS4][DEBUG]: getuid() : 1
  [PS4][DEBUG]: executing privilege scalation
  [PS4][DEBUG]: ps4KernelExecute ret=0
  [PS4][DEBUG]: getuid() : 0
  [PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x80C189C0
  [PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x80CA8A20
  [PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 83
  [PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
  [PS4][DEBUG]: [PS4LINK] Ready for connection 1
  [PS4][DEBUG]: [PS4LINK] Waiting for connection
  [PS4][DEBUG]: [PS4LINK] Command Thread Started.
  [PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 85
  [PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
  ps4sh>
  ```
  Check that last getuid value is 0 and no bind error displayed, if all is fine you will see that output if not your PlayStation browser perhaps will give you a memory error check again until you received the right output (UID threads and socks can change in your environment)
  
  You are ready to load your elf files...
  
  Next step show you a full session executing commands and loading samples.
  
 7) Session example using ps4link 
 
 First to see initial logs before open url in PlayStation 4 browser
 
 ```
  cd /usr/local/ps4dev/git/ps4link/ps4sh/bin 
  ./ps4sh   
  ps4sh version 1.0
  /Users/bigboss/.ps4shrc: No such file or directory

  log: [HOST][INFO]: [PS4SH] Ready
  ps4sh>
 ```
 
 after load ps4link on ps4 we will see logs from debugnet library and information about threads created by ps4link
 
 ```
 [PS4][INFO]: debugnet initialized
 [PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
 [PS4][INFO]: ready to have a lot of fun...
 [PS4][DEBUG]: getuid() : 1
 [PS4][DEBUG]: executing privilege scalation
 [PS4][DEBUG]: ps4KernelExecute ret=0
 [PS4][DEBUG]: getuid() : 0
 [PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x80C189C0
 [PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x80CA8A20
 [PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 83
 [PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
 [PS4][DEBUG]: [PS4LINK] Ready for connection 1
 [PS4][DEBUG]: [PS4LINK] Waiting for connection
 [PS4][DEBUG]: [PS4LINK] Command Thread Started.
 [PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 85
 [PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
 ps4sh>
 ```
 
 Run connect command
 
 
 ```
 ps4sh> connect
 log: [HOST][INFO]: [PS4SH] Connecting to fio ps4link ip 192.168.1.17
 log: [HOST][INFO]: [PS4SH] PlayStation is listening at 192.168.1.17
 log: [PS4][DEBUG]: [PS4LINK] Client connected from 192.168.1.3 port: 26562
 ps4sh> cd ../../samples/listproc/bin
 ps4sh> ls
 total 40
 -rwxr-xr-x  1 bigboss  staff  18856 17 nov 00:52 listproc.elf
 ps4sh> execwhoami
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=0 argv=
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execwhoami
 log: [PS4][DEBUG]: [PS4LINK] UID: 0, GID: 0
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> ?
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
 execuser    execelf :: Load and exec user elf. ....
 execkernel  execsprx :: Load and exec kernel elf. ....
 exitps4     exitps4 :: Finish ps4link in ps4 side. ....
 execdecrypt  decrypt :: decrypt file in ps4 side and dump to host0. ....
 execwhoami  execwhoami :: show uid and gid in ps4 side. ....
 execshowdir  execshowdir :: list file from directory in ps4 side. ....
 verbose     verbose :: Show verbose ps4sh messages. ( alt-v ).
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
 ps4sh> execdecrypt /mini-siscore.elf
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=1 argv=/mini-siscore.elf
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execdecrypt argc=1 argv=/mini-siscore.elf
 log: [PS4][DEBUG]: [PS4LINK] file name to decrypt mini-siscore.elf
 log: [PS4][DEBUG]: [PS4LINK] savefile in your host host0:mini-siscore.elf
 log: [PS4][DEBUG]: [PS4LINK] kernel hook
 log: [PS4][DEBUG]: [PS4LINK] open /mini-siscore.elf err : No such file or directory
 log: [PS4][DEBUG]: [PS4LINK] kernel unhook
 log: [PS4][DEBUG]: [PS4LINK] end command execdecrypt
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> execdecrypt /mini-syscore.elf
 log: [HOST][DEBUG]: [PS4SH] [PS4SH] argc=1 argv=/mini-syscore.elf
 log: [PS4][DEBUG]: [PS4LINK] commands listener received packet size (266)
 log: [PS4][DEBUG]: [PS4LINK] Received command execdecrypt argc=1 argv=/mini-syscore.elf
 log: [PS4][DEBUG]: [PS4LINK] file name to decrypt mini-syscore.elf
 log: [PS4][DEBUG]: [PS4LINK] savefile in your host host0:mini-syscore.elf
 log: [PS4][DEBUG]: [PS4LINK] kernel hook
 log: [PS4][DEBUG]: [PS4LINK] mmap /mini-syscore.elf : 201ea4000
 log: [PS4][DEBUG]: [PS4LINK] ehdr : 201ea40a0
 log: [PS4][DEBUG]: [PS4LINK] phdrs : 201ea40e0
 log: [PS4][DEBUG]: [PS4LINK] segment num : 4
 log: [PS4][DEBUG]: [PS4LINK] file open req (host0:mini-syscore.elf, 202 0)
 log: [HOST][DEBUG]: [PS4SH] Opening mini-syscore.elf flags 402
 log: [HOST][DEBUG]: [PS4SH] Open return 7
 log: [PS4][DEBUG]: [PS4LINK] file open reply received (ret 7)
 log: [PS4][DEBUG]: [PS4LINK] elf header + phdr size : 0x00000120
 log: [PS4][DEBUG]: [PS4LINK] file write req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] wrote 288 bytes (asked for 288)
 log: [PS4][DEBUG]: [PS4LINK] sbuf index : 0, offset : 0x0000000000004000, bufsz : 0x0000000000054000, filesz : 0x0000000000051684
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [HOST][DEBUG]: [PS4SH] 16384 result of lseek 16384 offset 0 whence
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 16384)
 log: [PS4][DEBUG]: [PS4LINK] file write req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1362 bytes (asked for 1362)
 log: [PS4][DEBUG]: [PS4LINK] sbuf index : 1, offset : 0x0000000000058000, bufsz : 0x0000000000004000, filesz : 0x0000000000001920
 log: [HOST][DEBUG]: [PS4SH] 360448 result of lseek 360448 offset 0 whence
 log: [PS4][DEBUG]: [PS4LINK] file lseek req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_lseek_file: lseek reply received (ret 360448)
 log: [PS4][DEBUG]: [PS4LINK] file write req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 1446 bytes (asked for 1446)
 log: [PS4][DEBUG]: [PS4LINK] wrote 478 bytes (asked for 478)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_file: file close req (fd: 7)
 log: [PS4][DEBUG]: [PS4LINK] ps4link_close_file: close reply received (ret 0)
 log: [PS4][DEBUG]: [PS4LINK] dump completed
 log: [PS4][DEBUG]: [PS4LINK] kernel unhook
 log: [PS4][DEBUG]: [PS4LINK] end command execdecrypt
 log: [PS4][DEBUG]: [PS4LINK] commands listener waiting for next command
 ps4sh> ls
 total 776
 -rwxr-xr-x  1 bigboss  staff   18816 17 nov 01:02 listproc.elf
 -rw-r--r--  1 bigboss  staff  376832 17 nov 01:05 mini-syscore.elf
 ps4sh> file mini-syscore.elf
 mini-syscore.elf: ELF 64-bit LSB executable, x86-64, version 1 (FreeBSD), statically linked, corrupted section header size
 
 ```
 
 you can check output from listproc.elf:
 
 * [without game loaded](https://gist.github.com/psxdev/cb3c9455aa06a5ae77a8be1280d14725)
 
 * [with game loaded](https://gist.github.com/psxdev/1248915a360c479d8ca2107dd706beef)
 
 8) ready to have a lot of fun :P
 
===================
 What next?
===================
  
 Improve code, incoporate new features to ps4sdk.
  
===================
 Last Changes
===================
  - fix ps4sh hang when lost connection with PlayStation 4
  - switched to libraries
  - switched to elf-loader fronted
  - add install.sh by droogie
  - ps4sdk compliant
  - Added listproc sample
  - Added execdecrypt command
  - Added execuser and execkernel commands
  - Added new commands to ps4sh
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
  - hitodama for ps4sdk and elfldr 
  - xerpi for ps4ftp code base :P
  - wskeu and zecoxao for sharing code to decrypt files i only add save option to host0 :P
  - droogie for testing and install script
  - kr105 for valid return code to userland in his dlclose poc
  - qwertyoruiop and cturt for sharing dlclose information
  - All people who collaborated in ps4dev
  
  