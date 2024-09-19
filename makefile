include config.mk

.PHONY: all clean format pmt pmt_base pmt_generator pmt_util pmt_util_parse pmt_util_parse_test pmt_util_parse_test_exe

all:                     pmt pmt_base pmt_generator pmt_util pmt_util_parse pmt_util_parse_test pmt_util_parse_test_exe

clean:
	cd src/pmt                     && ${MAKE} $@
	cd src/pmt/base                && ${MAKE} $@
	cd src/pmt/generator           && ${MAKE} $@
	cd src/pmt/util                && ${MAKE} $@
	cd src/pmt/util/parse          && ${MAKE} $@
	cd src/pmt/util/parse/test     && ${MAKE} $@
	cd src/pmt/util/parse/test/exe && ${MAKE} $@

format:
	cd src/pmt                     && ${MAKE} $@
	cd src/pmt/base                && ${MAKE} $@
	cd src/pmt/generator           && ${MAKE} $@
	cd src/pmt/util                && ${MAKE} $@
	cd src/pmt/util/parse          && ${MAKE} $@
	cd src/pmt/util/parse/test     && ${MAKE} $@
	cd src/pmt/util/parse/test/exe && ${MAKE} $@

# Phony projects
# - Libs
pmt:                 pmt/libpmt.a
pmt_base:            pmt/base/libpmt_base.a
pmt_util:            pmt/util/libpmt_util.a
pmt_util_parse:      pmt/util/parse/libpmt_util_parse.a
pmt_util_parse_test: pmt/util/parse/libpmt_util_parse_test.a
# - Exes
pmt_generator:           pmt/generator/pmt_generator
pmt_util_parse_test_exe: pmt/util/parse/test/exe/pmt_util_parse_test_exe

# Actual targets
# - Libs
pmt/libpmt.a:
	cd src/pmt && ${MAKE}

pmt/base/libpmt_base.a:
	cd src/pmt/base && ${MAKE}

pmt/util/libpmt_util.a: pmt pmt_base
	cd src/pmt/util && ${MAKE}

pmt/util/parse/libpmt_util_parse.a: pmt_util
	cd src/pmt/util/parse && ${MAKE}

pmt/util/parse/libpmt_util_parse_test.a: pmt_util_parse
	cd src/pmt/util/parse/test && ${MAKE}

# - Exes
pmt/generator/pmt_generator: pmt_base
	cd src/pmt/generator && ${MAKE}

pmt/util/parse/test/exe/pmt_util_parse_test_exe: pmt_util_parse_test 
	cd	src/pmt/util/parse/test/exe && ${MAKE}
