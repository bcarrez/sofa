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
#include <SofaBoundaryCondition/config.h>

#include <sofa/core/behavior/ProjectiveConstraintSet.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/BaseVector.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/vector.h>
#include <SofaBaseTopology/TopologySubsetData.h>
#include <set>

namespace sofa::component::projectiveconstraintset
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template <class DataTypes>
class FixedConstraintInternalData
{

};


/** Maintain a constant velocity.
 * If the particle is initially fixed then it is attached to its initial position.
 * Otherwise it keeps on drifting.
 * For maintaining particles fixed in any case, @sa ProjectToPointConstraint
*/
template <class DataTypes>
class FixedConstraint : public core::behavior::ProjectiveConstraintSet<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(FixedConstraint,DataTypes),SOFA_TEMPLATE(sofa::core::behavior::ProjectiveConstraintSet, DataTypes));

    using Index = sofa::Index;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::MatrixDeriv MatrixDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename MatrixDeriv::RowIterator MatrixDerivRowIterator;
    typedef typename MatrixDeriv::RowType MatrixDerivRowType;
    typedef Data<VecCoord> DataVecCoord;
    typedef Data<VecDeriv> DataVecDeriv;
    typedef Data<MatrixDeriv> DataMatrixDeriv;
    typedef helper::vector<Index> SetIndexArray;
    typedef sofa::component::topology::PointSubsetData< SetIndexArray > SetIndex;
    typedef sofa::core::topology::Point Point;
    typedef sofa::defaulttype::Vector3 Vector3;

protected:
    FixedConstraint();

    virtual ~FixedConstraint();

public:
    SetIndex d_indices;
    Data<bool> d_fixAll; ///< filter all the DOF to implement a fixed object
    Data<bool> d_showObject; ///< draw or not the fixed constraints
    Data<SReal> d_drawSize; ///< 0 -> point based rendering, >0 -> radius of spheres
    Data<bool> d_projectVelocity; ///< activate project velocity to set velocity to zero

    /// Link to be set to the topology container in the component graph.
    SingleLink<FixedConstraint<DataTypes>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;
protected:
    FixedConstraintInternalData<DataTypes>* data;
    friend class FixedConstraintInternalData<DataTypes>;


public:
    void clearConstraints();
    void addConstraint(Index index);
    void removeConstraint(Index index);

    // -- Constraint interface
    void init() override;
    void reinit() override;

    void projectResponse(const core::MechanicalParams* mparams, DataVecDeriv& resData) override;
    void projectVelocity(const core::MechanicalParams* mparams, DataVecDeriv& vData) override;
    void projectPosition(const core::MechanicalParams* mparams, DataVecCoord& xData) override;
    void projectJacobianMatrix(const core::MechanicalParams* mparams, DataMatrixDeriv& cData) override;


    void applyConstraint(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;
    void applyConstraint(const core::MechanicalParams* mparams, defaulttype::BaseVector* vector, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;

    /** Project the the given matrix (Experimental API).
      See doc in base parent class
      */
    void projectMatrix( sofa::defaulttype::BaseMatrix* /*M*/, unsigned /*offset*/ ) override;


    void draw(const core::visual::VisualParams* vparams) override;

    bool fixAllDOFs() const { return d_fixAll.getValue(); }

    class FCPointHandler : public sofa::component::topology::TopologyDataHandler<core::topology::BaseMeshTopology::Point, SetIndexArray >
    {
    public:
        typedef typename FixedConstraint<DataTypes>::SetIndexArray SetIndexArray;

        FCPointHandler(FixedConstraint<DataTypes>* _fc, sofa::component::topology::PointSubsetData<SetIndexArray>* _data)
            : sofa::component::topology::TopologyDataHandler<core::topology::BaseMeshTopology::Point, SetIndexArray >(_data), fc(_fc) {}


        void applyDestroyFunction(Index /*index*/, value_type& /*T*/);


        bool applyTestCreateFunction(Index /*index*/,
                const sofa::helper::vector< Index > & /*ancestors*/,
                const sofa::helper::vector< double > & /*coefs*/);
    protected:
        FixedConstraint<DataTypes> *fc;
    };

protected :
    /// Function check values of given indices
    void checkIndices();
    
    /// Handler for subset Data
    FCPointHandler* m_pointHandler;

};

#if  !defined(SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_FIXEDCONSTRAINT_CPP)
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Vec3Types>;
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Vec2Types>;
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Vec1Types>;
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Vec6Types>;
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Rigid3Types>;
extern template class SOFA_SOFABOUNDARYCONDITION_API FixedConstraint<defaulttype::Rigid2Types>;

#endif

} // namespace sofa::component::projectiveconstraintset
