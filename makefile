include config.mk

SRC_DIR     := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))src
GRAMMAR_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))grammars

.PHONY: all clean format rl default
default: all

#-- Libs -----------------------------------------------------------------------
#-- pmt --
PMT_DIR	= ${SRC_DIR}/pmt
PMT_SRC	= $(wildcard ${PMT_DIR}/*.cpp)
PMT_HDR = $(wildcard ${PMT_DIR}/*.hpp)
PMT_OBJ	= $(patsubst %.cpp, %.o, ${PMT_SRC})
PMT_LIB	= ${PMT_DIR}/libpmt.a
ALL_OBJ += ${PMT_OBJ}
ALL_LIBS	+= ${PMT_LIB}
ALL_SRC += ${PMT_SRC}
ALL_HDR	+= ${PMT_HDR}
${PMT_LIB}: ${PMT_OBJ}
pmt: ${PMT_LIB}

#-- pmt_base --
PMT_BASE_DIR	= ${SRC_DIR}/pmt/base
PMT_BASE_SRC	= $(wildcard ${PMT_BASE_DIR}/*.cpp)
PMT_BASE_HDR	= $(wildcard ${PMT_BASE_DIR}/*.hpp)
PMT_BASE_OBJ	= $(patsubst %.cpp, %.o, ${PMT_BASE_SRC})
PMT_BASE_LIB	= ${PMT_BASE_DIR}/libpmt_base.a
ALL_OBJ	+= ${PMT_BASE_OBJ}
ALL_LIBS	+= ${PMT_BASE_LIB}
ALL_SRC	+= ${PMT_BASE_SRC}
ALL_HDR	+= ${PMT_BASE_HDR}
${PMT_BASE_LIB}: ${PMT_BASE_OBJ}
pmt_base: ${PMT_BASE_LIB}

#--	pmt_base_test --
PMT_BASE_TEST_DIR	= ${SRC_DIR}/pmt/base/test
PMT_BASE_TEST_SRC	= $(wildcard ${PMT_BASE_TEST_DIR}/*.cpp)
PMT_BASE_TEST_HDR	= $(wildcard ${PMT_BASE_TEST_DIR}/*.hpp)
PMT_BASE_TEST_OBJ	= $(patsubst %.cpp, %.o, ${PMT_BASE_TEST_SRC})
PMT_BASE_TEST_LIB	= ${PMT_BASE_TEST_DIR}/libpmt_base_test.a
ALL_OBJ	+= ${PMT_BASE_TEST_OBJ}
ALL_LIBS	+= ${PMT_BASE_TEST_LIB}
ALL_SRC	+= ${PMT_BASE_TEST_SRC}
ALL_HDR	+= ${PMT_BASE_TEST_HDR}
${PMT_BASE_TEST_LIB}: ${PMT_BASE_TEST_OBJ}
pmt_base_test: ${PMT_BASE_TEST_LIB}

#-- pmt_parserbuilder --
PMT_PARSERBUILDER_DIR	= ${SRC_DIR}/pmt/parserbuilder
PMT_PARSERBUILDER_SRC	= $(wildcard ${PMT_PARSERBUILDER_DIR}/*.cpp)
PMT_PARSERBUILDER_HDR	= $(wildcard ${PMT_PARSERBUILDER_DIR}/*.hpp)
PMT_PARSERBUILDER_OBJ	= $(patsubst %.cpp, %.o, ${PMT_PARSERBUILDER_SRC})
PMT_PARSERBUILDER_LIB	= ${PMT_PARSERBUILDER_DIR}/libpmt_parserbuilder.a
ALL_OBJ	+= ${PMT_PARSERBUILDER_OBJ}
ALL_LIBS	+= ${PMT_PARSERBUILDER_LIB}
ALL_SRC	+= ${PMT_PARSERBUILDER_SRC}
ALL_HDR	+= ${PMT_PARSERBUILDER_HDR}
${PMT_PARSERBUILDER_LIB}: ${PMT_PARSERBUILDER_OBJ}
pmt_parserbuilder: ${PMT_PARSERBUILDER_LIB}

#-- pmt_util --
PMT_UTIL_DIR	= ${SRC_DIR}/pmt/util
PMT_UTIL_SRC	= $(wildcard ${PMT_UTIL_DIR}/*.cpp)
PMT_UTIL_HDR	= $(wildcard ${PMT_UTIL_DIR}/*.hpp)
PMT_UTIL_OBJ	= $(patsubst %.cpp, %.o, ${PMT_UTIL_SRC})
PMT_UTIL_LIB	= ${PMT_UTIL_DIR}/libpmt_util.a
ALL_OBJ	+= ${PMT_UTIL_OBJ}
ALL_LIBS	+= ${PMT_UTIL_LIB}
ALL_SRC	+= ${PMT_UTIL_SRC}
ALL_HDR	+= ${PMT_UTIL_HDR}
${PMT_UTIL_LIB}: ${PMT_UTIL_OBJ}
pmt_util: ${PMT_UTIL_LIB}

#-- pmt_util_test --
PMT_UTIL_TEST_DIR	= ${SRC_DIR}/pmt/util/test
PMT_UTIL_TEST_SRC	= $(wildcard ${PMT_UTIL_TEST_DIR}/*.cpp)
PMT_UTIL_TEST_HDR	= $(wildcard ${PMT_UTIL_TEST_DIR}/*.hpp)
PMT_UTIL_TEST_OBJ	= $(patsubst %.cpp, %.o, ${PMT_UTIL_TEST_SRC})
PMT_UTIL_TEST_LIB	= ${PMT_UTIL_TEST_DIR}/libpmt_util_test.a
ALL_OBJ	+= ${PMT_UTIL_TEST_OBJ}
ALL_LIBS	+= ${PMT_UTIL_TEST_LIB}
ALL_SRC	+= ${PMT_UTIL_TEST_SRC}
ALL_HDR	+= ${PMT_UTIL_TEST_HDR}
${PMT_UTIL_TEST_LIB}: ${PMT_UTIL_TEST_OBJ}
pmt_util_test: ${PMT_UTIL_TEST_LIB}

#-- pmt_util_parsect --
PMT_UTIL_PARSECT_DIR	= ${SRC_DIR}/pmt/util/parsect
PMT_UTIL_PARSECT_SRC	= $(wildcard ${PMT_UTIL_PARSECT_DIR}/*.cpp)
PMT_UTIL_PARSECT_HDR	= $(wildcard ${PMT_UTIL_PARSECT_DIR}/*.hpp)
PMT_UTIL_PARSECT_OBJ	= $(patsubst %.cpp, %.o, ${PMT_UTIL_PARSECT_SRC})
PMT_UTIL_PARSECT_LIB	= ${PMT_UTIL_PARSECT_DIR}/libpmt_util_parsect.a
ALL_OBJ	+= ${PMT_UTIL_PARSECT_OBJ}
ALL_LIBS	+= ${PMT_UTIL_PARSECT_LIB}
ALL_SRC	+= ${PMT_UTIL_PARSECT_SRC}
ALL_HDR	+= ${PMT_UTIL_PARSECT_HDR}
${PMT_UTIL_PARSECT_LIB}: ${PMT_UTIL_PARSECT_OBJ}
pmt_util_parsect: ${PMT_UTIL_PARSECT_LIB}

#-- pmt_util_parsert --
PMT_UTIL_PARSERT_DIR	= ${SRC_DIR}/pmt/util/parsert
PMT_UTIL_PARSERT_SRC	= $(wildcard ${PMT_UTIL_PARSERT_DIR}/*.cpp)
PMT_UTIL_PARSERT_HDR	= $(wildcard ${PMT_UTIL_PARSERT_DIR}/*.hpp)
PMT_UTIL_PARSERT_OBJ	= $(patsubst %.cpp, %.o, ${PMT_UTIL_PARSERT_SRC})
PMT_UTIL_PARSERT_LIB	= ${PMT_UTIL_PARSERT_DIR}/libpmt_util_parsert.a
ALL_OBJ	+= ${PMT_UTIL_PARSERT_OBJ}
ALL_LIBS	+= ${PMT_UTIL_PARSERT_LIB}
ALL_SRC	+= ${PMT_UTIL_PARSERT_SRC}
ALL_HDR	+= ${PMT_UTIL_PARSERT_HDR}
${PMT_UTIL_PARSERT_LIB}: ${PMT_UTIL_PARSERT_OBJ}
pmt_util_parsert: ${PMT_UTIL_PARSERT_LIB}

#-- Exes	-----------------------------------------------------------------------
#-- pmt_parserbuilder_exe --
PMT_PARSERBUILDER_EXE_DIR	= ${SRC_DIR}/pmt/parserbuilder/exe
PMT_PARSERBUILDER_EXE_SRC	= $(wildcard ${PMT_PARSERBUILDER_EXE_DIR}/*.cpp)
PMT_PARSERBUILDER_EXE_HDR	= $(wildcard ${PMT_PARSERBUILDER_EXE_DIR}/*.hpp)
PMT_PARSERBUILDER_EXE_OBJ	= $(patsubst %.cpp, %.o, ${PMT_PARSERBUILDER_EXE_SRC})
PMT_PARSERBUILDER_EXE_BIN	= ${PMT_PARSERBUILDER_EXE_DIR}/pmt_parserbuilder_exe
ALL_OBJ	+= ${PMT_PARSERBUILDER_EXE_OBJ}
ALL_BIN	+= ${PMT_PARSERBUILDER_EXE_BIN}
ALL_SRC	+= ${PMT_PARSERBUILDER_EXE_SRC}
ALL_HDR	+= ${PMT_PARSERBUILDER_EXE_HDR}
${PMT_PARSERBUILDER_EXE_BIN}: ${PMT_PARSERBUILDER_EXE_OBJ} ${PMT_PARSERBUILDER_LIB} ${PMT_UTIL_PARSERT_LIB} ${PMT_UTIL_PARSECT_LIB} ${PMT_UTIL_LIB} ${PMT_BASE_LIB} ${PMT_LIB}
	${CXX} ${CXXFLAGS} ${INCS} -o $@ $^
pmt_parserbuilder_exe: ${PMT_PARSERBUILDER_EXE_BIN}

#-- pmt_base_test_exe --
PMT_BASE_TEST_EXE_DIR	= ${SRC_DIR}/pmt/base/test/exe
PMT_BASE_TEST_EXE_SRC	= $(wildcard ${PMT_BASE_TEST_EXE_DIR}/*.cpp)
PMT_BASE_TEST_EXE_HDR	= $(wildcard ${PMT_BASE_TEST_EXE_DIR}/*.hpp)
PMT_BASE_TEST_EXE_OBJ	= $(patsubst %.cpp, %.o, ${PMT_BASE_TEST_EXE_SRC})
PMT_BASE_TEST_EXE_BIN	= ${PMT_BASE_TEST_EXE_DIR}/pmt_base_test_exe
ALL_OBJ	+= ${PMT_BASE_TEST_EXE_OBJ}
ALL_BIN	+= ${PMT_BASE_TEST_EXE_BIN}
ALL_SRC	+= ${PMT_BASE_TEST_EXE_SRC}
ALL_HDR	+= ${PMT_BASE_TEST_EXE_HDR}
${PMT_BASE_TEST_EXE_BIN}: ${PMT_BASE_TEST_EXE_OBJ} ${PMT_BASE_TEST_LIB} ${PMT_BASE_LIB} ${PMT_LIB}
	${CXX} ${CXXFLAGS} ${INCS} -o $@ $^
pmt_base_test_exe: ${PMT_BASE_TEST_EXE_BIN}

#-- pmt_util_test_exe --
PMT_UTIL_TEST_EXE_DIR	= ${SRC_DIR}/pmt/util/test/exe
PMT_UTIL_TEST_EXE_SRC	= $(wildcard ${PMT_UTIL_TEST_EXE_DIR}/*.cpp)
PMT_UTIL_TEST_EXE_HDR	= $(wildcard ${PMT_UTIL_TEST_EXE_DIR}/*.hpp)
PMT_UTIL_TEST_EXE_OBJ	= $(patsubst %.cpp, %.o, ${PMT_UTIL_TEST_EXE_SRC})
PMT_UTIL_TEST_EXE_BIN	= ${PMT_UTIL_TEST_EXE_DIR}/pmt_util_test_exe
ALL_OBJ	+= ${PMT_UTIL_TEST_EXE_OBJ}
ALL_BIN	+= ${PMT_UTIL_TEST_EXE_BIN}
ALL_SRC	+= ${PMT_UTIL_TEST_EXE_SRC}
ALL_HDR	+= ${PMT_UTIL_TEST_EXE_HDR}
${PMT_UTIL_TEST_EXE_BIN}: ${PMT_UTIL_TEST_EXE_OBJ} ${PMT_UTIL_TEST_LIB} ${PMT_UTIL_LIB} ${PMT_BASE_LIB} ${PMT_LIB}
	${CXX} ${CXXFLAGS} ${INCS} -o $@ $^
pmt_util_test_exe: ${PMT_UTIL_TEST_EXE_BIN}

#-- Grammars -------------------------------------------------------------------
#-- grm_lexer	--
${PMT_PARSERBUILDER_DIR}/grm_lexer.cpp: ${GRAMMAR_DIR}/grm_lexer.rl
	${RAGEL} -G2 -o $@ $<

#-- grm_parser	--
${PMT_PARSERBUILDER_DIR}/grm_parser-inl.hpp: ${GRAMMAR_DIR}/grm_parser.y
	-${LEMON} -T${GRAMMAR_DIR}/lempar.cpp $<
	mv ${GRAMMAR_DIR}/grm_parser.cpp ${PMT_PARSERBUILDER_DIR}/grm_parser-inl.hpp
	mv ${GRAMMAR_DIR}/grm_parser.h   ${PMT_PARSERBUILDER_DIR}/grm_ast-inl.hpp

#-- Generic --------------------------------------------------------------------
%.a: 
	${AR} rcs $@ $^

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} -I${SRC_DIR} -o $@ $<

#-- Clean ----------------------------------------------------------------------
clean:
	rm -f ${ALL_BIN} ${ALL_LIBS} ${ALL_OBJ}

#-- Format ---------------------------------------------------------------------
format:
	${FORMATTER} -i	${ALL_SRC} ${ALL_HDR}

#-- All ------------------------------------------------------------------------
all: ${ALL_BIN} ${ALL_LIBS}

#-- Rl	-------------------------------------------------------------------------
rl: ${PMT_PARSERBUILDER_DIR}/grm_lexer.cpp

#-- y --------------------------------------------------------------------------
y: ${PMT_PARSERBUILDER_DIR}/grm_parser-inl.hpp