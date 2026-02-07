#include "pmt/builder/pika_tables_emitter.hpp"

#include "pmt/builder/state_machine_tables.hpp"
#include "pmt/container/bitset.hpp"
#include "pmt/util/timestamp.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <map>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace pmt::builder {
using namespace pmt::container;
using namespace pmt::rt;
namespace {

auto make_header_include(std::string_view path_) -> std::string {
 if (path_.empty()) {
  return "";
 }
 return "#include \"" + std::string(path_) + "\"";
}

auto make_namespace_open(std::string_view ns_) -> std::string {
 if (ns_.empty()) {
  return "";
 }
 return "namespace " + std::string(ns_) + " {";
}

auto make_namespace_close(std::string_view ns_) -> std::string {
 if (ns_.empty()) {
  return "";
 }
 return "} // namespace " + std::string(ns_);
}

template <typename T_, std::invocable<T_ const&> F_>
auto format_list_generic(std::span<T_ const> values_, size_t ch_per_line_max_, F_&& f_) -> std::string {
 std::string out = " ";
 std::string delim;

 size_t ch_line_cur = out.size();
 for (size_t i = 0; i < values_.size(); ++i) {
  std::string const value = std::invoke(std::forward<F_>(f_), values_[i]);
  size_t ch_incr = delim.size() + value.size();
  out += std::exchange(delim, ", ");
  if (ch_line_cur + ch_incr > ch_per_line_max_) {
   out += "\n ";
   ch_line_cur = 1;
   // Delimiter was added on the previous line so don't count it towards the increment
   ch_incr = value.size();
  }
  ch_line_cur += ch_incr;
  out += value;
 }
 return out;
}

auto clause_tag_to_string(ClauseBase::Tag tag_) -> std::string {
 return "ClauseBase::Tag::" + ClauseBase::tag_to_string(tag_);
}

auto format_list(std::span<uint64_t const> values_, size_t digits_needed_, size_t ch_per_line_max_) -> std::string {
 return format_list_generic(values_, ch_per_line_max_, [&](uint64_t value_) { return "0x" + pmt::util::uint_to_string(value_, digits_needed_, pmt::util::hex_alphabet_uppercase); });
}

auto format_list(std::span<std::string const> values_, size_t ch_per_line_max_, bool add_quotes_) -> std::string {
 return format_list_generic(values_, ch_per_line_max_, [&](std::string const& value_) { return add_quotes_ ? "\"" + value_ + "\"" : value_; });
}

auto format_list(std::span<ClauseBase::Tag const> values_, size_t ch_per_line_max_) -> std::string {
 return format_list_generic(values_, ch_per_line_max_, clause_tag_to_string);
}

auto find_max_value(std::span<uint64_t const> values_) -> uint64_t {
 return values_.empty() ? 0 : *std::max_element(values_.begin(), values_.end());
}

auto pick_unsigned_type(uint64_t max_value_) -> std::string {
 // clang-format off
 std::string const bits =
  (max_value_ <= std::numeric_limits<uint8_t>::max())  ?  "8" :
  (max_value_ <= std::numeric_limits<uint16_t>::max()) ? "16" :
  (max_value_ <= std::numeric_limits<uint32_t>::max()) ? "32" :
                                                         "64";
 // clang-format on
 return "uint" + bits + "_t";
}

}  // namespace

PikaTablesEmitter::PikaTablesEmitter(Args args_)
 : _args(std::move(args_)) {
}

