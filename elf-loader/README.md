# Elf loader

Runs ps4sdk elf files in-process on your PS4.

## Prerequisites
* clang 4.x for ps4 part 
* make
* [ps4sdk](https://github.com/ps4dev/ps4sdk)
* node.js to run server.js (or any alternative to serve /local)
* ps4sh if you want to use ps4link loader 
* user loader is available but it is commented on Makefile if you choose use it compile ps4link elf mode instead loader mode and you can load following [user](https://github.com/ps4dev/elf-loader/blob/master/README.md)

## Important

The elf loader does not support dynamically linked executables. All libraries need to be statically linked into the executable. [ps4sdk](https://github.com/ps4dev/ps4sdk) provides a variaty of position independant, statically linkable libraries, such as a libc, for the PS4. Depending on their build system and requirenments, you can compile third party libraries using the ps4-lib target of the sdk. Alternatively you will have to alter their build system to compile them as PIC statically linked libraries.

## Example
```bash
#Populate ps4link main source
./copy_ps4link_sources.sh
# Build as raw binary to bin/ and then convert to ldr.js in /local (you can 'make keepelf=1' to debug)
make clean && make

# Start server
cd local
node server.js

# Start ps4sh
cd /usr/local/ps4dev/git/ps4lik/ps4sh/bin
./ps4sh
# Browse ps4 browser to local server (<local>:5350)
# Wait until the browser hangs in 'step 5'
# Repeat again if you get out of memory problem
#Output expected
[PS4][INFO]: debugnet initialized
[PS4][INFO]: Copyright (C) 2010,2016 Antonio Jose Ramos Marquez aka bigboss @psxdev
[PS4][INFO]: ready to have a lot of fun...
[PS4][DEBUG]: getuid() : 1
[PS4][DEBUG]: executing privilege scalation
[PS4][DEBUG]: ps4KernelExecute ret=0
[PS4][DEBUG]: getuid() : 0
[PS4][DEBUG]: [PS4LINK] Server request thread UID: 0x804AE480
[PS4][DEBUG]: [PS4LINK] Server command thread UID: 0x804AD640
[PS4][DEBUG]: [PS4LINK] Created ps4link_requests_sock: 82
[PS4][DEBUG]: [PS4LINK] bind to ps4link_requests_sock done
[PS4][DEBUG]: [PS4LINK] Ready for connection 1
[PS4][DEBUG]: [PS4LINK] Waiting for connection
[PS4][DEBUG]: [PS4LINK] Command Thread Started.
[PS4][DEBUG]: [PS4LINK] Created ps4link_commands_sock: 84
[PS4][DEBUG]: [PS4LINK] Command listener waiting for commands...
# Thread number and sockets number can be different in your environment

#Check that no bind error present in log

# If all is fine you can use connect command in ps4sh to connect and send commands 
```



