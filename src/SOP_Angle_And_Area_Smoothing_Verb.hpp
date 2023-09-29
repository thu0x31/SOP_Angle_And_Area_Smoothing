#pragma once

#include "SOP_Angle_And_Area_Smoothing.proto.h"
#include "SOP_Angle_And_Area_Smoothing_Info.hpp"

#include <GA/GA_Detail.h>
#include <GA/GA_GBMacros.h>
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

    std::tuple<TriangleMesh, std::vector<VertexDescriptor>> toSurfaceMesh(GU_Detail* geo) const
    {
    	TriangleMesh triangleMesh;
    	GA_Offset pointOffset;
    	GA_RWHandleV3 Phandle(geo->findAttribute(GA_ATTRIB_POINT, "P"));
    	std::vector<VertexDescriptor> indexmap;
    	GA_FOR_ALL_PTOFF(geo, pointOffset)
    	{
    		auto&& posHou = Phandle.get(pointOffset);
    		auto&& posCGAL = Point3(posHou.x(), posHou.y(), posHou.z());
    		auto&& vertex = triangleMesh.add_vertex(posCGAL);
    		indexmap.push_back(vertex);
    	}

        GEO_Primitive *prim;
    	std::vector<int32> pointIndexMap;
        GA_FOR_ALL_PRIMITIVES(geo, prim)
        {
            GA_Range pointRange = prim->getPointRange();
            for (GA_Iterator it(pointRange); !it.atEnd(); ++it)
            {
                GA_Offset pointOffset = (*it);
                GA_Index pointIndex = geo->pointIndex(pointOffset);

                pointIndexMap.push_back((int32)pointIndex);
            }
        }

        constexpr int PolyVerticesSize = 3;
    	for (size_t i = 0; i < pointIndexMap.size()/PolyVerticesSize; i++)
    	{
    		auto&& faceIndex = triangleMesh.add_face(
                    indexmap[pointIndexMap[i*PolyVerticesSize]],
                    indexmap[pointIndexMap[i*PolyVerticesSize+1]],
                    indexmap[pointIndexMap[i*PolyVerticesSize+2]]
                );

            if(faceIndex == TriangleMesh::null_face()) {
                std::cout << "null face" << std::endl;
            }
    	}

        return {triangleMesh, indexmap};
    }

    auto smoothing(TriangleMesh&& triangleMesh, const SOP_Angle_And_Area_SmoothingParms& parms) const
    {
        auto&& eif = get(CGAL::edge_is_feature, triangleMesh);
        CGAL::Polygon_mesh_processing::detect_sharp_edges(triangleMesh, parms.getAngle(), eif);
        for(auto&& e : edges(triangleMesh))
        {
            get(eif, e);
        }

        if (parms.getUseconstrainededge()) {
            CGAL::Polygon_mesh_processing::angle_and_area_smoothing(
                triangleMesh,
                CGAL::parameters::number_of_iterations(parms.getIteration())
                    .use_safety_constraints(parms.getSafetyconstraints())
                    .use_area_smoothing(parms.getUseareasmoothing())
                    .use_angle_smoothing(parms.getUseanglesmoothing())
                    .edge_is_constrained_map(eif)
                    .use_Delaunay_flips(parms.getUsedelaunayflips())
            );
        } else {
            CGAL::Polygon_mesh_processing::angle_and_area_smoothing(
                triangleMesh,
                CGAL::parameters::number_of_iterations(parms.getIteration())
                    .use_safety_constraints(parms.getSafetyconstraints())
                    .use_area_smoothing(parms.getUseareasmoothing())
                    .use_angle_smoothing(parms.getUseanglesmoothing())
                    .use_Delaunay_flips(parms.getUsedelaunayflips())
            );
        }

        return triangleMesh;
    }

    auto transferP(GU_Detail* geo, std::vector<VertexDescriptor>&& indexMap, TriangleMesh&& smoothedMesh) const
    {
    	GA_RWHandleV3 Phandle(geo->findAttribute(GA_ATTRIB_POINT, "P"));
    	GA_Offset pointOffset;

    	GA_FOR_ALL_PTOFF(geo, pointOffset)
    	{
            auto&& index = geo->pointIndex(pointOffset);
            auto&& pos = smoothedMesh.point(indexMap[index]);
            Phandle.set(indexMap[index].idx(), UT_Vector3F(pos.x(),pos.y(),pos.z()));
    	}
    }

    auto toHoudiniGeo(GU_Detail* geo, TriangleMesh&& smoothedMesh) const
    {
        geo->clearAndDestroy();

        std::map<CGAL::Surface_mesh<Point3>::Vertex_index, GA_Offset> vertexMap;
        const GA_Size&& numVertices = smoothedMesh.num_vertices();
        const GA_Offset&& startptoff = geo->appendPointBlock(numVertices);
        exint&& pointIndex = 0;
        for (auto&& v : smoothedMesh.vertices())
        {
            auto&& p = smoothedMesh.point(v);
            GA_Offset&& ptOffset = startptoff + pointIndex;
            geo->setPos3(ptOffset, UT_Vector3(p.x(), p.y(), p.z()));
            vertexMap[v] = ptOffset;

            pointIndex++;
        }

        std::vector<int> indices;
        for (auto&& f : smoothedMesh.faces())
        {
            geo->appendPrimitive(GA_PRIMPOLY);
            for (auto&& v : vertices_around_face(smoothedMesh.halfedge(f), smoothedMesh))
            {
                indices.push_back(vertexMap[v]);
            }
        }

        GA_PolyCounts polyCounts;
        polyCounts.append(3, smoothedMesh.number_of_faces());
        GEO_PrimPoly::buildBlock(
                            geo,
                            static_cast<GA_Offset>(0),
                            static_cast<GA_Size>(smoothedMesh.number_of_vertices()),
                            polyCounts,
                            indices.data()
                        );
    }

public:
    virtual void cook(const CookParms &cookparms) const
    {
        auto&& outputGeo = cookparms.gdh().gdpNC();

        auto&& inputGeo = cookparms.inputGeo(0);
        UT_ASSERT(inputGeo);

        // TODO: is Triangle

        outputGeo->clearAndDestroy();
        outputGeo->copy(*inputGeo);

        auto&& [triangleMesh, indexmap] = this->toSurfaceMesh(outputGeo);

        auto&& sopParameter = cookparms.parms<SOP_Angle_And_Area_SmoothingParms>();
        auto&& smoothedMesh = this->smoothing(std::move(triangleMesh), sopParameter);

        if(sopParameter.getUsedelaunayflips()) {
            this->toHoudiniGeo(outputGeo, std::move(smoothedMesh));
        } else {
            this->transferP(outputGeo, std::move(indexmap), std::move(smoothedMesh));
        }
    }
};
