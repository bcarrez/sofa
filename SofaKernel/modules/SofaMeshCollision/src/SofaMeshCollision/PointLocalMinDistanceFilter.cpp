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
#include <SofaMeshCollision/PointLocalMinDistanceFilter.h>

#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/visual/VisualParams.h>

#include <SofaMeshCollision/LineModel.h>
#include <SofaBaseTopology/TopologyData.inl>

#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/topology/Topology.h>

#include <sofa/simulation/Node.h>

#include <sofa/core/ObjectFactory.h>


namespace sofa::component::collision
{

PointInfo::PointInfo(LocalMinDistanceFilter *lmdFilters)
    : InfoFilter(lmdFilters)
{
}

void PointInfo::buildFilter(Index p_index)
{
    using sofa::simulation::Node;
    using sofa::helper::vector;
    using sofa::core::topology::BaseMeshTopology;


    bool debug=false;
    if(p_index == sofa::InvalidID)
        debug=true;

    m_noLineModel = false;

    // get the positions:
    const sofa::helper::vector<sofa::defaulttype::Vector3>& x = *this->position_filtering;
    const sofa::defaulttype::Vector3 &pt = x[p_index];

    // get the topology
    BaseMeshTopology* bmt = this->base_mesh_topology;
    const auto& edgesAroundVertex = bmt->getEdgesAroundVertex(p_index);
    const auto& trianglesAroundVertex = bmt->getTrianglesAroundVertex(p_index);

    if(edgesAroundVertex.size() ==0)
    {
        msg_warning("PointInfo")<<"no topology defined: no filtering"<<msgendl
                                <<"Mesh Topology found :"<<bmt->getName() ;

        m_noLineModel = true;
        setValid();
        return;
    }


    // compute the normal (nMean) of the point : IS IT STORED ANYWHERE ELSE ?
    // 1. using triangle around the point
    auto triIt = trianglesAroundVertex.begin();
    auto triItEnd = trianglesAroundVertex.end();
    sofa::defaulttype::Vector3 nMean;
    while (triIt != triItEnd)
    {
        const BaseMeshTopology::Triangle& triangle = bmt->getTriangle(*triIt);
        sofa::defaulttype::Vector3 nCur = (x[triangle[1]] - x[triangle[0]]).cross(x[triangle[2]] - x[triangle[0]]);
        nCur.normalize();
        nMean += nCur;
        ++triIt;
    }

    // 2. if no triangle around the point: compute an other normal using edges
    if (trianglesAroundVertex.empty())
    {
        msg_info_when(debug, "PointInfo") <<" trianglesAroundVertex.empty !";
        auto edgeIt = edgesAroundVertex.begin();
        auto edgeItEnd = edgesAroundVertex.end();

        while (edgeIt != edgeItEnd)
        {
            const BaseMeshTopology::Edge& edge = bmt->getEdge(*edgeIt);

            sofa::defaulttype::Vector3 l = (pt - x[edge[0]]) + (pt - x[edge[1]]);
            l.normalize();
            nMean += l;
            ++edgeIt;
        }
    }

    // 3. test to verify the normal value and normalize it
    if (nMean.norm() > 1e-20)
        nMean.normalize();
    else
    {
        msg_warning("PointInfo") << "PointInfo m_nMean is null";
    }

    msg_info_when(debug,"PointInfo")<<"  nMean ="<<nMean ;


    // Build the set of unit vector that are normal to the planes that defines the cone
    // for each plane, we can "extend" the cone: allow for a larger cone
    auto edgeIt = edgesAroundVertex.begin();
    auto edgeItEnd = edgesAroundVertex.end();

    m_computedData.clear();
    while (edgeIt != edgeItEnd)
    {
        const BaseMeshTopology::Edge& edge = bmt->getEdge(*edgeIt);

        sofa::defaulttype::Vector3 l = (pt - x[edge[0]]) + (pt - x[edge[1]]);
        l.normalize();



        double computedAngleCone = dot(nMean , l) * m_lmdFilters->getConeExtension();

        if (computedAngleCone < 0)
            computedAngleCone = 0.0;

        computedAngleCone += m_lmdFilters->getConeMinAngle();
        m_computedData.push_back(std::make_pair(l, computedAngleCone));
        ++edgeIt;

        msg_info_when(debug, "PointInfo") << "  l ="<<l<<"computedAngleCone ="
                                          << computedAngleCone<<"  for edge ["<<edge[0]<<"-"<<edge[1]<<"]" ;

    }


    setValid();
}



bool PointInfo::validate(const Index p, const defaulttype::Vector3 &PQ)
{

    bool debug=false;
    if (p == sofa::InvalidID)
        debug=true;

    if (isValid())
    {
        msg_info_when(debug, "PointInfo") << "Point "<<p<<" is valid";

        if (m_noLineModel)
        {
            msg_warning("PointInfo") << "No Line Model";
            return true;
        }

        TDataContainer::const_iterator it = m_computedData.begin();
        TDataContainer::const_iterator itEnd = m_computedData.end();

        while (it != itEnd)
        {
            msg_warning_when(debug, "PointInfo") <<" test avec direction : "<<it->first <<"   dot(it->first , PQ)="
                                                 <<dot(it->first , PQ)<<"    (-it->second * PQ.norm()) ="<<(-it->second * PQ.norm()) ;
            if (dot(it->first , PQ) <= (-it->second * PQ.norm()))
                return false;

            ++it;
        }

        return true;
    }
    else
    {
        msg_info_when(debug, "PointInfo") <<"Point "<<p<<" is not valid ------------ build" ;
        buildFilter(p);
        return validate(p, PQ);
    }
}


PointLocalMinDistanceFilter::PointLocalMinDistanceFilter()
    : m_pointInfo(initData(&m_pointInfo, "pointInfo", "point filter data"))
    , pointInfoHandler(nullptr)
    , bmt(nullptr)
{
}

PointLocalMinDistanceFilter::~PointLocalMinDistanceFilter()
{
    if (pointInfoHandler) delete pointInfoHandler;
}

void PointLocalMinDistanceFilter::init()
{
    bmt = getContext()->getMeshTopology();

    if (bmt != nullptr)
    {
        helper::vector< PointInfo >& pInfo = *(m_pointInfo.beginEdit());
        pInfo.resize(bmt->getNbPoints());
        m_pointInfo.endEdit();

        pointInfoHandler = new PointInfoHandler(this,&m_pointInfo);
        m_pointInfo.createTopologyHandler(bmt, pointInfoHandler);
        m_pointInfo.registerTopologicalData();
    }
    if(this->isRigid())
    {
        // Precomputation of the filters in the rigid case
        //points:
        helper::vector< PointInfo >& pInfo = *(m_pointInfo.beginEdit());
        for(Index p=0; p<pInfo.size(); p++)
        {
            pInfo[p].buildFilter(p);

        }
        m_pointInfo.endEdit();

    }

}



void PointLocalMinDistanceFilter::handleTopologyChange()
{
    if(this->isRigid())
    {
        msg_error() << "Filters optimization needed for topological change on rigid collision model";
        this->invalidate(); // all the filters will be recomputed, not only those involved in the topological change
    }
}

void PointLocalMinDistanceFilter::PointInfoHandler::applyCreateFunction(Index /*pointIndex*/, PointInfo &pInfo, const sofa::helper::vector<Index> &, const sofa::helper::vector< double >&)
{
    const PointLocalMinDistanceFilter *pLMDFilter = this->f;
    pInfo.setLMDFilters(pLMDFilter);

    sofa::core::topology::BaseMeshTopology * bmt = pLMDFilter->bmt; //getContext()->getMeshTopology();
    pInfo.setBaseMeshTopology(bmt);
    /////// TODO : template de la classe
    auto*  mstateVec3d= dynamic_cast<core::behavior::MechanicalState<sofa::defaulttype::Vec3Types>*>(pLMDFilter->getContext()->getMechanicalState());
    if(pLMDFilter->isRigid())
    {
        /////// TODO : template de la classe
        if(mstateVec3d != nullptr)
        {
            pInfo.setPositionFiltering(&(mstateVec3d->read(core::ConstVecCoordId::restPosition())->getValue()));
        }

    }
    else
    {
        /////// TODO : template de la classe
        if(mstateVec3d != nullptr)
        {
            pInfo.setPositionFiltering(&mstateVec3d->read(core::ConstVecCoordId::position())->getValue());
        }
    }

}



int PointLocalMinDistanceFilterClass = core::RegisterObject("This class manages Point collision models cones filters computations and updates.")
        .add< PointLocalMinDistanceFilter >()
        ;

} //namespace sofa::component::collision
