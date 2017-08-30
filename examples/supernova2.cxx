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
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPropPicker.h>
#include <vtkCornerAnnotation.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>

//using namespace std;

vtkNew<ExternalVTKWidget> externalVTKWidget;
vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
static int windowId = -1;
static int windowH = 800;
static int windowW = 800;
static int NUM_ACTORS = 7;
std::vector <vtkSmartPointer<vtkActor>> actors;
std::vector <const char *> titles;
std::vector <const char *> annotations;
vtkSmartPointer<vtkCornerAnnotation> titleAnnotation = 
    vtkSmartPointer<vtkCornerAnnotation>::New();
vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation = 
    vtkSmartPointer<vtkCornerAnnotation>::New();
vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
    vtkSmartPointer<vtkCubeAxesActor>::New();

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

// set up annotations
void displayAnnotations() {
    // title annotations 
    titleAnnotation->SetLinearFontScaleFactor( 2 );
    titleAnnotation->SetNonlinearFontScaleFactor( 1 );
    titleAnnotation->SetMaximumFontSize( 40 );
    titleAnnotation->GetTextProperty()->SetColor( 0, 0, 0 );
    titleAnnotation->GetTextProperty()->BoldOn();
    
    // caption annotations
    cornerAnnotation->SetLinearFontScaleFactor(2);
    cornerAnnotation->SetNonlinearFontScaleFactor(1);
    cornerAnnotation->SetMaximumFontSize(40);
    cornerAnnotation->GetTextProperty()->SetColor(0, 0, 0);
    cornerAnnotation->GetTextProperty()->SetBackgroundColor(1,1,1);
    cornerAnnotation->GetTextProperty()->FrameOn();

    ren->AddViewProp(titleAnnotation);
    ren->AddViewProp(cornerAnnotation);
}

// Handle mouse events
class MouseInteractorHighLightActor : public vtkInteractorStyleTrackballCamera {
    public:
    static MouseInteractorHighLightActor* New();
    vtkTypeMacro(MouseInteractorHighLightActor, vtkInteractorStyleTrackballCamera);


    MouseInteractorHighLightActor() {
        LastPickedActor = NULL;
        LastPickedProperty = vtkProperty::New();
        for(int i = 0; i < NUM_ACTORS; i++) {
            OtherActors.push_back(actors[i]);
        }
        for(int i = 0; i < NUM_ACTORS; i++) {
            OtherProperties.push_back(vtkProperty::New());
        }
    }
  
    virtual ~MouseInteractorHighLightActor() {
        LastPickedProperty->Delete();

        for(int i = 0; i < NUM_ACTORS; i++) {
            OtherProperties[i]->Delete();
        }
    }
  
    virtual void OnLeftButtonDown() VTK_OVERRIDE{
        std::cout << "here" << std::endl;
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // Pick from this location.
        vtkSmartPointer<vtkPropPicker>  picker =
          vtkSmartPointer<vtkPropPicker>::New();
        picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

        // If we picked something before, reset its property
        if (this->LastPickedActor) {
            this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);

            for(int i = 0; i < NUM_ACTORS; i++) {
                this->OtherActors[i]->GetProperty()->DeepCopy(this->OtherProperties[i]);
            }
        }
        titleAnnotation->ClearAllTexts();
        cornerAnnotation->ClearAllTexts();

        this->LastPickedActor = picker->GetActor();

        if (this->LastPickedActor) {
            // Save the property of the picked actor so that we can
            // restore it next time
            this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());
            for(int i = 0; i < NUM_ACTORS; i++) {
                this->OtherProperties[i]->DeepCopy(this->OtherActors[i]->GetProperty());
            }
            // Highlight the picked actor by changing its properties
            for(int i = 0; i < NUM_ACTORS; i++) {
                if(actors[i] != LastPickedActor) {
                    this->OtherActors[i]->GetProperty()->SetColor(0.87, 0.88, 0.91);
                } else {
                    this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);
                    titleAnnotation->SetText(2, titles[i]);
                    cornerAnnotation->SetText(0, annotations[i]);
                }
            }

            this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
            this->LastPickedActor->GetProperty()->SetSpecular(0.0);
        }

        // Forward events
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    private:
        vtkActor    *LastPickedActor;
        vtkProperty *LastPickedProperty;
        std::vector <vtkSmartPointer<vtkActor>> OtherActors;
        std::vector <vtkSmartPointer<vtkProperty>> OtherProperties;
};

vtkStandardNewMacro(MouseInteractorHighLightActor);

// display cube axes
void displayAxes() {
    cubeAxesActor->SetBounds(-60, 70, -45, 50, -50, 50);
    cubeAxesActor->SetCamera(ren->GetActiveCamera());
    cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
    cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

    cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
    cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

    cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
    cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);

    cubeAxesActor->DrawXGridlinesOn();
    cubeAxesActor->DrawYGridlinesOn();
    cubeAxesActor->DrawZGridlinesOn();

    cubeAxesActor->SetGridLineLocation(
    cubeAxesActor->VTK_GRID_LINES_FURTHEST);

    cubeAxesActor->XAxisMinorTickVisibilityOff();
    cubeAxesActor->YAxisMinorTickVisibilityOff();
    cubeAxesActor->ZAxisMinorTickVisibilityOff();

    ren->AddActor(cubeAxesActor);
}

