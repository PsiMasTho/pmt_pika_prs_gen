#!/bin/bash

# Resolve the directory of this script
scriptdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Generate the parsing code
pmt_parser_builder_tui                                           \
 --input-grammar-file        "${scriptdir}/grammar.grm"          \
 --output-lexer-source-file  "${scriptdir}/lexer_tables.cpp"     \
 --output-lexer-header-file  "${scriptdir}/lexer_tables.hpp"     \
 --output-parser-source-file "${scriptdir}/parser_tables.cpp"    \
 --output-parser-header-file "${scriptdir}/parser_tables.hpp"    \
 --output-id-constants-file  "${scriptdir}/id_constants-inl.hpp" \
 --output-id-strings-file    "${scriptdir}/id_strings-inl.hpp"   \
 --lexer-class-name          LexerTables                         \
 --parser-class-name         ParserTables                        \
 --namespace-name            pmt::parser::grammar                \
 --no-dotfiles