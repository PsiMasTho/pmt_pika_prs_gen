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
 std::ofstream _os_graph;
 std::ofstream _os_accepts_table;
 std::string _title;

 // -$ Functions $-
 // --$ Lifetime $--
public:
 TerminalGraphWriter(pmt::sm::StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, std::string const& accepts_table_filename_, AcceptsToLabelFn terminal_to_name_fn_);

 // --$ Inherited: pmt::sm::GraphWriter $--
private:
 void symbol_set_to_transitions(pmt::base::IntervalSet<pmt::sm::SymbolType> const& in_symbol_intervals_, std::vector<Transition>& out_transitions_) override;
 auto get_layout_direction() const -> LayoutDirection override;
 auto get_graph_title() const -> std::string override;
};

}  // namespace pmt::parser::builder