void PikaTablesEmitter::emit() {
 std::string const timestamp = pmt::util::get_timestamp();
 std::string const header_include = make_header_include(_args._header_include_path);
 std::string const id_constants_include = make_header_include(_args._id_constants_include_path);
 std::string const ns_open = make_namespace_open(_args._namespace_name);
 std::string const ns_close = make_namespace_close(_args._namespace_name);

 if (_args._output_header != nullptr) {
  replace_skeleton_label(_args._header_skel, "TIMESTAMP", timestamp);
  replace_skeleton_label(_args._header_skel, "NAMESPACE_OPEN", ns_open);
  replace_skeleton_label(_args._header_skel, "NAMESPACE_CLOSE", ns_close);
  replace_skeleton_label(_args._header_skel, "CLASS_NAME", _args._class_name);
  *_args._output_header << _args._header_skel;
 }

 if (_args._output_source != nullptr) {
  replace_skeleton_label(_args._source_skel, "TIMESTAMP", timestamp);
  replace_skeleton_label(_args._source_skel, "HEADER_INCLUDE", header_include);
  replace_skeleton_label(_args._source_skel, "ID_CONSTANTS_INCLUDE", id_constants_include);
  replace_skeleton_label(_args._source_skel, "NAMESPACE_OPEN", ns_open);
  replace_skeleton_label(_args._source_skel, "NAMESPACE_CLOSE", ns_close);
  replace_skeleton_label(_args._source_skel, "CLASS_NAME", _args._class_name);

  replace_skeleton_label(_args._source_skel, "BITSET_CHUNK_TYPE", "uint64_t");

  replace_terminal_tables();
  replace_clauses();
  replace_rules();

  *_args._output_source << _args._source_skel;
 }
}

void PikaTablesEmitter::replace_terminal_tables() {
 StateMachineTables const& terminal_tables = _args._pika_tables.get_terminal_state_machine_tables_full();
 StateMachine const& state_machine = terminal_tables.get_state_machine();
 size_t const state_count = state_machine.get_state_count();
 replace_number(_args._source_skel, "", "TERMINAL_STATE_COUNT", state_count);

 std::vector<uint64_t> transitions_lowers;
 std::vector<uint64_t> transitions_uppers;
 std::vector<uint64_t> transitions_values;
 std::vector<uint64_t> transitions_offsets{0};
 transitions_offsets.reserve(state_count + 1);

 std::vector<uint64_t> final_ids;
 std::vector<uint64_t> final_ids_offsets{0};
 final_ids_offsets.reserve(state_count + 1);

 for (size_t state_nr = 0; state_nr < state_count; ++state_nr) {
  State const* state = state_machine.get_state(state_nr);
  assert(state != nullptr);

  state->get_symbol_transitions().for_each_interval([&](StateNrType const& state_nr_next_, Interval<SymbolType> interval_) {
   transitions_lowers.push_back(interval_.get_lower());
   transitions_uppers.push_back(interval_.get_upper());
   transitions_values.push_back(state_nr_next_);
  });
  transitions_offsets.push_back(transitions_values.size());

  size_t const final_id_count = terminal_tables.get_state_final_id_count(state_nr);
  final_ids_offsets.push_back(final_id_count + final_ids_offsets.back());
  for (size_t j = 0; j < final_id_count; ++j) {
   final_ids.push_back(terminal_tables.get_state_final_id(state_nr, j));
  }
 }

 std::vector<uint64_t> transitions;
 transitions.reserve(transitions_values.size() * 3);
 transitions.insert(transitions.end(), transitions_lowers.begin(), transitions_lowers.end());
 transitions.insert(transitions.end(), transitions_uppers.begin(), transitions_uppers.end());
 transitions.insert(transitions.end(), transitions_values.begin(), transitions_values.end());

 replace_numeric_list(_args._source_skel, "TERMINAL_TRANSITIONS_TYPE", "TERMINAL_TRANSITIONS", transitions, true);
 replace_numeric_list(_args._source_skel, "TERMINAL_TRANSITIONS_OFFSETS_TYPE", "TERMINAL_TRANSITIONS_OFFSETS", transitions_offsets, true);
 replace_numeric_list(_args._source_skel, "TERMINAL_FINAL_IDS_TYPE", "TERMINAL_FINAL_IDS", final_ids, false);
 replace_numeric_list(_args._source_skel, "TERMINAL_FINAL_IDS_OFFSETS_TYPE", "TERMINAL_FINAL_IDS_OFFSETS", final_ids_offsets, true);
 replace_number(_args._source_skel, "", "TERMINAL_FINAL_IDS_SIZE", final_ids.size());
 replace_number(_args._source_skel, "", "TERMINAL_TRANSITIONS_SIZE", transitions.size());
}

