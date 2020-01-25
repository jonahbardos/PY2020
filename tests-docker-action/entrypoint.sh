echo "test"
apt-get install clang-format
git clone https://github.com/Sarcasm/run-clang-format.git
./run-clang-format.py -r ./src
git clone https://github.com/catchorg/Catch2.git
cd ./Catch2
cmake -B./build -H. -DBUILD_TESTING=OFF
cmake --build ./build/ --target install
cd ..
mkdir ./build
cd build
cmake ../src
cd ..
cmake --build ./build
./build/tests
