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
#include <SofaOpenglVisual/OglRenderingSRGB.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/ObjectFactory.h>


namespace sofa
{

namespace component
{

namespace visualmodel
{

using namespace simulation;

//Register RenderingSRGB in the Object Factory
int OglRenderingSRGBClass = core::RegisterObject("OglRenderingSRGB")
        .add< OglRenderingSRGB >()
        ;

void OglRenderingSRGB::fwdDraw(core::visual::VisualParams* /*vp*/)
{
#if defined(GL_FRAMEBUFFER_SRGB)
    glEnable(GL_FRAMEBUFFER_SRGB);
#endif
}

void OglRenderingSRGB::bwdDraw(core::visual::VisualParams* /*vp*/)
{
#if defined(GL_FRAMEBUFFER_SRGB)
    glDisable(GL_FRAMEBUFFER_SRGB);
#endif
}

}
}
}
