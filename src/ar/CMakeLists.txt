find_package(OpenCV REQUIRED)

add_executable(ar_demo
  ARTester.cpp
  Detector.cpp
  Tag.cpp)
target_link_libraries(ar_demo ${OpenCV_LIBS})
