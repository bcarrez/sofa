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
#ifndef SOFA_COMPONENT_MAPPING_BaseStrainMAPPING_H
#define SOFA_COMPONENT_MAPPING_BaseStrainMAPPING_H

#include <Flexible/config.h>
#include <sofa/core/Mapping.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/simulation/Simulation.h>
#include <sofa/core/MechanicalParams.h>
#include <sofa/core/Mapping.h>

#include <SofaEigen2Solver/EigenSparseMatrix.h>

#include <sofa/helper/IndexOpenMP.h>


#include "../types/DeformationGradientTypes.h"
#include "../types/StrainTypes.h"


namespace sofa
{
namespace component
{
namespace mapping
{


/** Abstract interface to allow for resizing
*/
class SOFA_Flexible_API BaseStrainMapping : public virtual core::objectmodel::BaseObject
{
protected:
    BaseStrainMapping() {}
private:
    BaseStrainMapping(const BaseStrainMapping& b);
    BaseStrainMapping& operator=(const BaseStrainMapping& b);

public:
    virtual void resizeOut()=0;
    virtual void applyJT()=0;
};



/** Abstract one-to-one mapping (one parent->one child) using JacobianBlocks or sparse eigen matrix
*/

template <class JacobianBlockType>
class BaseStrainMappingT : public core::Mapping<typename JacobianBlockType::In,typename JacobianBlockType::Out>, public BaseStrainMapping
{
public:
    typedef core::Mapping<typename JacobianBlockType::In, typename JacobianBlockType::Out> Inherit;
    SOFA_ABSTRACT_CLASS2(SOFA_TEMPLATE(BaseStrainMappingT,JacobianBlockType), SOFA_TEMPLATE2(core::Mapping,typename JacobianBlockType::In,typename JacobianBlockType::Out),BaseStrainMapping);

    /** @name  Input types    */
    //@{
    typedef typename JacobianBlockType::In In;
    typedef typename In::Coord InCoord;
    typedef typename In::Deriv InDeriv;
    typedef typename In::VecCoord InVecCoord;
    typedef typename In::VecDeriv InVecDeriv;
    typedef typename In::MatrixDeriv InMatrixDeriv;
    typedef typename In::Real Real;
    //@}

    /** @name  Output types    */
    //@{
    typedef typename JacobianBlockType::Out Out;
    typedef typename Out::Coord OutCoord;
    typedef typename Out::Deriv OutDeriv;
    typedef typename Out::VecCoord OutVecCoord;
    typedef typename Out::VecDeriv OutVecDeriv;
    typedef typename Out::MatrixDeriv OutMatrixDeriv;
    //    enum { spatial_dimensions = Out::spatial_dimensions };
    //@}

    /** @name  Jacobian types    */
    //@{
    typedef JacobianBlockType BlockType;
    typedef helper::vector<BlockType>  SparseMatrix;

    typedef typename BlockType::MatBlock  MatBlock;  ///< Jacobian block matrix
    typedef linearsolver::EigenSparseMatrix<In,Out>    SparseMatrixEigen;

    typedef typename BlockType::KBlock  KBlock;  ///< stiffness block matrix
    typedef linearsolver::EigenSparseMatrix<In,In>    SparseKMatrixEigen;
    //@}
	
    void resizeOut() override
    {
        if(this->f_printLog.getValue()) std::cout<<this->getName()<<"::resizeOut()"<<std::endl;

        unsigned int size = this->fromModel->getSize();

        this->toModel->resize(size);

        jacobian.resize(size);

        reinit();
    }

    //Pierre-Luc : I added this function to be able to use the mapping functionnalities without using the whole component
    virtual void initJacobianBlock( helper::vector<BlockType>& /*jacobianBlock*/)
    {
        std::cout << SOFA_CLASS_METHOD << " : Do nothing" << std::endl;
    }

    /** @name Mapping functions */
    //@{
    void init() override
    {
        if( core::behavior::BaseMechanicalState* stateFrom = this->fromModel.get()->toBaseMechanicalState() )
            maskFrom = &stateFrom->forceMask;
        if( core::behavior::BaseMechanicalState* stateTo = this->toModel.get()->toBaseMechanicalState() )
            maskTo = &stateTo->forceMask;

        // init jacobians
        baseMatrices.resize( 1 ); // just a wrapping for getJs()
        baseMatrices[0] = &eigenJacobian;

        resizeOut();
        Inherit::init();
    }

    void reinit() override
    {
        if(this->assemble.getValue()) updateJ();

        // clear forces and force apply       
        Inherit::reinit();
    }

    using Inherit::apply;
    using Inherit::applyJ;
    using Inherit::applyJT;

    void applyJT() override
    {
        applyJT(NULL, *this->fromModel->write(core::VecDerivId::force()), *this->toModel->read(core::ConstVecDerivId::force()));
    }

