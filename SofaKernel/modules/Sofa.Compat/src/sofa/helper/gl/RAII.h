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

#include <sofa/helper/config.h>

#if __has_include(<sofa/gl/RAII.h>)
#include <sofa/gl/RAII.h>

SOFA_DEPRECATED_HEADER(v21.06, "sofa/gl/RAII.h")
#define GL_RAII_ENABLE_WRAPPER

#else
#error "OpenGL headers have been moved to Sofa.GL; you will need to link against your library if you need OpenGL, and include <sofa/gl/RAII.h> instead of this one."
#endif

#ifdef GL_RAII_ENABLE_WRAPPER

namespace sofa::helper::gl
{
    template <GLenum Flag>
    using Enable = sofa::gl::Enable<Flag>;

    template <GLenum Flag>
    using Disable = sofa::gl::Disable<Flag>;


} // namespace sofa::helper::gl

#endif // GL_RAII_ENABLE_WRAPPER

#undef GL_RAII_ENABLE_WRAPPER
