#include "pmt/parser/source_position.hpp"

namespace pmt::parser {

SourcePosition::SourcePosition(LinenoType lineno_, ColnoType colno_)
 : _lineno(lineno_)
 , _colno(colno_) {
 }

}