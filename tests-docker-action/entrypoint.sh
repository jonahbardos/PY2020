echo "test"
git clone https://github.com/catchorg/Catch2.git
cd ./Catch2
cmake -B./build -H. -DBUILD_TESTING=OFF
cmake --build ./build/ --target install
cd ..
mkdir ./build
cmake -B./build ./src
cmake --build ./build
./build/tests
