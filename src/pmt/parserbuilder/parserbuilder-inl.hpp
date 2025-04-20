// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/parserbuilder.hpp"
#endif
// clang-format on

#include "pmt/util/smct/graph_writer.hpp"

#include <fstream>
#include <iostream>

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
void ParserBuilder::write_dot(Context& context_, pmt::util::smct::StateMachine const& state_machine_) {
  if (state_machine_.size() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(context_._dot_file_count++) + ".dot");
  pmt::util::smct::GraphWriter<TAG_>::write_dot(file, state_machine_, [&context_](size_t accepts_) { return accepts_to_label(context_, accepts_); });
}

}  // namespace pmt::parserbuilder