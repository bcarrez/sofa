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
#define SOFA_COMPONENT_COLLISION_BARYCENTRICPENALITYCONTACT_CPP
#include <SofaMeshCollision/BarycentricPenalityContact.inl>
#include <SofaMeshCollision/BarycentricContactMapper.h>
#include <SofaMeshCollision/RigidContactMapper.inl>

namespace sofa::component::collision
{

using namespace core::collision;
using simulation::Node;

Creator<Contact::Factory, BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>> > SphereSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel> > SphereRigidSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<RigidSphereModel, RigidSphereModel> > RigidSphereRigidSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>> > SpherePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>> > RigidSpherePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>> > PointPointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>> > LinePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>> > LineLinePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>> > LineSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel> > LineRigidSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>> > TriangleSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel> > TriangleRigidSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>> > TrianglePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>> > TriangleLinePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>> > TriangleTrianglePenalityContactClass("default",true);

Creator<Contact::Factory, BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, CylinderCollisionModel<sofa::defaulttype::Rigid3Types>> > CylinderCylinderPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>> > CylinderTrianglePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>> > CylinderSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, RigidSphereModel> > CylinderRigidSpherePenalityContactClass("default",true);



template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<RigidSphereModel, RigidSphereModel>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>>;

template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, CylinderCollisionModel<sofa::defaulttype::Rigid3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>;
template class SOFA_SOFAMESHCOLLISION_API BarycentricPenalityContact<CylinderCollisionModel<sofa::defaulttype::Rigid3Types>, RigidSphereModel>;

} //namespace sofa::component::collision
