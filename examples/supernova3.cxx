#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <api/MinVR.h>

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
#include <vtkPerspectiveTransform.h>
#include <vtkCustomExternalOpenGLCamera.h>

using namespace std;

class DemoVRVTKApp: public MinVR::VRApp {
  // Data values that were global in the supernova2.cxx file are defined as
  // private members of the VRApp.
private:

    vtkNew<ExternalVTKWidget> externalVTKWidget;
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New(); 
    vtkSmartPointer<vtkCustomExternalOpenGLCamera> camera;
    vector <vtkSmartPointer<vtkActor>> actors;
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;

    vtkNew<vtkTransform> transform;
    int press_x, press_y;
    int release_x, release_y;
    int NUM_ACTORS = 7;
  
    // These functions from demo2.cpp are not needed here:
    //
    //    init()
    //    makeWindow()
    //    resizeWindow()
    //    ... also most of the processKeys() methods.
    //
    // The functionality of these methods is assumed by the MinVR apparatus.

    // This contains a bunch of sanity checks from the graphics
    // initialization of demo2.xx.  They are still useful with MinVR.
    void _checkContext() {

    // There is one more graphics library used here, called GLEW.  This
    // library sorts through the various OpenGL updates and changes and
    // allows a user to pretend that it's all a consistent and simple
    // system.  The 'core profile' refers to some modern OpenGL
    // enhancements that are not so modern as of 2017.  Set this to true
    // to get those enhancements.
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
      throw std::runtime_error("Failed to initialize GLEW");
    }

    // Now that we have a graphics context, let's look at what's inside.
    std::cout << "Hardware check: "
              << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
              << " / "
              << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
              << std::endl;

    if (glewIsSupported("GL_VERSION_2_1")) {
      std::cout << "Software check: Ready for OpenGL 2.1." << std::endl;
    } else {
      throw std::runtime_error("Software check: OpenGL 2.1 not supported.");
    }

    // This is the background color of the viewport.
    glClearColor(0.1 , 0.0, 0.4, 1.0);

    // Now we're ready to start issuing OpenGL calls.  Start by enabling
    // the modes we want.  The DEPTH_TEST is how you get hidden faces.
    glEnable(GL_DEPTH_TEST);

    if (glIsEnabled(GL_DEPTH_TEST)) {
      std::cout << "Depth test enabled" << std::endl;
    } else {
      std::cout << "No depth test enabled" << std::endl;
    }

