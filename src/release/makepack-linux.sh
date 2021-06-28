version=$1
cp ../gkcd ./
cp ../gkc-cli ./
cp ../qml/README.md ./
cp ../qml/README_EN.md ./
cp ../../Guide.html ./
tar jcvf gkc-v${version}-linux.tar.bz2 gkcd gkc-cli README.md README_EN.md Guide.html

