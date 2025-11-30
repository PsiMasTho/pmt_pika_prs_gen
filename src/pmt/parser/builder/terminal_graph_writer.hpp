#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/ct/graph_writer.hpp"

#include <fstream>
#include <functional>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::parser::builder {

class TerminalGraphWriter : public pmt::util::sm::ct::GraphWriter {
 // -$ Types / Constants $-
public:
 using AcceptsToLabelFn = std::function<std::string(pmt::util::sm::AcceptsIndexType)>;

private:
 // -$ Data $-
 AcceptsToLabelFn _terminal_to_name_fn;
 std::ofstream _os_graph;
 std::string _title;

 // -$ Functions $-
 // --$ Lifetime $--
public:
 TerminalGraphWriter(pmt::util::sm::ct::StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_);

 // --$ Inherited: pmt::util::sm::ct::GraphWriter $--
private:
 auto accepts_to_label(size_t accepts_) -> std::string override;
 auto symbols_to_label(pmt::util::sm::SymbolKindType kind_, pmt::base::IntervalSet<pmt::util::sm::SymbolValueType> const& symbols_) -> std::string override;
 auto symbol_kind_to_edge_color(pmt::util::sm::SymbolKindType kind_) -> Color override;
 auto symbol_kind_to_edge_style(pmt::util::sm::SymbolKindType kind_) -> EdgeStyle override;
 auto symbol_kind_to_font_flags(pmt::util::sm::SymbolKindType kind_) -> FontFlags override;
 auto get_layout_direction() const -> LayoutDirection override;
 auto get_graph_title() const -> std::string override;
};

}  // namespace pmt::parser::builder