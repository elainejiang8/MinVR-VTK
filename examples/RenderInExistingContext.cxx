
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkNew.h"
#include "vtkOpenGLRenderer.h"
#include "vtkMatrix4x4.h"
#include "vtkEarthSource.h"
#include "vtkTransform.h"
#include "vtkOpenGLProperty.h"
#include "vtkLightCollection.h"
#include "vtkLight.h"
#include <vtkExternalOpenGLRenderWindow.h>
#include <ExternalVTKWidget.h>
#include <vtkSmartPointer.h>

vtkNew<ExternalVTKWidget> externalVTKWidget;
vtkSmartPointer<vtkRenderer> ren = externalVTKWidget->AddRenderer();
vtkNew<vtkExternalOpenGLRenderWindow> renWin;
vtkNew<vtkActor> coneActor;


vtkNew<vtkTransform> transform;
//transform->Identity();

int press_x, press_y;
int release_x, release_y;
float x_angle = 0.0;
float y_angle = 0.0;
float scale_size = 1;
int xform_mode = 0;
#define XFORM_NONE    0
#define XFORM_ROTATE  1
#define XFORM_SCALE 2

void initGlut()
{
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120, 1, .1, 1000);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void Reshape( int width, int height )
{
  renWin->SetSize( width, height );
}

void opengl_draw()
{
    glutWireTeapot(.5);
}

void Draw()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    externalVTKWidget->SetRenderWindow(renWin.GetPointer());
    
#if 0
    double f[16];
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, f);
    ren->GetActiveCamera()->SetModelTransformMatrix(f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,5,0,0,0,0,1,0);
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);//
#elif 1
    // vtk camera -> set opengl modelview matrix
    // problem: rotation
    vtkCamera *camera = ren->GetActiveCamera();
    double eyein[3] = {0,0,5};
    double upin[3] = {0, 1, 0};
    //double focal[3] = {0,0,0};
#if 1 // light follow camera
    camera->SetPosition(transform->TransformVector(eyein));
    camera->SetViewUp(transform->TransformVector(upin));
#else // static light
    camera->SetModelTransformMatrix(transform->GetMatrix());
    camera->SetPosition(eyein);
    vtkLightCollection *lights =  ren->GetLights();
    lights->InitTraversal();
    vtkLight *l;
    while( (l = lights->GetNextItem())!=NULL)
    {
        vtkNew<vtkMatrix4x4> m;
        m->Identity();
        l->SetPosition(1, 0,0);
        //l->SetTransformMatrix(m.GetPointer());
    }
    camera->SetViewUp(0,1,0);
#endif
    camera->SetFocalPoint(0,0,0);
    // get modelview matrix
    vtkMatrix4x4 *m = camera->GetModelViewTransformMatrix();
    double *f = &m->Element[0][0];
    // opengl
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultTransposeMatrixd(f);
    // draw
    opengl_draw();
#elif 0
    // opengl modelview -> vtk camera
    //problem: light does not follow camera
    // camera - vtk
    vtkCamera *camera = ren->GetActiveCamera();
    camera->SetPosition(0,0,-5);
    camera->SetFocalPoint(0,0,0);
    camera->SetViewUp(0,1,0);
    // camera - opengl
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,-5,0,0,0,0,1,0);
    double f[16];
    // geometry - opengl
    glPushMatrix();
    {
        glLoadIdentity();
        glRotatef(x_angle, 0, 1,0);
        glRotatef(y_angle, 1,0,0);
        glScalef(scale_size, scale_size, scale_size);//
        glGetDoublev(GL_MODELVIEW_MATRIX, f);
        // transpose
        double g[16];
        g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
        g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
        g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
        g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
        ren->GetActiveCamera()->SetModelTransformMatrix(g);
    }
    glPopMatrix();
    glMultMatrixd(f);
    // draw
    opengl_draw();
#else
    // rotate on actor
    //vtkMatrix4x4 m;

    //coneActor->SetUserMatrix();

    // camera - vtk
    vtkCamera *camera = ren->GetActiveCamera();
    camera->SetPosition(0,0,-5);
    camera->SetFocalPoint(0,0,0);
    camera->SetViewUp(0,1,0);

    // transpose - vtk
    coneActor->SetOrientation(0,1,0);
    coneActor->RotateX(y_angle);
    coneActor->RotateY(x_angle);
    coneActor->SetScale(scale_size);

    // camera - opengl
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,-5,0,0,0,0,1,0);

    // transpose - opengl
    double f[16];
    coneActor->GetMatrix(f);
    // transpose
    double g[16];
    g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
    g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
    g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
    g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
    glMultMatrixd(g);

    // draw
    opengl_draw();


#endif


    glLoadIdentity();
    renWin->Render();




    glutSwapBuffers();
}


void MouseButton(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
      press_x = x; press_y = y;
      if (button == GLUT_LEFT_BUTTON)
        xform_mode = XFORM_ROTATE;
       else if (button == GLUT_RIGHT_BUTTON)
        xform_mode = XFORM_SCALE;
    }
    else if (state == GLUT_UP) {
        xform_mode = XFORM_NONE;
    }
}


void MouseMotion(int x, int y)
{
    if (xform_mode==XFORM_ROTATE) {
      x_angle = (x - press_x)/2;
      //if (x_angle > 180) x_angle -= 360;
      //else if (x_angle <-180) x_angle += 360;

      y_angle = (y - press_y)/2;
      //if (y_angle > 90) y_angle = 90;
      //else if (y_angle <-90) y_angle = -90;

      double axis[3];
      axis[0] = -y_angle;
      axis[1] = -x_angle;
      axis[2] = 0;
      double mag = (y_angle*y_angle+x_angle*x_angle);
      transform->RotateWXYZ(mag, axis);
    }
    else if (xform_mode == XFORM_SCALE){
      float old_size = scale_size;
      scale_size = (1 - (y - press_y)/120.0);
      if (scale_size <0) scale_size = old_size;

      transform->Scale(scale_size, scale_size, scale_size);
    }
    press_x = x;
    press_y = y;
    glutPostRedisplay();
}


int main( int argc, char **argv )
{
  // GLUT initialization
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow( "VTK-GLUT Example") ;
  glutReshapeFunc( Reshape );
  glutDisplayFunc( Draw );
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);

  // Creation of a simple VTK pipeline
  vtkNew<vtkConeSource> cone;

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection( cone->GetOutputPort() );

  coneActor->SetMapper( coneMapper.GetPointer() );
    
  

  ren->AddActor( coneActor.GetPointer() );
  renWin->AddRenderer( ren.GetPointer() );

  initGlut();
  // Here is the trick: we ask the RenderWindow to join the current OpenGL context created by GLUT
  //renWin->InitializeFromCurrentContext();
  //ren->EraseOff(); // important!
  ren->LightFollowCameraOn();
  ren->TwoSidedLightingOn();


  // Let's start the main GLUT rendering loop
  glutMainLoop();

  return EXIT_SUCCESS;
}
