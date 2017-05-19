cmake_minimum_required(VERSION 2.8)
project(gr-gs CXX)

# Setup complex type

if(NOT COMPLEX_SIZE)
    set(COMPLEX_SIZE 8)
endif()

if(COMPLEX_SIZE EQUAL 8)
    set(COMPLEX_BASE "float")
elseif(COMPLEX_SIZE EQUAL 16)
    set(COMPLEX_BASE "double")
else()
    message(FATAL_ERROR "COMPLEX_SIZE must be 8 or 16.")
endif()

# Set compile flags for gcc and clang

if(UNIX)
    set(CMAKE_CXX_FLAGS
        "$ENV{CXXFLAGS} -std=c++14 -Wall -Werror -pthread -fvisibility-inlines-hidden -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_RELEASE
        #"$ENV{CXXFLAGS} -O3 -fomit-frame-pointer -std=c++14 -Wall -Werror -pthread -fvisibility-inlines-hidden -fvisibility=hidden")
        "$ENV{CXXFLAGS} -O3 -fomit-frame-pointer -std=c++14 -Wall -Werror -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG
        "$ENV{CXXFLAGS} -Og -fno-omit-frame-pointer -g -ggdb -std=c++14 -Wall -Werror -pthread")
endif()
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include")

# Find Boost

set(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
find_package(Boost 1.35.0 REQUIRED COMPONENTS filesystem system)
include_directories("${Boost_INCLUDE_DIRS}")

# Find GNU Radio

set(GR_REQUIRED_COMPONENTS RUNTIME FILTER)
find_package(Gnuradio "3.7.0" REQUIRED)
include_directories("${GNURADIO_RUNTIME_INCLUDE_DIRS}")

# Guided Scrambling GNU Radio Module

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/gr-gs/config.h @ONLY)
add_library(gnuradio-gs SHARED
    src/MSW.cpp
    src/WRDS.cpp
    src/MSW2.cpp
    src/Analyzer.cpp
    src/Words.cpp
    src/Primitives.cpp
    src/Scrambler.cpp
    src/ScramblerGroup.cpp
    src/ScramblerGroup.cpp
    src/GuidedScrambler_impl.cpp
    src/Descrambler_impl.cpp
    src/SymbolGenerator_impl.cpp
    src/SymbolMapper_impl.cpp
    src/Average_impl.cpp
    src/Distribution_impl.cpp
    src/Integrate_impl.cpp
    src/ErrorCount_impl.cpp)
set_target_properties(gnuradio-gs PROPERTIES DEFINE_SYMBOL "gnuradio_gs_EXPORTS")
target_link_libraries(gnuradio-gs PUBLIC
    "${Boost_LIBRARIES}"
    "${GNURADIO_FILTER_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
install(TARGETS gnuradio-gs LIBRARY DESTINATION lib)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/GuidedScrambler.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/Descrambler.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/SymbolGenerator.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/SymbolMapper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/Average.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/Distribution.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/Integrate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/ErrorCount.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/Primitives.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gr-gs/exceptions.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/gr-gs/config.h"
    DESTINATION "include/gr-gs")

# Guided Scrambling unit tests

enable_testing()

add_executable(Analyzer_test EXCLUDE_FROM_ALL
    tests/Analyzer.cpp
    src/Analyzer.cpp
    src/Words.cpp
    src/MSW2.cpp
    src/MSW.cpp
    src/WRDS.cpp)
add_test("gr::gs::GuidedScrambling::Analyzer" Analyzer_test)

add_executable(Descrambler_test EXCLUDE_FROM_ALL
    tests/Descrambler.cpp
    src/Words.cpp
    src/Descrambler_impl.cpp)
