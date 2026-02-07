#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast);
PMT_FW_DECL_NS_CLASS(pmt::rt, RuleParametersBase);

namespace pmt::meta {

struct AstPosition {
 pmt::rt::Ast* _parent;
 size_t _child_idx;
};

struct GatheringFrame {
 AstPosition _position;
 bool _expanded;
};

struct AstNodeKey {
 pmt::rt::Ast const* _node;
};

struct AstNodeHash {
 auto operator()(AstNodeKey const& key_) const -> size_t;
};

struct AstNodeEq {
 auto operator()(AstNodeKey const& lhs_, AstNodeKey const& rhs_) const -> bool;
};

void add_rule(pmt::rt::Ast& ast_dest_root_, std::string const& rule_name_, pmt::rt::RuleParametersBase const& rule_parameters_, pmt::rt::Ast const& expr_);

}  // namespace pmt::meta
