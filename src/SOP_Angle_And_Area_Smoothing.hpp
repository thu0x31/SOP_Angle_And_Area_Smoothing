#pragma once

#include "SOP_Angle_And_Area_Smoothing_Verb.hpp"

#include <SOP/SOP_Node.h>

class SOP_Angle_And_Area_Smoothing : public SOP_Node
{
public:
    static PRM_Template *buildTemplates();
    static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
    {
        return new SOP_Angle_And_Area_Smoothing(net, name, op);
    }
    const SOP_NodeVerb *cookVerb() const override
    { 
        return SOP_Angle_And_Area_Smoothing_Verb::theVerb.get();
    }
    
protected:
    SOP_Angle_And_Area_Smoothing(OP_Network *net, const char *name, OP_Operator *op)
        : SOP_Node(net, name, op)
    {
        // All verb SOPs must manage data IDs, to track what's changed
        // from cook to cook.
        mySopFlags.setManagesDataIDs(true);
    }
    ~SOP_Angle_And_Area_Smoothing() override {}
    /// Since this SOP implements a verb, cookMySop just delegates to the verb.
    OP_ERROR cookMySop(OP_Context &context) override
    {
        return cookMyselfAsVerb(context);
    }
};