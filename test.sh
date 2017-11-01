# before script
mkdir _build
cd _build
pwd

# run the build script
export GENERATOR="Visual Studio 14 2015 Win64"
export BOOST="/c/Projects/boost_1_61_0"
export BOOST_DI_DIR=`pwd`/../_prereqs/boost_di
export INSTALL_PREFIX=`pwd`/../_install
export OPENSSL_PATH="/c/OpenSSL/bin"
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=ON -DSimpleAmqpClient_DIR=${INSTALL_PREFIX} -DBOOST_DI_DIR=${BOOST_DI_DIR} -DBOOST_ROOT=${BOOST} ..
cmake --build .

export PATH="${PATH}:${BOOST}/stage/lib:${INSTALL_PREFIX}/bin:${OPENSSL_PATH}"
./Debug/masstransit_cpp_tests
