#pragma once

#include "SOP_Angle_And_Area_Smoothing.proto.h"
#include "SOP_Angle_And_Area_Smoothing_Info.hpp"

#include <GA/GA_Detail.h>
#include <GA/GA_GBMacros.h>
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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/angle_and_area_smoothing.h>
#include <CGAL/Polygon_mesh_processing/detect_features.h>

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
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point3 = Kernel::Point_3;
    using TriangleMesh = CGAL::Surface_mesh<Point3>;
    using VertexDescriptor = boost::graph_traits<TriangleMesh>::vertex_descriptor;
    using FaceDescriptor = TriangleMesh::Face_index;

    std::tuple<TriangleMesh, std::vector<VertexDescriptor>> toSurfaceMesh(GU_Detail* inputGeo) const
    {
    	TriangleMesh triangleMesh;
    	GA_Offset pointOffset;
    	GA_RWHandleV3 Phandle(inputGeo->findAttribute(GA_ATTRIB_POINT, "P"));
    	std::vector<VertexDescriptor> indexmap;
    	GA_FOR_ALL_PTOFF(inputGeo, pointOffset)
    	{
    		auto&& posHou = Phandle.get(pointOffset);
    		auto&& posCGAL = Point3(posHou.x(), posHou.y(), posHou.z());
    		auto&& vertex = triangleMesh.add_vertex(posCGAL);
    		indexmap.push_back(vertex);
    	}

        GEO_Primitive *prim;
    	std::vector<int32> pointOffsetMap;
    	GA_FOR_ALL_PRIMITIVES(inputGeo, prim)
    	{
    		GA_Range ptRange = prim->getPointRange(true);
    		for (GA_Iterator it(ptRange); !it.atEnd(); ++it)
    		{
    			GA_Offset ptoffset = (*it);
    			pointOffsetMap.push_back((int32)ptoffset);
    		}
    	}

    	for (size_t i =0;i< pointOffsetMap.size()/3;i++)
    	{
    		triangleMesh.add_face(indexmap[pointOffsetMap[i*3]], indexmap[pointOffsetMap[i*3+1]], indexmap[pointOffsetMap[i*3+2]]);
    	}

        return {triangleMesh, indexmap};
    }

    auto smoothing(TriangleMesh&& triangleMesh) const
    {
        
        auto&& eif = get(CGAL::edge_is_feature, triangleMesh);
        int32 angle = 60; //TODO: パラメータ化
        CGAL::Polygon_mesh_processing::detect_sharp_edges(triangleMesh, angle, eif);
        int sharp_counter = 0;
        for(auto&& e : edges(triangleMesh)) 
        {
            if(get(eif, e)) 
            {
                ++sharp_counter;
            }
        }

        unsigned int nb_iterations = 10; //TODO: パラメータ化
        CGAL::Polygon_mesh_processing::angle_and_area_smoothing(
                        triangleMesh,
                        CGAL::parameters::number_of_iterations(nb_iterations)
                            .use_safety_constraints(false) // authorize all moves
                            .edge_is_constrained_map(eif)
                    );

        return triangleMesh;
    }

    auto transferP(GU_Detail* geo, std::vector<VertexDescriptor>&& indexMap, TriangleMesh&& smoothedMesh) const
    {
    	GA_RWHandleV3 Phandle(geo->findAttribute(GA_ATTRIB_POINT, "P"));
    	GA_Offset pointOffset;
    	GA_FOR_ALL_PTOFF(geo, pointOffset)
    	{
            Phandle.set(pointOffset, {0,0,0});
    	}
    }

public:
    virtual void cook(const CookParms &cookparms) const
    {
        if (!cookparms.inputGeo(0))
            return;

        auto&& geo = cookparms.gdh().gdpNC();
        auto&& [triangleMesh, indexmap] = this->toSurfaceMesh(geo);
        auto&& smoothedMesh = this->smoothing(std::move(triangleMesh));
        this->transferP(geo, std::move(indexmap), std::move(smoothedMesh));
    }

};
