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
#include <SofaUserInteraction/config.h>

#include <SofaUserInteraction/InteractionPerformer.h>
#include <SofaBaseCollision/BaseContactMapper.h>
#include <sofa/core/behavior/BaseForceField.h>
#include <SofaDeformable/SpringForceField.h>
#include <SofaDeformable/StiffSpringForceField.h>
#include <SofaGraphComponent/AttachBodyButtonSetting.h>
#include <sofa/core/visual/DisplayFlags.h>

namespace sofa::component::collision
{

struct BodyPicked;

template <class DataTypes>
class AttachBodyPerformer: public TInteractionPerformer<DataTypes>
{
public:
    typedef sofa::component::collision::BaseContactMapper< DataTypes >        MouseContactMapper;
    typedef sofa::core::behavior::MechanicalState< DataTypes >         MouseContainer;
    typedef sofa::core::behavior::BaseForceField              MouseForceField;

    AttachBodyPerformer(BaseMouseInteractor *i);
    virtual ~AttachBodyPerformer();

    void start();
    void execute();
    void draw(const core::visual::VisualParams* vparams);
    void clear();

    void setStiffness(SReal s) {stiffness=s;}
    void setArrowSize(float s) {size=s;}
    void setShowFactorSize(float s) {showFactorSize = s;}

    virtual void configure(configurationsetting::MouseButtonSetting* setting)
    {
        configurationsetting::AttachBodyButtonSetting* s = dynamic_cast<configurationsetting::AttachBodyButtonSetting*>(setting);
        if (s)
        {
            setStiffness(s->stiffness.getValue());
            setArrowSize((float)s->arrowSize.getValue());
            setShowFactorSize((float)s->showFactorSize.getValue());
        }
    }

protected:
    SReal stiffness;
    SReal size;
    SReal showFactorSize;

    virtual bool start_partial(const BodyPicked& picked);
    /*
    initialise MouseForceField according to template.
    StiffSpringForceField for Vec3
    JointSpringForceField for Rigid3
    */

    MouseContactMapper  *mapper;
    MouseForceField::SPtr m_forcefield;

    core::visual::DisplayFlags flags;
};

#if  !defined(SOFA_COMPONENT_COLLISION_ATTACHBODYPERFORMER_CPP)
extern template class SOFA_SOFAUSERINTERACTION_API  AttachBodyPerformer<defaulttype::Vec2Types>;
extern template class SOFA_SOFAUSERINTERACTION_API  AttachBodyPerformer<defaulttype::Vec3Types>;
extern template class SOFA_SOFAUSERINTERACTION_API  AttachBodyPerformer<defaulttype::Rigid3Types>;
#endif

} // namespace sofa::component::collision
