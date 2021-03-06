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

#include "itktubePDFSegmenter.h"

#include "SegmentConnectedComponentsUsingParzenPDFsCLP.h"

template< class T, unsigned int N >
int DoIt( int argc, char * argv[] );

// Description:
// Get the PixelType and ComponentType from fileName
void GetImageType( std::string fileName,
  itk::ImageIOBase::IOPixelType &pixelType,
  itk::ImageIOBase::IOComponentType &componentType )
{
  typedef itk::Image<short, 3> ImageType;
  itk::ImageFileReader<ImageType>::Pointer imageReader =
        itk::ImageFileReader<ImageType>::New();
  imageReader->SetFileName( fileName.c_str() );
  imageReader->UpdateOutputInformation();

  pixelType = imageReader->GetImageIO()->GetPixelType();
  componentType = imageReader->GetImageIO()->GetComponentType();
}

// Description:
// Get the PixelTypes and ComponentTypes from fileNames
void GetImageTypes( std::vector<std::string> fileNames,
  std::vector<itk::ImageIOBase::IOPixelType> &pixelTypes,
  std::vector<itk::ImageIOBase::IOComponentType> &componentTypes )
{
  pixelTypes.clear();
  componentTypes.clear();

  // For each file, find the pixel and component type
  for( std::vector<std::string>::size_type i = 0; i < fileNames.size(); i++ )
    {
    itk::ImageIOBase::IOPixelType pixelType;
    itk::ImageIOBase::IOComponentType componentType;
    GetImageType( fileNames[i],
                  pixelType,
                  componentType );
    pixelTypes.push_back( pixelType );
    componentTypes.push_back( componentType );
    }
}

//
// Helper function to check whether the attributes of input image match
// those of the label map.
//
template< class TInputImage, class TLabelMap >
bool
CheckImageAttributes( const TInputImage * input,
                     const TLabelMap * mask )
{
  assert( input );
  assert( mask );
  std::cout << "inputVolume1: " << std::endl;
  std::cout << "  size =  " << input->GetLargestPossibleRegion().GetSize() << std::endl;
  std::cout << "  index = " << input->GetLargestPossibleRegion().GetIndex() << std::endl;
  std::cout << "  origin = " << input->GetOrigin() << std::endl;
  std::cout << "mask: " << std::endl;
  std::cout << "  size =  " << mask->GetLargestPossibleRegion().GetSize() << std::endl;
  std::cout << "  index = " << mask->GetLargestPossibleRegion().GetIndex() << std::endl;
  std::cout << "  origin = " << mask->GetOrigin() << std::endl;
  const typename TInputImage::PointType inputOrigin = input->GetOrigin();
  const typename TLabelMap::PointType maskOrigin = mask->GetOrigin();
  const typename TInputImage::RegionType inputRegion = input->GetLargestPossibleRegion();
  const typename TLabelMap::RegionType maskRegion = mask->GetLargestPossibleRegion();
  return inputOrigin.GetVnlVector().is_equal( maskOrigin.GetVnlVector(), 0.01 )
         && inputRegion.GetIndex() == maskRegion.GetIndex()
         && inputRegion.GetSize() == maskRegion.GetSize();
}

