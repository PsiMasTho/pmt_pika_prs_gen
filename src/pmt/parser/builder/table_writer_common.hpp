#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <concepts>
#include <functional>
#include <span>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::parser::builder {

class TableWriterCommon {
 public:
  // -$ Types / Constants $-
  using TransitionMaskQueryFn = std::function<pmt::base::Bitset::ChunkSpanConst(pmt::util::sm::StateNrType)>;

  // -$ Functions $-
  // --$ Other $--

  template <typename T_>
    requires std::integral<typename T_::value_type>
  static void replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, T_ begin_, T_ end_);

  template <std::integral T_>
  static void replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, std::span<T_ const> const& entries_);
  static void replace_array(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, std::span<std::string const> const& entries_);

  static void replace_transitions(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& prefix_, pmt::util::sm::ct::StateMachine const& state_machine_);

  static auto as_hex(std::integral auto value_, bool hex_prefix_ = true) -> std::string;

  static void replace_transition_masks(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_, std::string const& label_, pmt::util::sm::StateNrType state_nr_max_, TransitionMaskQueryFn const& fn_query_mask_);

  static void replace_timestamp(pmt::util::SkeletonReplacerBase& skeleton_replacer_, std::string& str_);

  template <typename T_>
    requires std::integral<typename T_::value_type>
  static auto get_smallest_unsigned_type(T_ begin_, T_ end_) -> std::string;

  template <std::integral T_>
  static auto get_smallest_unsigned_type(std::span<T_ const> data_) -> std::string;

 private:
  static auto calculate_numeric_entries_per_line(size_t max_width_) -> size_t;
};

}  // namespace pmt::parser::builder

#include "pmt/parser/builder/table_writer_common-inl.hpp"