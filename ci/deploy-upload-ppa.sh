#!/bin/bash
set -x

cmake --version
mkdir build
mv doc/sphinx/build doc/sphinx/built
cd build

if [ -z "DISTRIBS" ]
then
	echo "Please define the 'DISTRIBS' environment variable."
	exit 1
fi


if [ -z "$CXX" ]
then
	echo "Please define the 'CXX' environment variable."
	exit 1
fi

if [ -z "$THREADS" ]
then
	echo "The 'THREADS' environment variable is not set, default value = 1."
	THREADS=1
fi

if [ -z "$NAME" ]
then
	echo "The 'NAME' environment variable is not set, default value = 'build_deploy_upload_ppa'."
	NAME="build_deploy_upload_ppa"
fi

if [ -z "$LFLAGS" ]
then
	cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER=$CXX \
	         -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$CFLAGS" \
	         $CMAKE_OPT -DCMAKE_INSTALL_PREFIX="$NAME" \
	         -DAFF3CT_UPLOAD_PPA="ON" \
	         -DAFF3CT_PPA_DISTRIB="$DISTRIBS"
	rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
else
	cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER=$CXX \
	         -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$CFLAGS" \
	         -DCMAKE_EXE_LINKER_FLAGS="$LFLAGS" \
	         $CMAKE_OPT -DCMAKE_INSTALL_PREFIX="$NAME" \
	         -DAFF3CT_UPLOAD_PPA="ON"\
	         -DAFF3CT_PPA_DISTRIB="$DISTRIBS"
	rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
fi

make -j $THREADS -k
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
