#include "pmt/parser/builder/pika_program_emitter.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/parser/builder/emitter_utils.hpp"
#include "pmt/parser/builder/state_machine_tables.hpp"
#include "pmt/util/timestamp.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <span>
#include <string_view>
#include <vector>

namespace pmt::parser::builder {
using namespace pmt::parser::rt;
namespace {

auto to_camel_case(std::string_view input_) -> std::string {
 std::string out;
 out.reserve(input_.size());
 bool capitalize = true;
 for (unsigned char ch : input_) {
  if (ch == '_' || ch == '-' || ch == ' ') {
   capitalize = true;
   continue;
  }
  if (capitalize) {
   out.push_back(static_cast<char>(std::toupper(ch)));
   capitalize = false;
  } else {
   out.push_back(static_cast<char>(ch));
  }
 }
 return out;
}

auto namespace_from_include_path(std::string_view include_path_) -> std::string {
 std::filesystem::path path(include_path_);
 std::filesystem::path parent = path.parent_path();
 if (parent.empty()) {
  return "pmt::parser::rt";
 }

 std::string out;
 for (auto const& part : parent) {
  std::string const name = part.string();
  if (name.empty() || name == ".") {
   continue;
  }
  if (!out.empty()) {
   out += "::";
  }
  out += name;
 }

 return out.empty() ? "pmt::parser::rt" : out;
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

auto max_value(std::vector<uint64_t> const& values_) -> uint64_t {
 if (values_.empty()) {
  return 0;
 }
 return *std::max_element(values_.begin(), values_.end());
}

auto hex_digits(uint64_t value_) -> size_t {
 size_t digits = 1;
 while (value_ >= 16) {
  value_ >>= 4;
  ++digits;
 }
 return digits;
}

auto format_hex_list(std::vector<uint64_t> const& values_, size_t width_, size_t per_line_) -> std::string {
 std::string out;
 for (size_t i = 0; i < values_.size(); ++i) {
  if (i % per_line_ == 0) {
   if (!out.empty()) {
    out += "\n";
   }
   out += " ";
  }
  out += format_hex(values_[i], width_);
  if (i + 1 != values_.size()) {
   out += ", ";
  }
 }
 return out;
}

auto format_list(std::vector<std::string> const& values_, size_t per_line_) -> std::string {
 std::string out;
 for (size_t i = 0; i < values_.size(); ++i) {
  if (i % per_line_ == 0) {
   if (!out.empty()) {
    out += "\n";
   }
   out += " ";
  }
  out += values_[i];
  if (i + 1 != values_.size()) {
   out += ", ";
  }
 }
 return out;
}

auto clause_tag_to_string(ClauseBase::Tag tag_) -> std::string {
 switch (tag_) {
  case ClauseBase::Tag::Sequence:
   return "ClauseBase::Tag::Sequence";
  case ClauseBase::Tag::Choice:
   return "ClauseBase::Tag::Choice";
  case ClauseBase::Tag::Hidden:
   return "ClauseBase::Tag::Hidden";
  case ClauseBase::Tag::Identifier:
   return "ClauseBase::Tag::Identifier";
  case ClauseBase::Tag::CharsetLiteral:
   return "ClauseBase::Tag::CharsetLiteral";
  case ClauseBase::Tag::OneOrMore:
   return "ClauseBase::Tag::OneOrMore";
  case ClauseBase::Tag::NotFollowedBy:
   return "ClauseBase::Tag::NotFollowedBy";
  case ClauseBase::Tag::Epsilon:
   return "ClauseBase::Tag::Epsilon";
  default:
   return "ClauseBase::Tag::Epsilon";
 }
}

auto pick_unsigned_type(uint64_t max_value_) -> std::string {
 if (max_value_ <= std::numeric_limits<uint8_t>::max()) {
  return "uint8_t";
 }
 if (max_value_ <= std::numeric_limits<uint16_t>::max()) {
  return "uint16_t";
 }
 if (max_value_ <= std::numeric_limits<uint32_t>::max()) {
  return "uint32_t";
 }
 return "uint64_t";
}

void set_bit(std::vector<uint64_t>& chunks_, size_t index_) {
 size_t const chunk_idx = index_ / pmt::base::Bitset::ChunkBit;
 size_t const bit_idx = index_ % pmt::base::Bitset::ChunkBit;
 chunks_[chunk_idx] |= (uint64_t(1) << bit_idx);
}

}  // namespace

PikaProgramEmitter::PikaProgramEmitter(Args args_)
 : _args(std::move(args_)) {
 if (_args._namespace_name.empty()) {
  _args._namespace_name = namespace_from_include_path(_args._header_include_path);
 }
 if (_args._class_name.empty()) {
  _args._class_name = to_camel_case(std::filesystem::path(_args._header_include_path).stem().string());
 }
 if (_args._class_name.empty()) {
  _args._class_name = "PikaProgram";
 }
}

void PikaProgramEmitter::emit() {
 std::string header = read_stream(_args._header_skel_file, "pika program header skeleton");
 std::string source = read_stream(_args._source_skel_file, "pika program source skeleton");

 std::string const timestamp = pmt::util::get_timestamp();
 std::string const ns_open = make_namespace_open(_args._namespace_name);
 std::string const ns_close = make_namespace_close(_args._namespace_name);

 replace_skeleton_label(header, "TIMESTAMP", timestamp);
 replace_skeleton_label(header, "NAMESPACE_OPEN", ns_open);
 replace_skeleton_label(header, "NAMESPACE_CLOSE", ns_close);
 replace_skeleton_label(header, "CLASS_NAME", _args._class_name);

 replace_skeleton_label(source, "TIMESTAMP", timestamp);
 replace_skeleton_label(source, "HEADER_INCLUDE_PATH", _args._header_include_path);
 replace_skeleton_label(source, "NAMESPACE_OPEN", ns_open);
 replace_skeleton_label(source, "NAMESPACE_CLOSE", ns_close);
 replace_skeleton_label(source, "CLASS_NAME", _args._class_name);

 auto const& terminal_tables = static_cast<StateMachineTables const&>(_args._program.get_terminal_state_machine_tables());
 auto const& state_machine = terminal_tables.get_state_machine();
 std::span<pmt::sm::StateNrType const> const state_nrs = state_machine.get_state_nrs();
 size_t const state_count = state_nrs.empty() ? 0 : static_cast<size_t>(state_nrs.back()) + 1;

 std::vector<uint64_t> terminal_lowers;
 std::vector<uint64_t> terminal_uppers;
 std::vector<uint64_t> terminal_values;
 std::vector<uint64_t> terminal_offsets;
 terminal_offsets.reserve(state_count + 1);
 terminal_offsets.push_back(0);

 for (size_t state_nr = 0; state_nr < state_count; ++state_nr) {
  auto const* state = state_machine.get_state(state_nr);
  if (state != nullptr) {
   state->get_symbol_transitions().for_each_interval([&](pmt::sm::StateNrType const& next_state_, auto interval_) {
    terminal_lowers.push_back(static_cast<uint64_t>(interval_.get_lower()));
    terminal_uppers.push_back(static_cast<uint64_t>(interval_.get_upper()));
    terminal_values.push_back(static_cast<uint64_t>(next_state_));
   });
  }
  terminal_offsets.push_back(terminal_lowers.size());
 }

 size_t const terminal_transition_count = terminal_lowers.size();
 size_t const terminal_state_count = std::max(state_count, terminal_transition_count);

 terminal_lowers.resize(terminal_state_count, 0);
 terminal_uppers.resize(terminal_state_count, 0);
 terminal_values.resize(terminal_state_count, 0);
 terminal_offsets.resize(terminal_state_count + 1, terminal_transition_count);

 std::vector<uint64_t> terminal_transitions;
 terminal_transitions.reserve(terminal_state_count * 3);
 terminal_transitions.insert(terminal_transitions.end(), terminal_lowers.begin(), terminal_lowers.end());
 terminal_transitions.insert(terminal_transitions.end(), terminal_uppers.begin(), terminal_uppers.end());
 terminal_transitions.insert(terminal_transitions.end(), terminal_values.begin(), terminal_values.end());

 pmt::sm::AcceptsIndexType max_accept = 0;
 bool saw_accepts = false;
 for (size_t state_nr = 0; state_nr < state_count; ++state_nr) {
  auto const* state = state_machine.get_state(state_nr);
  if (state != nullptr && !state->get_accepts().empty()) {
   saw_accepts = true;
   max_accept = std::max(max_accept, state->get_accepts().highest());
  }
 }

 size_t const terminal_accepts_chunk_count = saw_accepts ? pmt::base::Bitset::get_required_chunk_count(static_cast<size_t>(max_accept + 1)) : 0;
 std::vector<uint64_t> terminal_accepts(terminal_state_count * terminal_accepts_chunk_count, 0);

 for (size_t state_nr = 0; state_nr < state_count; ++state_nr) {
  if (terminal_accepts_chunk_count == 0) {
   break;
  }
  pmt::base::Bitset accepts_bitset(static_cast<size_t>(max_accept + 1), false);
  auto const* state = state_machine.get_state(state_nr);
  if (state != nullptr) {
   state->get_accepts().for_each_key([&](pmt::sm::AcceptsIndexType accept_) { accepts_bitset.set(static_cast<size_t>(accept_), true); });
  }
  auto const chunks = accepts_bitset.get_chunks();
  size_t const offset = state_nr * terminal_accepts_chunk_count;
  for (size_t i = 0; i < terminal_accepts_chunk_count; ++i) {
   terminal_accepts[offset + i] = static_cast<uint64_t>(chunks[i]);
  }
 }

 size_t const clause_count = _args._program.get_clause_count();
 std::vector<std::string> clause_tags;
 std::vector<uint64_t> clause_child_ids;
 std::vector<uint64_t> clause_child_offsets;
 std::vector<uint64_t> clause_seed_parent_ids;
 std::vector<uint64_t> clause_seed_parent_offsets;
 std::vector<uint64_t> clause_special_ids;
 pmt::base::Bitset clause_can_match_zero(clause_count, false);

 clause_tags.reserve(clause_count);
 clause_child_offsets.reserve(clause_count + 1);
 clause_seed_parent_offsets.reserve(clause_count + 1);
 clause_special_ids.reserve(clause_count);
 clause_child_offsets.push_back(0);
 clause_seed_parent_offsets.push_back(0);

 for (size_t clause_id = 0; clause_id < clause_count; ++clause_id) {
  ClauseBase const& clause = _args._program.fetch_clause(clause_id);
  ClauseBase::Tag const tag = clause.get_tag();
  clause_tags.push_back(clause_tag_to_string(tag));

  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   clause_child_ids.push_back(static_cast<uint64_t>(clause.get_child_id_at(i)));
  }
  clause_child_offsets.push_back(clause_child_ids.size());

  for (size_t i = 0; i < clause.get_seed_parent_count(); ++i) {
   clause_seed_parent_ids.push_back(static_cast<uint64_t>(clause.get_seed_parent_id_at(i)));
  }
  clause_seed_parent_offsets.push_back(clause_seed_parent_ids.size());

  uint64_t special_id = 0;
  if (tag == ClauseBase::Tag::CharsetLiteral) {
   special_id = static_cast<uint64_t>(clause.get_literal_id());
  } else if (tag == ClauseBase::Tag::Identifier) {
   special_id = static_cast<uint64_t>(clause.get_rule_id());
  }
  clause_special_ids.push_back(special_id);

  if (clause.can_match_zero()) {
   clause_can_match_zero.set(clause_id, true);
  }
 }

