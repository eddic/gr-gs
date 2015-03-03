CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (gr-isatec CXX)
SET (CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wall -Werror -march=core-avx2 -mtune=core-avx2 -O3 -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer")

INCLUDE_DIRECTORIES ("include")

FIND_PACKAGE (Boost 1.55.0 REQUIRED COMPONENTS filesystem system)
SET(GR_REQUIRED_COMPONENTS RUNTIME)
FIND_PACKAGE(Gnuradio "3.7.0" REQUIRED)

# GNU Radio Module

ADD_LIBRARY (gnuradio-isatec SHARED
	src/MSW.cpp
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
SET(GR_REQUIRED_COMPONENTS QTGUI BLOCKS ANALOG)
FIND_PACKAGE(Gnuradio "3.7.6")
IF(Qt4_FOUND AND Gnuradio_FOUND)
	# Guided Scrambling demonstration

	QT4_WRAP_CPP(guidedScrambler_HEADERS_MOC "include/gui.hpp")
	QT4_WRAP_UI(guidedScrambler_FORMS_HEADERS "ui/gs.ui")
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	ADD_EXECUTABLE(guidedScrambler
		"src/gui.cpp"
		${guidedScrambler_HEADERS_MOC}
		${guidedScrambler_FORMS_HEADERS})
	TARGET_INCLUDE_DIRECTORIES ( guidedScrambler PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${Boost_INCLUDE_DIRS}"
		"${GNURADIO_RUNTIME_INCLUDE_DIRS}"
		"${GNURADIO_ALL_INCLUDE_DIRS}")
	TARGET_LINK_LIBRARIES ( guidedScrambler PUBLIC
		gnuradio-isatec
		"${Boost_LIBRARIES}"
		"${GNURADIO_ALL_LIBRARIES}"
		"${QT_LIBRARIES}")
	ADD_DEPENDENCIES ( guidedScrambler gnuradio-isatec)
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
	INSTALL (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/html" DESTINATION
		share/gr-isatec/doc)
ENDIF(DOXYGEN_FOUND)
