#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GLKit/GLKMatrix4.h>

#include <vtkWarpVector.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkDataSetAttributes.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkPolyDataReader.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkOpenGLCamera.h>
#include <vtkNew.h>
#include <ExternalVTKWidget.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmoothPolyDataFilter.h>

vtkNew<ExternalVTKWidget> externalVTKWidget;
vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
static int windowId = -1;
static int windowH = 800;
static int windowW = 800;
vtkSmartPointer<vtkActor> actors[7];

// interaction global variables
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


void initialize() {
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;
    externalVTKWidget->SetRenderWindow(renWin.GetPointer());
    
    renWin->AddRenderer(ren);
    
    /**********************************************************/
    
    // create transfer mapping scalar value to color
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(-1.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(-0.5, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(0.5, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(1, 0.0, 0.2, 0.0);

    /**********************************************************/
    
    std::string files[7] = {"../data/newsi-ascii.vtk", "../data/newjets-ascii.vtk", "../data/fekcorr-ascii.vtk", "../data/newar-ascii.vtk", "../data/newhetg-ascii.vtk", "../data/newopt-ascii.vtk", "../data/newsi-ascii.vtk"};
    
    for(int i = 0; i < 7; i++) {
        vtkSmartPointer<vtkPolyDataReader> reader =
        vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(files[i].c_str());
        reader->Update();
        vtkSmartPointer<vtkPolyData> inputPolyData = reader->GetOutput();

        // Merge duplicate points, and/or remove unused points and/or remove degenerate cells
        vtkSmartPointer<vtkCleanPolyData> clean =
        vtkSmartPointer<vtkCleanPolyData>::New();
        clean->SetInputData(inputPolyData);
        
        vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
        vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
        smoothFilter->SetInputConnection(clean->GetOutputPort());
        smoothFilter->SetNumberOfIterations(15);
        smoothFilter->SetRelaxationFactor(0.1);
        smoothFilter->FeatureEdgeSmoothingOff();
        smoothFilter->BoundarySmoothingOn();
        smoothFilter->Update();

        // Update normals on newly smoothed polydata
        vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
        normals->SetInputConnection(smoothFilter->GetOutputPort());
        normals->ComputePointNormalsOn();
        normals->ComputeCellNormalsOn();
        normals->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(normals->GetOutputPort());
        //mapper->ScalarVisibilityOff();
        
        mapper->SetLookupTable(colorTransferFunction);

        vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetInterpolationToFlat();
        actor->GetProperty()->SetOpacity(0.8);

        ren->AddActor(actor);
        actors[i] = actor;
    }
    
    actors[0]->GetProperty()->SetColor(0.97,0.45,0.91);
    actors[1]->GetProperty()->SetColor(0.6,0.99,0.73); // jets
    actors[2]->GetProperty()->SetColor(0.49,0.94,0.89); // 
    actors[3]->GetProperty()->SetColor(0.95,0.95,0.33);
    actors[4]->GetProperty()->SetColor(0.87,0.59,0.94);
    actors[5]->GetProperty()->SetColor(0.94,0.32,0.4);


    /**********************************************************/
    
    ren->SetBackground(0.87, 0.88, 0.91);
    ren->ResetCamera();
}


void display() {
    // Enable depth testing. Demonstrates OpenGL context being managed by external
    // application i.e. GLUT in this case.
    glEnable(GL_DEPTH_TEST);

    // Buffers being managed by external application i.e. GLUT in this case.
    glClearColor(0.87f, 0.88f, 0.91f, 1.0f); // Set background color to gray and opaque
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

    glFlush();  // Render now

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    

    vtkOpenGLCamera *camera = (vtkOpenGLCamera *)ren->GetActiveCamera();
    // camera default position set at (4.68744, 2.67252, 360.229) 
    camera->SetPosition(4, 2, 360);
    camera->SetFocalPoint(0,0,0); // initial direction
    camera->SetViewUp(0,1,0); // controls "up" direction for camera
    
    for(int i = 0; i < 7; i++) {
        // transpose - vtk
        //actors[i]->SetOrientation(0,0,0);
        actors[i]->RotateX(y_angle);
        actors[i]->RotateY(x_angle);
        actors[i]->SetScale(scale_size);
        
//        // transpose - opengl
//        double f[16];
//        actors[i]->GetMatrix(f);
//
//        // transpose
//        double g[16];
//        g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
//        g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
//        g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
//        g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
//        glMultMatrixd(g); // multiply current matrix with specified matrix
    }

    // camera - opengl
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    GLKMatrix4MakeLookAt(0,0,-5,0,0,0,0,1,0);

    externalVTKWidget->GetRenderWindow()->Render();
    glutSwapBuffers();
}

void MouseButton(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
      press_x = x; press_y = y;
      if (button == GLUT_LEFT_BUTTON) {
        xform_mode = XFORM_ROTATE;
      } else if (button == GLUT_RIGHT_BUTTON) {
        xform_mode = XFORM_SCALE;
      }
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

    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(windowW, windowH);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    windowId = glutCreateWindow("VTK External Window Test"); // Create a window with the given title
    initialize();
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutIdleFunc(display); 
    glutReshapeFunc(handleResize); // Register resize callback handler for window resize
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glewInit();
    
    glutMainLoop();  // Enter the infinitely event-processing loop
    
    return EXIT_SUCCESS;
    
}