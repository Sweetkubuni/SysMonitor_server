how to compile

first install on linux the following libraries

sudo apt-get install rapidjson-dev
apt-get install libevent-dev

g++ -Wall -std=c++11 server.cpp -levent -o server