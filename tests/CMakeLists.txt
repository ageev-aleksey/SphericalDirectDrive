cmake_minimum_required(VERSION 3.0.2)

project(TEST_SPPU_SERIAL)

set(GTEST_DIR ${PROJECT_SOURCE_DIR}/googletest)
set(TESTS_SOURCE
	 ${PROJECT_SOURCE_DIR}/src/test_buffer.cpp
	 ${PROJECT_SOURCE_DIR}/src/test_message.cpp
	 ${PROJECT_SOURCE_DIR}/src/test_package.cpp
	 ${PROJECT_SOURCE_DIR}/src/test_field.cpp
	 ${PROJECT_SOURCE_DIR}/src/test_mutex.cpp
	  ${PROJECT_SOURCE_DIR}/src/test_EventLoop.cpp)

if(${MSVC})
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(${GTEST_DIR})

add_executable(${PROJECT_NAME} ${TESTS_SOURCE})
target_link_libraries(${PROJECT_NAME} PUBLIC SPPU_SERIAL gtest gtest_main)