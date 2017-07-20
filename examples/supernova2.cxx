#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GLKit/GLKMatrix4.h>

#include <vtkWarpVector.h>
#include <vtkXMLPolyDataReader.h>
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
#include <vtkNew.h>
#include <ExternalVTKWidget.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkPolyDataReader.h>

vtkNew<ExternalVTKWidget> externalVTKWidget;
vtkSmartPointer<vtkRenderer> ren = externalVTKWidget->AddRenderer();
static int windowId = -1;
static int windowH = 800;
static int windowW = 800;
vtkSmartPointer<vtkActor> actors[7];

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
    
    vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();
    
    /**********************************************************/
    
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(7);
    double opacity = 0.3;
    lut->SetTableValue(0, 0, 0, 1, opacity);
    lut->SetTableValue(1, 0, 1.0, 0, opacity);
    lut->SetTableValue(2, 0.6, 1.0, 0.0, opacity);
    lut->SetTableValue(3, 1.0, 1.0, 0.0, 0.7);
    lut->SetTableValue(4, 1.0, 0.8, 0.0, opacity);
    lut->SetTableValue(5, 1.0, 0.4, 0.0, opacity);
    lut->SetTableValue(6, 1.0, 0.0, 0.0, 1);
    lut->SetTableRange(0,6);
    lut->Build();
    
    /**********************************************************/
    
    std::string files[7] = {"../data/newsi-ascii.vtk", "../data/newjets-ascii.vtk", "../data/fekcorr-ascii.vtk", "../data/newar-ascii.vtk", "../data/newhetg-ascii.vtk", "../data/newopt-ascii.vtk", "../data/newsi-ascii.vtk"};
    
    for(int i = 0; i < 7; i++) {
        vtkSmartPointer<vtkPolyDataReader> reader =
        vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(files[i].c_str());
        reader->Update();
        vtkSmartPointer<vtkPolyData> inputPolyData = reader->GetOutput();

        vtkSmartPointer<vtkCleanPolyData> clean =
        vtkSmartPointer<vtkCleanPolyData>::New();
        clean->SetInputData(inputPolyData);

        // Generate normals
        vtkSmartPointer<vtkPolyDataNormals> normals =
        vtkSmartPointer<vtkPolyDataNormals>::New();
        normals->SetInputConnection(clean->GetOutputPort());
        normals->SplittingOff();
        

        vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(normals->GetOutputPort());
        //mapper->ScalarVisibilityOff();
        
        mapper->SetLookupTable(lut);
        mapper->SetScalarVisibility(1);
        mapper->SetScalarRange(0,6);

        vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetInterpolationToFlat();

        ren->AddActor(actor);
        actors[i] = actor;
    }

    /**********************************************************/
    
    ren->SetBackground(0, 0, 0);
    //ren->SetActiveCamera(camera);
    //ren->ResetCamera();
    renWin->AddRenderer(ren);
}


void display() {
    // Enable depth testing. Demonstrates OpenGL context being managed by external
    // application i.e. GLUT in this case.
    glEnable(GL_DEPTH_TEST);

    // Buffers being managed by external application i.e. GLUT in this case.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

    glFlush();  // Render now

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // no shading
//    GLfloat lightpos[] = {10.0f, 10.0f, 10.0f, 1.0f};
//    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
   // color
    GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    GLfloat specular[] = {0.5f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    GLfloat ambient[] = {1.0f, 1.0f, 0.2f,  1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);


//    vtkCamera *camera = ren->GetActiveCamera();
////    camera->SetPosition(0,0,-100);
//    camera->SetFocalPoint(0,0,0); // initial direction
//    camera->SetViewUp(0,1,0); // controls "up" direction for camera
//    ren->ResetCamera();

    
    for(int i = 0; i < 7; i++) {
        // transpose - vtk
        actors[i]->SetOrientation(0,0,0);
        actors[i]->RotateX(y_angle);
        actors[i]->RotateY(x_angle);
        actors[i]->SetScale(scale_size);
        
        // transpose - opengl
        double f[16];
        actors[i]->GetMatrix(f);

        // transpose
        double g[16];
        g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
        g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
        g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
        g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
        glMultMatrixd(g); // multiply current matrix with specified matrix
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
        //std::cerr <<"Left" << std::endl;
        xform_mode = XFORM_ROTATE;
      } else if (button == GLUT_RIGHT_BUTTON) {
        //std::cerr <<"Right" << std::endl;
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

    //GLKMatrix4MakePerspective(60, (GLfloat)w/(GLfloat)h, 1, 100);   // only the window is changing, not the camera
    //glMatrixMode(GL_MODELVIEW);
  
    
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
    //glutIdleFunc(display); 
    glutReshapeFunc(handleResize); // Register resize callback handler for window resize
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    //matexit(onexit);  // Register callback to uninitialize on exit
    glewInit();
    
    glutMainLoop();  // Enter the infinitely event-processing loop
    
    return EXIT_SUCCESS;
    
}