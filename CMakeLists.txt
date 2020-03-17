cmake_minimum_required(VERSION 3.0.2)

project(SPPU_SERIAL)

set(SOURCES_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDES_DIR ${PROJECT_SOURCE_DIR}/includes)
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

find_package(Boost REQUIRED)
if(Boost_FOUND)
	message(STATUS "Boost found version: ${Boost_VERSION}" )
endif()

	set(SOURCE_EVENT_LOOP
			${SOURCES_DIR}/event_loop/IMessage.cpp
			${SOURCES_DIR}/event_loop/Connector.cpp
			${SOURCES_DIR}/event_loop/Event.cpp
			${SOURCES_DIR}/event_loop/EventLoop.cpp
			${SOURCES_DIR}/event_loop/Timer.cpp)

	set(SOURCE_SERIALIZATION 
	  #${SOURCES_DIR}/serialization/BinaryBuffer.cpp
      #${SOURCES_DIR}/sdd_protocol/reader_writer.cpp
	  ${SOURCES_DIR}/serialization/Field.cpp
	   ${SOURCES_DIR}/serialization/Message.cpp)
	
	set(SOURCE_SDD_PROTOCOL
			${SOURCES_DIR}/sdd_protocol/Package.cpp
			${SOURCES_DIR}/sdd_protocol/PackageError.cpp
			${SOURCES_DIR}/sdd_protocol/PackageFactory.cpp
			${SOURCES_DIR}/sdd_protocol/State.cpp
			${SOURCES_DIR}/sdd_protocol/Position.cpp
			${SOURCES_DIR}/sdd_protocol/Mode.cpp
			${SOURCES_DIR}/sdd_protocol/Light.cpp
			${SOURCES_DIR}/sdd_protocol/PackageBuffer.cpp)

	set(SOURCE_SERIAL
			${SOURCES_DIR}/serial/Mutex.cpp
			${SOURCES_DIR}/serial/MutexError.cpp
			${SOURCES_DIR}/serial/Serial.cpp
			${SOURCES_DIR}/serial/SerialError.cpp
			${SOURCES_DIR}/serial/AsyncSerial.cpp)
		#${SOURCES_DIR}/serial/Buffer.cpp)

	add_library(${PROJECT_NAME} STATIC ${SOURCE_SERIALIZATION}
				${SOURCE_SDD_PROTOCOL} ${SOURCE_SERIAL} ${SOURCE_EVENT_LOOP})
	
	add_executable(MAIN ${SOURCES_DIR}/main.cpp)
	target_link_libraries(MAIN ${PROJECT_NAME})

	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC 
								${INCLUDES_DIR}/serialization 
								${INCLUDES_DIR}/sdd_protocol
								${INCLUDES_DIR}/serial
								${INCLUDES_DIR}/event_loop
								${Boost_INCLUDE_DIRS})
	#add_library(proj::protocol ALIAS ${PROJECT_NAME})

	add_subdirectory(${TESTS_DIR})