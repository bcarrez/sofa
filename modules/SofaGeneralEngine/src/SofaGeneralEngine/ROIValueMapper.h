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


namespace sofa::component::engine
{

/**
 * This class returns a list of values from value-indices pairs
 */

class ROIValueMapper : public sofa::core::DataEngine
{
public:
    typedef core::DataEngine Inherited;

    SOFA_CLASS(ROIValueMapper,Inherited);
    typedef SReal Real;
    typedef unsigned int Index;

    //Input
    Data<unsigned int> nbROIs; ///< size of indices/value vector
    helper::vectorData<helper::vector<Index> > f_indices;
    helper::vectorData<Real> f_value;

    //Output
    Data<sofa::helper::vector<Real> > f_outputValues; ///< New vector of values

    //Parameter
    Data<Real> p_defaultValue; ///< Default value for indices out of ROIs

    void init() override
    {
        addInput(&nbROIs);
        f_indices.resize(nbROIs.getValue());
        f_value.resize(nbROIs.getValue());

        addOutput(&f_outputValues);
        setDirtyValue();
    }

    void reinit() override
    {
        f_indices.resize(nbROIs.getValue());
        f_value.resize(nbROIs.getValue());
        update();
    }


    /// Parse the given description to assign values to this object's fields and potentially other parameters
    void parse ( sofa::core::objectmodel::BaseObjectDescription* arg ) override
    {
        f_indices.parseSizeData(arg, nbROIs);
        f_value.parseSizeData(arg, nbROIs);
        Inherit1::parse(arg);
    }

    /// Assign the field values stored in the given map of name -> value pairs
    void parseFields ( const std::map<std::string,std::string*>& str ) override
    {
        f_indices.parseFieldsSizeData(str, nbROIs);
        f_value.parseFieldsSizeData(str, nbROIs);
        Inherit1::parseFields(str);
    }

protected:

    ROIValueMapper(): Inherited()
        , nbROIs ( initData ( &nbROIs,(unsigned int)0,"nbROIs","size of indices/value vector" ) )
        , f_indices(this, "indices", "ROIs", helper::DataEngineInput)
        , f_value(this, "value", "Values", helper::DataEngineInput)
        , f_outputValues(initData(&f_outputValues, "outputValues", "New vector of values"))
        , p_defaultValue(initData(&p_defaultValue, (Real) 0.0, "defaultValue", "Default value for indices out of ROIs"))
    {
    }

    ~ROIValueMapper() override {}

    void doUpdate() override
    {
        size_t nb = nbROIs.getValue();
        f_indices.resize(nb);
        f_value.resize(nb);
        if(!nb) return;

        const Real& defaultValue = p_defaultValue.getValue();
        helper::WriteOnlyAccessor< Data< helper::vector<Real> > > outputValues = f_outputValues;
        outputValues.clear();

        for(size_t j=0; j<nb;j++)
        {
            helper::ReadAccessor< Data< helper::vector<Index> > > indices = f_indices[j];
            const Real& value = f_value[j]->getValue();

            for(size_t i=0 ; i<indices.size() ; i++)
            {
                Index ind = indices[i];
                if (ind >= outputValues.size())
                {
                    size_t oldSize = outputValues.size();
                    outputValues.resize(ind+1);
                    for (size_t j=oldSize ; j<ind+1 ; j++) outputValues[j] = defaultValue;
                }
                outputValues[ind] = value;
            }
        }
    }

};


} //namespace sofa::component::engine
