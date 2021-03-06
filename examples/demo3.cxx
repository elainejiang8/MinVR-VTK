#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <api/MinVR.h>

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
#include <vtkOpenGLCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkCallbackCommand.h>
#include <vtkTransform.h>
#include <vtkPerspectiveTransform.h>
#include <vtkCustomExternalOpenGLCamera.h>

class DemoVRVTKApp: public MinVR::VRApp {
  // Data values that were global in the demo2.cxx file are defined as
  // private members of the VRApp.
private:

    vtkNew<ExternalVTKWidget> externalVTKWidget;
    std::string filename;
    vtkSmartPointer<vtkRenderer> ren; 
    vtkSmartPointer<vtkCustomExternalOpenGLCamera> camera;
    vtkNew<vtkActor> actor;
    vtkNew<vtkVolume> volume;
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;

    vtkNew<vtkTransform> transform;
    int press_x, press_y;
    int release_x, release_y;

  
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
        ren = vtkSmartPointer<vtkRenderer>::New();
        externalVTKWidget->SetRenderWindow(renWin.GetPointer());
        renWin->AddRenderer(ren);

        // read the data from a vtk file
        vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        reader->SetFileName("../data/ironProt.vtk");
        reader->Update();

        // create transfer mapping scalar value to opacity
        vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
        opacityTransferFunction->AddPoint(20, 0.0);
        opacityTransferFunction->AddPoint(255, 0.2);

        // create transfer mapping scalar value to color
        vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
        colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
        colorTransferFunction->AddRGBPoint(64.0, 1.0, 0.0, 0.0);
        colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);
        colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);
        colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);

        // the property describes how the data will look
        vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
        volumeProperty->SetColor(colorTransferFunction);
        volumeProperty->SetScalarOpacity(opacityTransferFunction);
        volumeProperty->ShadeOn();
        volumeProperty->SetInterpolationTypeToLinear();

        // the mapper/ray cast function knows how to render the data
        vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
        volumeMapper->SetInputConnection(reader->GetOutputPort());

        // the volume holds the mapper and the property and can be used to position/orient the volume
        vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
        volume->SetMapper(volumeMapper);
        volume->SetProperty(volumeProperty);
        volume->SetOrigin(0, 0, 0);
        volume->SetPosition(0, 0, 0);

        ren->AddVolume(volume);

        ren->SetBackground(0.87, 0.88, 0.91);
        renWin->SetSize(1000, 1000);
        
        ren->ResetCamera();
    }


    public:
    DemoVRVTKApp(int argc, char** argv) :
    MinVR::VRApp(argc, argv) {

    }

    /// The MinVR apparatus invokes this method whenever there is a new
    /// event to process.
    void onVREvent(const MinVR::VREvent &event) {

        // Quit if the escape button is pressed
//        if (event.getName() == "KbdEsc_Down") {
//            shutdown();
//        } else if (event.getName() == "FrameStart")
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

            //camera->SetViewTransformMatrix(view);

            double proj[16];
            for(int i = 0; i < 16; i++) {
                proj[i] = glm::value_ptr(projMatrix)[i];
            }

            //camera->SetProjectionTransformMatrix(proj);
            
            camera->SetPosition(33.5,33.5,257.686);
            //camera->SetFocalPoint(0,0,0); // initial direction
            camera->SetViewUp(0,1,0); // controls "up" direction for camera
//
//            // transpose - vtk
//            //volume->SetOrientation(0,1,0);
//            volume->RotateX(y_angle);
//            volume->RotateY(x_angle);
//            volume->SetScale(scale_size);
//
//            // transpose - opengl
//            double f[16];
//            volume->GetMatrix(f);
//            glm::mat4 model = glm::make_mat4(f);
//            model = glm::transpose(model);
//            glm::mat4 f1 = projMatrix * viewMatrix * model;
//
//            // transpose
//            double g[16]; 
//            
//            for(int i = 0; i < 16; i++) {
//                g[i] = glm::value_ptr(f1)[i];
//               // std::cout << g[i] << " ";
//            }
//            //std::cout << std::endl;
//            //glm::value_ptr(f1);
////            g[0] = f[0]; g[1] = f[4]; g[2] = f[8]; g[3] = f[12];
////            g[4] = f[1]; g[5] = f[5]; g[6] = f[9]; g[7] = f[13];
////            g[8] = f[2]; g[9] = f[6]; g[10]= f[10];g[11]= f[14];
////            g[12]= f[3]; g[13]= f[7]; g[14]= f[11];g[15]= f[15];
//            
//           // glMultMatrixd(g); // multiply current matrix with specified matrix

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
    throw std::runtime_error("\nNeed two args, including the config file.\nTry 'bin/demo3 -c ../config/desktop-freeglut.xml'");

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
