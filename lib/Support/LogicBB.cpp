//===- LogicBB.cpp - Infrastructure for working w/ logical BBs --*- C++ -*-===//
//
// Implements the infrastructure useful for handling logical basic
// blocks (logical BBs) in Handshake functions.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Support/LogicBB.h"

using namespace circt;
using namespace dynamatic;

dynamatic::LogicBBs dynamatic::getLogicBBs(handshake::FuncOp funcOp) {
  dynamatic::LogicBBs logicBBs;
  for (auto &op : funcOp.getOps())
    if (auto bbAttr = op.getAttrOfType<mlir::IntegerAttr>(BB_ATTR); bbAttr)
      logicBBs.blocks[bbAttr.getValue().getZExtValue()].push_back(&op);
    else
      logicBBs.outOfBlocks.push_back(&op);
  return logicBBs;
}

bool dynamatic::inheritBB(Operation *srcOp, Operation *dstOp) {
  if (auto bb = srcOp->getAttrOfType<mlir::IntegerAttr>(BB_ATTR)) {
    dstOp->setAttr(BB_ATTR, bb);
    return true;
  }
  return false;
}

bool dynamatic::inheritBBFromValue(Value val, Operation *dstOp) {
  if (Operation *defOp = val.getDefiningOp())
    return inheritBB(defOp, dstOp);
  dstOp->setAttr(BB_ATTR,
                 OpBuilder(val.getContext()).getUI32IntegerAttr(ENTRY_BB));
  return true;
}

std::optional<unsigned> dynamatic::getLogicBB(Operation *op) {
  if (auto bb = op->getAttrOfType<mlir::IntegerAttr>(BB_ATTR))
    return bb.getUInt();
  return {};
}