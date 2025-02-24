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

#include <SofaMiscMapping/config.h>

#include <SofaBaseMechanics/BarycentricMapping.h>
#include <SofaBaseTopology/TopologyData.h>
#include <SofaBaseTopology/TetrahedronSetTopologyContainer.h>
#include <SofaBaseTopology/TetrahedronSetGeometryAlgorithms.h>

#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperTetrahedronSetTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperHexahedronSetTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperTriangleSetTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperQuadSetTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperEdgeSetTopology.h>

#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperRegularGridTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperSparseGridTopology.h>
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperMeshTopology.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa::component::mapping
{

/// Class allowing barycentric mapping computation on a TetrahedronSetTopology in Vec3 -> Rigid case

template<class In, class Out>
class BarycentricMapperTetrahedronSetTopologyRigid : public TopologyBarycentricMapper<In,Out>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(BarycentricMapperTetrahedronSetTopologyRigid,In,Out),SOFA_TEMPLATE2(TopologyBarycentricMapper,In,Out));
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData3D MappingData;
    typedef helper::fixed_array<MappingData,3> MappingOrientData;

    typedef typename In::VecCoord VecCoord;
    typedef typename In::VecDeriv VecDeriv;

    enum { NIn = Inherit::NIn };
    enum { NOut = Inherit::NOut };
    typedef typename Inherit::MBloc MBloc;
    typedef typename Inherit::MatrixType MatrixType;
    typedef typename MatrixType::Index MatrixTypeIndex;

    typedef typename Inherit::ForceMask ForceMask;

    using Index = sofa::Index;

protected:
    topology::PointData< sofa::helper::vector<MappingData > >  map; ///< mapper data
    topology::PointData< sofa::helper::vector<MappingOrientData > >  mapOrient; ///< mapper data for mapped frames

    VecCoord actualTetraPosition;

    topology::TetrahedronSetTopologyContainer*			_fromContainer;
    topology::TetrahedronSetGeometryAlgorithms<In>*	_fromGeomAlgo;

    MatrixType* matrixJ;
    bool updateJ;

    /// TEMP
    VecDeriv actualOut;
    typename Out::VecCoord actualPos;

    /// TEMP
    BarycentricMapperTetrahedronSetTopologyRigid(topology::TetrahedronSetTopologyContainer* fromTopology, topology::PointSetTopologyContainer* _toTopology);
    virtual ~BarycentricMapperTetrahedronSetTopologyRigid() {}

public:
    void clear(std::size_t reserve=0) override;

    Index addPointInTetra(const Index index, const SReal* baryCoords) override;
    Index addPointOrientationInTetra( const Index tetraIndex, const sofa::defaulttype::Matrix3 baryCoorsOrient );

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in) override;

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in ) override;
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in ) override;
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in ) override;
    void applyJT( typename In::MatrixDeriv& out, const typename Out::MatrixDeriv& in ) override;

    const sofa::defaulttype::BaseMatrix* getJ(int outSize, int inSize) override;

    void draw(const core::visual::VisualParams*,const typename Out::VecCoord& out, const typename In::VecCoord& in) override;
    void resize( core::State<Out>* toModel ) override;

    //virtual int addContactPointFromInputMapping(const typename In::VecDeriv& in, const sofa::defaulttype::Vector3& /*pos*/, std::vector< std::pair<int, double> > & /*baryCoords*/);
};

template<class TInReal, class TOutReal>
class BarycentricMapperTetrahedronSetTopology< sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3,TInReal>,sofa::defaulttype::Vec<3,TInReal>,TInReal>, sofa::defaulttype::StdRigidTypes<3,TOutReal> > : public BarycentricMapperTetrahedronSetTopologyRigid< sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3,TInReal>,sofa::defaulttype::Vec<3,TInReal>,TInReal>, sofa::defaulttype::StdRigidTypes<3,TOutReal> >
{
public:
    typedef sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3,TInReal>,sofa::defaulttype::Vec<3,TInReal>,TInReal> In;
    typedef sofa::defaulttype::StdRigidTypes<3,TOutReal> Out;
    SOFA_CLASS(SOFA_TEMPLATE2(BarycentricMapperTetrahedronSetTopology,In,Out),SOFA_TEMPLATE2(BarycentricMapperTetrahedronSetTopologyRigid,In,Out));
    typedef BarycentricMapperTetrahedronSetTopologyRigid<In,Out> Inherit;

    BarycentricMapperTetrahedronSetTopology(topology::TetrahedronSetTopologyContainer* fromTopology, topology::PointSetTopologyContainer* _toTopology)
        : Inherit(fromTopology, _toTopology)
    {}

};



#if  !defined(SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPINGRIGID_CPP)
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapping< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperRegularGridTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperSparseGridTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperMeshTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperEdgeSetTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperTriangleSetTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperQuadSetTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperTetrahedronSetTopologyRigid< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperTetrahedronSetTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapperHexahedronSetTopology< defaulttype::Vec3Types, defaulttype::Rigid3Types >;


#endif

namespace _topologybarycentricmapper_
{
    extern template class SOFA_SOFAMISCMAPPING_API TopologyBarycentricMapper< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
} // namesapce _topologybarycentricmapper_

namespace _barycentricmapper_
{
    extern template class SOFA_SOFAMISCMAPPING_API BarycentricMapper< defaulttype::Vec3Types, defaulttype::Rigid3Types >;
} // namesapce _barycentricmapper_

} // namespace sofa::component::mapping