target_link_libraries(Descrambler_test PRIVATE
    "${Boost_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
add_test("gr::gs::Descrambler" Descrambler_test)

add_executable(Words_test EXCLUDE_FROM_ALL
    tests/Words.cpp
    src/Words.cpp
    src/Analyzer.cpp
    src/MSW2.cpp
    src/MSW.cpp
    src/WRDS.cpp)
target_link_libraries(Words_test PRIVATE
    "${Boost_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
add_test("gr::gs::GuidedScrambling::Words" Words_test)

add_executable(Scrambler_test EXCLUDE_FROM_ALL
    tests/Scrambler.cpp
    src/Scrambler.cpp
    src/Analyzer.cpp
    src/Words.cpp
    src/MSW2.cpp
    src/MSW.cpp
    src/WRDS.cpp
    src/Descrambler_impl.cpp)
target_link_libraries(Scrambler_test PRIVATE
    "${Boost_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
add_test("gr::gs::GuidedScrambling::Scrambler" Scrambler_test)

add_executable(ScramblerGroup_test EXCLUDE_FROM_ALL
    tests/ScramblerGroup.cpp
    src/Scrambler.cpp
    src/ScramblerGroup.cpp
    src/Analyzer.cpp
    src/MSW2.cpp
    src/Words.cpp
    src/MSW.cpp
    src/WRDS.cpp
    src/Descrambler_impl.cpp)
target_link_libraries(ScramblerGroup_test PRIVATE
    "${Boost_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
add_test("gr::gs::GuidedScrambling::ScramblerGroup" ScramblerGroup_test)

add_executable(GuidedScrambler_test EXCLUDE_FROM_ALL
    tests/GuidedScrambler.cpp
    src/Scrambler.cpp
    src/ScramblerGroup.cpp
    src/Analyzer.cpp
    src/MSW2.cpp
    src/Words.cpp
    src/MSW.cpp
    src/WRDS.cpp
    src/Descrambler_impl.cpp
    src/GuidedScrambler_impl.cpp)
target_link_libraries(GuidedScrambler_test PRIVATE
    "${Boost_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")
add_test("gr::gs::GuidedScrambler" GuidedScrambler_test)

add_custom_target(tests DEPENDS
    Descrambler_test
    Analyzer_test
    Words_test
    Scrambler_test
    ScramblerGroup_test
    GuidedScrambler_test)

# Documentation

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc" DESTINATION
        share/gr-gs
        OPTIONAL)
endif(DOXYGEN_FOUND)

# Swig

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs 2)
include_directories("${PYTHON_INCLUDE_DIRS}")

foreach(incdir ${GNURADIO_RUNTIME_INCLUDE_DIRS})
    include_directories(${incdir}/gnuradio/swig)
endforeach(incdir)

set(CMAKE_SWIG_FLAGS -fvirtual -modern -keyword -w511)
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/gr_gs.i"
    PROPERTIES CPLUSPLUS ON)
swig_add_module(gr_gs python "${CMAKE_CURRENT_SOURCE_DIR}/gr_gs.i")
swig_link_libraries(gr_gs
    gnuradio-gs
    "${PYTHON_LIBRARIES}"
    "${Boost_LIBRARIES}"
    "${GNURADIO_FILTER_LIBRARIES}"
    "${GNURADIO_RUNTIME_LIBRARIES}")

execute_process(COMMAND python2 -c "
from distutils import sysconfig
print sysconfig.get_python_lib(plat_specific=True, prefix='')
" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
install(TARGETS _gr_gs DESTINATION ${PYTHON_SITE_PACKAGES})
install(FILES ${CMAKE_BINARY_DIR}/gr_gs.py DESTINATION ${PYTHON_SITE_PACKAGES})

# GNU Radio Companion

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_GuidedScrambler.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_Descrambler.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_SymbolMapper.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_SymbolGenerator.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_Average.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_Distribution.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_Integrate.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_primitive.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_defaultConstellation.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gr-gs_ErrorCount.xml"
    DESTINATION "share/gnuradio/grc/blocks/")
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/grc/gs_demo.grc"
    DESTINATION "share/gr-gs")
