#pragma once

#include <string>
#include <vector>

namespace pmt::sandbox::glyphscribe {

class Ruleset {
public:
 std::vector<std::string> _ligs;  // ligatures, length >= 2
 std::vector<char> _omit;         // letters to omit by default (a..z)
};

}  // namespace pmt::sandbox::glyphscribe