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
#include <SofaGeneralEngine/config.h>



#include <sofa/core/DataEngine.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/vectorData.h>
#include <sofa/helper/SVector.h>


namespace sofa::component::engine
{

/**
 * This class merges a list of ROIs (vector<Indices>) into a single Data (vector<svector<Indices>>)
 */

class SOFA_SOFAGENERALENGINE_API MergeROIs : public sofa::core::DataEngine
{
public:
    typedef core::DataEngine Inherited;

    SOFA_CLASS(MergeROIs,Inherited);
    typedef sofa::Index Index;

    //Input
    Data<unsigned int> d_nbROIs; ///< size of indices/value vector
    helper::vectorData<helper::vector<Index> > f_indices;

    //Output
    Data<helper::vector<helper::SVector<Index> > > d_outputIndices; ///< Vector of ROIs

    void init() override;
    void reinit() override;

    /// Parse the given description to assign values to this object's fields and potentially other parameters
    void parse ( core::objectmodel::BaseObjectDescription* arg ) override;

    /// Assign the field values stored in the given map of name -> value pairs
    void parseFields ( const std::map<std::string,std::string*>& str ) override;

protected:

    MergeROIs(): Inherited()
        , d_nbROIs ( initData ( &d_nbROIs,(unsigned int)0,"nbROIs","size of indices/value vector" ) )
        , f_indices(this, "indices", "ROIs", helper::DataEngineInput)
        , d_outputIndices(initData(&d_outputIndices, "roiIndices", "Vector of ROIs"))
    {
    }

    ~MergeROIs() override {}

    void doUpdate() override;

};


} //namespace sofa::component::engine
