#!/bin/bash

# Resolve the directory of this script
scriptdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Generate the parsing code
pmt_parser_builder_tui                                                        \
 --input-grammar-file "${scriptdir}/grammar_2.grm"                            \
 --pika-program-header-include-filename "pmt/parser/grammar/pika_program.hpp" \
 --pika-program-output-header-file "${scriptdir}/pika_program.hpp"            \
 --pika-program-output-source-file "${scriptdir}/pika_program.cpp"            \
 --pika-program-class-name PikaProgram                                        \
 --pika-program-namespace-name pmt::parser::grammar                           \
 --id-strings-output-file "${scriptdir}/id_strings_2-inl.hpp"                     \
 --id-constants-output-file "${scriptdir}/id_constants_2-inl.hpp"                 \
 --no-dotfiles