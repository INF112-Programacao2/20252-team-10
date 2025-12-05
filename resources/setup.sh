#!/bin/bash

echo "Instalando dependências..."

sudo apt-get update

sudo apt-get install lsb-release

sudo apt-get install gnupg

sudo apt-get install wget

mkdir temp

cd temp

sudo wget https://dev.mysql.com/get/mysql-apt-config_0.8.36-1_all.deb

sudo dpkg -i mysql-apt-config_0.8.36-1_all.deb

sudo apt-get update

sudo apt install libmysqlcppconn-dev

sudo apt-get install mysql-server

cd ..

rm -rf temp

echo "Fim"
