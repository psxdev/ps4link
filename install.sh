#!/bin/bash
# Quick and Dirty auto-installation and configuration script for PS4link 
# on macOS Sierra (maybe universal, but I've only tested on macOS)
# "VIC BOSS!" - droogie 
#

#check for git,python
command -v git >/dev/null 2>&1 || { printf >&2 "git is required but not installed.\r\n"; exit 1; }
command -v python >/dev/null 2>&1 || { printf >&2 "python is required but not installed.\r\n"; exit 1; }

printf "\e[1;32m***** Installing PS4link *****\r\n\e[0m"
#Setup environment
printf "\e[0;32mEnter your PC IP\e[0m >\r\n"
read PC_IP
printf "\e[0;32mEnter your PS4 IP\e[0m >\r\n"
read PS4_IP
printf "\e[0;32mCreating our working directory:\e[0m [/usr/local/ps4dev/]\r\n"
printf "\e[0;32m(Administrator password needed for this task)\r\n"
sudo mkdir /usr/local/ps4dev/
printf "\e[0;32mSetting permissions:\e[0m [$LOGNAME:staff]\r\n"
sudo chown $LOGNAME:staff /usr/local/ps4dev/
cd /usr/local/ps4dev/
printf "\e[0;32mSetting environment variables:\e[0m [/usr/local/ps4dev/ps4dev.sh]\r\n\r\n"
PS4DEV=/usr/local/ps4dev;export PS4DEV
PATH=$PS4DEV/host-osx/x86_64-pc-freebsd9/bin:$PS4DEV/toolchain/bin:$PATH
ps4sdk=$PS4DEV/ps4sdk;export ps4sdk
printf "PS4DEV=/usr/local/ps4dev;export PS4DEV\nPATH=\$PS4DEV/host-osx/x86_64-pc-freebsd9/bin:\$PS4DEV/toolchain/bin:\$PATH\nps4sdk=\$PS4DEV/ps4sdk;export ps4sdk\n" > ps4dev.sh
chmod +x ps4dev.sh

#Setup clang
printf "\e[0;32mDownloading clang...\e[0m\r\n\r\n"
mkdir git && cd git
mkdir crossllvm && cd crossllvm
git clone http://llvm.org/git/llvm.git
cd llvm/tools
git clone http://llvm.org/git/clang.git
git clone http://llvm.org/git/lld.git
cd ../projects
git clone http://llvm.org/git/compiler-rt.git
cd ../..
mkdir build && cd build

printf "\r\n\e[0;32mConfiguring clang...\e[0m\r\n\r\n"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ps4dev/toolchain -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_DEFAULT_TARGET_TRIPLE=x86_64-scei-ps4 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCOMPILER_RT_BUILD_BUILTINS:BOOL=OFF -DCOMPILER_RT_BUILD_SANITIZERS:BOOL=OFF -DCOMPILER_RT_CAN_EXECUTE_TESTS:BOOL=OFF -DCOMPILER_RT_INCLUDE_TESTS:BOOL=OFF -DLLVM_TOOL_COMPILER_RT_BUILD:BOOL=OFF  -DCLANG_BUILD_EXAMPLES:BOOL=ON -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_C_FLAGS="-Wdocumentation -Wno-documentation-deprecated-sync" -DCMAKE_CXX_FLAGS="-std=c++11 -Wdocumentation -Wno-documentation-deprecated-sync" -DLLVM_LIT_ARGS="-v" ../llvm

printf "\r\n\e[0;32mBuilding clang...\e[0m\r\n\r\n"
cmake --build .
cmake --build . --target install

#Setup Binutils
printf "\r\n\e[0;32mDownloading Binutils...\e[0m\r\n\r\n"
cd $PS4DEV
wget https://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.gz
tar xfv binutils-2.25.tar.gz
cd binutils-2.25/
printf "\r\n\e[0;32mConfiguring Binutils...\e[0m\r\n\r\n"
./configure --prefix="$PS4DEV/host-osx" --target="x86_64-pc-freebsd9" \
  --disable-nls \
  --disable-dependency-tracking \
  --disable-werror \
  --enable-ld \
  --enable-lto \
  --enable-plugins \
  --enable-poison-system-directories
printf "\r\n\e[0;32mBuilding and Installing Binutils...\e[0m\r\n\r\n"
make && make install
cd ../
rm -rf binutils*
printf "\r\n\e[0;32mFix orbis-ld reference...\e[0m\r\n\r\n"
cd /usr/local/ps4dev/host-osx/x86_64-pc-freebsd9/bin/
cp ld orbis-ld

#Setup PS4SDK
printf "\r\n\e[0;32mDownloading PS4SDK...\e[0m\r\n\r\n"
cd /usr/local/ps4dev/git
git clone http://github.com/ps4dev/ps4sdk
cd ps4sdk
printf "\r\n\e[0;32mBuild PS4SDK...\e[0m\r\n\r\n"
make
printf "\r\n\e[0;32mSetup symbolic links...\e[0m\r\n\r\n"
mkdir /usr/local/ps4dev/ps4sdk/
cp -frv include $PS4DEV/ps4sdk
cp -frv make $PS4DEV/ps4sdk
cp -frv lib $PS4DEV/ps4sdk
cp crt0.s $PS4DEV/ps4sdk

#Setup PS4link
printf "\r\n\e[0;32mDownloading PS4link...\e[0m\r\n\r\n"
cd /usr/local/ps4dev/git
git clone http://github.com/psxdev/ps4link
cd ps4link

printf "\r\n\e[0;32mBuilding and Installing libdebugnet...\e[0m\r\n\r\n"
cd libdebugnet
make & make install
cd ..

printf "\r\n\e[0;32mBuilding and Installing libps4link...\e[0m\r\n\r\n"
cd libps4link
make & make install
cd ..

printf "\r\n\e[0;32mConfiguring and Building PS4link...\e[0m\r\n\r\n"
sed -i -e 's/ps4LinkInit(".*"/ps4LinkInit("'$PC_IP'"/' /usr/local/ps4dev/git/ps4link/ps4link/source/main.c
cd elfldr
sh copy_ps4link_sources.sh
cd ps4link
make

printf "\r\n\e[0;32mConfiguring and Building ps4sh...\e[0m\r\n\r\n"
sed -i -e 's/dst_ip\[16\] = ".*"/dst_ip[16] = "'$PS4_IP'"/' /usr/local/ps4dev/git/ps4link/ps4sh/src/ps4sh.c
cd /usr/local/ps4dev/git/ps4link/ps4sh
make

printf "\r\n\e[0;32mBuilding all samples...\e[0m\r\n\r\n"
cd /usr/local/ps4dev/git/ps4link/samples
cd sample
make
cp bin/sample.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
cd ../payload
make
cp bin/payload.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
cd ../ps4ftp
make
cp bin/ps4ftp.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
cd ../listproc
make
cp bin/listproc.elf /usr/local/ps4dev/git/ps4link/ps4sh/bin
printf "\r\nAll ps4link sample binaries are compiled and with ps4sh in /usr/local/ps4dev/git/ps4link/ps4sh/bin\r\n"
printf "Your elfldr is compiled and sitting in the local web directory in /usr/local/ps4dev/git/ps4link/elfldr/local\r\n"
printf "Host the elfldr via 'node server.js' and browse to http://$PC_IP:5350 on your PS4\r\n"
printf "Reference the README for more detailed instructions...\r\n"
printf "\e[1;32mDone.\r\n\e[0m\r\n"
