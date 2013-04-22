#!/bin/bash
: ${QNX_HOST?" Need to set QNX_HOST. Run 'source /path/to/bbndk/bbndk-env.sh'"}

cd build
rm -rf *
PATH=$QNX_HOST/usr/bin/:$PATH \
CC=$QNX_HOST/usr/bin/cc \
CXX=$QNX_HOST/usr/bin/qcc \
cmake ../
make


# qcc -o src/main/dummy/SDL_dummy_main.o ../src/main/dummy/SDL_dummy_main.c -V4.6.3,gcc_ntoarmv7le -w1 -shared -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/TouchControlOverlay/public -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/SDL/include -I/Applications/bbndk/target_10_1_0_1020/qnx6/../target-override/usr/include -D__PLAYBOOK__ -D_FORTIFY_SOURCE=2 -c -O0 -g -fstack-protector-strong

# qcc -o src/USDeviceTime_posix.o ../../../src/uslscore/USDeviceTime_posix.cpp -V4.6.3,gcc_ntoarmv7le_cpp -w1 -shared -I/Applications/bbndk/target_10_1_0_1020/qnx6/usr/include/freetype2 -I/Applications/bbndk/target_10_1_0_1020/qnx6/../target-override/usr/include -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/../src/ -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/../3rdparty/lua-5.1.3/src/ -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/../3rdparty/ooid-0.99/ -I/Users/nictuku/Documents/development/zipline/moai-dev/momentics/../3rdparty/ -D_FORTIFY_SOURCE=2 -c -O2 -fstack-protector-strong -frecord-gcc-switches


#qcc -o MoaiSample src/main.o src/bbutil.o -lthird-party -lmoaicore -luntz -lmoaiext-untz -luslscore -laku -lzlcore -lbps -lscreen -lEGL -lGLESv2 -lfreetype -lpng -lm -lcurl -lsqlite3 -lssl -lSDL -lGLESv1_CM -lz -lsocket -lcrypto -V4.6.3,gcc_ntoarmv7le_cpp -w1 -lang-c++ -g -Wl,-z,relro -Wl,-z,now -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/third-party/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/moaicore/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/untz/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/moaiext-untz/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/uslscore/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/aku/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/zlcore/Device-Debug -L/Users/nictuku/Documents/development/zipline/moai-dev/momentics/SDL/Device-Debug -L/Applications/bbndk/target_10_1_0_1020/qnx6/../target-override/armle-v7/lib -L/Applications/bbndk/target_10_1_0_1020/qnx6/../target-override/armle-v7/usr/lib


#qcc    -D__QNX__ -Wl,-search_paths_first -Wl,-headerpad_max_install_names   CMakeFiles/blackberry-host.dir/Users/nictuku/Documents/development/zipline/moai-dev-cmake/src/hosts/BlackBerry10/src/main.cpp.o CMakeFiles/blackberry-host.dir/Users/nictuku/Documents/development/zipline/moai-dev-cmake/src/hosts/BlackBerry10/src/bbutil.c.o  -o blackberry-host  -lscreen 

#/Applications/bbndk/host_10_1_0_132/darwin/x86/usr/bin/qcc  -V4.6.3,gcc_ntoarmv7le_cpp -D__QNX__ -Wl,-search_paths_first -Wl,-headerpad_max_install_names -w1 -lang-c++ -g -Wl,-z,relro -Wl,-z,now CMakeFiles/blackberry-host.dir/Users/nictuku/Documents/development/zipline/moai-dev-cmake/src/hosts/BlackBerry10/src/main.cpp.o CMakeFiles/blackberry-host.dir/Users/nictuku/Documents/development/zipline/moai-dev-cmake/src/hosts/BlackBerry10/src/bbutil.c.o  -o blackberry-host  -lscreen