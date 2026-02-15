#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::pika::rt, Ast);
PMT_FW_DECL_NS_CLASS(pmt::pika::rt, RuleParametersBase);

namespace pmt::pika::meta {

struct AstPosition {
 pmt::pika::rt::Ast* _parent;
 size_t _child_idx;
};

struct GatheringFrame {
 AstPosition _position;
 bool _expanded;
};

struct AstNodeKey {
 pmt::pika::rt::Ast const* _node;
};

struct AstNodeHash {
 auto operator()(AstNodeKey const& key_) const -> size_t;
};

struct AstNodeEq {
 auto operator()(AstNodeKey const& lhs_, AstNodeKey const& rhs_) const -> bool;
};

auto add_rule(pmt::pika::rt::Ast& ast_dest_root_, std::string const& rule_name_, pmt::pika::rt::RuleParametersBase const& rule_parameters_, pmt::pika::rt::Ast const& expr_) -> pmt::pika::rt::Ast*;

}  // namespace pmt::pika::meta
