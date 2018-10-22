#!/bin/sh
sudo apt-get update
sudo apt-get install e2fslibs-dev -y
gcc -o age age.c -lext2fs
gcc -o age1 age1.c -lext2fs
sudo chown root ./age
sudo chmod u+s ./age
sudo chown root ./age1
sudo chmod u+s ./age1
echo "-------------------------------------------------------------"
echo "Usage ./age <filename> - prints age of a given file"
echo "Usage ./age1 <filename> - prints creation, modification, and access times of a given file or all the files in the given directory"
#etc.