 std::vector<uint64_t> clause_can_match_zero_chunks;
 clause_can_match_zero_chunks.reserve(clause_can_match_zero.get_chunk_count());
 for (pmt::base::Bitset::ChunkType chunk : clause_can_match_zero.get_chunks()) {
  clause_can_match_zero_chunks.push_back(static_cast<uint64_t>(chunk));
 }

 size_t const rule_count = _args._program.get_rule_count();
 std::vector<std::string> rule_display_names;
 std::vector<std::string> rule_id_strings;
 std::vector<uint64_t> rule_id_values;
 std::vector<bool> rule_merge;
 std::vector<bool> rule_unpack;
 std::vector<bool> rule_hide;

 rule_display_names.reserve(rule_count);
 rule_id_strings.reserve(rule_count);
 rule_id_values.reserve(rule_count);
 rule_merge.reserve(rule_count);
 rule_unpack.reserve(rule_count);
 rule_hide.reserve(rule_count);

 for (size_t rule_id = 0; rule_id < rule_count; ++rule_id) {
  pmt::parser::rt::RuleParametersBase const& params = _args._program.fetch_rule_parameters(rule_id);
  rule_display_names.push_back(std::string("\"") + std::string(params.get_display_name()) + "\"");
  rule_id_strings.push_back(std::string("\"") + std::string(params.get_id_string()) + "\"");
  rule_id_values.push_back(static_cast<uint64_t>(params.get_id_value()));
  rule_merge.push_back(params.get_merge());
  rule_unpack.push_back(params.get_unpack());
  rule_hide.push_back(params.get_hide());
 }

