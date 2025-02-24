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
#include <SofaDeformable/config.h>

#include <sofa/helper/types/RGBAColor.h>

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/helper/vector.h>
#include <SofaEigen2Solver/EigenSparseMatrix.h>


namespace sofa::core::behavior
{

template< class T > class MechanicalState;

} // namespace sofa::core::behavior

namespace sofa::component::forcefield
{

/**
* @brief This class describes a simple elastic springs ForceField between DOFs positions and rest positions.
*
* Springs are applied to given degrees of freedom between their current positions and their rest shape positions.
* An external MechanicalState reference can also be passed to the ForceField as rest shape position.
*/
template<class DataTypes>
class RestShapeSpringsForceField : public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(RestShapeSpringsForceField, DataTypes), SOFA_TEMPLATE(core::behavior::ForceField, DataTypes));

    typedef core::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::CPos CPos;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef helper::vector< sofa::Index > VecIndex;
    typedef helper::vector< Real >	 VecReal;

    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;

    Data< helper::vector< sofa::Index > > d_points; ///< points controlled by the rest shape springs
    Data< VecReal > d_stiffness; ///< stiffness values between the actual position and the rest shape position
    Data< VecReal > d_angularStiffness; ///< angularStiffness assigned when controlling the rotation of the points
    Data< helper::vector< CPos > > d_pivotPoints; ///< global pivot points used when translations instead of the rigid mass centers
    Data< helper::vector< sofa::Index > > d_external_points; ///< points from the external Mechancial State that define the rest shape springs
    Data< bool > d_recompute_indices; ///< Recompute indices (should be false for BBOX)
    Data< bool > d_drawSpring; ///< draw Spring
    Data< sofa::helper::types::RGBAColor > d_springColor; ///< spring color. (default=[0.0,1.0,0.0,1.0])

    SingleLink<RestShapeSpringsForceField<DataTypes>, sofa::core::behavior::MechanicalState< DataTypes >, BaseLink::FLAG_STOREPATH|BaseLink::FLAG_STRONGLINK> l_restMState;
    linearsolver::EigenBaseSparseMatrix<typename DataTypes::Real> matS;

protected:
    RestShapeSpringsForceField();

public:
    /// BaseObject initialization method.
    void bwdInit() override ;
    void parse(core::objectmodel::BaseObjectDescription *arg) override ;
    void reinit() override ;

    /// Add the forces.
    void addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;

    void addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx) override;

    SReal getPotentialEnergy(const core::MechanicalParams* mparams, const DataVecCoord& x) const override
    {
        SOFA_UNUSED(mparams);
        SOFA_UNUSED(x);

        msg_warning() << "Method getPotentialEnergy not implemented yet.";
        return 0.0;
    }

    /// Brings ForceField contribution to the global system stiffness matrix.
    void addKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix ) override;

    void addSubKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix, const helper::vector<sofa::Index> & addSubIndex ) override;

    void draw(const core::visual::VisualParams* vparams) override;


    const DataVecCoord* getExtPosition() const;
    const VecIndex& getIndices() const { return m_indices; }
    const VecIndex& getExtIndices() const { return (useRestMState ? m_ext_indices : m_indices); }

    void updateForceMask() override;

protected :

    void recomputeIndices();
    bool checkOutOfBoundsIndices();
    bool checkOutOfBoundsIndices(const VecIndex &indices, const sofa::Size dimension);

    VecIndex m_indices;
    VecIndex m_ext_indices;
    helper::vector<CPos> m_pivots;

    SReal lastUpdatedStep;

private :

    bool useRestMState; /// An external MechanicalState is used as rest reference.
};

#if  !defined(SOFA_COMPONENT_FORCEFIELD_RESTSHAPESPRINGSFORCEFIELD_CPP)
extern template class SOFA_SOFADEFORMABLE_API RestShapeSpringsForceField<sofa::defaulttype::Vec3Types>;
extern template class SOFA_SOFADEFORMABLE_API RestShapeSpringsForceField<sofa::defaulttype::Vec1Types>;
extern template class SOFA_SOFADEFORMABLE_API RestShapeSpringsForceField<sofa::defaulttype::Rigid3Types>;
#endif

} // namespace sofa::component::forcefield