    // This is just a performance enhancement that allows OpenGL to
    // ignore faces that are facing away from the camera.
    glEnable(GL_CULL_FACE);
    }


    void _initializeScene() {
        vtkNew<vtkExternalOpenGLRenderWindow> renWin;
        externalVTKWidget->SetRenderWindow(renWin.GetPointer());
        
        renWin->AddRenderer(ren);
        externalVTKWidget->GetRenderWindow()->Render();

       /**********************************************************/
    
        // create transfer mapping scalar value to color
        vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
        colorTransferFunction->AddRGBPoint(-1.0, 0.0, 0.0, 0.0);
        colorTransferFunction->AddRGBPoint(-0.5, 1.0, 0.0, 0.0);
        colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 1.0);
        colorTransferFunction->AddRGBPoint(0.5, 0.0, 1.0, 0.0);
        colorTransferFunction->AddRGBPoint(1, 0.0, 0.2, 0.0);

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

            mapper->SetLookupTable(colorTransferFunction);

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
    
        ren->SetBackground(0.87, 0.88, 0.91);
        ren->ResetCamera();
    }


    public:
    DemoVRVTKApp(int argc, char** argv) :
    MinVR::VRApp(argc, argv) {
        
    }

    /// The MinVR apparatus invokes this method whenever there is a new
    /// event to process.
    void onVREvent(const MinVR::VREvent &event) {

    //event.print();
        // Quit if the escape button is pressed
//        if (event.getName() == "KbdEsc_Down") {
//            shutdown();
//        } else if (event.getName() == "FrameStart") {
//          //_oscillator = event.getDataAsFloat("ElapsedSeconds");
//        }

    }

    /// \brief Set the render context.
    ///
    /// The onVRRender methods are the heart of the MinVR rendering
    /// apparatus.  Some render calls are shared among multiple views,
    /// for example a stereo view has two renders, with the same render
    /// context.
    void onVRRenderGraphicsContext(const MinVR::VRGraphicsState &renderState) {

        // Check if this is the first call.  If so, do some initialization. 
        if (renderState.isInitialRenderCall()) {
          _checkContext();
          _initializeScene();
        }
    }

    /// This is the heart of any graphics program, the render function.
    /// It is called each time through the main graphics loop, and
    /// re-draws the scene according to whatever has changed since the
    /// last time it was drawn.
    void onVRRenderGraphics(const MinVR::VRGraphicsState &renderState) {
        // Only draw if the application is still running.
        if (isRunning()) {
            
            // Enable depth testing. Demonstrates OpenGL context being managed by external
            // application i.e. GLUT in this case.
            glEnable(GL_DEPTH_TEST);

            // Buffers being managed by external application i.e. GLUT in this case.
            glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Set background color to black and opaque
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

            glFlush();  // Render now

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);          
            
            
            // Second the load() step.  We let MinVR give us the projection
            // matrix from the render state argument to this method.
            const float* pm = renderState.getProjectionMatrix();
            glm::mat4 projMatrix = glm::mat4( pm[0],  pm[1], pm[2], pm[3],
                                            pm[4],  pm[5], pm[6], pm[7],
                                            pm[8],  pm[9],pm[10],pm[11],
                                            pm[12],pm[13],pm[14],pm[15]);

            // The draw step.  We let MinVR give us the view matrix.
            const float* vm = renderState.getViewMatrix();
            glm::mat4 viewMatrix = glm::mat4( vm[0],  vm[1], vm[2], vm[3],
                                            vm[4],  vm[5], vm[6], vm[7],
                                            vm[8],  vm[9],vm[10],vm[11],
                                            vm[12],vm[13],vm[14],vm[15]);


            camera = (vtkCustomExternalOpenGLCamera *)ren->GetActiveCamera();

            double view[16];
            for(int i = 0; i < 16; i++) {
                view[i] = glm::value_ptr(viewMatrix)[i];
            }

            camera->SetViewTransformMatrix(view);

            double proj[16];
            for(int i = 0; i < 16; i++) {
                proj[i] = glm::value_ptr(projMatrix)[i];
            }

            camera->SetProjectionTransformMatrix(proj);
            
//            camera->SetPosition(4,2,360);
//            camera->SetFocalPoint(0,0,0); // initial direction
//            camera->SetViewUp(0,1,0); // controls "up" direction for camera
            
            
            externalVTKWidget->GetRenderWindow()->Render();
            // We let MinVR swap the graphics buffers.
            // glutSwapBuffers();
        }
    }
    
    };

    

    // The main function is just a shell of its former self.  Just
    // initializes a MinVR graphics object and runs it.
    int main(int argc, char **argv) {

    // Let's see what arguments the user invoked this program with.
    std::cout << "Invoked with argc=" << argc << " arguments." << std::endl;
    for (int i = 0; i < argc ; i++) {
    std::cout << "argv[" << i << "]: " << std::string(argv[i]) << std::endl;
    }

    // If there weren't enough args, throw an error and explain what the
    // user should have done.
    if (argc < 2) {
    throw std::runtime_error("\nNeed a config file.\nTry 'bin/supernova3 -c ../config/desktop-freeglut.xml'");

    }

    // Is the MINVR_ROOT variable set?  MinVR usually needs this to find
    // some important things.
    if (getenv("MINVR_ROOT") == NULL) {
    std::cout << "***** No MINVR_ROOT -- MinVR might not be found *****" << std::endl 
              << "MinVR is found (at runtime) via the 'MINVR_ROOT' variable."
              << std::endl << "Try 'export MINVR_ROOT=/my/path/to/MinVR'."
              << std::endl;
    }

    // Initialize the app.
    DemoVRVTKApp app(argc, argv);

    // Run it.
    app.run();

    // We never get here.
    return 0;
}