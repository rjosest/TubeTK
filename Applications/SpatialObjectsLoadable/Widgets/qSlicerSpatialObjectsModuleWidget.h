/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Michael Jeulin-L, Kitware Inc.

==============================================================================*/

#ifndef __qSlicerSpatialObjectsModuleWidget_h
#define __qSlicerSpatialObjectsModuleWidget_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerSpatialObjectsModuleWidgetsExport.h"

class qSlicerSpatialObjectsModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLSpatialObjectsNode;

/// \ingroup Slicer_QtModules_SpatialObjects
class Q_SLICER_MODULE_SPATIALOBJECTS_WIDGETS_EXPORT
qSlicerSpatialObjectsModuleWidget : public qSlicerAbstractModuleWidget
{
Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerSpatialObjectsModuleWidget(QWidget *parent=0);
  virtual ~qSlicerSpatialObjectsModuleWidget();

  vtkMRMLSpatialObjectsNode spatialObjectsNode() const;

public slots:
  void setSpatialObjectsNode(vtkMRMLNode*);
  void setSpatialObjectsNode(vtkMRMLSpatialObjectsNode*);
  void setSolidTubeColor(bool);

signals:
  void currentNodeChanged(vtkMRMLNode*);
  void currentNodeChanged(vtkMRMLSpatialObjectsNode*);

protected:
  virtual void setup();

protected:
  QScopedPointer<qSlicerSpatialObjectsModuleWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerSpatialObjectsModuleWidget);
  Q_DISABLE_COPY(qSlicerSpatialObjectsModuleWidget);
};

#endif
