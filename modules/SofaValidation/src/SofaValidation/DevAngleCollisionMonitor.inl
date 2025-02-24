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
#include <SofaValidation/DevAngleCollisionMonitor.h>
#include <sofa/core/visual/VisualParams.h>

namespace sofa::component::misc
{

template <class DataTypes>
DevAngleCollisionMonitor<DataTypes>::DevAngleCollisionMonitor()
    : maxDist( initData(&maxDist, (Real)1.0, "maxDist", "alarm distance for proximity detection"))
    , pointsCM(nullptr)
    , surfaceCM(nullptr)
    , intersection(nullptr)
    , narrowPhaseDetection(nullptr)
{
}

template <class DataTypes>
void DevAngleCollisionMonitor<DataTypes>::init()
{
    if (!this->mstate1 || !this->mstate2)
    {
        msg_error() << "Init: mstate1 or mstate2 not found.";
        return;
    }

    sofa::core::objectmodel::BaseContext* c1 = this->mstate1->getContext();
    c1->get(pointsCM, core::objectmodel::BaseContext::SearchDown);
    if (pointsCM == nullptr)
    {
        msg_error() << "Init: object1 PointCollisionModel<sofa::defaulttype::Vec3Types> not found.";
        return;
    }
    sofa::core::objectmodel::BaseContext* c2 = this->mstate2->getContext();
    c2->get(surfaceCM, core::objectmodel::BaseContext::SearchDown);
    if (surfaceCM == nullptr)
    {
        msg_error() << "Init: object2 TriangleCollisionModel<sofa::defaulttype::Vec3Types> not found.";
        return;
    }

    intersection = sofa::core::objectmodel::New<sofa::component::collision::NewProximityIntersection>();
    this->addSlave(intersection);
    intersection->init();

    narrowPhaseDetection = sofa::core::objectmodel::New<sofa::component::collision::BVHNarrowPhase>();
    this->addSlave(narrowPhaseDetection);
    narrowPhaseDetection->init();
}

template <class DataTypes>
void DevAngleCollisionMonitor<DataTypes>::eval()
{
    if (!this->mstate1 || !this->mstate2 || !surfaceCM || !pointsCM || !intersection || !narrowPhaseDetection) return;

    const VecCoord& x = this->mstate1->read(core::ConstVecCoordId::position())->getValue();
    surfaceCM->computeBoundingTree(6);
    pointsCM->computeBoundingTree(6);
    intersection->setAlarmDistance(maxDist.getValue());
    intersection->setContactDistance(0.0);
    narrowPhaseDetection->setInstance(this);
    narrowPhaseDetection->setIntersectionMethod(intersection.get());
    sofa::helper::vector<std::pair<sofa::core::CollisionModel*, sofa::core::CollisionModel*> > vectCMPair;
    vectCMPair.push_back(std::make_pair(surfaceCM->getFirst(), pointsCM->getFirst()));

    narrowPhaseDetection->beginNarrowPhase();
    msg_info() << "narrow phase detection between " <<surfaceCM->getClassName()<< " and " << pointsCM->getClassName();
    narrowPhaseDetection->addCollisionPairs(vectCMPair);
    narrowPhaseDetection->endNarrowPhase();

    /// gets the pairs Triangle-Line detected in a radius lower than maxDist
    const core::collision::NarrowPhaseDetection::DetectionOutputMap& detectionOutputs = narrowPhaseDetection->getDetectionOutputs();

    core::collision::NarrowPhaseDetection::DetectionOutputMap::const_iterator it = detectionOutputs.begin();
    core::collision::NarrowPhaseDetection::DetectionOutputMap::const_iterator itend = detectionOutputs.end();

    while (it != itend)
    {

        const ContactVector* contacts = dynamic_cast<const ContactVector*>(it->second);

        if (contacts != nullptr)
        {
            core::collision::DetectionOutput c;

            double minNorm = ((*contacts)[0].point[0] - (*contacts)[0].point[1]).norm();

            msg_info() << contacts->size() << " contacts detected.";
            for (unsigned int i=0; i<contacts->size(); i++)
            {
                if ((*contacts)[i].elem.first.getCollisionModel() == surfaceCM)
                {
                    if ((*contacts)[i].elem.second.getCollisionModel() == pointsCM)
                    {
                        if ((*contacts)[i].elem.second.getIndex() == (x.size()-1))
                        {
                            double norm = ((*contacts)[i].point[0] - (*contacts)[i].point[1]).norm();
                            if (norm < minNorm)
                            {
                                c = (*contacts)[i];
                                minNorm = norm;
                            }
                        }
                        /*			int pi = (*contacts)[i].elem.second.getIndex();
                        			if ((*contacts)[i].value < dmin[pi])
                        			{
                        			    dmin[pi] = (Real)((*contacts)[i].value);
                        			    xproj[pi] = (*contacts)[i].point[0];
                        			}*/
                    }
                }
                else if ((*contacts)[i].elem.second.getCollisionModel() == surfaceCM)
                {
                    if ((*contacts)[i].elem.first.getCollisionModel() == pointsCM)
                    {
                        if ((*contacts)[i].elem.first.getIndex() == (x.size()-1))
                        {
                            double norm = ((*contacts)[i].point[0] - (*contacts)[i].point[1]).norm();

                            if (norm < minNorm)
                            {
                                c = (*contacts)[i];
                                minNorm = norm;
                            }
                        }

// 			int pi = (*contacts)[i].elem.first.getIndex();
// 			if ((*contacts)[i].value < dmin[pi])
// 			{
// 			    dmin[pi] = (Real)((*contacts)[i].value);
// 			    xproj[pi] = (*contacts)[i].point[1];
// 			}
                    }
                }
            }
            if (c.elem.second.getCollisionModel() == surfaceCM)
            {
                if (c.elem.first.getCollisionModel() == pointsCM)
                {
                    msg_info() << "tip point " << c.point[0];
                    msg_info() << "nearest skeleton point " << c.point[1];
                }
            }
            else
            {
                if (c.elem.first.getCollisionModel() == surfaceCM)
                {
                    if (c.elem.second.getCollisionModel() == pointsCM)
                    {
                        msg_info() << "tip point " << c.point[1];
                        msg_info() << "nearest skeleton point " << c.point[0];
                    }
                }
            }
        }
        it++;
    }
}

} // namespace sofa::component::misc
