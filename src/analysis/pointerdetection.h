#ifndef EGALITO_ANALYSIS_POINTERDETECTION_H
#define EGALITO_ANALYSIS_POINTERDETECTION_H

#include <vector>
#include <map>
#include "types.h"
#include "analysis/controlflow.h"
#include "analysis/slicingmatch.h"

class Function;
class Instruction;
class UDState;

class PointerDetection {
private:
    Function *function;
    ControlFlowGraph cfg;

    // just for comparing against slicing
    std::map<Instruction *, address_t> found;

public:
    PointerDetection(Function *function) : function(function), cfg(function) {}
    void detect();

private:
    void detectAtADR(UDState *state);
    void detectAtADRP(UDState *state);

    void checkLink(Instruction *instruction, address_t target);
};

class PageOffsetList {
private:
    typedef TreePatternBinary<TreeNodeAddition,
        TreePatternCapture<TreePatternTerminal<TreeNodePhysicalRegister>>,
        TreePatternCapture<TreePatternTerminal<TreeNodeConstant>>
    > OffsetAdditionForm;

    typedef TreePatternUnary<TreeNodeDereference,
        OffsetAdditionForm
    > PointerLoadForm;

    std::map<UDState *, int64_t> list;
    std::map<UDState *, std::vector<int>> seen;

public:
    bool detectOffset(UDState *state, int reg);
    bool detectOffsetAfterCopy(UDState *state, int reg);
    bool detectOffsetAfterPush(UDState *state, int reg);

    bool findInAdd(UDState *state, int reg);
    bool findInLoad(UDState *state, int reg);
    bool findInStore(UDState *state, int reg);
    void addToList(UDState *state, int64_t offset)
        { list[state] = offset; }
    size_t getCount() const { return list.size(); }
    const std::map<UDState *, int64_t>& getList() const { return list; }
};
#endif