template< class T, unsigned int N >
int DoIt( int argc, char * argv[] )
{
  PARSE_ARGS;

  itk::TimeProbesCollectorBase timeCollector;

  typedef T                                InputPixelType;
  typedef itk::Image< InputPixelType, 3 >  InputImageType;
  typedef itk::Image< unsigned short, 3 >  LabelMapType;
  typedef itk::Image< float, 3 >           ProbImageType;
  typedef itk::Image< float, N >           PDFImageType;

  typedef itk::ImageFileReader< InputImageType >   ImageReaderType;
  typedef itk::ImageFileReader< LabelMapType >     LabelMapReaderType;
  typedef itk::ImageFileWriter< LabelMapType >     LabelMapWriterType;
  typedef itk::ImageFileWriter< ProbImageType >    ProbImageWriterType;
  typedef itk::ImageFileWriter< PDFImageType >     PDFImageWriterType;
  typedef itk::ImageFileReader< PDFImageType >     PDFImageReaderType;

  typedef itk::tube::PDFSegmenter< InputImageType, N, LabelMapType >
    PDFSegmenterType;
  typename PDFSegmenterType::Pointer pdfSegmenter = PDFSegmenterType::New();

  timeCollector.Start( "LoadData" );

  LabelMapReaderType::Pointer  inLabelMapReader = LabelMapReaderType::New();
  inLabelMapReader->SetFileName( labelmap.c_str() );
  inLabelMapReader->Update();
  pdfSegmenter->SetLabelMap( inLabelMapReader->GetOutput() );

  typename ImageReaderType::Pointer reader;
  for( unsigned int i = 0; i < N; i++ )
    {
    reader = ImageReaderType::New();
    if( i == 0 )
      {
      reader->SetFileName( inputVolume1.c_str() );
      }
    else if( i == 1 )
      {
      reader->SetFileName( inputVolume2.c_str() );
      }
    else if( i == 2 )
      {
      reader->SetFileName( inputVolume3.c_str() );
      }
    else if( i == 3 )
      {
      reader->SetFileName( inputVolume4.c_str() );
      }
    else
      {
      std::cout << "ERROR: current command line xml file limits"
                << " this filter to 4 input images" << std::endl;
      return 1;
      }
    reader->Update();
    if( !CheckImageAttributes( reader->GetOutput(),
        inLabelMapReader->GetOutput() ) )
      {
      std::cout << "Image attributes of inputVolume" << i+1
        << " and label map do not match.  Please check size, spacing, "
        << "origin." << std::endl;
      return EXIT_FAILURE;
      }
    pdfSegmenter->SetInput( i, reader->GetOutput() );
    }

  timeCollector.Stop( "LoadData" );

  for( unsigned int o = 0; o < objectId.size(); o++ )
    {
    pdfSegmenter->AddObjectId( objectId[o] );
    }
  pdfSegmenter->SetVoidId( voidId );
  pdfSegmenter->SetErodeRadius( erodeRadius );
  pdfSegmenter->SetHoleFillIterations( holeFillIterations );
  if( objectPDFWeight.size() == objectId.size() )
    {
    for( unsigned int i = 0; i < objectPDFWeight.size(); i++ )
      {
      pdfSegmenter->SetObjectPDFWeight( i, objectPDFWeight[i] );
      }
    }
  pdfSegmenter->SetProbabilityImageSmoothingStandardDeviation(
    probSmoothingStdDev );
  pdfSegmenter->SetDraft( draft );
  pdfSegmenter->SetReclassifyNotObjectLabels( reclassifyNotObjectLabels );
  pdfSegmenter->SetReclassifyObjectLabels( reclassifyObjectLabels );
  if( forceClassification )
    {
    pdfSegmenter->SetForceClassification( true );
    }

  if( loadClassPDFBase.size() > 0 )
    {
    unsigned int numClasses = pdfSegmenter->GetNumberOfClasses();
    std::cout << "loading classes" << std::endl;
    for( unsigned int i = 0; i < numClasses; i++ )
      {
      std::string fname = loadClassPDFBase;
      char c[80];
      std::sprintf(c, ".c%u.mha", i );
      fname += std::string( c );
      typename PDFImageReaderType::Pointer pdfImageReader =
        PDFImageReaderType::New();
      pdfImageReader->SetFileName( fname.c_str() );
      pdfImageReader->Update();
      typename PDFImageType::Pointer img = pdfImageReader->GetOutput();
      typename PDFImageType::PointType origin = img->GetOrigin();
      typename PDFImageType::SpacingType spacing = img->GetSpacing();
      if( i == 0 )
        {
        std::vector< double > tmpOrigin;
        std::vector< double > tmpSpacing;
        for( unsigned int d=0; d<N; ++d )
          {
          tmpOrigin[d] = origin[d];
          tmpSpacing[d] = spacing[d];
          }
        pdfSegmenter->SetBinMin( tmpOrigin );
        pdfSegmenter->SetBinSize( tmpSpacing );
        }
      for( unsigned int d=0; d<N; ++d )
        {
        origin[d] = 0;
        spacing[d] = 1;
        }
      img->SetOrigin( origin );
      img->SetSpacing( spacing );
      pdfSegmenter->SetClassPDFImage( i, img );
      }
    pdfSegmenter->ClassifyImages();
    }
  else
    {
    pdfSegmenter->Update();
    pdfSegmenter->ClassifyImages();
    }

  timeCollector.Start( "Save" );

  if( saveClassProbabilityVolumeBase.size() > 0 )
    {
    unsigned int numClasses = pdfSegmenter->GetNumberOfClasses();
    for( unsigned int i = 0; i < numClasses; i++ )
      {
      std::string fname = saveClassProbabilityVolumeBase;
      char c[80];
      std::sprintf(c, ".c%u.mha", i );
      fname += std::string( c );
      ProbImageWriterType::Pointer probImageWriter =
        ProbImageWriterType::New();
      probImageWriter->SetFileName( fname.c_str() );
      probImageWriter->SetInput( pdfSegmenter->
        GetClassProbabilityForInput( i ) );
      probImageWriter->Update();
      }
    }

  LabelMapWriterType::Pointer writer = LabelMapWriterType::New();
  writer->SetFileName( outputVolume.c_str() );
  writer->SetInput( pdfSegmenter->GetLabelMap() );
  writer->Update();

  if( saveClassPDFBase.size() > 0 )
    {
    unsigned int numClasses = pdfSegmenter->GetNumberOfClasses();
    for( unsigned int i = 0; i < numClasses; i++ )
      {
      itk::Index< PDFImageType::ImageDimension > indx;
      indx.Fill( 100 );
      std::string fname = saveClassPDFBase;
      char c[80];
      std::sprintf(c, ".c%u.mha", i );
      fname += std::string( c );
      typename PDFImageWriterType::Pointer pdfImageWriter =
        PDFImageWriterType::New();
      pdfImageWriter->SetFileName( fname.c_str() );
      typename PDFImageType::PointType origin;
      typename PDFImageType::SpacingType spacing;
      for( unsigned int d = 0; d < N; d++ )
        {
        origin[d] = pdfSegmenter->GetBinMin()[d];
        spacing[d] = pdfSegmenter->GetBinSize()[d];
        }
      typename PDFImageType::PointType originOrg;
      typename PDFImageType::SpacingType spacingOrg;
      typename PDFImageType::Pointer img =
        pdfSegmenter->GetClassPDFImage( i );
      originOrg = img->GetOrigin();
      spacingOrg = img->GetSpacing();
      img->SetOrigin( origin );
      img->SetSpacing( spacing );
      pdfImageWriter->SetInput( img );
      pdfImageWriter->Update();
      img->SetOrigin( originOrg );
      img->SetSpacing( spacingOrg );
      }
    }

  timeCollector.Stop( "Save" );

  timeCollector.Report();

  return 0;
}

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  if( objectId.size() < 2)
    {
    std::cout << "Please specify a foreground and a background object." << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    GetImageType( inputVolume1, pixelType, componentType );

    int N = 1;
    if( inputVolume2.length() > 1 )
      {
      ++N;
      if( inputVolume3.length() > 1 )
        {
        ++N;
        if( inputVolume4.length() > 1 )
          {
          ++N;
          }
        }
      }

    switch( componentType )
      {
      case itk::ImageIOBase::UCHAR:
        if( N == 1 )
          {
          return DoIt<unsigned char, 1>( argc, argv );
          }
        else if( N == 2 )
          {
          return DoIt<unsigned char, 2>( argc, argv );
          }
        else if( N == 3 )
          {
          return DoIt<unsigned char, 3>( argc, argv );
          }
        else
          {
          return DoIt<unsigned char, 4>( argc, argv );
          }
        break;
      case itk::ImageIOBase::USHORT:
        if( N == 1 )
          {
          return DoIt<unsigned short, 1>( argc, argv );
          }
        else if( N == 2 )
          {
          return DoIt<unsigned short, 2>( argc, argv );
          }
        else if( N == 3 )
          {
          return DoIt<unsigned short, 3>( argc, argv );
          }
        else
          {
          return DoIt<unsigned short, 4>( argc, argv );
          }
        break;
      case itk::ImageIOBase::CHAR:
      case itk::ImageIOBase::SHORT:
        if( N == 1 )
          {
          return DoIt<short, 1>( argc, argv );
          }
        else if( N == 2 )
          {
          return DoIt<short, 2>( argc, argv );
          }
        else if( N == 3 )
          {
          return DoIt<short, 3>( argc, argv );
          }
        else
          {
          return DoIt<short, 4>( argc, argv );
          }
        break;
      case itk::ImageIOBase::UINT:
      case itk::ImageIOBase::INT:
      case itk::ImageIOBase::ULONG:
      case itk::ImageIOBase::LONG:
      case itk::ImageIOBase::FLOAT:
      case itk::ImageIOBase::DOUBLE:
        if( N == 1 )
          {
          return DoIt<float, 1>( argc, argv );
          }
        else if( N == 2 )
          {
          return DoIt<float, 2>( argc, argv );
          }
        else if( N == 3 )
          {
          return DoIt<float, 3>( argc, argv );
          }
        else
          {
          return DoIt<float, 4>( argc, argv );
          }
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
