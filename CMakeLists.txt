CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (gr-isatec CXX)
#SET (CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wall -Werror -march=native -mtune=native -O3 -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer")
SET (CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wall -Werror -march=native -mtune=native -O0 -fvisibility=hidden -fvisibility-inlines-hidden -ggdb")

INCLUDE_DIRECTORIES ("include")

FIND_PACKAGE (Boost 1.55.0 REQUIRED COMPONENTS filesystem system)
SET(GR_REQUIRED_COMPONENTS RUNTIME)
FIND_PACKAGE(Gnuradio "3.7.0" REQUIRED)

# GNU Radio Module

ADD_LIBRARY (gnuradio-isatec SHARED
	src/MSW.cpp
	src/WRDS.cpp
	src/MSW2.cpp
	src/Analyzer.cpp
	src/Scrambler.cpp
	src/ScramblerGroup.cpp
	src/GuidedScrambler_impl.cpp
	src/SymbolGenerator_impl.cpp
	src/PulseGenerator_impl.cpp)
TARGET_INCLUDE_DIRECTORIES ( gnuradio-isatec PRIVATE
	"${Boost_INCLUDE_DIRS}"
	"${GNURADIO_RUNTIME_INCLUDE_DIRS}"
	"${GNURADIO_ALL_INCLUDE_DIRS}")
TARGET_LINK_LIBRARIES ( gnuradio-isatec PUBLIC
	"${Boost_LIBRARIES}"
	"${GNURADIO_ALL_LIBRARIES}")
INSTALL (DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/include/gr-isatec"
	DESTINATION "include")
INSTALL (TARGETS gnuradio-isatec LIBRARY DESTINATION lib)

# Guided Scrambling unit tests

ADD_EXECUTABLE ( guidedScramblingTest
	src/MSW.cpp
	src/WRDS.cpp
	src/MSW2.cpp
	src/Analyzer.cpp
	src/Scrambler.cpp
	src/ScramblerGroup.cpp
	src/GuidedScrambler_impl.cpp
	src/Word_test.cpp
	src/Analyzer_test.cpp
	src/Scrambler_test.cpp
	src/ScramblerGroup_test.cpp
	src/GuidedScrambler_test.cpp
	src/test.cpp
	src/Descrambler_test.cpp
	src/Descrambler_impl.cpp)
TARGET_INCLUDE_DIRECTORIES ( guidedScramblingTest PRIVATE
	"${Boost_INCLUDE_DIRS}"
	"${GNURADIO_RUNTIME_INCLUDE_DIRS}"
	"${GNURADIO_ALL_INCLUDE_DIRS}")
TARGET_LINK_LIBRARIES ( guidedScramblingTest PUBLIC
	"${Boost_LIBRARIES}"
	"${GNURADIO_ALL_LIBRARIES}")

# Applications

FIND_PACKAGE(Qt4)
SET(GR_REQUIRED_COMPONENTS QTGUI BLOCKS ANALOG UHD)
FIND_PACKAGE(Gnuradio "3.7.6")
FIND_PACKAGE(UHD)
IF(Qt4_FOUND AND Gnuradio_FOUND AND UHD_FOUND)
	# Guided Scrambling demonstration

	QT4_WRAP_CPP(guidedScramblingDemo_HEADERS_MOC "include/guidedScramblingDemo.hpp")
	QT4_WRAP_UI(guidedScramblingDemo_FORMS_HEADERS "ui/guidedScramblingDemo.ui")
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	ADD_EXECUTABLE(guidedScramblingDemo
		"src/guidedScramblingDemo.cpp"
		${guidedScramblingDemo_HEADERS_MOC}
		${guidedScramblingDemo_FORMS_HEADERS})
	TARGET_INCLUDE_DIRECTORIES ( guidedScramblingDemo PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${Boost_INCLUDE_DIRS}"
		"${UHD_INCLUDE_DIRS}"
		"${GNURADIO_RUNTIME_INCLUDE_DIRS}"
		"${GNURADIO_ALL_INCLUDE_DIRS}")
	TARGET_LINK_LIBRARIES ( guidedScramblingDemo PUBLIC
		gnuradio-isatec
		"${Boost_LIBRARIES}"
		"${GNURADIO_ALL_LIBRARIES}"
		"${UHD_LIBRARIES}"
		"${QT_LIBRARIES}")
	ADD_DEPENDENCIES ( guidedScramblingDemo gnuradio-isatec)
ENDIF()

FIND_PACKAGE(Qt4)
SET(GR_REQUIRED_COMPONENTS QTGUI BLOCKS ANALOG DIGITAL AUDIO FILTER UHD)
FIND_PACKAGE(Gnuradio "3.7.6")
FIND_PACKAGE(UHD)
IF(Qt4_FOUND AND Gnuradio_FOUND AND UHD_FOUND)
   # General Audio Receiver

   QT4_WRAP_CPP(audioReceiver_HEADERS_MOC "include/audioReceiver.hpp")
   QT4_WRAP_UI(audioReceiver_FORMS_HEADERS "ui/audioReceiver.ui")
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	ADD_EXECUTABLE(audioReceiver
		"src/audioReceiver.cpp"
		${audioReceiver_HEADERS_MOC}
		${audioReceiver_FORMS_HEADERS})
	TARGET_INCLUDE_DIRECTORIES ( audioReceiver PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${Boost_INCLUDE_DIRS}"
		"${UHD_INCLUDE_DIRS}"
		"${GNURADIO_RUNTIME_INCLUDE_DIRS}"
		"${GNURADIO_ALL_INCLUDE_DIRS}")
	TARGET_LINK_LIBRARIES ( audioReceiver PUBLIC
		"${Boost_LIBRARIES}"
		"${GNURADIO_ALL_LIBRARIES}"
		"${UHD_LIBRARIES}"
		"${QT_LIBRARIES}")
ENDIF()

# Documentation

FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	ADD_CUSTOM_TARGET(doc ALL
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	INSTALL (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc" DESTINATION
		share/gr-isatec)
ENDIF(DOXYGEN_FOUND)
