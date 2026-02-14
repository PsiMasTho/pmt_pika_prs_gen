#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/rt/primitives.hpp"

#include <functional>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::sm, StateMachine)

namespace pmt::builder {

using FinalIdToStringFn = std::function<std::string(pmt::rt::IdType)>;
auto terminal_fsm_to_dot_str(pmt::sm::StateMachine const& state_machine_, FinalIdToStringFn const& final_id_to_str_, std::string terminal_dotfile_skel_) -> std::string;

}  // namespace pmt::builder
