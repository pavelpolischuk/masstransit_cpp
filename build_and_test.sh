export GENERATOR="Visual Studio 14 2015 Win64"
export BOOST="/c/Projects/boost_1_61_0"
export INSTALL_PREFIX=`pwd`/_install

# install pre-reqs
mkdir -p _prereqs
pushd _prereqs

git clone --single-branch https://teamcityhost/gitlab/cpp_common/boost_di.git
export BOOST_DI_DIR=`pwd`/boost_di


git clone --single-branch https://teamcityhost/gitlab/cpp_common/rabbitmq-c.git
cd rabbitmq-c
export RABBITMQC_DIR=${INSTALL_PREFIX}
export RABBITMQC_SSL=ON
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${RABBITMQC_DIR} -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DBUILD_TOOLS=OFF -DENABLE_SSL_SUPPORT=${RABBITMQC_SSL} .
cmake --build . --target install

cd ..

git clone --single-branch https://teamcityhost/gitlab/cpp_common/simple_amqp_client.git
cd simple_amqp_client
export SimpleAmqpClient_DIR=${INSTALL_PREFIX}
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${SimpleAmqpClient_DIR} -DBUILD_TESTS=OFF -DRabbitmqc_DIR=${RABBITMQ_C_DIR} -DBOOST_ROOT=${BOOST} -DRabbitmqc_SSL_ENABLED=${RABBITMQC_SSL} .
cmake --build . --target install

popd

# before script
mkdir _build
cd _build
pwd

# run the build script
cmake -G "${GENERATOR}" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=ON -DSimpleAmqpClient_DIR=${SimpleAmqpClient_DIR} -DBOOST_DI_DIR=${BOOST_DI_DIR} -DBOOST_ROOT=${BOOST} ..
cmake --build .

export PATH="${PATH}:${BOOST}/stage/lib:${INSTALL_PREFIX}/bin:${OPENSSL_PATH}"
./Debug/masstransit_cpp_tests
