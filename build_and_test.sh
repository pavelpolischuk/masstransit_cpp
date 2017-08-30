# install pre-reqs
mkdir -p _prereqs
pushd _prereqs

export GENERATOR="Visual Studio 14 2015 Win64"
export BOOST="/c/Projects/boost_1_61_0"


git clone https://teamcityhost/gitlab/cpp_common/boost_di.git
export BOOST_DI_DIR=`pwd`/boost_di


git clone https://teamcityhost/gitlab/cpp_common/rabbitmq-c.git
cd rabbitmq-c
export RABBITMQC_DIR=`pwd`/../../_install
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${RABBITMQC_DIR} -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DBUILD_TOOLS=OFF -DENABLE_SSL_SUPPORT=OFF .
cmake --build . --target install

cd ..

git clone https://teamcityhost/gitlab/cpp_common/simple_amqp_client.git
cd simple_amqp_client
export SimpleAmqpClient_DIR=`pwd`/../../_install
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${SimpleAmqpClient_DIR} -DBUILD_TESTS=OFF -DRabbitmqc_DIR=${RABBITMQ_C_DIR} -DBOOST_ROOT=${BOOST} .
cmake --build . --target install

popd

# before script
mkdir _build
cd _build
pwd

# run the build script
export INSTALL_PREFIX=../_install
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=ON -DSimpleAmqpClient_DIR=${SimpleAmqpClient_DIR} -DBOOST_DI_DIR=${BOOST_DI_DIR} -DBOOST_ROOT=${BOOST} ..
cmake --build .

export PATH="${PATH}:${BOOST}/stage/lib:${INSTALL_PREFIX}/bin"
./Debug/masstransit_cpp_tests
