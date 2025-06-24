#pragma once

#include "pmt/parser/generic_id.hpp"

namespace pmt::parser::grammar {

class Ast {
public:
 // -$ Types / Constants $-
 // clang-format off
  static inline char const        NAME_RESERVED_PREFIX_CH       = '@';                                                        // NOLINT
  static inline std::string const TERMINAL_DIRECT_PREFIX        = std::string(1, NAME_RESERVED_PREFIX_CH) + "direct_";        // NOLINT
  static inline std::string const TERMINAL_COMMENT_OPEN_PREFIX  = std::string(1, NAME_RESERVED_PREFIX_CH) + "comment_open_";  // NOLINT
  static inline std::string const TERMINAL_COMMENT_CLOSE_PREFIX = std::string(1, NAME_RESERVED_PREFIX_CH) + "comment_close_"; // NOLINT
  static inline std::string const TERMINAL_NAME_WHITESPACE      = std::string(1, NAME_RESERVED_PREFIX_CH) + "whitespace";     // NOLINT
  static inline std::string const TERMINAL_NAME_START           = std::string(1, NAME_RESERVED_PREFIX_CH) + "start";          // NOLINT
  static inline std::string const NAME_EOI                      = std::string(1, NAME_RESERVED_PREFIX_CH) + "eoi";            // NOLINT
  static inline std::string const TERMINAL_NAME_LINECOUNT       = std::string(1, NAME_RESERVED_PREFIX_CH) + "linecount";      // NOLINT
 // clang-format on

 enum : GenericId::IdType {
#include "pmt/parser/grammar/id_constants-inl.hpp"
  NtTerminalHidden,
  NtPermute,
  NtPermuteDelimited
 };

 static auto id_to_string(GenericId::IdType id_) -> std::string;
};

}  // namespace pmt::parser::grammar