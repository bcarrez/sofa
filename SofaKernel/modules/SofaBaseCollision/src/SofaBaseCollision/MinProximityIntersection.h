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
#include <SofaBaseCollision/config.h>

#include <SofaBaseCollision/BaseProximityIntersection.h>

namespace sofa::component::collision
{

class SOFA_SOFABASECOLLISION_API MinProximityIntersection : public BaseProximityIntersection
{
public:
    SOFA_CLASS(MinProximityIntersection,BaseProximityIntersection);
    Data<bool> useSphereTriangle; ///< activate Sphere-Triangle intersection tests
    Data<bool> usePointPoint; ///< activate Point-Point intersection tests
    Data<bool> useSurfaceNormals; ///< Compute the norms of the Detection Outputs by considering the normals of the surfaces involved.
    Data<bool> useLinePoint; ///< activate Line-Point intersection tests
    Data<bool> useLineLine; ///< activate Line-Line  intersection tests
    Data<bool> useTriangleLine;

protected:
    MinProximityIntersection();
public:
    typedef core::collision::IntersectorFactory<MinProximityIntersection> IntersectorFactory;

    void init() override;

    bool getUseSurfaceNormals();

    void draw(const core::visual::VisualParams* vparams) override;

    bool testIntersection(Cube& cube1, Cube& cube2);
    int computeIntersection(Cube& cube1, Cube& cube2, OutputVector* contacts);

    template<typename SphereType1, typename SphereType2>
    bool testIntersection(SphereType1& sph1, SphereType2& sph2)
    {
        const auto alarmDist = this->getAlarmDistance() + sph1.getProximity() + sph2.getProximity();
        return DiscreteIntersection::testIntersectionSphere(sph1, sph2, alarmDist);
    }
    template<typename SphereType1, typename SphereType2>
    int computeIntersection(SphereType1& sph1, SphereType2& sph2, OutputVector* contacts)
    {
        const auto alarmDist = this->getAlarmDistance() + sph1.getProximity() + sph2.getProximity();
        const auto contactDist = this->getContactDistance() + sph1.getProximity() + sph2.getProximity();
        return DiscreteIntersection::computeIntersectionSphere(sph1, sph2, contacts, alarmDist, contactDist);
    }

private:
    SReal mainAlarmDistance;
    SReal mainContactDistance;
};

} // namespace sofa::component::collision

namespace sofa::core::collision
{
#if  !defined(SOFA_COMPONENT_COLLISION_MINPROXIMITYINTERSECTION_CPP)
extern template class SOFA_SOFABASECOLLISION_API IntersectorFactory<component::collision::MinProximityIntersection>;
#endif
} // namespace sofa::core::collision
