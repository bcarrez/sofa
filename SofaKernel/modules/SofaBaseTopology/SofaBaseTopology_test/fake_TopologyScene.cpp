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

#include <sofa/helper/system/FileRepository.h>
#include <SofaLoader/MeshObjLoader.h>
#include <SofaBase/initSofaBase.h>
#include <sofa/helper/Utils.h>
#include "fake_TopologyScene.h"

using namespace sofa::simpleapi;
using namespace sofa::core::topology;

fake_TopologyScene::fake_TopologyScene(const std::string& filename, TopologyElementType topoType, bool staticTopo)
    : m_topoType(topoType)
    , m_filename(filename)
    , m_staticTopology(staticTopo)
{
    //force load sofabase
    sofa::component::initSofaBase();
    sofa::helper::system::DataRepository.addFirstPath(SOFABASETOPOLOGY_TEST_RESOURCES_DIR);
    
    loadMeshFile();
}

bool fake_TopologyScene::loadMeshFile()
{
    m_simu = createSimulation("DAG");
    m_root = createRootNode(m_simu, "root");

    createObject(m_root, "RequiredPlugin", {
        { "name", "SofaLoader" } });

    std::string loaderType = "MeshObjLoader";
    if (m_topoType == TopologyElementType::TETRAHEDRON || m_topoType == TopologyElementType::HEXAHEDRON)
        loaderType = "MeshGmshLoader";


    auto loader = createObject(m_root, loaderType, {
        { "name","loader" },
        { "filename", sofa::helper::system::DataRepository.getFile(m_filename) } });

    auto meca = createObject(m_root, "MechanicalObject", {
        { "name", "dof" },
        { "position", "@loader.position"} });
       
   
    if (m_staticTopology)
    {
        auto topo = createObject(m_root, "MeshTopology", {
            { "name", "topoCon" },
            { "src", "@loader" }
        });        
    }
    else
    {
        std::string topoType = "";
        if (m_topoType == TopologyElementType::POINT)
            topoType = "Point";
        else if (m_topoType == TopologyElementType::EDGE)
            topoType = "Edge";
        else if (m_topoType == TopologyElementType::TRIANGLE)
            topoType = "Triangle";
        else if (m_topoType == TopologyElementType::QUAD)
            topoType = "Quad";
        else if (m_topoType == TopologyElementType::TETRAHEDRON)
            topoType = "Tetrahedron";
        else if (m_topoType == TopologyElementType::HEXAHEDRON)
            topoType = "Hexahedron";

        // create topology components
        auto topo = createObject(m_root, topoType+"SetTopologyContainer", {
            { "name", "topoCon" },
            { "src", "@loader" }
        });
        
        createObject(m_root, topoType + "SetTopologyModifier", {{ "name", "topoMod" }});
        createObject(m_root, topoType + "SetGeometryAlgorithms", { { "name", "topoGeo" } });
    }

    m_simu->init(m_root.get());
   
    return true;
}

