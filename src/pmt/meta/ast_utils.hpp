#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast);

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

}  // namespace pmt::meta
