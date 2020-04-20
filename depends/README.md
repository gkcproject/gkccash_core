# Usage

## Ubuntu 18.04 or 16.04 step by step

#### 1、Install System dependencies
    sudo apt install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git libgl1-mesa-dev python cmake
#### 2、Change into depends directory
    cd depends
#### 3、Just Make (N is a number of how many threads you want to use)
    make -jN
#### 4、Change into top directory
    cd ../
#### 5、Perform autogen script
    ./autogen.sh
#### 6、Perform configuration script
    ./configure --prefix=`pwd`/depends/x86_64-pc-linux-gnu
#### 7、Just Make
    make -jN
#### 8、Find your executables in directories: src/, src/qml/build/, etc. And be aware static executables on Ubuntu is not supported by now, because of missing static lib of libGL.a in libgl1-meas-dev package this can be fixed by compile libgl1-meas-dev from source code.  
#### Do it, if you have time, thanks!



## Windows step by step

    For Windows, we do succeed to compile from source in msys2 environment, but believe me you can not tolerate the compile speed of msys2.  
    To make the best use of your hardware and save time, we suggest you follow the steps below:

#### 1、Prepare both ubuntu 18.04 and windows 10 environment on real or virtual machine.

#### 2、Prepare ubuntu 18.04 environment and then install System dependencies
    sudo apt install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git python g++-mingw-w64-x86-64

#### 3、Change into depends directory
    cd depends
    
#### 4、Change mingw threads to posix
    sudo update-alternatives --config x86_64-w64-mingw32-g++  
    choose posix

#### 5、Just Make (N is a number of how many threads you want to use)
    make HOST=x86_64-w64-mingw32 -jN

#### 6、Prepare winidows 10 environment and then install mingw64 and DirectXSDK:
    a) Download mingw64 from:  
        https://osdn.net/projects/mingw/releases/  
        or  
        https://sourceforge.net/projects/mingw-w64/  
    b) Install mingw64 with configuration( update when ubuntu updates):  
        Version: 7.3.0  
        Architecture: x86_64  
        Threads: posix  
        Exception: seh  
        Build revision: 0  
    c) Install mingw64  
    d) Copy openssl libs and headers compiled in step 4 from ubuntu 18.04 to windows 10 mingw directory  
        where are the files located:   
         depends/86_64-w64-mingw32/include/openssl     copy this full directory  
         depends/86_64-w64-mingw32/lib/libcrypto.a  
         depends/86_64-w64-mingw32/lib/libssl.a  

        where are the directorys to paste:  
         if you choose default install options, it is most likely in C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\x86_64-w64-mingw32  
         copy libs and openssl directory to include and lib separately   


    e) Download DirectXSDK from:   
        https://www.microsoft.com/en-us/download/details.aspx?id=6812  
    f) Install DirectXSDK  

#### 7、Download Qt 5.10.1 source code and compile from source 
    a) Download Qt 5.10.1 source code from:   
        http://download.qt.io/archive/qt/5.10/5.10.1/single/qt-everywhere-src-5.10.1.zip  
    b) Upzip,   
       don't forget to change "where you want to install" to a real diretory,  
       and you must create this directory first,  
       make sure openssl libs and headers were copied to mingw64 install directory as descripted in section 6.d,  
    c) Open mingw cmd by double click mingw-w64.bat in where you install mingw (if you choose default install options, it is most likely in C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0),  
    d) Change into Qt source code directory and configure with options:  
        Call configure.bat -debug-and-release -static -opensource -confirm-license -c++std c++14 -angle -opengl es2 -nomake example -nomake tests -skip wayland -skip purchasing -skip serialbus -skip qtserialport -skip qtgamepad -skip qtscript -skip qtdoc -qt-zlib -qt-pcre -qt-libpng -qt-freetype -openssl-linked OPENSSL_LIBS="-lssl -lcrypto -lgdi32" -prefix "where you want to install" -no-icu -no-iconv -platform win32-g++  
    e) Just Make  
        mingw-make -jN  
    f) After Make is completed run install:  
        mingw-make install   

#### 8、Replace Ubuntu Mingw libs and header files with those installed on windows:
    a) where are the files located:   
       if you choose default install options, it is most likely in C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\x86_64-w64-mingw32,  
       be careful this is a different directory from those mentioned above  
    b) copy lib and include directory to   
       where are the directorys to paste:  
       on Ubuntu 18.04 , open path /usr/x86_64-w64-mingw32, delete lib and include directory and paste those you copied from windows. 

#### 9、Replace Ubuntu qml depend files with those compiled on windows in step 7:
     a) where are the files located:   
        Qt files were installed into directory you defined in section 7.d with -prefix "where you want to install" option,
        the directory looks like this:  
          bin\  
          doc\  
          include\  
          lib\  
          mkspecs\  
          ...  
          translations\  

       copy all except two: mkspecs\ and bin\ directories to   
       where are the directorys to paste:  
       on Ubuntu 18.04 , open path depends\x86_64-w64-mingw32\qml delete all except two: mkspecs\ and bin\ directories and paste those you copied from windows.   

#### 10、Congratulations!  
    After several miserable steps, you setup an environment to cross compile windows executables. Why so bother? Yes, It is.   
    But this is the fastest way and please refer to https://wiki.qt.io/Qt_5_on_Windows_ANGLE_and_OpenGL.  
    If you want your qml executables run with different Windows versions & graphics cards with openGL enabled which provides amazing image,animation and dynamic effects,  
    you'd better choose ANGLE as backend. But compile qt with ANGLE needs DirectXSDK which is not available on Ubuntu, and at the meantime, mingw tool-chain on Ubuntu 18.04 is lack of some libs and header files needed by ANGLE.  
    Ok, long story! But we succeed in the end yeah? Now:  

      a) Change into gkc source code top directory  
         cd "to where that is"  
      b) Perform autogen script  
         ./autogen.sh  
      c) Perform configuration script  
         ./configure --prefix=`pwd`/depends/x86_64-w64-mingw32  
      d) Just Make  
         make -jN  
      e) Find your executables in directories: src/, src/qml/build/release, etc. Enjoy!  








