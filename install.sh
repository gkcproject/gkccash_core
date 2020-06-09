#!/bin/sh


if [ ! -d "installDir/" ];then
  mkdir installDir
  mkdir installDir/lib
  mkdir installDir/include
fi

rm -rf installDir/lib/*
rm -rf installDir/include/*


rsync -vra --include-from=installInclude.list  --exclude "*" src/ installDir/include
rsync -vra --include-from=installLib.list  --exclude "*" src/ installDir/lib


mv installDir/lib/libzerocoin/*.a installDir/lib
mv installDir/lib/crypto/*.a installDir/lib
mv installDir/lib/cryptopp/*.a installDir/lib
mv installDir/lib/leveldb/*.a installDir/lib
mv installDir/lib/lz4/lib/*.a installDir/lib
mv installDir/lib/secp256k1/.libs/*.a installDir/lib
mv installDir/lib/univalue/.libs/*.a installDir/lib


rm -rf installDir/lib/libzerocoin
rm -rf installDir/lib/crypto
rm -rf installDir/lib/cryptopp
rm -rf installDir/lib/leveldb
rm -rf installDir/lib/lz4
rm -rf installDir/lib/secp256k1
rm -rf installDir/lib/univalue