void PikaTablesEmitter::replace_clauses() {
 size_t const clause_count = _args._pika_tables.get_clause_count();
 replace_number(_args._source_skel, "", "CLAUSE_COUNT", clause_count);
 replace_skeleton_label(_args._source_skel, "CLAUSE_CLASS_ID_TYPE", pick_unsigned_type(clause_count - 1));
 std::vector<ClauseBase::Tag> clause_tags;
 std::vector<uint64_t> clause_child_ids;
 std::vector<uint64_t> clause_child_offsets{0};
 std::vector<uint64_t> clause_seed_parent_ids;
 std::vector<uint64_t> clause_seed_parent_offsets{0};
 std::vector<uint64_t> clause_special_ids;
 Bitset clause_can_match_zero(clause_count, false);

 clause_tags.reserve(clause_count);
 clause_child_offsets.reserve(clause_count + 1);
 clause_seed_parent_offsets.reserve(clause_count + 1);
 clause_special_ids.reserve(clause_count);

 for (size_t clause_id = 0; clause_id < clause_count; ++clause_id) {
  ClauseBase const& clause = _args._pika_tables.fetch_clause(clause_id);
  ClauseBase::Tag const tag = clause.get_tag();
  clause_tags.push_back(tag);

  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   clause_child_ids.push_back(clause.get_child_id_at(i));
  }
  clause_child_offsets.push_back(clause_child_ids.size());

  for (size_t i = 0; i < clause.get_seed_parent_count(); ++i) {
   clause_seed_parent_ids.push_back(clause.get_seed_parent_id_at(i));
  }
  clause_seed_parent_offsets.push_back(clause_seed_parent_ids.size());
  // clang-format off
  uint64_t const special_id =
   (tag == ClauseBase::Tag::CharsetLiteral) ? clause.get_literal_id() :
   (tag == ClauseBase::Tag::Identifier)     ? clause.get_rule_id() :
                                              0;
  // clang-format on

  clause_special_ids.push_back(special_id);

  if (clause.can_match_zero()) {
   clause_can_match_zero.set(clause_id, true);
  }
 }

 replace_bitset(_args._source_skel, "CLAUSE_CAN_MATCH_ZERO_TYPE", "CLAUSE_CAN_MATCH_ZERO_SIZE", "CLAUSE_CAN_MATCH_ZERO", clause_can_match_zero);
 replace_tag_list(_args._source_skel, "CLAUSE_TAGS", clause_tags);
 replace_numeric_list(_args._source_skel, "CLAUSE_CHILD_IDS_TYPE", "CLAUSE_CHILD_IDS", clause_child_ids, true);
 replace_numeric_list(_args._source_skel, "CLAUSE_CHILD_IDS_OFFSETS_TYPE", "CLAUSE_CHILD_IDS_OFFSETS", clause_child_offsets, true);
 replace_number(_args._source_skel, "", "CLAUSE_CHILD_IDS_SIZE", clause_child_ids.size());
 replace_numeric_list(_args._source_skel, "CLAUSE_SEED_PARENT_IDS_TYPE", "CLAUSE_SEED_PARENT_IDS", clause_seed_parent_ids, true);
 replace_numeric_list(_args._source_skel, "CLAUSE_SEED_PARENT_IDS_OFFSETS_TYPE", "CLAUSE_SEED_PARENT_IDS_OFFSETS", clause_seed_parent_offsets, true);
 replace_number(_args._source_skel, "", "CLAUSE_SEED_PARENT_IDS_SIZE", clause_seed_parent_ids.size());
 replace_numeric_list(_args._source_skel, "CLAUSE_SPECIAL_ID_TYPE", "CLAUSE_SPECIAL_IDS", clause_special_ids, true);
}

