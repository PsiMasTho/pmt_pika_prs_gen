#pragma once

#include "pmt/sm/graph_writer.hpp"

#include <fstream>
#include <functional>

namespace pmt::parser::builder {

class TerminalGraphWriter : public pmt::sm::GraphWriter {
 // -$ Types / Constants $-
public:
 using AcceptsToLabelFn = std::function<std::string(pmt::sm::AcceptsIndexType)>;

private:
 // -$ Data $-
 AcceptsToLabelFn _terminal_to_name_fn;
 std::ofstream _os_graph;
 std::string _title;

 // -$ Functions $-
 // --$ Lifetime $--
public:
 TerminalGraphWriter(pmt::sm::StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_);

 // --$ Inherited: pmt::sm::GraphWriter $--
private:
 auto accepts_to_label(size_t accepts_) -> std::string override;

 void symbol_set_to_transitions(pmt::base::IntervalSet<pmt::sm::SymbolType> const& in_symbol_intervals_, std::vector<Transition>& out_transitions_) override;
 auto get_layout_direction() const -> LayoutDirection override;
 auto get_graph_title() const -> std::string override;
};

}  // namespace pmt::parser::builder