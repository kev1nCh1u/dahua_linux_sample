#!/bin/bash

ErrFunc()
{
	echo Compile $1 Err!
	exit
}

#确认编译平台
if [ "$1" = "" ]; then
	compilePlatform=x86
elif [ $1 = "armhf" ] || [ $1 = "aarch64" ]; then
	compilePlatform=$1
else
	echo "ERROR: Arguments is invalid."
	exit 1
fi
 
if [ $compilePlatform = "armhf" ]; then
 
	 #qmake根据实际路径修改
	QMAKE=/cross_jenkins/arm-gnueabihf-qt-5.5.1/bin/qmake
	$QMAKE ShowImage_armhf.pro

	#编译
	echo complie $compilePlatform
	make CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ AR=arm-linux-gnueabihf-ar STRIP=arm-linux-gnueabihf-strip clean;make CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ AR=arm-linux-gnueabihf-ar STRIP=arm-linux-gnueabihf-strip

elif [ $compilePlatform = "aarch64" ]; then

	#qmake根据实际路径修改
	QMAKE=/opt/arm64-qt-5.5.1/bin/qmake
	$QMAKE ShowImage_aarch64.pro
	
	#编译
	echo complie $compilePlatform
	make CC=aarch64-unknown-linux-gnu-gcc CXX=aarch64-unknown-linux-gnu-g++ AR=aarch64-unknown-linux-gnu-ar STRIP=aarch64-unknown-linux-gnu-strip clean;make CC=aarch64-unknown-linux-gnu-gcc CXX=aarch64-unknown-linux-gnu-g++ AR=aarch64-unknown-linux-gnu-ar STRIP=aarch64-unknown-linux-gnu-strip
	
else

#Create Makefile
echo create makefile 
HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`
if [ $HOST_ARCH = "x86_64" ]; then
  #qmake根据实际路径修改
  QMAKE=/opt/Qt5.5.1/5.5/gcc_64/bin/qmake
  $QMAKE ShowImage_m64x86.pro
else
  #qmake根据实际路径修改
  QMAKE=/opt/Qt5.5.1/5.5/gcc/bin/qmake
  $QMAKE ShowImage_m32x86.pro
fi

#Compile Demo
if [ $HOST_ARCH = "x86_64" ]; then
  platform=m64x86-linux
elif [ $HOST_ARCH = "i686" ]; then
  platform=m32x86-linux
else
  platform=$1
fi
echo complie $platform
make clean;make

fi


