#include "pmt/util/smrt/source_position.hpp"

namespace pmt::util::smrt {

SourcePosition::SourcePosition(LinenoType lineno_, ColnoType colno_)
 : _lineno(lineno_)
 , _colno(colno_) {
 }

}