void PikaTablesEmitter::replace_rules() {
 size_t const rule_count = _args._pika_tables.get_rule_count();
 enum : size_t {
  StringTableDisplayNames = 0,
  StringTableIdStrings = 1,
 };
 std::map<std::string, std::pair<std::unordered_set<IdType>, std::unordered_set<IdType>>> string_table_map;
 std::map<pmt::rt::IdType, std::unordered_set<IdType>> rule_id_table_map;
 Bitset rule_parameter_booleans(rule_count * 3, false);

 for (size_t rule_id = 0; rule_id < rule_count; ++rule_id) {
  pmt::rt::RuleParametersBase const& params = _args._pika_tables.fetch_rule_parameters(rule_id);
  std::get<StringTableDisplayNames>(string_table_map[std::string(params.get_display_name())]).insert(rule_id);
  std::get<StringTableIdStrings>(string_table_map[std::string(params.get_id_string())]).insert(rule_id);
  rule_id_table_map[params.get_id_value()].insert(rule_id);

  rule_parameter_booleans.set(rule_id, params.get_merge());
  rule_parameter_booleans.set(rule_id + rule_count, params.get_unpack());
  rule_parameter_booleans.set(rule_id + rule_count + rule_count, params.get_hide());
 }

 std::vector<std::string> string_table;
 string_table.reserve(string_table_map.size());
 for (auto const& [str, _] : string_table_map) {
  string_table.push_back(str);
 }

 std::vector<pmt::rt::IdType> rule_id_table;
 rule_id_table.reserve(rule_id_table_map.size());
 for (auto const& [id_value, _] : rule_id_table_map) {
  rule_id_table.push_back(id_value);
 }

 std::vector<uint64_t> rule_id_indirect(rule_count);
 Bitset rule_ids_done(rule_count, false);
 for (size_t i = 0; i < rule_id_table.size(); ++i) {
  std::unordered_set<IdType> const& rule_id_set = rule_id_table_map[rule_id_table[i]];
  for (IdType const rule_id : rule_id_set) {
   rule_id_indirect[rule_id] = i;
   rule_ids_done.set(rule_id, true);
  }
 }

 std::vector<std::string> rule_id_table_enum_strings;
 rule_id_table_enum_strings.reserve(rule_id_table.size());
 for (pmt::rt::IdType const id : rule_id_table) {
  if (ReservedIds::is_reserved_id(id)) {
   rule_id_table_enum_strings.push_back("ReservedIds::" + ReservedIds::id_to_string(id));
  } else {
   rule_id_table_enum_strings.push_back("IdConstants::" + _args._pika_tables.get_id_table().id_to_string(id));
  }
 }

 assert(rule_ids_done.all());

 std::vector<uint64_t> rule_parameter_display_names_indirect(rule_count);
 Bitset rule_parameters_done(rule_count, false);
 std::vector<uint64_t> rule_parameter_id_strings_indirect(rule_count);
 Bitset rule_id_strings_done(rule_count, false);
 for (size_t i = 0; i < string_table.size(); ++i) {
  auto const& [display_name_set, id_string_set] = string_table_map[string_table[i]];
  for (IdType const rule_id : display_name_set) {
   rule_parameter_display_names_indirect[rule_id] = i;
   rule_parameters_done.set(rule_id, true);
  }
  for (IdType const rule_id : id_string_set) {
   rule_parameter_id_strings_indirect[rule_id] = i;
   rule_id_strings_done.set(rule_id, true);
  }
 }

 assert(rule_parameters_done.all());
 assert(rule_id_strings_done.all());

 replace_number(_args._source_skel, "", "RULE_PARAMETER_COUNT", rule_count);
 replace_number(_args._source_skel, "", "STRING_TABLE_SIZE", string_table.size());
 replace_number(_args._source_skel, "", "RULE_PARAMETER_ID_TABLE_SIZE", rule_id_table.size());
 replace_skeleton_label(_args._source_skel, "RULE_PARAMETER_CLASS_ID_TYPE", pick_unsigned_type(rule_count - 1));
 replace_skeleton_label(_args._source_skel, "RULE_PARAMETER_ID_TABLE_TYPE", pick_unsigned_type(find_max_value(rule_id_table)));
 replace_string_list(_args._source_skel, "STRING_TABLE", string_table, true);
 replace_numeric_list(_args._source_skel, "RULE_PARAMETER_ID_INDIRECT_TYPE", "RULE_PARAMETER_ID_INDIRECT", rule_id_indirect, true);
 replace_numeric_list(_args._source_skel, "RULE_PARAMETER_ID_STRING_INDIRECT_TYPE", "RULE_PARAMETER_ID_STRINGS_INDIRECT", rule_parameter_id_strings_indirect, true);
 replace_numeric_list(_args._source_skel, "RULE_PARAMETER_DISPLAY_NAME_INDIRECT_TYPE", "RULE_PARAMETER_DISPLAY_NAMES_INDIRECT", rule_parameter_display_names_indirect, true);
 replace_numeric_enum_list(_args._source_skel, "RULE_PARAMETER_ID_TABLE", rule_id_table_enum_strings);
 replace_bitset(_args._source_skel, "RULE_PARAMETER_BOOLEANS_TYPE", "RULE_PARAMETER_BOOLEANS_SIZE", "RULE_PARAMETER_BOOLEANS", rule_parameter_booleans);
}

