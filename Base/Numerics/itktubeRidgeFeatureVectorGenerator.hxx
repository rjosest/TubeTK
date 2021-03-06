/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#ifndef __itktubeRidgeFeatureVectorGenerator_hxx
#define __itktubeRidgeFeatureVectorGenerator_hxx

#include "itktubeNJetImageFunction.h"
#include "itktubeRidgeFeatureVectorGenerator.h"
#include "tubeMatrixMath.h"

#include <itkImage.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkProgressReporter.h>
#include <itkTimeProbesCollectorBase.h>

#include <limits>

namespace itk
{

namespace tube
{

template< class TImage >
RidgeFeatureVectorGenerator< TImage >
::RidgeFeatureVectorGenerator( void )
{
  m_IntensityMin = 0;
  m_IntensityMax = 0;
  m_Scales.resize( 0 );
}

template< class TImage >
RidgeFeatureVectorGenerator< TImage >
::~RidgeFeatureVectorGenerator( void )
{
}

template< class TImage >
void
RidgeFeatureVectorGenerator< TImage >
::SetIntensityMin( float intensityMin )
{
  m_IntensityMin = intensityMin;
}

template< class TImage >
float
RidgeFeatureVectorGenerator< TImage >
::GetIntensityMin( void ) const
{
  return m_IntensityMin;
}

template< class TImage >
void
RidgeFeatureVectorGenerator< TImage >
::SetIntensityMax( float intensityMax )
{
  m_IntensityMax = intensityMax;
}

template< class TImage >
float
RidgeFeatureVectorGenerator< TImage >
::GetIntensityMax( void ) const
{
  return m_IntensityMax;
}

template< class TImage >
unsigned int
RidgeFeatureVectorGenerator< TImage >
::GetNumberOfFeatures( void ) const
{
  const unsigned int numFeatures = m_Scales.size() * 5 + 6;

  return numFeatures;
}

template< class TImage >
typename RidgeFeatureVectorGenerator< TImage >::FeatureVectorType
RidgeFeatureVectorGenerator< TImage >
::GetFeatureVector( const IndexType & indx ) const
{
  const unsigned int numFeatures = this->GetNumberOfFeatures();

  FeatureVectorType featureVector;
  featureVector.set_size( numFeatures );

  typedef NJetImageFunction< ImageType > NJetFunctionType;
  typename NJetFunctionType::Pointer njet = NJetFunctionType::New();
  typename NJetFunctionType::VectorType v;
  typename NJetFunctionType::MatrixType m;
  njet->SetInputImage( this->m_InputImageList[0] );

  unsigned int featureCount = 0;
  double extremeScale = 0;
  double extremeIntensity = 0;
  double extremeRidgeness = 0;
  double extremeRoundness = 0;
  double extremeLevelness = 0;
  double extremeCurvature = 0;
  for( unsigned int s=0; s<m_Scales.size(); s++ )
    {
    double ridgeness = njet->RidgenessAtIndex( indx, m_Scales[s] );
    featureVector[featureCount++] = njet->GetMostRecentIntensity();
    featureVector[featureCount++] = ridgeness;
    featureVector[featureCount++] = njet->GetMostRecentRidgeRoundness();
    featureVector[featureCount++] = njet->GetMostRecentRidgeLevelness();
    featureVector[featureCount++] = njet->GetMostRecentRidgeCurvature();
    if( s == 0 || ridgeness > extremeRidgeness )
      {
      extremeScale = m_Scales[s];
      extremeIntensity = njet->GetMostRecentIntensity();
      extremeRidgeness = ridgeness;
      extremeRoundness = njet->GetMostRecentRidgeRoundness();
      extremeLevelness = njet->GetMostRecentRidgeLevelness();
      extremeCurvature = njet->GetMostRecentRidgeCurvature();
      }
    }
  featureVector[featureCount++] = extremeScale;
  featureVector[featureCount++] = extremeIntensity;
  featureVector[featureCount++] = extremeRidgeness;
  featureVector[featureCount++] = extremeRoundness;
  featureVector[featureCount++] = extremeLevelness;
  featureVector[featureCount++] = extremeCurvature;

  return featureVector;
}

template< class TImage >
typename RidgeFeatureVectorGenerator< TImage >::FeatureValueType
RidgeFeatureVectorGenerator< TImage >
::GetFeatureVectorValue( const IndexType & indx, unsigned int fNum ) const
{
  const unsigned int numFeatures = this->GetNumberOfFeatures();

  if( fNum < numFeatures - 8 )
    {
    typedef NJetImageFunction< ImageType > NJetFunctionType;
    typename NJetFunctionType::Pointer njet = NJetFunctionType::New();
    typename NJetFunctionType::VectorType v;
    typename NJetFunctionType::MatrixType m;
    njet->SetInputImage( this->m_InputImageList[0] );

    unsigned int featureCount = 0;
    for( unsigned int s=0; s<m_Scales.size(); s++ )
      {
      double ridgeness = njet->RidgenessAtIndex( indx, m_Scales[s] );
      if( featureCount++ == fNum )
        {
        return njet->GetMostRecentIntensity();
        }
      if( featureCount++ == fNum )
        {
        return ridgeness;
        }
      if( featureCount++ == fNum )
        {
        return njet->GetMostRecentRidgeRoundness();
        }
      if( featureCount++ == fNum )
        {
        return njet->GetMostRecentRidgeLevelness();
        }
      if( featureCount++ == fNum )
        {
        return njet->GetMostRecentRidgeCurvature();
        }
      }
    }

  return this->GetFeatureVector( indx )[fNum];
}

template< class TImage >
void
RidgeFeatureVectorGenerator< TImage >
::SetScales( const RidgeScalesType & scales )
{
  m_Scales = scales;
}

template< class TImage >
const std::vector< double > &
RidgeFeatureVectorGenerator< TImage >
::GetScales( void ) const
{
  return m_Scales;
}

template< class TImage >
void
RidgeFeatureVectorGenerator< TImage >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Scales.size() = " << m_Scales.size() << std::endl;
}

} // End namespace tube

} // End namespace itk

#endif // End !defined(__itktubeRidgeFeatureVectorGenerator_hxx)
