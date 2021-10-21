#!/bin/bash

ErrFunc()
{
	echo Compile $1 Err!
	exit
}

#Create Makefile
echo create makefile 
HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`
if [ $HOST_ARCH = "x86_64" ]; then
  #qmake根据实际路径修改
  QMAKE=/opt/Qt5.5.1/5.5/gcc_64/bin/qmake
  $QMAKE ShowImage_m64x86.pro
elif [ $HOST_ARCH = "i686" ]; then
  #qmake根据实际路径修改
  QMAKE=/opt/Qt5.5.1/5.5/gcc/bin/qmake
  $QMAKE ShowImage_m32x86.pro
elif [ $HOST_ARCH = "aarch64" ]; then
  cp depends/Jetson_TX2/libfreetype.so ../../../lib/
  #qmake根据实际路径修改
  QMAKE=/home/nvidia/qt/qt-everywhere-opensource-src-5.5.1/qtbase/bin/qmake
  $QMAKE ShowImage_Jetson_TX2.pro
fi

#Compile Demo
if [ $HOST_ARCH = "x86_64" ]; then
  platform=m64x86-linux
elif [ $HOST_ARCH = "i686" ]; then
  platform=m32x86-linux
elif [ $HOST_ARCH = "aarch64" ]; then
  platform=Jetson_TX2-linux
fi
echo complie $platform
make clean
make -i PLATFORM=$platform -j4 || ErrFunc $platform




