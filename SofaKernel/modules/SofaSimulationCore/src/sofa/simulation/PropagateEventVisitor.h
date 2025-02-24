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
#ifndef SOFA_SIMULATION_PROPAGATEEVENTACTION_H
#define SOFA_SIMULATION_PROPAGATEEVENTACTION_H

#include <sofa/simulation/Visitor.h>
#include <sofa/core/objectmodel/Event.h>


namespace sofa
{

namespace simulation
{

/**
Visitor used to propagate an event in the the data structure.
Propagation is done top-down until the event is handled.

	@author The SOFA team </www.sofa-framework.org>
*/
class SOFA_SIMULATION_CORE_API PropagateEventVisitor : public sofa::simulation::Visitor
{
public:
    PropagateEventVisitor(const core::ExecParams* params, sofa::core::objectmodel::Event* e);

    ~PropagateEventVisitor() override;

    Visitor::Result processNodeTopDown(simulation::Node* node) override;
    void processObject(simulation::Node*, core::objectmodel::BaseObject* obj);

    const char* getClassName() const override { return "PropagateEventVisitor"; }
    virtual std::string getInfos() const override { return std::string(m_event->getClassName());  }
protected:
    sofa::core::objectmodel::Event* m_event;
};


}

}

#endif
