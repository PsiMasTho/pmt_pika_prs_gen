#pragma once

#include "pmt/builder/state_machine.hpp"
#include "pmt/rt/primitives.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <fstream>
#include <functional>
#include <string>

namespace pmt::builder {

class TerminalDotfileWriter {
 // -$ Types / Constants $-
public:
 using FinalIdToStringFn = std::function<std::string(pmt::rt::FinalIdType)>;

private:
 // -$ Data $-
 StateMachine const& _state_machine;
 std::ostream& _os_graph;
 std::ifstream& _skel_file;
 pmt::util::SkeletonReplacerBase _replacer;
 FinalIdToStringFn _final_id_to_string_fn;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 TerminalDotfileWriter(StateMachine const& state_machine_, std::ostream& os_graph_, std::ifstream& skel_file_, FinalIdToStringFn final_id_to_string_fn_);

 // -$ Other $-
 void write_dot();
};

}  // namespace pmt::builder
