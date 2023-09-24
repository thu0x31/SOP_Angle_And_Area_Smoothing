#pragma once

#include "SOP_Angle_And_Area_Smoothing.proto.h"

#include "SOP_Angle_And_Area_Smoothing_Info.hpp"
#include <GA/GA_Detail.h>
#include <GA/GA_PolyCounts.h>
#include <GEO/GEO_PrimPoly.h>
#include <GEO/GEO_Point.h>
#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_on_sphere_2.h>
#include <CGAL/Projection_on_sphere_traits_3.h>

#include <vector>
#include <unordered_map>

class SOP_Angle_And_Area_Smoothing_Verb : public SOP_NodeVerb
{
public:
    inline static const SOP_NodeVerb::Register<SOP_Angle_And_Area_Smoothing_Verb> theVerb;

    SOP_Angle_And_Area_Smoothing_Verb() {}
    virtual ~SOP_Angle_And_Area_Smoothing_Verb() {}

    virtual SOP_NodeParms *allocParms() const
    {
        return new SOP_Angle_And_Area_SmoothingParms();
    }

    virtual UT_StringHolder name() const 
    {
        return SOP_Angle_And_Area_Smoothing_Info::TYPENAME;
    }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const 
    {
        return COOK_GENERIC; 
    }

private:
    auto makePoly() const
    {
    }

public:
    virtual void cook(const CookParms &cookparms) const
    {
    }

};
