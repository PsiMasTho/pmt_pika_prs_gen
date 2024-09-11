include config.mk

.PHONY: all clean format pmt pmt_base pmt_pwgen pmt_generator

all:                     pmt pmt_base pmt_pwgen pmt_generator

clean:
	cd src/pmt           && ${MAKE} $@
	cd src/pmt/base      && ${MAKE} $@
	cd src/pmt/pwgen     && ${MAKE} $@
	cd src/pmt/generator && ${MAKE} $@

format:
	cd src/pmt           && ${MAKE} $@
	cd src/pmt/base      && ${MAKE} $@
	cd src/pmt/pwgen     && ${MAKE} $@
	cd src/pmt/generator && ${MAKE} $@

# Phony projects
# - Libs
pmt:      pmt/libpmt.a
pmt_base: pmt/base/libpmt_base.a
# - Exes
pmt_pwgen:     pmt/pwgen/pmt_pwgen
pmt_generator: pmt/generator/pmt_generator

# Actual targets
# - Libs
pmt/libpmt.a:
	cd src/pmt && ${MAKE}

pmt/base/libpmt_base.a:
	cd src/pmt/base && ${MAKE}

# - Exes
pmt/pwgen/pmt_pwgen: pmt_base
	cd src/pmt/pwgen && ${MAKE}

pmt/generator/pmt_generator: pmt_base
	cd src/pmt/generator && ${MAKE}
