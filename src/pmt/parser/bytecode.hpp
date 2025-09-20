#pragma once

#include <cstdint>

namespace pmt::parser {

using ProgramCounterType = uint32_t;
using OpType = uint32_t;
using ArgType = uint32_t;

enum : uint64_t {
 BitWidthOp = 8,
 BitWidthArg = 32 - BitWidthOp,

 HasArg = 1u << (BitWidthOp - 1),

 OpCall = 0 | HasArg,
 OpJmp = 1 | HasArg,
 OpDrop = 2,
 OpPushCnt = 3 | HasArg,
 OpDecCntJnz = 4 | HasArg,
 OpMerge = 5,
 OpRet = 6 | HasArg,
 OpChoice = 7 | HasArg,
 OpCommit = 8 | HasArg,
 OpLitSeq = 9 | HasArg,
};

class Instruction {
public:
 uint32_t _op : BitWidthOp;
 uint32_t _arg : BitWidthArg;
};

}  // namespace pmt::parser