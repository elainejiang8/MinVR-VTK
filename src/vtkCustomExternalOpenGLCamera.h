/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCustomExternalOpenGLCamera.h

  Copyright (c) Elaine Jiang
=========================================================================*/
/**
 * @class   vtkCustomExternalOpenGLCamera
 * @brief   OpenGL camera
 *
 * vtkCustomExternalOpenGLCamera is a concrete implementation of the abstract class
 * vtkCamera.  vtkCustomExternalOpenGLCamera interfaces to the OpenGL rendering library.
 * This class extends vtkOpenGLCamera by introducing API wherein the camera
 * matrices can be set explicitly by the application.
*/

#ifndef vtkCustomExternalOpenGLCamera_h
#define vtkCustomExternalOpenGLCamera_h

#include "vtkRenderingExternalModule.h" // For export macro
#include "vtkOpenGLCamera.h"

class VTKRENDERINGEXTERNAL_EXPORT vtkCustomExternalOpenGLCamera :
  public vtkOpenGLCamera
{
public:
  static vtkCustomExternalOpenGLCamera *New();
  vtkTypeMacro(vtkCustomExternalOpenGLCamera, vtkOpenGLCamera);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
//  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Implement base class method.
   */
  void Render(vtkRenderer *ren) VTK_OVERRIDE;
//   virtual void Render(vtkRenderer *ren);

  /**
   * Set the view transform matrix
   */
  void SetViewTransformMatrix(const double elements[16]);

  /**
   * Set the projection matrix
   */
  void SetProjectionTransformMatrix(const double elements[16]);

protected:
  vtkCustomExternalOpenGLCamera();
  ~vtkCustomExternalOpenGLCamera() VTK_OVERRIDE {}
//  virtual ~vtkCustomExternalOpenGLCamera() {}

  /**
   * These methods should only be used within vtkCamera.cxx.
   * Bypass computation if user provided the view transform
   */
  void ComputeViewTransform() VTK_OVERRIDE;
//  void ComputeViewTransform();

private:
  bool UserProvidedViewTransform;

  vtkCustomExternalOpenGLCamera(const vtkCustomExternalOpenGLCamera&) VTK_DELETE_FUNCTION;
  void operator=(const vtkCustomExternalOpenGLCamera&) VTK_DELETE_FUNCTION;
};

#endif
