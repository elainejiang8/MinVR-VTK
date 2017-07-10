#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include "../src/math_3d.h"

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <ExternalVTKWidget.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkCallbackCommand.h>
#include <vtkTransform.h>

vtkNew<ExternalVTKWidget> externalVTKWidget;
static bool initialized = false;
static int windowId = -1;
static int windowH = 501;
static int windowW = 500;
std::string filename;
vtkSmartPointer<vtkRenderer> ren = externalVTKWidget->AddRenderer();
vtkNew<vtkActor> actor;
vtkNew<vtkVolume> volume;

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


// define an interaction style
class KeyPressInteractorStyle: public vtkInteractorStyleTrackballCamera {
    public:
        static KeyPressInteractorStyle* New();
        vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
    
        
        virtual void OnKeyPress() override {
            // get the key press
            vtkRenderWindowInteractor *rwi = this->Interactor;
            std::string key = rwi->GetKeySym();
            
            // output the key that was pressed
            std::cout << "Pressed " << key << std::endl;
            
            // handle an arrow key
            if(key == "Up") {
                std::cout << "The up arrow was pressed." << std::endl;
            }
            if(key == "a") {
                std::cout << "The a key was pressed." << std::endl;
            }
            
            // forward events
            vtkInteractorStyleTrackballCamera::OnKeyPress();
        }
};
vtkStandardNewMacro(KeyPressInteractorStyle);


void initialize() {
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;
    externalVTKWidget->SetRenderWindow(renWin.GetPointer());

    vtkNew<vtkPolyDataMapper> mapper;
    actor->SetMapper(mapper.GetPointer());
    
//    // read the data from a vtk file
//    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
//    reader->SetFileName(filename.c_str());
//    reader->Update();
//    
//    // create transfer mapping scalar value to opacity
//    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
////    opacityTransferFunction->AddPoint(20, 0.0);
////    opacityTransferFunction->AddPoint(255, 0.2);
//    opacityTransferFunction->AddPoint(0.0,0.0);
//  opacityTransferFunction->AddPoint(80.0,1.0);
//  opacityTransferFunction->AddPoint(80.1,0.0);
//  opacityTransferFunction->AddPoint(255.0,0.0);
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
//    // the mapper knows how to render the data
//    vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
//    volumeMapper->SetInputConnection(reader->GetOutputPort());
//    
//    // the volume holds the mapper and the property and can be used to position/orient the volume
//    //vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
//    volume->SetMapper(volumeMapper);
//    volume->SetProperty(volumeProperty);
//
//    volume->SetPosition(0, 0, 0);
//    //ren->AddVolume(volume.GetPointer());
//    ren->AddViewProp(volume.GetPointer());
//    ren->ResetCamera();
//    volumeMapper->SetRequestedRenderModeToRayCast();
    
    ren->AddActor(actor.GetPointer());
    vtkNew<vtkCubeSource> cs;
    mapper->SetInputConnection(cs->GetOutputPort());
    actor->RotateX(45.0);
    actor->RotateY(45.0);
    ren->ResetCamera();

    initialized = true;
}


void display() {
    // Enable depth testing. Demonstrates OpenGL context being managed by external
    // application i.e. GLUT in this case.
    glEnable(GL_DEPTH_TEST);

    // Buffers being managed by external application i.e. GLUT in this case.
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Set background color to black and opaque
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

    glFlush();  // Render now
    
    // draw gl triangles
    glBegin(GL_TRIANGLES);
        glVertex3f(-1.5,-1.5,0.0);
        glVertex3f(1.5,0.0,0.0);
        glVertex3f(0.0,1.5,1.0);
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // no shading
//    GLfloat lightpos[] = {10.0f, 10.0f, 10.0f, 1.0f};
//    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
   // color
    GLfloat diffuse[] = {1.0f, 0.8f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    GLfloat specular[] = {0.5f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    GLfloat ambient[] = {1.0f, 1.0f, 0.2f,  1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);


    vtkCamera *camera = ren->GetActiveCamera();
//    camera->SetPosition(0,0,0);
//    camera->SetFocalPoint(0,0,0); // initial direction
//    camera->SetViewUp(0,1,0); // controls "up" direction for camera
    //camera->Azimuth(150);
    //camera->Elevation(60);
    ren->ResetCamera();

    // transpose - vtk
    volume->SetOrientation(0,1,0);
    volume->RotateX(y_angle);
    volume->RotateY(x_angle);
    volume->SetScale(scale_size);

    // camera - opengl
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(0,0,-5,0,0,0,0,1,0);

    // transpose - opengl
    double f[16];
    volume->GetMatrix(f);
    
    // transpose
    double g[16];
    g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
    g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
    g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
    g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
    glMultMatrixd(g); // multiply current matrix with specified matrix

    
    externalVTKWidget->GetRenderWindow()->Render();
    glutSwapBuffers();
    //std::cerr <<"In callback" << std::endl;
   
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
      y_angle = (y - press_y)/2;

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


void handleResize(int w, int h)
{
  externalVTKWidget->GetRenderWindow()->SetSize(w, h);
  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr <<"Required arguments: vtkFile" << std::endl;
        return EXIT_FAILURE;
    }
    
    filename = argv[1]; // "/Data/ironProt.vtk;
    
    
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(windowW, windowH);   // Set the window's initial width & height
    glutInitWindowPosition(201, 201); // Position the window's initial top-left corner
    windowId = glutCreateWindow("VTK External Window Test"); // Create a window with the given title
    initialize();
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutIdleFunc(display); 
    glutReshapeFunc(handleResize); // Register resize callback handler for window resize
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    //matexit(onexit);  // Register callback to uninitialize on exit
    glewInit();
    
    glutMainLoop();  // Enter the infinitely event-processing loop
    
    return EXIT_SUCCESS;
    
}