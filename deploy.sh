#!/bin/sh

VSERSION=rocketmq-client4cpp-linux-x64-1.0.0-beta1
CWD_DIR=`pwd`
TMP_DIR=`date +%Y%m%d_%H%M%S`

git clone https://github.com/RocketMQ-Community/rocketmq-client4cpp $TMP_DIR

DEPLOY_BUILD_HOME=$CWD_DIR/$TMP_DIR

#
# build json
#
cd $DEPLOY_BUILD_HOME/libs/jsoncpp-src-0.5.0/makefiles
make

#
# build client
#
cd $DEPLOY_BUILD_HOME/project
make


#
# deploy
#
cd $CWD_DIR
DEPLOY_INSTALL_HOME=$CWD_DIR/${TMP_DIR}_install/$VSERSION
mkdir -p $DEPLOY_INSTALL_HOME/lib
cp $DEPLOY_BUILD_HOME/bin/librocketmq64.so $DEPLOY_INSTALL_HOME/lib/
cp -r $DEPLOY_BUILD_HOME/include $DEPLOY_INSTALL_HOME/
cp -r $DEPLOY_BUILD_HOME/example $DEPLOY_INSTALL_HOME/

cd $CWD_DIR/${TMP_DIR}_install/

tar cvf ${VSERSION}.tar $VSERSION
gzip ${VSERSION}.tar