void PikaTablesEmitter::replace_numeric_list(std::string& dest_, std::string_view label_typename_, std::string_view label_list_, std::span<uint64_t const> values_, bool padded_digits_) {
 uint64_t const max_value = find_max_value(values_);
 if (!label_typename_.empty()) {
  replace_skeleton_label(dest_, label_typename_, pick_unsigned_type(max_value));
 }

 size_t const digits_needed = padded_digits_ ? pmt::util::digits_needed(max_value, 16) : 1;
 replace_skeleton_label(dest_, label_list_, format_list(values_, digits_needed, _args._ch_per_line_max));
}

void PikaTablesEmitter::replace_tag_list(std::string& dest_, std::string_view label_list_, std::span<pmt::rt::ClauseBase::Tag const> values_) {
 replace_skeleton_label(dest_, label_list_, format_list(values_, _args._ch_per_line_max));
}

void PikaTablesEmitter::replace_numeric_enum_list(std::string& dest_, std::string_view label_list_, std::span<std::string const> values_) {
 replace_skeleton_label(dest_, label_list_, format_list(values_, _args._ch_per_line_max, false));
}

void PikaTablesEmitter::replace_string_list(std::string& dest_, std::string_view label_list_, std::span<std::string const> values_, bool add_quotes_) {
 replace_skeleton_label(dest_, label_list_, format_list(values_, _args._ch_per_line_max, add_quotes_));
}

void PikaTablesEmitter::replace_number(std::string& dest_, std::string_view label_typename_, std::string_view label_value_, uint64_t value_) {
 if (!label_typename_.empty()) {
  replace_skeleton_label(dest_, label_typename_, pick_unsigned_type(value_));
 }
 replace_skeleton_label(_args._source_skel, label_value_, "0x" + pmt::util::uint_to_string(value_, 0, pmt::util::hex_alphabet_uppercase));
}

void PikaTablesEmitter::replace_bitset(std::string& dest_, std::string_view label_typename_, std::string_view label_chunk_count_, std::string_view label_value_, pmt::container::Bitset const& value_) {
 if (!label_typename_.empty()) {
  replace_skeleton_label(dest_, label_typename_, "uint64_t");
 }
 if (!label_chunk_count_.empty()) {
  replace_number(dest_, "", label_chunk_count_, value_.get_chunk_count());
 }
 replace_skeleton_label(dest_, label_value_, format_list(value_.get_chunks(), 0, _args._ch_per_line_max));
}

}  // namespace pmt::builder