    //Pierre-Luc : I added these function to be able to use the mapping functionnalities without using the whole component
    virtual void applyBlock(Data<OutVecCoord>& /*dOut*/, const Data<InVecCoord>& /*dIn*/, helper::vector<BlockType>& /*jacobianBlock*/)
    {
        std::cout << SOFA_CLASS_METHOD << " : do nothing" << std::endl;
    }

    virtual void apply(Data<OutVecCoord>& dOut, const Data<InVecCoord>& dIn)
    {
        if(this->f_printLog.getValue()) std::cout<<this->getName()<<":apply"<<std::endl;

        const InVecCoord&  in = dIn.getValue();

        helper::vector< BlockType > jacobianBlock;
        jacobianBlock.resize(in.size());
        initJacobianBlock(jacobianBlock);
        applyBlock(dOut, dIn, jacobianBlock);
    }

    virtual void applyJ(Data<OutVecDeriv>& dOut, const Data<InVecDeriv>& dIn)
    {
        if(this->f_printLog.getValue()) std::cout<<this->getName()<<":applyJ"<<std::endl;

        const InVecDeriv&  in = dIn.getValue();

        helper::vector< BlockType > jacobianBlock;
        jacobianBlock.resize(in.size());
        initJacobianBlock(jacobianBlock);

        OutVecDeriv& out = *dOut.beginWriteOnly();
#ifdef _OPENMP
        #pragma omp parallel for if (this->d_parallel.getValue())
#endif
        for(sofa::helper::IndexOpenMP<unsigned int>::type i=0; i < jacobianBlock.size(); i++)
        {
            out[i]=OutDeriv();
            jacobianBlock[i].addmult(out[i],in[i]);
        }
		dOut.endEdit();
    }

    virtual void apply(const core::MechanicalParams * /*mparams*/ , Data<OutVecCoord>& dOut, const Data<InVecCoord>& dIn) override
    {
        if(this->f_printLog.getValue()) std::cout<<this->getName()<<":apply"<<std::endl;

        helper::ReadAccessor<Data<InVecCoord> > inpos (*this->fromModel->read(core::ConstVecCoordId::position()));
        helper::ReadAccessor<Data<OutVecCoord> > outpos (*this->toModel->read(core::ConstVecCoordId::position()));
        if(inpos.size()!=outpos.size()) this->resizeOut();

        OutVecCoord& out = *dOut.beginWriteOnly();
        const InVecCoord& in = dIn.getValue();

#ifdef _OPENMP
        #pragma omp parallel for if (this->d_parallel.getValue())
#endif
        for(int i=0; i < static_cast<int>(jacobian.size()); i++)
        {
            out[i]=OutCoord();
            jacobian[i].addapply(out[i],in[i]);
        }
        dOut.endEdit();

        if(!BlockType::constant) if(this->assemble.getValue()) updateJ();
    }

    virtual void applyJ(const core::MechanicalParams * /*mparams*/ , Data<OutVecDeriv>& dOut, const Data<InVecDeriv>& dIn) override
    {
        if(this->assemble.getValue())  eigenJacobian.mult(dOut,dIn);
        else
        {
            OutVecDeriv& out = *dOut.beginWriteOnly();
            const InVecDeriv& in = dIn.getValue();

#ifdef _OPENMP
        #pragma omp parallel for if (this->d_parallel.getValue())
#endif
            for(int i=0; i < static_cast<int>(jacobian.size()); i++)
            {
                out[i]=OutDeriv();
                jacobian[i].addmult(out[i],in[i]);
            }
            dOut.endEdit();
        }
    }


    virtual void applyJT(const core::MechanicalParams * /*mparams*/ , Data<InVecDeriv>& dIn, const Data<OutVecDeriv>& dOut) override
    {
        if(this->assemble.getValue())  eigenJacobian.addMultTranspose(dIn,dOut);
        else
        {
            InVecDeriv& in = *dIn.beginEdit();
            const OutVecDeriv& out = dOut.getValue();

#ifdef _OPENMP
        #pragma omp parallel for if (this->d_parallel.getValue())
#endif
            for(int i=0; i < static_cast<int>(jacobian.size()); i++)
            {
                jacobian[i].addMultTranspose(in[i],out[i]);
            }

            dIn.endEdit();
        }
    }

    virtual void applyJT(const core::ConstraintParams * /*cparams*/ , Data<InMatrixDeriv>& /*out*/, const Data<OutMatrixDeriv>& /*in*/) override
    {

    }


