#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <functional>
#include <iosfwd>
#include <limits>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class GraphWriter : public pmt::util::SkeletonReplacerBase {
 public:
 // -$ Types / Constants $-
  enum class EdgeStyle : size_t{
   Dashed,
   Dotted,
   Solid,
   Bold
  };

  enum class NodeShape : size_t {
   Box,
   Circle,
   DoubleCircle,
   Ellipse,
   Octagon,
   DoubleOctagon,
  };

  struct Color {
    uint8_t _r = std::numeric_limits<uint8_t>::max();
    uint8_t _g = std::numeric_limits<uint8_t>::max();
    uint8_t _b = std::numeric_limits<uint8_t>::max();
  };
 
  using AcceptsToLabelFn = std::function<std::string(size_t)>;
  using SymbolsToLabelFn = std::function<std::string(pmt::base::IntervalSet<pmt::util::smrt::SymbolType> const&)>;
  using SymbolKindToColorFn = std::function<Color(pmt::util::smrt::SymbolType)>;
  using SymbolKindToEdgeStyleFn = std::function<EdgeStyle(pmt::util::smrt::SymbolType)>;

  struct WriterArgs {
   std::ostream& _os_graph;
   std::istream& _is_graph_skel;
   StateMachine const& _state_machine;
  };

  struct StyleArgs {
   StyleArgs();
   AcceptsToLabelFn _accepts_to_label_fn;
   SymbolsToLabelFn _symbols_to_label_fn;
   SymbolKindToColorFn _symbol_kind_to_color_fn;
   SymbolKindToEdgeStyleFn _symbol_kind_to_edge_style_fn;
   std::string _title = "State Machine";
   std::string _accepts_label = "Accepts";
   Color _accepting_node_color = Color{._r = 0, ._g = 0, ._b = 255};
   NodeShape _accepting_node_shape = NodeShape::DoubleCircle;
   Color _nonaccepting_node_color = Color{._r = 0, ._g = 0, ._b = 0};
   NodeShape _nonaccepting_node_shape = NodeShape::Circle;
   Color _epsilon_edge_color = Color{._r = 0, ._g = 255, ._b = 0};
   EdgeStyle _epsilon_edge_style = EdgeStyle::Solid;
  };

  private:
  // -$ Data $-
  std::ostream* _os = nullptr;
  WriterArgs* _writer_args = nullptr;
  StyleArgs _style_args;

  public:
  // -$ Functions $-
  // --$ Other $--
  static auto accepts_to_label_default(size_t accepts_) -> std::string;
  static auto symbols_to_label_default(pmt::base::IntervalSet<pmt::util::smrt::SymbolType> const& symbols_) -> std::string;
  static auto symbol_kind_to_color_default(pmt::util::smrt::SymbolType kind_) -> Color;
  static auto symbol_kind_to_edge_style_default(pmt::util::smrt::SymbolType kind_) -> EdgeStyle;

  void write_dot(WriterArgs& writer_args_, StyleArgs style_args_);

 private:
  void replace_accepting_node_shape(std::string& str_);
  void replace_accepting_node_color(std::string& str_);
  void replace_accepting_nodes(std::string& str_);
  void replace_nonaccepting_node_shape(std::string& str_);
  void replace_nonaccepting_node_color(std::string& str_);
  void replace_epsilon_edge_color(std::string& str_);
  void replace_epsilon_edges(std::string& str_);
  void replace_symbol_edges(std::string& str_);
  void replace_accepts_label(std::string& str_);
  void replace_accepts_table(std::string& str_);
  void replace_graph_title(std::string& str_);

  void replace_timestamp(std::string& str_);

  static auto is_displayable(pmt::util::smrt::SymbolType symbol_) -> bool;
  static auto to_displayable(pmt::util::smrt::SymbolType symbol_) -> std::string;

  static auto to_string(EdgeStyle edge_style_) -> std::string;
  static auto to_string(NodeShape node_shape_) -> std::string;
  static auto to_string(Color color_) -> std::string;
};

}  // namespace pmt::util::smct