 size_t const rule_chunk_count = pmt::base::Bitset::get_required_chunk_count(rule_count);
 std::vector<uint64_t> rule_parameter_booleans(rule_chunk_count * 3, 0);
 if (rule_chunk_count != 0) {
  size_t const rule_bits_per_set = rule_chunk_count * pmt::base::Bitset::ChunkBit;
  for (size_t rule_id = 0; rule_id < rule_count; ++rule_id) {
   if (rule_merge[rule_id]) {
    set_bit(rule_parameter_booleans, rule_id);
   }
   if (rule_unpack[rule_id]) {
    set_bit(rule_parameter_booleans, rule_bits_per_set + rule_id);
   }
   if (rule_hide[rule_id]) {
    set_bit(rule_parameter_booleans, rule_bits_per_set * 2 + rule_id);
   }
  }
 }

 auto const terminal_transitions_type = pick_unsigned_type(max_value(terminal_transitions));
 auto const terminal_transitions_offsets_type = pick_unsigned_type(max_value(terminal_offsets));
 auto const clause_child_ids_offsets_type = pick_unsigned_type(max_value(clause_child_offsets));
 auto const clause_child_ids_type = pick_unsigned_type(max_value(clause_child_ids));
 auto const clause_seed_parent_ids_offsets_type = pick_unsigned_type(max_value(clause_seed_parent_offsets));
 auto const clause_seed_parent_ids_type = pick_unsigned_type(max_value(clause_seed_parent_ids));
 auto const clause_special_id_type = pick_unsigned_type(max_value(clause_special_ids));