    virtual void applyDJT(const core::MechanicalParams* mparams, core::MultiVecDerivId parentDfId, core::ConstMultiVecDerivId childForceId ) override
    {
        if(BlockType::constant) return;

        Data<InVecDeriv>& parentForceData = *parentDfId[this->fromModel.get()].write();
        const Data<InVecDeriv>& parentDisplacementData = *mparams->readDx(this->fromModel);
        const Data<OutVecDeriv>& childForceData = *mparams->readF(this->toModel);

        helper::WriteAccessor<Data<InVecDeriv> > parentForce (parentForceData);
        helper::ReadAccessor<Data<InVecDeriv> > parentDisplacement (parentDisplacementData);
        helper::ReadAccessor<Data<OutVecDeriv> > childForce (childForceData);

        if( assemble.getValue() ) // assembled version
        {
            if( K.compressedMatrix.nonZeros() )
            {
                K.addMult(parentForceData,parentDisplacementData,sofa::core::mechanicalparams::kFactor(mparams));
            }
            else // force local assembly
            {
                updateK( mparams, childForceId );
                K.addMult(parentForceData,parentDisplacementData,sofa::core::mechanicalparams::kFactor(mparams));
                K.resize(0,0); // forgot about this matrix
            }
        }
        else
        {
#ifdef _OPENMP
        #pragma omp parallel for if (this->d_parallel.getValue())
#endif
            for(int i=0; i < static_cast<int>(jacobian.size()); i++)
            {
                jacobian[i].addDForce( parentForce[i], parentDisplacement[i], childForce[i], sofa::core::mechanicalparams::kFactor(mparams) );
            }
        }
    }

    const defaulttype::BaseMatrix* getJ(const core::MechanicalParams * /*mparams*/) override
    {
        if(!this->assemble.getValue()/* || !BlockType::constant*/)  // J should have been updated in apply() that is call before (when assemble==1)
        {
            updateJ();
            serr<<"Please, with an assembled solver, set assemble=1\n";
        }
        return &eigenJacobian;
    }

    // Compliant plugin API
    virtual const helper::vector<sofa::defaulttype::BaseMatrix*>* getJs() override
    {
        if(!this->assemble.getValue()/* || !BlockType::constant*/)  // J should have been updated in apply() that is call before (when assemble==1)
        {
            updateJ();
            serr<<"Please, with an assembled solver, set assemble=1\n";
        }
        return &baseMatrices;
    }

    virtual void updateK( const core::MechanicalParams* mparams, core::ConstMultiVecDerivId childForceId ) override
    {
        SOFA_UNUSED(mparams);
        if( BlockType::constant /*|| !assemble.getValue()*/ ) { K.resize(0,0); return; }

        const OutVecDeriv& childForce = childForceId[this->toModel.get()].read()->getValue();

        unsigned int size = this->fromModel->getSize();
        K.resizeBlocks(size,size);
        for(size_t i=0; i<jacobian.size(); i++)
            K.insertBackBlock( i, i, jacobian[i].getK(childForce[i]) );
        K.compress();
    }


    virtual const defaulttype::BaseMatrix* getK() override
    {
        if( BlockType::constant || !K.compressedMatrix.nonZeros() ) return NULL;
        else return &K;
    }


    void draw(const core::visual::VisualParams* /*vparams*/) override
    {
    }

    //@}



    Data<bool> assemble; ///< Assemble the matrices (Jacobian and Geometric Stiffness) or use optimized matrix/vector multiplications
    Data< bool > d_parallel;		///< use openmp ?

protected:
    BaseStrainMappingT (core::State<In>* from = NULL, core::State<Out>* to= NULL)
        : Inherit ( from, to )
        , assemble ( initData ( &assemble,false, "assemble","Assemble the matrices (Jacobian and Geometric Stiffness) or use optimized matrix/vector multiplications" ) )
        , d_parallel(initData(&d_parallel, false, "parallel", "use openmp parallelisation?"))
        , maskFrom(NULL)
        , maskTo(NULL)
    {

    }

    ~BaseStrainMappingT() override     { }

    SparseMatrix jacobian;   ///< Jacobian of the mapping

    helper::StateMask* maskFrom;  ///< Subset of master DOF, to cull out computations involving null forces or displacements
    helper::StateMask* maskTo;    ///< Subset of slave DOF, to cull out computations involving null forces or displacements

    SparseMatrixEigen eigenJacobian;  ///< Assembled Jacobian matrix
    helper::vector<defaulttype::BaseMatrix*> baseMatrices;      ///< Vector of jacobian matrices, for the Compliant plugin API
    void updateJ()
    {
        unsigned int insize = this->fromModel->getSize();
        unsigned int outsize = this->toModel->getSize();

        eigenJacobian.resizeBlocks(outsize,insize);
        for(size_t i=0; i<jacobian.size(); i++)
            eigenJacobian.insertBackBlock( i, i, jacobian[i].getJ() );
        eigenJacobian.compress();
    }

    SparseKMatrixEigen K;  ///< Assembled geometric stiffness matrix
};



} // namespace mapping

} // namespace component

} // namespace sofa


#endif
