/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCustomExternalOpenGLCamera.cxx

  Copyright (c) 

=========================================================================*/
#include "vtkCustomExternalOpenGLCamera.h"
#include "vtkgluPickMatrix.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLError.h"
#include "vtkOpenGL.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOutputWindow.h"
#include "vtkPerspectiveTransform.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"

#include <cmath>

vtkStandardNewMacro(vtkCustomExternalOpenGLCamera);

//----------------------------------------------------------------------------
vtkCustomExternalOpenGLCamera::vtkCustomExternalOpenGLCamera()
{
  this->UserProvidedViewTransform = true;
}

//----------------------------------------------------------------------------
// Implement base class method.
void vtkCustomExternalOpenGLCamera::Render(vtkRenderer *ren)
{
  vtkOpenGLClearErrorMacro();

  int  lowerLeft[2];
  int usize, vsize;

  vtkOpenGLRenderWindow *win = vtkOpenGLRenderWindow::SafeDownCast(ren->GetRenderWindow());

  // find out if we should stereo render
  this->Stereo = (ren->GetRenderWindow())->GetStereoRender();
  ren->GetTiledSizeAndOrigin(&usize, &vsize, lowerLeft, lowerLeft+1);

  // Take the window position into account
  for (int i = 0; i < 2; ++i)
  {
    lowerLeft[i] = lowerLeft[i] + ren->GetRenderWindow()->GetPosition()[i];
  }

  // if were on a stereo renderer draw to special parts of screen
  if (this->Stereo)
  {
    switch ((ren->GetRenderWindow())->GetStereoType())
    {
      case VTK_STEREO_CRYSTAL_EYES:
        if (this->LeftEye)
        {
          if (ren->GetRenderWindow()->GetDoubleBuffer())
          {
            glDrawBuffer(static_cast<GLenum>(win->GetBackLeftBuffer()));
            glReadBuffer(static_cast<GLenum>(win->GetBackLeftBuffer()));
          }
          else
          {
            glDrawBuffer(static_cast<GLenum>(win->GetFrontLeftBuffer()));
            glReadBuffer(static_cast<GLenum>(win->GetFrontLeftBuffer()));
          }
        }
        else
        {
           if (ren->GetRenderWindow()->GetDoubleBuffer())
           {
            glDrawBuffer(static_cast<GLenum>(win->GetBackRightBuffer()));
            glReadBuffer(static_cast<GLenum>(win->GetBackRightBuffer()));
           }
          else
          {
            glDrawBuffer(static_cast<GLenum>(win->GetFrontRightBuffer()));
            glReadBuffer(static_cast<GLenum>(win->GetFrontRightBuffer()));
          }
        }
        break;
      case VTK_STEREO_LEFT:
        this->LeftEye = 1;
        break;
      case VTK_STEREO_RIGHT:
        this->LeftEye = 0;
        break;
      default:
        break;
    }
  }
  else
  {
    if (ren->GetRenderWindow()->GetDoubleBuffer())
    {
      glDrawBuffer(static_cast<GLenum>(win->GetBackBuffer()));

      // Reading back buffer means back left. see OpenGL spec.
      // because one can write to two buffers at a time but can only read from
      // one buffer at a time.
      glReadBuffer(static_cast<GLenum>(win->GetBackBuffer()));
    }
    else
    {
      glDrawBuffer(static_cast<GLenum>(win->GetFrontBuffer()));

      // Reading front buffer means front left. see OpenGL spec.
      // because one can write to two buffers at a time but can only read from
      // one buffer at a time.
      glReadBuffer(static_cast<GLenum>(win->GetFrontBuffer()));
    }
  }

  glViewport(lowerLeft[0], lowerLeft[1], usize, vsize);
  glEnable(GL_SCISSOR_TEST);
  glScissor(lowerLeft[0], lowerLeft[1], usize, vsize);

  // some renderer subclasses may have more complicated computations for the
  // aspect ratio. So take that into account by computing the difference
  // between our simple aspect ratio and what the actual renderer is reporting.
  double aspect[2];
  ren->ComputeAspect();
  ren->GetAspect(aspect);
  double aspect2[2];
  ren->vtkViewport::ComputeAspect();
  ren->vtkViewport::GetAspect(aspect2);
  double aspectModification = aspect[0] * aspect2[1] / (aspect[1] * aspect2[0]);

  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

  glMatrixMode(GL_PROJECTION);
  if (usize && vsize)
  {
    matrix->DeepCopy(this->GetProjectionTransformMatrix(
                       aspectModification * usize / vsize, -1, 1));
    matrix->Transpose();
  }
  if (ren->GetIsPicking())
  {
    int size[2] = {usize, vsize};
    glLoadIdentity();
    vtkgluPickMatrix(ren->GetPickX(), ren->GetPickY(),
                     ren->GetPickWidth(), ren->GetPickHeight(),
                     lowerLeft, size);
    glMultMatrixd(matrix->Element[0]);
  }
  else
  {
    // insert camera view transformation
    glLoadMatrixd(matrix->Element[0]);
  }

  // push the model view matrix onto the stack, make sure we
  // adjust the mode first
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  matrix->DeepCopy(this->GetViewTransformMatrix());
  matrix->Transpose();

  // insert camera view transformation
  if(this->UserProvidedViewTransform)
  {
    glLoadMatrixd(matrix->Element[0]);
  }
  else
  {
    glMultMatrixd(matrix->Element[0]);
  }

  matrix->Delete();

  if ((ren->GetRenderWindow())->GetErase() && ren->GetErase()
      && !ren->GetIsPicking())
  {
    ren->Clear();
  }

  vtkOpenGLCheckErrorMacro("failed after Render");
}

//----------------------------------------------------------------------------
void vtkCustomExternalOpenGLCamera::SetViewTransformMatrix(
  const double elements[16])
{
  if (!elements)
  {
    return;
  }
  // Transpose the matrix to undo the transpose that VTK does internally
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  matrix->DeepCopy(elements);
  matrix->Transpose();
  this->ViewTransform->SetMatrix(matrix);
  this->ModelViewTransform->SetMatrix(matrix);
  this->UserProvidedViewTransform = true;
  
  // create a camera-view transformation
//    vtkSmartPointer<vtkPerspectiveTransform> transform = vtkSmartPointer<vtkPerspectiveTransform>::New();
//    // set matrix to view matrix
//    transform->SetMatrix(matrix);
//    this->SetUserViewTransform(transform);
  matrix->Delete();
}

//----------------------------------------------------------------------------
void vtkCustomExternalOpenGLCamera::SetProjectionTransformMatrix(
  const double elements[16])
{
  if (!elements)
  {
    return;
  }
  // Transpose the matrix to undo the transpose that VTK does internally
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  matrix->DeepCopy(elements);
  matrix->Transpose();

  this->SetExplicitProjectionTransformMatrix(matrix);
  this->SetUseExplicitProjectionTransformMatrix(true);
  matrix->Delete();
}

//----------------------------------------------------------------------------
void vtkCustomExternalOpenGLCamera::ComputeViewTransform()
{
  if (this->UserProvidedViewTransform)
  {
    // Do not do anything
    return;
  }
  else
  {
    this->Superclass::ComputeViewTransform();
  }
}

//----------------------------------------------------------------------------
void vtkCustomExternalOpenGLCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