 replace_skeleton_label(source, "TERMINAL_TRANSITIONS_TYPE", terminal_transitions_type);
 replace_skeleton_label(source, "TERMINAL_TRANSITIONS_OFFSETS_TYPE", terminal_transitions_offsets_type);
 replace_skeleton_label(source, "CLAUSE_CHILD_IDS_OFFSETS_TYPE", clause_child_ids_offsets_type);
 replace_skeleton_label(source, "CLAUSE_CHILD_IDS_TYPE", clause_child_ids_type);
 replace_skeleton_label(source, "CLAUSE_SEED_PARENT_IDS_OFFSETS_TYPE", clause_seed_parent_ids_offsets_type);
 replace_skeleton_label(source, "CLAUSE_SEED_PARENT_IDS_TYPE", clause_seed_parent_ids_type);
 replace_skeleton_label(source, "CLAUSE_SPECIAL_ID_TYPE", clause_special_id_type);

 replace_skeleton_label(source, "TERMINAL_STATE_COUNT", format_hex(terminal_state_count, hex_digits(terminal_state_count)));
 replace_skeleton_label(source, "TERMINAL_ACCEPTS_CHUNK_COUNT", format_hex(terminal_accepts_chunk_count, hex_digits(terminal_accepts_chunk_count)));
 replace_skeleton_label(source, "CLAUSE_COUNT", format_hex(clause_count, hex_digits(clause_count)));
 replace_skeleton_label(source, "CLAUSE_CHILD_IDS_SIZE", format_hex(clause_child_ids.size(), hex_digits(clause_child_ids.size())));
 replace_skeleton_label(source, "CLAUSE_SEED_PARENT_IDS_SIZE", format_hex(clause_seed_parent_ids.size(), hex_digits(clause_seed_parent_ids.size())));
 replace_skeleton_label(source, "RULE_PARAMETER_COUNT", format_hex(rule_count, hex_digits(rule_count)));

