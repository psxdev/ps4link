PS4LINK FOR PS4
=================
 
===================
 What does this do?
===================
 
  ps4link is a library for PS4 to communicate and use host file system with ps4client host tool. It is the same method that we used in ps2dev days, so basically it is the same protocol than ps2link and ps2client have been using since 2003.
  
  Functions availables are defined like native sce functions so it is easy for homebrew developer to use these new functions:
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
  
  
==================
  How do I use it?
==================

 1) Compile 
 
  Like other examples in PS4-SDK
  edit source/main.c and change your mac/linux server ip and your base directory for example host0:/usr/local
  
  ```
  cd psp2link
  make
  ```

 2) Run sample on your ps4 when html button freeze you are ready to run pc/mac client part
   
  ```
  ps4client -h ipofyourps4 listen 
  ```
  
 
 You will see logs in terminal window output from your PlayStation 4 using libdebugnet udp log feauture 
 
 Sample is showing only open, read , write and directory entry list operations, you have full io r/w access to your hard disk
 
 ```
 ./ps4client -h yourps4ip listen
 Client connected from xxx.xxx.xxx.xxx port: 49859
  Client reconnected
 sock ps4link_fileio set 86 connected 1
 Waiting for connection
 ps4link initialized and connected from pc/mac
 file open req (host0:/usr/local/ps4dev/test.txt, 0 0)
 Opening /usr/local/ps4dev/test.txt flags 0
 Open return 5
 file open reply received (ret 5)
 file lseek req (fd: 5)
 32 result of lseek 0 offset 2 whence
 ps4link_lseek_file: lseek reply received (ret 32)
 file lseek req (fd: 5)
 0 result of lseek 0 offset 0 whence
 ps4link_lseek_file: lseek reply received (ret 0)
 ps4link_read_file: Reply said there's 32 bytes to read (wanted 32)
 Content of file: Hello world from txt file on pc
 ps4link_file: file close req (fd: 5)
 ps4link_close_file: close reply received (ret 0)
 file open req (host0:/usr/local/ps4dev/test1.txt, 101 0)
 Opening /usr/local/ps4dev/test1.txt flags 601
 Open return 5
 file open reply received (ret 5)
 file write req (fd: 5)
 wrote 21 bytes (asked for 21)
 ps4LinkWrite wrote 21 bytes
 ps4link_file: file close req (fd: 5)
 ps4link_close_file: close reply received (ret 0)
 dir open req (host0:/usr/local/ps4dev)
 dir open reply received (ret 0)
 List entries
 dir read req (0)
 dir read reply received (ret 1)
 . [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 .. [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 .DS_Store [FILE]
 dir read req (0)
 dir read reply received (ret 1)
 bigbsd [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 core [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 doc [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 git [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 mount_bigbsd.sh [FILE]
 dir read req (0)
 dir read reply received (ret 1)
 nuevo [DIR]
 dir read req (0)
 dir read reply received (ret 1)
 test.txt [FILE]
 dir read req (0)
 dir read reply received (ret 1)
 test1.txt [FILE]
 dir read req (0)
 dir read reply received (ret 0)
 ps4link_file: dir close req (fd: 0)
 dir close reply received (ret 0)
 Aborting server_requests_sock
 sceNetAccept error (0x80410104)
 exit thread requests
 closing fileio_sock
 closing server_request_sock
 ```
 
 3) ready to have a lot of fun :P
 
===================
 What next?
===================
  
  Well, this library can be extended adding differents commands like ps2link was defined.
  It is a internal tool to let me debug camera code and reverse data structures saving to mac with an easy and well known interface for file io. 
  
===========================
  Credits
===========================
  
  Special thanks goes to:
  
  - ps2dev old comrades. 
  - All people who collaborated in PS4SDK