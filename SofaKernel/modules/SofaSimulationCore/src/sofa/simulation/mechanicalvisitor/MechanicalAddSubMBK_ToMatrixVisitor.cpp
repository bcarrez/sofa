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

#include <sofa/simulation/mechanicalvisitor/MechanicalAddSubMBK_ToMatrixVisitor.h>

#include <sofa/core/behavior/BaseForceField.h>

namespace sofa::simulation::mechanicalvisitor
{

MechanicalAddSubMBK_ToMatrixVisitor::MechanicalAddSubMBK_ToMatrixVisitor(const core::MechanicalParams *mparams,
                                                                         const sofa::core::behavior::MultiMatrixAccessor *_matrix,
                                                                         const helper::vector<unsigned int> &Id)
        : MechanicalVisitor(mparams) ,  matrix(_matrix), subMatrixIndex(Id) //,m(_m),b(_b),k(_k)
{
}

Visitor::Result
MechanicalAddSubMBK_ToMatrixVisitor::fwdMechanicalState(simulation::Node *, core::behavior::BaseMechanicalState *)
{
    //ms->setOffset(offsetOnExit);
    return RESULT_CONTINUE;
}

Visitor::Result
MechanicalAddSubMBK_ToMatrixVisitor::fwdForceField(simulation::Node *, core::behavior::BaseForceField *ff)
{
    if (matrix != nullptr)
    {
        assert( !ff->isCompliance.getValue() ); // if one day this visitor has to be used with compliance, K from compliance should not be added (by tweaking mparams with kfactor=0)
        ff->addSubMBKToMatrix(this->mparams, matrix, subMatrixIndex);
    }

    return RESULT_CONTINUE;
}
}