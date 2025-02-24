/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaMeshCollision/config.h>

#include <SofaMeshCollision/LocalMinDistanceFilter.h>
#include <SofaMeshCollision/LineLocalMinDistanceFilter.h>
#include <SofaMeshCollision/PointLocalMinDistanceFilter.h>
#include <SofaBaseTopology/TopologyData.h>

#include <sofa/defaulttype/VecTypes.h>

namespace sofa::component::collision
{
    
/**
 * @brief LocalMinDistance cone information class for a Triangle collision primitive.
 */
class TriangleInfo : public InfoFilter //< Triangle >
{
    using Index = sofa::Index;
    typedef sofa::core::topology::BaseMeshTopology::Triangle Triangle;
public:
    /**
     * @brief Default constructor.
     */
    TriangleInfo(LocalMinDistanceFilter *lmdFilters = nullptr);

    /**
     * @brief Default destructor.
     */
    ~TriangleInfo() override {}

    /**
     * @brief Returns the validity of a detected contact according to this TriangleInfo.
     */
    bool validate(const Index /*p*/, const defaulttype::Vector3 & /*PQ*/) override;
    /**
     * @brief Output stream.
     */
    inline friend std::ostream& operator<< ( std::ostream& os, const TriangleInfo& /*ti*/ )
    {
        return os;
    }

    /**
     * @brief Input stream.
     */
    inline friend std::istream& operator>> ( std::istream& in, TriangleInfo& /*ti*/ )
    {
        return in;
    }

    /**
     * @brief Computes the region of interest cone of the Triangle primitive.
     */
    //virtual void buildFilter(const Triangle & /*t*/);
    void buildFilter(Index /*t*/) override;

protected:


    sofa::defaulttype::Vector3 m_normal; ///< Stored normal of the triangle.
};



/**
 * @brief
 */
class SOFA_SOFAMESHCOLLISION_API TriangleLocalMinDistanceFilter : public LocalMinDistanceFilter
{
public:
    SOFA_CLASS(TriangleLocalMinDistanceFilter, LocalMinDistanceFilter);

protected:
    TriangleLocalMinDistanceFilter();
    ~TriangleLocalMinDistanceFilter() override;

public:

    /**
     * @brief Scene graph initialization method.
     */
    void init() override;

    /**
     * @brief Handle topological changes.
     */
    void handleTopologyChange() override;

    /**
     * @name These methods check the validity of a found intersection.
     * According to the local configuration around the found intersected primitive, we build a "Region Of Interest" geometric cone.
     * Pertinent intersections have to belong to this cone, others are not taking into account anymore.
     * If the filtration cone is unbuilt or invalid, these methods launch the build or update.
     */
    //@{

    /**
     * @brief Point Collision Primitive validation method.
     */
    bool validPoint(const Index pointIndex, const defaulttype::Vector3 &PQ);

    /**
     * @brief Line Collision Primitive validation method.
     */
    bool validLine(const Index lineIndex, const defaulttype::Vector3 &PQ);

    /**
     * @brief Triangle Collision Primitive validation method.
     */
    bool validTriangle(const Index triangleIndex, const defaulttype::Vector3 &PQ);

    //@}

    /**
     * @brief New Points creations callback.
     */
    class PointInfoHandler : public topology::TopologyDataHandler<core::topology::BaseMeshTopology::Point, helper::vector<PointInfo> >
    {
    public:
        PointInfoHandler(TriangleLocalMinDistanceFilter* _f, topology::PointData<helper::vector<PointInfo> >* _data) : topology::TopologyDataHandler<core::topology::BaseMeshTopology::Point, helper::vector<PointInfo> >(_data), f(_f) {}

        void applyCreateFunction(Index pointIndex, PointInfo& m, const sofa::helper::vector< Index > &,
                const sofa::helper::vector< double > &);
    protected:
        TriangleLocalMinDistanceFilter* f;
    };

    /**
     * @brief New Edges creations callback.
     */
    class LineInfoHandler : public topology::TopologyDataHandler<core::topology::BaseMeshTopology::Edge, helper::vector<LineInfo> >
    {
    public:
        LineInfoHandler(TriangleLocalMinDistanceFilter* _f, topology::EdgeData<helper::vector<LineInfo> >* _data) : topology::TopologyDataHandler<core::topology::BaseMeshTopology::Edge, helper::vector<LineInfo> >(_data), f(_f) {}

        void applyCreateFunction(Index edgeIndex, LineInfo& m, const core::topology::BaseMeshTopology::Edge&, const sofa::helper::vector< Index > &,
                const sofa::helper::vector< double > &);
    protected:
        TriangleLocalMinDistanceFilter* f;
    };

    /**
     * @brief New Triangles creations callback.
     */
    class TriangleInfoHandler : public topology::TopologyDataHandler<core::topology::BaseMeshTopology::Triangle, helper::vector<TriangleInfo> >
    {
    public:
        TriangleInfoHandler(TriangleLocalMinDistanceFilter* _f, topology::TriangleData<helper::vector<TriangleInfo> >* _data) : topology::TopologyDataHandler<core::topology::BaseMeshTopology::Triangle, helper::vector<TriangleInfo> >(_data), f(_f) {}

        void applyCreateFunction(Index triangleIndex, TriangleInfo& m, const core::topology::BaseMeshTopology::Triangle&, const sofa::helper::vector< Index > &,
                const sofa::helper::vector< double > &);
    protected:
        TriangleLocalMinDistanceFilter* f;
    };

    /// Link to be set to the topology container in the component graph.
    SingleLink<TriangleLocalMinDistanceFilter, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

private:
    topology::PointData< sofa::helper::vector<PointInfo> > m_pointInfo; ///< point filter data
    topology::EdgeData< sofa::helper::vector<LineInfo> > m_lineInfo; ///< line filter data
    topology::TriangleData< sofa::helper::vector<TriangleInfo> > m_triangleInfo; ///< triangle filter data

    PointInfoHandler* pointInfoHandler;
    LineInfoHandler* lineInfoHandler;
    TriangleInfoHandler* triangleInfoHandler;

    core::topology::BaseMeshTopology *bmt;
};


} //namespace sofa::component::collision