// hard code Kim's annotations
void initializeAnnotations() {
    annotations.push_back("At the center of Cas A is a neutron star, a small \nultra-dense star created by the supernova."); // sill (purple)
    annotations.push_back("In green, two jets of material are seen. \nThese jets funnel material and energy \nduring and after the explosion."); // jets (green)
    annotations.push_back("The light blue portions of this model \nrepresent radiation from the element \niron as seen in X-ray light from Chandra. \nIron is forged in the very core of the \nstar but ends up on the outside \nof the expanding ring of debris."); // fek (blue)
    annotations.push_back("The yellow portions of the model represent \ninfrared data from the Spitzer Space Telescope. \nThis is cooler debris that has yet to \nbe superheated by a passing shock wave"); // arll (yellow)
    annotations.push_back("The dark blue colored elements of the model \nrepresent the outer blast wave of the \nexplosion as seen in X-rays by Chandra as well \nas optical and infrared light, much of which is silicon."); // (dark blue)
    annotations.push_back("The red colored elements of the model represent \nthe outer blast wave of the explosion as seen in \nX-rays by Chandra as well as optical and infrared \nlight, much of which is silicon."); // outer knots (red)
    annotations.push_back("The Cas A supernova remnant acts like a \nrelativistic pinball machine by accelerating \nelectrons to enormous energies. This \narea shows where the acceleration is taking \nplace in an expanding shock wave generated \nby the explosion."); // reverse shock sphere (pink)
    
    titles.push_back("Neutron Star"); // purple
    titles.push_back("Fiducial Jets"); // green
    titles.push_back("FeK (Chandra Telescope)"); // blue
    titles.push_back("ArII (Spitzer Telescope)"); // yellow
    titles.push_back("Si (Chandra Telescope, HETG)"); // dark blue
    titles.push_back("Outer Knots"); // red
    titles.push_back("Reverse Shock Sphere"); // pink
}

void initialize() {
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;
    externalVTKWidget->SetRenderWindow(renWin.GetPointer());
        
    renWin->AddRenderer(ren);    

    /**********************************************************/
    
    std::string files[7] = {"../data/cco-ascii.vtk", "../data/newjets-ascii.vtk", "../data/fekcorr-ascii.vtk", "../data/newar-ascii.vtk", "../data/newhetg-ascii.vtk", "../data/newopt-ascii.vtk", "../data/newsi-ascii.vtk"};
    
    for(int i = 0; i < NUM_ACTORS; i++) {
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
        mapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetInterpolationToFlat();
        actor->GetProperty()->SetOpacity(0.8);

        ren->AddActor(actor);
        actors.push_back(actor);
    }
    
    // color actors
    actors[0]->GetProperty()->SetColor(0.97,0.45,0.91); // sill (purple)
    actors[1]->GetProperty()->SetColor(0.6,0.99,0.73); // jets (green)
    actors[2]->GetProperty()->SetColor(0.49,0.94,0.89); // fek (blue)
    actors[3]->GetProperty()->SetColor(0.95,0.95,0.33); // arll (yellow)
    actors[4]->GetProperty()->SetColor(0.26,0.59,0.94); // (dark blue)
    actors[5]->GetProperty()->SetColor(0.94,0.32,0.4); // outer knots (red)
    actors[6]->GetProperty()->SetColor(0.96,0.70,0.93); // reverse shock sphere (pink)

    /**********************************************************/
    
    // Set the custom type to use for interaction.
//    vtkSmartPointer<MouseInteractorHighLightActor> style =
//    vtkSmartPointer<MouseInteractorHighLightActor>::New();
//    style->SetDefaultRenderer(ren);
//    
//    interactor->SetRenderWindow(renWin.GetPointer());
//    interactor->SetInteractorStyle(style);
    
    /**********************************************************/
    initializeAnnotations();
    displayAnnotations();
    displayAxes();
    
    ren->SetBackground(0.87, 0.88, 0.91);
    ren->ResetCamera();
//    interactor->Initialize();
    
    vtkOpenGLCamera *camera = (vtkOpenGLCamera *)ren->GetActiveCamera();
    // camera default position set at (4.68744, 2.67252, 360.229) 
    camera->SetPosition(4, 2, 360);
    camera->SetViewUp(0,1,0); // controls "up" direction for camera
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
    
    
    for(int i = 0; i < 7; i++) {
        // transpose - vtk
        actors[i]->RotateX(y_angle);
        actors[i]->RotateY(x_angle);
        actors[i]->SetScale(scale_size);
    }
    cubeAxesActor->RotateX(y_angle);
    cubeAxesActor->RotateY(x_angle);
    cubeAxesActor->SetScale(scale_size);

    //interactor->Render();
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
    //interactor->Start();
    
    return EXIT_SUCCESS;
    
}