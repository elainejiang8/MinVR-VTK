// This example tests the vtkRenderingExternal module by drawing a GLUT window
// and rendering a VTK cube in it. It uses an ExternalVTKWidget and sets a
// vtkExternalOpenGLRenderWindow to it.
//
// The test also demonstrates the use of
// PreserveColorBuffer and PreserveDepthBuffer flags on the
// vtkExternalOpenGLRenderer by drawing a GL_TRIANGLE in the scene before
// drawing the vtk sphere.

#include <GL/glew.h>
#include <GL/freeglut.h>
//// GLUT includes
//#if defined(__APPLE__)
// # include <GLUT/glut.h> // Include GLUT API.
//#else
//# if defined(_WIN32)
//# include "vtkWindows.h" // Needed to include OpenGL header on Windows.
//# endif // _WIN32
//# include <GL/glut.h> // Include GLUT API.
//#endif

// STD includes
#include <iostream>

// VTK includes
#include <ExternalVTKWidget.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLight.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkTesting.h>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

#include <vtkTransform.h>
#include <vtkRenderWindowInteractor.h>

namespace {

// Global variables used by the glutDisplayFunc and glutIdleFunc
vtkNew<ExternalVTKWidget> externalVTKWidget;
static bool initialized = false;
static int NumArgs;
char** ArgV;
static bool tested = false;
static int retVal = 0;
static int windowId = -1;
static int windowH = 501;
static int windowW = 500;
std::string filename;

vtkNew<vtkTransform> transform;
int press_x, press_y;
int release_x, release_y;
float x_angle = 0.0;
float y_angle = 0.0;
float scale_size = 1;
int xform_mode = 0;
#define XFORM_NONE    0
#define XFORM_ROTATE  1
#define XFORM_SCALE 2
    
vtkSmartPointer<vtkRenderWindowInteractor> iren;

static void MakeCurrentCallback(vtkObject* vtkNotUsed(caller),
                                long unsigned int vtkNotUsed(eventId),
                                void * vtkNotUsed(clientData),
                                void * vtkNotUsed(callData))
{
  if (initialized)
  {
    glutSetWindow(windowId);
  }
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display()
{
  if (!initialized)
  {
//          vtkSmartPointer<vtkExternalOpenGLRenderWindow> renWin = vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
//    externalVTKWidget->SetRenderWindow(renWin);
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;
    externalVTKWidget->SetRenderWindow(renWin.GetPointer());
      
//    iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
//    iren->SetRenderWindow(renWin);

    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(MakeCurrentCallback);
    renWin->AddObserver(vtkCommand::WindowMakeCurrentEvent,
                        callback.GetPointer());
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.GetPointer());
    vtkRenderer* ren = externalVTKWidget->AddRenderer();
      
//    // read the data from a vtk file
//    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
//    reader->SetFileName(filename.c_str());
//    reader->Update();
//    
//    // create transfer mapping scalar value to opacity
//    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
//    opacityTransferFunction->AddPoint(20, 0.0);
//    opacityTransferFunction->AddPoint(255, 0.2);
//    
//    // create transfer mapping scalar value to color
//    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
//    colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
//    colorTransferFunction->AddRGBPoint(64.0, 1.0, 0.0, 0.0);
//    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);
//    colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);
//    colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);
//    
//    // the property describes how the data will look
//    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
//    volumeProperty->SetColor(colorTransferFunction);
//    volumeProperty->SetScalarOpacity(opacityTransferFunction);
//    volumeProperty->ShadeOn();
//    volumeProperty->SetInterpolationTypeToLinear();
//    
//    // the mapper/ray cast function knows how to render the data
//    vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
//    vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkVolumeRayCastMapper>::New();
//    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
//    volumeMapper->SetInputConnection(reader->GetOutputPort());
//    
//    // the volume holds the mapper and the property and can be used to position/orient the volume
//    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
//    volume->SetMapper(volumeMapper);
//    volume->SetProperty(volumeProperty);
    
    
//    ren->AddVolume(volume);
//    ren->SetBackground(1, 1, 1);
//    renWin->SetSize(600, 600);
    ren->AddActor(actor.GetPointer());
    vtkNew<vtkCubeSource> cs;
    mapper->SetInputConnection(cs->GetOutputPort());
    actor->RotateX(45.0);
    actor->RotateY(45.0);
    ren->ResetCamera();
      
      

    initialized = true;
      //renWin->Render();
  }

  // Enable depth testing. Demonstrates OpenGL context being managed by external
  // application i.e. GLUT in this case.
  glEnable(GL_DEPTH_TEST);
//
//  // Buffers being managed by external application i.e. GLUT in this case.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

  glFlush();  // Render now
  glBegin(GL_TRIANGLES);
    glVertex3f(-1.5,-1.5,0.0);
    glVertex3f(1.5,0.0,0.0);
    glVertex3f(0.0,1.5,1.0);
  glEnd();

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
//  GLfloat lightpos[] = {-0.5f, 1.0f, 1.0f, 1.0f};
//  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
//  GLfloat diffuse[] = {0.0f, 0.8f, 0.8f, 1.0f};
//  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//  GLfloat specular[] = {0.5f, 0.0f, 0.0f, 1.0f};
//  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//  GLfloat ambient[] = {1.0f, 1.0f, 0.2f,  1.0f};
//  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
  externalVTKWidget->GetRenderWindow()->Render();
   // iren->Start();
  glutSwapBuffers();
}
    
void MouseButton(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
      std::cout << "Pressing down" << std::endl;
      press_x = x; press_y = y;
      if (button == GLUT_LEFT_BUTTON)
        xform_mode = XFORM_ROTATE;
       else if (button == GLUT_RIGHT_BUTTON)
        xform_mode = XFORM_SCALE;
    }
    else if (state == GLUT_UP) {
        std::cout << "Released" << std::endl;
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

void test()
{
  bool interactiveMode = true;
  //vtkTesting* t = vtkTesting::New();
  for(int cc = 1; cc < NumArgs; cc++)
  {
    //t->AddArgument(ArgV[cc]);
    if (strcmp(ArgV[cc], "-I") == 0)
    {
        std::cout << "Interactive mode on." << std::endl;
      interactiveMode = true;
    }
  }
  //t->SetRenderWindow(externalVTKWidget->GetRenderWindow());
  if (!tested)
  {
    //retVal = t->RegressionTest(0);
    tested = true;
  }
  ///t->Delete();
  if (!interactiveMode)
  {
    // Exit out of the infinitely running loop
    std::cout << "Exited infinite loop." << std::endl;
    exit(!retVal);
  }
}

void handleResize(int w, int h)
{
  std::cout << "Resizing window." << std::endl;
  externalVTKWidget->GetRenderWindow()->SetSize(w, h);
  glutPostRedisplay();
}

void onexit(void)
{
  initialized = false;
}

} // end anon namespace

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char* argv[])
{
  //filename = argv[1]; // "/Data/ironProt.vtk;
  NumArgs = argc;
  ArgV = argv;
  glutInit(&argc, argv);                 // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(windowW, windowH);   // Set the window's initial width & height
  glutInitWindowPosition(101, 201); // Position the window's initial top-left corner
  windowId = glutCreateWindow("VTK External Window Test"); // Create a window with the given title
  glutDisplayFunc(display); // Register display callback handler for window re-paint
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);
  //glutIdleFunc(test); // Register test callback handler for vtkTesting
  glutReshapeFunc(handleResize); // Register resize callback handler for window resize
  atexit(onexit);  // Register callback to uninitialize on exit
  glewInit();
  glutMainLoop();  // Enter the infinitely event-processing loop
  return 0;
}
