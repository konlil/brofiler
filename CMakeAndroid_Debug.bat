cd ./MyBuildAndroid
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=CMake\Toolchain\Android_ndk10.cmake --debug-output ^
	-DCMAKE_MAKE_PROGRAM="[your_ndk_dir]\\win32\\android-ndk-r10e\\prebuilt\\windows-x86_64\\bin\\make.exe"