 replace_skeleton_label(source, "CLAUSE_TAGS", format_list(clause_tags, 6));

 replace_skeleton_label(source, "CLAUSE_CHILD_IDS", format_hex_list(clause_child_ids, hex_digits(max_value(clause_child_ids)), 20));
 replace_skeleton_label(source, "CLAUSE_CHILD_IDS_OFFSETS", format_hex_list(clause_child_offsets, hex_digits(max_value(clause_child_offsets)), 20));

 replace_skeleton_label(source, "CLAUSE_SEED_PARENT_IDS", format_hex_list(clause_seed_parent_ids, hex_digits(max_value(clause_seed_parent_ids)), 20));
 replace_skeleton_label(source, "CLAUSE_SEED_PARENT_IDS_OFFSETS", format_hex_list(clause_seed_parent_offsets, hex_digits(max_value(clause_seed_parent_offsets)), 20));

 replace_skeleton_label(source, "CLAUSE_SPECIAL_IDS", format_hex_list(clause_special_ids, hex_digits(max_value(clause_special_ids)), 20));
 replace_skeleton_label(source, "CLAUSE_CAN_MATCH_ZERO", format_hex_list(clause_can_match_zero_chunks, hex_digits(max_value(clause_can_match_zero_chunks)), 20));

 replace_skeleton_label(source, "TERMINAL_TRANSITIONS", format_hex_list(terminal_transitions, hex_digits(max_value(terminal_transitions)), 20));
 replace_skeleton_label(source, "TERMINAL_TRANSITIONS_OFFSETS", format_hex_list(terminal_offsets, hex_digits(max_value(terminal_offsets)), 20));
 replace_skeleton_label(source, "TERMINAL_ACCEPTS", format_hex_list(terminal_accepts, hex_digits(max_value(terminal_accepts)), 20));

 replace_skeleton_label(source, "RULE_PARAMETER_DISPLAY_NAMES", format_list(rule_display_names, 6));
 replace_skeleton_label(source, "RULE_PARAMETER_ID_STRINGS", format_list(rule_id_strings, 6));
 replace_skeleton_label(source, "RULE_PARAMETER_ID_VALUES", format_hex_list(rule_id_values, hex_digits(max_value(rule_id_values)), 10));
 replace_skeleton_label(source, "RULE_PARAMETER_BOOLEANS", format_hex_list(rule_parameter_booleans, hex_digits(max_value(rule_parameter_booleans)), 20));

 _args._output_header << header;
 _args._output_source << source;
}

}  // namespace pmt::parser::builder
