#!/bin/sh
echo "downloading and installing almost all files necessary for NorthernBites development. user input is necessary for the first several install. you will be notified when the installation wil proceed unattended"

echo "Please enter your Bowdoin username:"
read USER

echo "Please enter your Bowdoin password"
read -s PASSWORD

echo "installing webots"
test -e webots-6.2.4.dmg || curl -Ok http://www.cyberbotics.com/cdrom/mac/webots/webots-6.2.4.dmg
hdiutil attach webots-6.2.4.dmg
sudo cp -R /Volumes/Webots/Webots /Applications/Webots

echo "setting up webots permissions and symlinks"
pushd /Applications/Webots/projects/contests/robotstadium/controllers/

sudo chmod 777 nao_team_1/
mv nao_team_1/nao_team_1 nao_team_1/nao_team_1_BACKUP
sudo ln -s nao_team_1/bin/man nao_team_1/nao_team_1
popd

echo "install cmake"
test -e cmake-2.6.4-Darwin-universal.dmg || curl -Oku $USER:$PASSWORD https://robocup.bowdoin.edu/files/software/nao/cmake-2.6.4-Darwin-universal.dmg
hdiutil attach cmake-2.6.4-Darwin-universal.dmg
sudo installer -pkg /Volumes/cmake-2.6.4-Darwin-universal/cmake-2.6.4-Darwin-universal.pkg/ -target /
hdiutil detach /Volumes/cmake-2.6.4-Darwin-universal

echo "install git"
test -e git-1.7.1-intel-leopard.dmg || curl -Ok http://git-osx-installer.googlecode.com/files/git-1.7.1-intel-leopard.dmg
hdiutil attach git-1.7.1-intel-leopard.dmg
pushd /Volumes/Git\ 1.7.1\ Intel\ Leopard/
sudo ./uninstall.sh
sudo installer -pkg git-1.7.1-intel-leopard.pkg -target /
sudo ./setup\ git\ PATH\ for\ non-terminal\ programs.sh
popd
hdiutil detach /Volumes/Git\ 1.7.1\ Intel\ Leopard/

echo "installing git completion"
curl -Oku $USER:$PASSWORD https://robocup.bowdoin.edu/files/software/git-completion.sh
sudo mv git-completion.sh /usr/local/git/share

echo "downloading ant"
test -e apache-ant-1.8.1-bin.tar.gz || curl -Ok http://mirror.its.uidaho.edu/pub/apache/ant/binaries/apache-ant-1.8.1-bin.tar.gz
echo "unpacking ant"
rm -rf apache-ant-1.8.1 && tar - xjf apache-ant-1.8.1-bin.tar.gz
echo "moving ant into place"
mv apache-ant-1.8.1/ /usr/local/ant

echo "setting global path variables"
sudo chmod a+w /etc/bashrc
echo "export WEBOTS_HOME=/Applications/Webots" >> /etc/bashrc
echo "export PS1='\[\033[01;32m\]\u@\h\[\033[01;34m\] \w\[\033[31m\]`git branch 2>/dev/null|cut -f2 -d\* -s`\[\033[01;34m\]$\[\033[00m\] '" >> /etc/bashrc
echo "PATH=$PATH:/usr/local/git/bin:/sw/bin/" >> /etc/bashrc
echo "source git-completion.sh" >> /etc/bashrc
echo "export ANT_HOME=/usr/local/ant" >> /etc/bashrc
echo "export PATH=${PATH}:${ANT_HOME}/bin" >> /etc/bashrc
sudo chmod a-w /etc/bashrc

echo "the installation will now proceed unattended"
echo "downloading zlib, dependency for ccache"
test -e zlib-1.2.5.tar.gz || curl -Ok http://zlib.net/zlib-1.2.5.tar.gz
echo "unpacking zlib"
rm -rf zlib-1.2.5 && tar -xjf zlib-1.2.5.tar.gz || exit 1
echo "installing zlib"
pushd zlib-1.2.5 && \
    ./configure && make test && make install && popd || exit 1

echo "downloading ccache"
test -e ccache-3.0 || curl -Ok http://samba.org/ftp/ccache/ccache-3.0.tar.gz
echo "unpacking ccache"
rm -rf ccache-3.0 && tar -xjf ccache-3.0.tar.gz || exit 1
echo "installing ccache"
pushd ccache-3.0 && \
    ./configure && make test && make install && popd || exit 1

echo "downloading naoqi"
test -e mac-nao-sdk-1.6.m1.tar.bz2 || curl -Oku $USER:$PASSWORD https://robocup.bowdoin.edu/files/software/nao/NaoQi/1.6.m1/mac-nao-sdk-1.6.m1.tar.bz2

echo "Unpacking and installing SDK"
if [ ! -e /usr/local/nao-1.6 ]; then
  mkdir -p /usr/local/nao-1.6
tar --strip 1 -C /usr/local/nao-1.6 -xzf mac-nao-sdk-1.6.m1.tar.bz2

echo "downloading Boost"
test -e boost_1_43_0.tar.gz || curl -Ok http://downloads.sourceforge.net/project/boost/boost/1.43.0/boost_1_43_0.tar.gz

echo "unpacking boost"
rm -rf boost_1_43_0 && tar -xjf boost_1_43_0.tar.gz || exit 1
echo "building boost"
pushd boost_1_43_0 && \
    ./bootstrap.sh --with-libraries=python,signals && \
    ./bjam address-model=32_64 install && \
    popd || exit 1
