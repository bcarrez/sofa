#ifndef SOFA_FLEXIBLE_ImageDensityMass_INL
#define SOFA_FLEXIBLE_ImageDensityMass_INL


#include "ImageDensityMass.h"

#include "../shapeFunction/BaseShapeFunction.h"

#include <sofa/core/behavior/MultiMatrixAccessor.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/gl/template.h>

namespace sofa
{

namespace component
{

namespace mass
{

using namespace	sofa::component::topology;
using namespace core::topology;
using namespace sofa::defaulttype;
using namespace sofa::core::behavior;


///////////////////////////////////////////


template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::clear()
{
    m_massMatrix.clear();
}




//////////////////////////////////



template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::init()
{
    Inherited::init();

    // get the shape function component
    if( !m_shapeFunction )  this->getContext()->get( m_shapeFunction, core::objectmodel::BaseContext::SearchDown );
    if( !m_shapeFunction )  this->getContext()->get( m_shapeFunction, core::objectmodel::BaseContext::SearchUp );
    if( !m_shapeFunction )
    {
        msg_error() << "ShapeFunction<"<<ShapeFunctionTypes::Name()<<"> component not found";
        return;
    }

    reinit();

    if( f_printMassMatrix.getValue() )
    {
        msg_info()<<m_massMatrix<<msgendl
                 <<"Total Volume = "<<m_totalVolume<<" ("<<pow((double)m_totalVolume,1.0/3.0)<<")"<<msgendl
                 <<"Total Mass = "<<m_totalMass;
    }
}


template < class DataTypes, class ShapeFunctionTypes, class MassType >
typename ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::Real ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getVoxelVolume( const TransformType& transform ) const
{
    // by default, it returns the voxel volume
    return transform.getScale()[0] * transform.getScale()[1] * transform.getScale()[2];
}


template < class DataTypes, class ShapeFunctionTypes, class MassType >
const typename ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::VecCoord* ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getX0()
{
    return &this->mstate->read(core::ConstVecCoordId::restPosition())->getValue();
}


template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::reinit()
{

    const VecCoord& DOFX0 = *getX0();

    // eventually resize and always clear
    m_massMatrix.resizeBloc( DOFX0.size(), DOFX0.size() ); // one block per dof


    const TransformType& transform = f_transform.getValue();
    double voxelVolume = getVoxelVolume( transform );

    m_totalMass = 0;
    m_totalVolume = 0;

    // get the density image
    const cimg_library::CImg<double>& densityImage = f_densityImage.getValue().getCImg(0);

    // for each density voxel
    cimg_forXYZ( densityImage, x, y, z )
    {
        // get the voxel density from the image
        double voxelDensity = densityImage( x, y, z );

        if( voxelDensity > 0 )
        {
            // the voxel position in space
            sCoord voxelPos = transform.fromImage( typename TransformType::Coord( x, y, z ) );

            // compute interpolation points/weights
            VRef controlPoints;  ///< The cp indices. controlPoints[j] is the index of the j-th parent influencing child.
            VReal weights; ///< The cp weights. weights[j] is the weight of the j-th parent influencing child.
            m_shapeFunction->computeShapeFunction( voxelPos, controlPoints, weights);

            // get the voxel density
            double voxelMass = voxelDensity * voxelVolume * 1000.0; // warning, the density is given for a ratio between kg and dm^3, so there is a factor 1000 to obtain kg from m^3

            m_totalMass += voxelMass;
            m_totalVolume += voxelVolume;

            // check the real number of control points
            unsigned nbControlPoints = 0;
            for( unsigned k=0; k<controlPoints.size() && weights[k]>0 ; ++k,++nbControlPoints );

            // precompute the interpolation matrix for each control points
            VecLinearJacobianBlock linearJacobians;
            linearJacobians.resize( nbControlPoints );
            for( unsigned k=0; k<nbControlPoints ; k++ )
                linearJacobians[k].init( DOFX0[controlPoints[k]], voxelPos, voxelPos, typename LinearJacobianBlock::MaterialToSpatial(), weights[k], Gradient(), Hessian() );

            MassType JltmJk;
            bool notNull;

            // for each control point influencing the voxel
            for( unsigned k=0; k<nbControlPoints ; k++ )
            {
                const unsigned cp_k = controlPoints[k];

                // influence of the same dof with itself
                JltmJk = J1tmJ0( linearJacobians[k], linearJacobians[k], voxelMass, notNull );

                if( notNull )
                {
                    MassType& Mkk = *m_massMatrix.wbloc(cp_k,cp_k,true);

                    if( f_lumping.getValue()==DIAGONAL_LUMPING )
                    {
                        for( int w=0 ; w<Deriv::total_size ; ++w )
                            for( int v=0 ; v<Deriv::total_size ; ++v )
                                Mkk[w][w] += JltmJk[w][v];
                    }
                    else
                        Mkk += JltmJk;
                }





                for( unsigned l=k+1; l<nbControlPoints ; l++ )
                {
                    const unsigned cp_l = controlPoints[l];

                    // influence of 2 different dofs, ie non-diagonal terms
                    JltmJk = J1tmJ0( linearJacobians[k], linearJacobians[l], voxelMass, notNull );

                    if( notNull )
                    {
                        if( f_lumping.getValue() == DIAGONAL_LUMPING )
                        {
                            // sum to the diagonal term on the same line
                            MassType& Mkk = *m_massMatrix.wbloc(cp_k,cp_k,true);
                            for( int w=0 ; w<Deriv::total_size ; ++w )
                                for( int v=0 ; v<Deriv::total_size ; ++v )
                                    Mkk[w][w] += JltmJk[w][v];

                            MassType& Mll = *m_massMatrix.wbloc(cp_l,cp_l,true);
                            for( int w=0 ; w<Deriv::total_size ; ++w )
                                for( int v=0 ; v<Deriv::total_size ; ++v )
                                    Mll[w][w] += JltmJk[v][w];

                        }
                        else if( f_lumping.getValue() == BLOCK_LUMPING )
                        {
                            // sum to the diagonal bloc on the same line
                            MassType& Mkk = *m_massMatrix.wbloc(cp_k,cp_k,true);
                            Mkk += JltmJk;

                            MassType& Mll = *m_massMatrix.wbloc(cp_l,cp_l,true);
                            Mll += JltmJk.transposed();
                        }
                        else
                        {
                            MassType& Mkl = *m_massMatrix.wbloc(cp_k,cp_l,true);
                            Mkl += JltmJk;

                            MassType& Mlk = *m_massMatrix.wbloc(cp_l,cp_k,true);
                            Mlk += JltmJk.transposed();
                        }
                    }
                }
            }
        }
    }
}




template < class DataTypes, class ShapeFunctionTypes, class MassType >
MassType ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::J1tmJ0( LinearJacobianBlock& J0, LinearJacobianBlock& J1, Real voxelMass, bool& notNull )
{
    MassType M;
    for( int w=0 ; w<DataTypes::deriv_total_size ; ++w ) // for all cinematic dof
    {
        Deriv m;
        Deriv acc; acc[w] = 1; // create a pseudo acceleration, to compute JtJ line by line
        defaulttype::Vec3Types::Deriv force;

        // map the pseudo acceleration from the dof level to the voxel level
        J0.addmult( force, acc );

        // compute a pseudo-force at voxel level f=ma
        force *= voxelMass;

        // bring back the pseudo-force at dof level
        J1.addMultTranspose( m , force );

        for( int v=0 ; v<DataTypes::deriv_total_size ; ++v ) // for all cinematic dof
            M[w][v] = m[v];
    }

    // check if M is not null
    notNull = false;
    for( int w=0 ; w<DataTypes::deriv_total_size && !notNull ; ++w )
        for( int v=0 ; v<DataTypes::deriv_total_size && !notNull ; ++v )
            if( M[w][v] !=0 ) notNull = true; // TODO should the equality have a threshold?

    return M;
}





///////////////////////////////////////////












// -- Mass interface
template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addMDx( const core::MechanicalParams*, DataVecDeriv& res, const DataVecDeriv& dx, SReal factor )
{
    VecDeriv& _res = *res.beginEdit();

    if( factor == 1.0 )
    {
        const VecDeriv& _dx = dx.getValue();
        m_massMatrix.addMul( _res, _dx );
    }
    else
    {
        VecDeriv _dx = dx.getValue();

        for( unsigned int i=0 ; i<_dx.size() ; i++ )
        {
            _dx[i] *= factor;
        }

        m_massMatrix.addMul( _res, _dx );
    }
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::accFromF( const core::MechanicalParams*, DataVecDeriv& acc, const DataVecDeriv& f )
{
    if( f_lumping.getValue()==DIAGONAL_LUMPING )
    {
        VecDeriv& _acc = *acc.beginWriteOnly();
        const VecDeriv& _f = f.getValue();

        for( unsigned int i=0 ; i<_f.size() ; i++ )
        {
            int rowId = i * m_massMatrix.getRowIndex().size() / m_massMatrix.rowBSize();
            const typename MassMatrix::Bloc& b = m_massMatrix.getColsValue()[m_massMatrix.getRowBegin()[rowId]];
            for( int bi = 0; bi < m_massMatrix.getBlockRows(); ++bi )
                _acc[i][bi] = _f[i][bi] / b[bi][bi];
        }

        acc.endEdit();
    }
    else if( f_lumping.getValue()==BLOCK_LUMPING )
    {
        VecDeriv& _acc = *acc.beginWriteOnly();
        const VecDeriv& _f = f.getValue();

        for( unsigned int i=0 ; i<_f.size() ; i++ )
        {
            int rowId = i * m_massMatrix.getRowIndex().size() / m_massMatrix.rowBSize();
            const typename MassMatrix::Bloc& b = m_massMatrix.getColsValue()[m_massMatrix.getRowBegin()[rowId]];
            typename MassMatrix::Bloc invb;
            invb.invert( b );
            _acc[i] = invb * _f[i];
            // todo either store the inverted matrices or do not compute the inverse but store a factorization
        }

        acc.endEdit();
    }
    else
    {
        serr<<"ImageDensityMass::accFromF not yet implemented when not lumped matrix (need the assembled matrix inversion)"<<sendl;
    }
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
SReal ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getKineticEnergy( const core::MechanicalParams*, const DataVecDeriv& v ) const
{
    const VecDeriv& _v = v.getValue();
    SReal e = 0;

    VecDeriv Mv = m_massMatrix * _v;

    for( unsigned int i=0 ; i<_v.size() ; i++ )
        e += _v[i] * Mv[i];

    return e/2;
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
SReal ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getPotentialEnergy( const core::MechanicalParams*, const DataVecCoord& x ) const
{
    const VecCoord& _x = x.getValue();

    VecCoord Mx = m_massMatrix * _x;

    SReal e = 0;
    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2] );
    for( unsigned int i=0 ; i<_x.size() ; i++ )
    {
        e -= Mx[i]*theGravity;
    }
    return e;
}



template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addGravityToV(const core::MechanicalParams* mparams, DataVecDeriv& d_v)
{
    if(mparams)
    {
        VecDeriv& v = *d_v.beginEdit();

        // gravity
        Vec3d g ( this->getContext()->getGravity() * sofa::core::mechanicalparams::dt(mparams) );
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * sofa::core::mechanicalparams::dt(mparams);

        // add weight force
        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
        d_v.endEdit();
    }
}


template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addForce(const core::MechanicalParams* /*mparams*/, DataVecDeriv& f, const DataVecCoord& /*x*/, const DataVecDeriv& /*v*/)
{
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue()) return;

    VecDeriv& _f = *f.beginEdit();

    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set( theGravity, g[0], g[1], g[2] );


    // add weight
    m_massMatrix.template addMul_by_line<Real,VecDeriv,Deriv>( _f, theGravity );

    f.endEdit();
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::addMToMatrix(const core::MechanicalParams *mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
    sofa::core::behavior::MultiMatrixAccessor::MatrixRef r = matrix->getMatrix(this->mstate);
    BaseMatrix* m = r.matrix;

    Real mFactor = (Real)sofa::core::mechanicalparams::mFactorIncludingRayleighDamping(mparams, this->rayleighMass.getValue());

    for (unsigned int xi = 0; xi < m_massMatrix.getRowIndex().size(); ++xi)
    {
        typename MassMatrix::Index iN = m_massMatrix.getRowIndex()[xi] * m_massMatrix.getBlockRows();
        typename MassMatrix::Range rowRange( m_massMatrix.getRowBegin()[xi], m_massMatrix.getRowBegin()[xi+1]);
        for (int xj = rowRange.begin(); xj < rowRange.end(); ++xj)
        {
            typename MassMatrix::Index jN = m_massMatrix.getColsIndex()[xj] * m_massMatrix.getBlockCols();
            const typename MassMatrix::Bloc& b = m_massMatrix.getColsValue()[xj];
            for (int bi = 0; bi < m_massMatrix.getBlockRows(); ++bi)
                for (int bj = 0; bj < m_massMatrix.getBlockCols(); ++bj)
                    m->add( iN+bi, jN+bj, b[bi][bj]*mFactor );
        }
    }
}

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::getElementMass(sofa::Index index, defaulttype::BaseMatrix *m ) const
{
    // warning the mass needs to be diagonal-lumped per dof

    //    std::cerr<<"ImageDensityMass::getElementMass "<<std::endl;

    static const BaseMatrix::Index dimension = (BaseMatrix::Index) DataTypes::deriv_total_size;

    if( m->rowSize() != dimension || m->colSize() != dimension ) m->resize( dimension, dimension );

    m->clear();

    //    for( unsigned i=0 ; i<dimension; ++i )
    //        m->set( i,i,1);

    BaseMatrix::Index i = index;
    BaseMatrix::Index bi = 0;
    m_massMatrix.split_row_index( i, bi );

    BaseMatrix::Index rowId = i * m_massMatrix.getRowIndex().size() / m_massMatrix.rowBSize();
    if( m_massMatrix.sortedFind( m_massMatrix.getRowIndex(), i, rowId ) )
    {
        typename MassMatrix::Range rowRange( m_massMatrix.getRowBegin()[rowId], m_massMatrix.getRowBegin()[rowId+1] );
        for( int xj = rowRange.begin() ; xj < rowRange.end() ; ++xj )
        {
            const typename MassMatrix::Bloc& b = m_massMatrix.getColsValue()[xj];
            for ( int bi = 0; bi < m_massMatrix.getBlockRows() ; ++bi )
                for ( int bj = 0; bj < m_massMatrix.getBlockCols() ; ++bj )
                    m->add( bi, bi, b[bi][bj] ); // diagonal lumping
            //                      m->add( bi, bj, b[bi][bj] ); // block lumping
        }
    }


    //    for( unsigned i=0 ; i<dimension; ++i )
    //        if( m->element( i , i ) == 0 )
    //        {
    //            m->set( i, i, 1 );
    //            std::cerr<<"ImageDensityMass::getElementMass wtf?"<<std::endl;
    //        }
}

///////////////////////

template < class DataTypes, class ShapeFunctionTypes, class MassType >
void ImageDensityMass< DataTypes, ShapeFunctionTypes, MassType >::draw(const core::visual::VisualParams* /*vparams*/)
{

}



} // namespace mass

} // namespace component

} // namespace sofa

#endif // SOFA_FLEXIBLE_ImageDensityMass_INL
