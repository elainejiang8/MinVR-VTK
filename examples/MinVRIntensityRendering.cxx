#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageClip.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>

#include <api/MinVR.h>

class ExampleVTKApp: public MinVR::VRApp {
    private:
        // variables that were orginally global go here
//        void _checkContext() {
//    
//            // There is one more graphics library used here, called GLEW.  This
//            // library sorts through the various OpenGL updates and changes and
//            // allows a user to pretend that it's all a consistent and simple
//            // system.  The 'core profile' refers to some modern OpenGL
//            // enhancements that are not so modern as of 2017.  Set this to true
//            // to get those enhancements.
//            glewExperimental = true; // Needed for core profile
//            if (glewInit() != GLEW_OK) {
//              throw std::runtime_error("Failed to initialize GLEW");
//            }
//
//            // Now that we have a graphics context, let's look at what's inside.
//            std::cout << "Hardware check: "
//                      << glGetString(GL_RENDERER)  // e.g. Intel 3000 OpenGL Engine
//                      << " / "
//                      << glGetString(GL_VERSION)    // e.g. 3.2 INTEL-8.0.61
//                      << std::endl;
//
//            if (glewIsSupported("GL_VERSION_2_1")) {
//              std::cout << "Software check: Ready for OpenGL 2.1." << std::endl;
//            } else {
//              throw std::runtime_error("Software check: OpenGL 2.1 not supported.");
//            }
//
//            // This is the background color of the viewport.
//            glClearColor(0.1 , 0.0, 0.4, 1.0);
//
//            // Now we're ready to start issuing OpenGL calls.  Start by enabling
//            // the modes we want.  The DEPTH_TEST is how you get hidden faces.
//            glEnable(GL_DEPTH_TEST);
//
//            if (glIsEnabled(GL_DEPTH_TEST)) {
//              std::cout << "Depth test enabled" << std::endl;
//            } else {
//              std::cout << "No depth test enabled" << std::endl;
//            }
//
//            // This is just a performance enhancement that allows OpenGL to
//            // ignore faces that are facing away from the camera.
//            glEnable(GL_CULL_FACE);
//
//        }
    
    public:
      ExampleVTKApp(int argc, char** argv):
        MinVR::VRApp(argc, argv) {
            // scene
            // shader and lights
            // files
        }
      /// The MinVR apparatus invokes this method whenever there is a new
	  /// event to process.
	  void onVREvent(const MinVR::VREvent &event) {
        // Quit if the escape button is pressed
		if (event.getName() == "KbdEsc_Down") {
			shutdown();
        }
      }
          
      /// The onVRRender methods are the heart of the MinVR rendering
      /// apparatus.  Some render calls are shared among multiple views,
      /// for example a stereo view has two renders, with the same render
      /// context.
      void onVRRenderGraphicsContext(const MinVR::VRGraphicsState &renderState) {

        // Check if this is the first call.  If so, do some initialization. 
        if (renderState.isInitialRenderCall()) {
//          _checkContext();
//          _initializeScene();
//          _scene.prepare();
        }
      }
          
      /// This is the heart of any graphics program, the render function.
      /// It is called each time through the main graphics loop, and
      /// re-draws the scene according to whatever has changed since the
      /// last time it was drawn.
	void onVRRenderGraphics(const MinVR::VRGraphicsState &renderState) {
		// Only draw if the application is still running.
		if (isRunning()) {

      
        }
    }
};


int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    std::cerr << "Required arguments: vtkFile" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1]; //  "/Data/ironProt.vtk";

  // Create the renderers, render window, and interactor
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> iren = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  vtkSmartPointer<vtkRenderer> ren =
    vtkSmartPointer<vtkRenderer>::New();
  renWin->AddRenderer(ren);
   
  // Read the data from a vtk file
  vtkSmartPointer<vtkStructuredPointsReader> reader = 
    vtkSmartPointer<vtkStructuredPointsReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
 
  // Create a transfer function mapping scalar value to opacity
  vtkSmartPointer<vtkPiecewiseFunction> oTFun = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  oTFun->AddSegment(0, 1.0, 256, 0.1);
 
  vtkSmartPointer<vtkColorTransferFunction> cTFun = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  cTFun->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
  cTFun->AddRGBPoint( 255, 1.0, 1.0, 1.0 );
 
  // Need to crop to actually see minimum intensity
  vtkSmartPointer<vtkImageClip> clip = 
    vtkSmartPointer<vtkImageClip>::New();
  clip->SetInputConnection( reader->GetOutputPort() );
  clip->SetOutputWholeExtent(0,66,0,66,30,37);
  clip->ClipDataOn();
 
  vtkSmartPointer<vtkVolumeProperty> property = 
    vtkSmartPointer<vtkVolumeProperty>::New();
  property->SetScalarOpacity(oTFun);
  property->SetColor(cTFun);
  property->SetInterpolationTypeToLinear();
 
  vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper = 
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
  mapper->SetBlendModeToMinimumIntensity();
  mapper->SetInputConnection( clip->GetOutputPort() );
 
  vtkSmartPointer<vtkVolume> volume = 
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(mapper);
  volume->SetProperty(property);
 
  ren->AddViewProp(volume);
  renWin->Render();
  iren->Start();
    
  // Is the MINVR_ROOT variable set?  MinVR usually needs this to find
  // some important things.
  if (getenv("MINVR_ROOT") == NULL) {
    std::cout << "***** No MINVR_ROOT -- MinVR might not be found *****" << std::endl 
              << "MinVR is found (at runtime) via the 'MINVR_ROOT' variable."
              << std::endl << "Try 'export MINVR_ROOT=/my/path/to/MinVR'."
              << std::endl;
  }
  
  // Initialize the app.
  ExampleVTKApp app(argc, argv);

  // Run it.
  app.run();
 
  return EXIT_SUCCESS;
}
