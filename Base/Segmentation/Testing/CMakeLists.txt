##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

include_regular_expression( "^.*$" )

include( Midas3FunctionAddTest )
set( MIDAS_REST_URL http://midas3.kitware.com/midas/api/rest )
set( MIDAS_KEY_DIR ${TubeTK_SOURCE_DIR}/MIDAS_Keys )

find_package( PythonInterp )
if( PYTHON_EXECUTABLE )
  add_test( PrintSelf-BaseSegmentation ${PYTHON_EXECUTABLE}
    ${TubeTK_SOURCE_DIR}/CMake/PrintSelfCheck.py
    ${TubeTK_SOURCE_DIR}/Base/Segmentation )
endif( PYTHON_EXECUTABLE )

set( BASE_SEGMENTATION_TESTS
  $<TARGET_FILE:tubeBaseSegmentationTests> )

set( BASE_SEGMENTATION_HEADER_TEST
  $<TARGET_FILE:tubeBaseSegmentationHeaderTest> )

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( tubeBaseSegmentation_SRCS
  tubeBaseSegmentationPrintTest.cxx
  itktubeCVTImageFilterTest.cxx
  itktubePDFSegmenterTest.cxx
  itktubeRadiusExtractorTest.cxx
  itktubeRadiusExtractorTest2.cxx
  itktubeRidgeExtractorTest.cxx
  itktubeRidgeExtractorTest2.cxx
  itktubeRidgeSeedFilterTest.cxx
  itktubeTubeExtractorTest.cxx )

include_directories(
  ${TubeTK_SOURCE_DIR}/Base/Common
  ${TubeTK_SOURCE_DIR}/Base/Numerics
  ${TubeTK_SOURCE_DIR}/Base/Filtering
  ${TubeTK_SOURCE_DIR}/Base/Segmentation )

add_executable( tubeBaseSegmentationHeaderTest
  tubeBaseSegmentationHeaderTest.cxx )
target_link_libraries( tubeBaseSegmentationHeaderTest
  TubeTKNumerics
  ${ITK_LIBRARIES} )

add_executable( tubeBaseSegmentationTests
  tubeBaseSegmentationTests.cxx
  ${tubeBaseSegmentation_SRCS} )
target_link_libraries( tubeBaseSegmentationTests
  TubeTKNumerics
  ${ITK_LIBRARIES} )

add_test( NAME tubeBaseSegmentationHeaderTest
  COMMAND ${BASE_SEGMENTATION_HEADER_TEST} )

add_test( NAME tubeBaseSegmentationPrintTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
  tubeBaseSegmentationPrintTest )

Midas3FunctionAddTest( NAME itktubeCVTImageFilterTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    itktubeCVTImageFilterTest
      MIDAS{GDS0015_1.mha.md5}
      ${TEMP}/itkTubeCVTImageFilterTest.mha )

Midas3FunctionAddTest( NAME itktubePDFSegmenterTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    --compare MIDAS{itkTubePDFSegmenterTest_mask.mha.md5}
      ${TEMP}/itkTubePDFSegmenterTest_mask.mha
    itktubePDFSegmenterTest
      MIDAS{ES0015_Large.mha.md5}
      MIDAS{ES0015_Large.mha.md5}
      true
      5.0
      MIDAS{GDS0015_Large-TrainingMask.mha.md5}
      ${TEMP}/itkTubePDFSegmenterTest_prob0.mha
      ${TEMP}/itkTubePDFSegmenterTest_pdf0.mha
      ${TEMP}/itkTubePDFSegmenterTest_prob1.mha
      ${TEMP}/itkTubePDFSegmenterTest_pdf1.mha
      ${TEMP}/itkTubePDFSegmenterTest_mask.mha
      ${TEMP}/itkTubePDFSegmenterTest_labeledFeatureSpace.mha )

Midas3FunctionAddTest( NAME itktubePDFSegmenterTest2
  COMMAND ${BASE_SEGMENTATION_TESTS}
    --compare MIDAS{itkTubePDFSegmenterTest2_mask.mha.md5}
      ${TEMP}/itkTubePDFSegmenterTest2_mask.mha
    itktubePDFSegmenterTest
      MIDAS{im0001.crop.mha.md5}
      MIDAS{im0001.crop.contrast.mha.md5}
      false
      0.4
      MIDAS{im0001.vk.mask.crop.mha.md5}
      ${TEMP}/itkTubePDFSegmenterTest2_prob0.mha
      ${TEMP}/itkTubePDFSegmenterTest2_pdf0.mha
      ${TEMP}/itkTubePDFSegmenterTest2_prob1.mha
      ${TEMP}/itkTubePDFSegmenterTest2_pdf1.mha
      ${TEMP}/itkTubePDFSegmenterTest2_mask.mha
      ${TEMP}/itkTubePDFSegmenterTest2_labeledFeatureSpace.mha )

Midas3FunctionAddTest( NAME itktubeRidgeExtractorTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    --compare MIDAS{itkTubeRidgeExtractorTest.mha.md5}
      ${TEMP}/itkTubeRidgeExtractorTest.mha
    itktubeRidgeExtractorTest
      MIDAS{Branch.n010.sub.mha.md5}
      ${TEMP}/itkTubeRidgeExtractorTest.mha )

Midas3FunctionAddTest( NAME itktubeRidgeExtractorTest2
  COMMAND ${BASE_SEGMENTATION_TESTS}
    itktubeRidgeExtractorTest2
      MIDAS{Branch.n010.sub.mha.md5}
      MIDAS{Branch-truth_Subs.tre.md5} )

Midas3FunctionAddTest( NAME itktubeRadiusExtractorTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    itktubeRadiusExtractorTest
      MIDAS{Branch.n010.mha.md5}
      MIDAS{Branch-truth.tre.md5} )

Midas3FunctionAddTest( NAME itktubeRadiusExtractorTest2
  COMMAND ${BASE_SEGMENTATION_TESTS}
    itktubeRadiusExtractorTest2
      MIDAS{Branch.n010.mha.md5}
      MIDAS{Branch-truth.tre.md5} )

Midas3FunctionAddTest( NAME itktubeTubeExtractorTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    itktubeTubeExtractorTest
      MIDAS{Branch.n010.sub.mha.md5}
      MIDAS{Branch-truth.tre.md5} )

Midas3FunctionAddTest( NAME itktubeRidgeSeedFilterTest
  COMMAND ${BASE_SEGMENTATION_TESTS}
    --compare MIDAS{itkTubeRidgeSeedFilterTest_Output.mha.md5}
      ${TEMP}/itkTubeRidgeSeedFilterTest_Output.mha
    itktubeRidgeSeedFilterTest
      MIDAS{im0001.crop.contrast.mha.md5}
      MIDAS{im0001.vk.maskRidge.crop.mha.md5}
      255 127
      ${TEMP}/itkTubeRidgeSeedFilterTest_Class0PDF.mha
      ${TEMP}/itkTubeRidgeSeedFilterTest_Feature0Image.mha
      ${TEMP}/itkTubeRidgeSeedFilterTest_Output.mha
      ${TEMP}/itkTubeRidgeSeedFilterTest_MaxScale.mha )
