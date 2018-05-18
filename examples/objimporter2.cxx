#include "GL/glew.h"
#include <GL/freeglut.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <api/MVRDemo.h>
#include <main/VREventHandler.h>
#include <math/VRMath.h>

#include <vtkPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkDataSetAttributes.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <ExternalVTKWidget.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <vtkExternalOpenGLCamera.h>
#include <vtkLight.h>
#include <vtkExternalLight.h>
#include <vtkExternalOpenGLRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkOBJImporter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyData.h>
#include <vtkTexture.h>
#include <vtkNamedColors.h>


using namespace std;

class DemoVRVTKApp: public MVRDemo {
	// Data values that were global in the supernova2.cxx file are defined as
	// private members of the VRApp.
private:
    vtkNew<ExternalVTKWidget> externalVTKWidget;
	vtkSmartPointer<vtkRenderer> ren;
    vtkSmartPointer<vtkExternalOpenGLCamera> camera;
    vector <vtkSmartPointer<vtkActor> > actors;
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;

	int NUM_ACTORS = 7;
  
	// The sensitivity numbers indicate the size of the increment to the carpetPosition and 
	// direction when the joystick is operated (i.e. a joystick event is received). The threshold 
	// indicates a minimum joystick position.  Smaller values will be ignored.
	float joystickAngularSensitivity = 0.03f;
	float joystickLinearSensitivity = 0.1f;
	float joystickThreshold = 0.1f;
	#define M_TWOPI 6.2831852f
	#ifdef WIN32
	#define M_PI 3.1415926
	#endif
	// Keeps track of which controller we are listening to. Could be "L", "R", "1", "2".
	// Use "X" for listening to nobody.
	std::string activeController;
	std::string activeLRController;

	// This is true if we are in the process of selecting something to look at.  i.e. the
	// user has pressed the trigger button on the controller, but hasn't let it go yet.
	bool selectButtonPressed;

	// Use this to display the legends for a little bit and then make them go away.
	int textTimer;


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
      vtkSmartPointer<vtkOBJImporter> importer = vtkSmartPointer<vtkOBJImporter>::New();
      importer->SetFileName(argv[1]);
      importer->SetFileNameMTL(argv[2]);
      //importer->SetTexturePath(argv[3]);

      vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

      vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
      vtkSmartPointer<vtkRenderWindow> renWin =
        vtkSmartPointer<vtkRenderWindow>::New();
      vtkSmartPointer<vtkRenderWindowInteractor> iren =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();

      renWin->AddRenderer(renderer);
      renderer->SetBackground (colors->GetColor3d("Black").GetData());
      renderer->GradientBackgroundOn();

      iren->SetRenderWindow(renWin);
      importer->SetRenderWindow(renWin);
      importer->Update();

      vtkSmartPointer<vtkActorCollection> actors =
        vtkSmartPointer<vtkActorCollection>::New();
      actors = renderer->GetActors();
      actors->InitTraversal();
      std::cout << "There are " << actors->GetNumberOfItems() << " actors" << std::endl;

      for (vtkIdType a = 0; a < actors->GetNumberOfItems(); ++a)
      {
        std::cout << importer->GetOutputDescription(a) << std::endl;

        vtkActor * actor = actors->GetNextActor();

        // OBJImporter turns texture interpolation off
        if (actor->GetTexture())
        {
          actor->GetTexture()->InterpolateOn();
        }

        vtkPolyData *pd = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInput());
        vtkSmartPointer<vtkCleanPolyData> clean =
          vtkSmartPointer<vtkCleanPolyData>::New();
        clean->SetInputData(pd);
        clean->Update();

        vtkSmartPointer<vtkPolyDataNormals> normals =
          vtkSmartPointer<vtkPolyDataNormals>::New();
        normals->SetInputConnection(clean->GetOutputPort());
        normals->SetInputData(pd);
        normals->SplittingOff();
        normals->Update();
        vtkPolyDataMapper *mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
        mapper->SetInputData(normals->GetOutput());
      }
      renWin->Render();
      iren->Start();

    }


    public:
    bool movingSlide;
    bool picked;
    glm::mat4 lastWandPos;
    glm::mat4 slideMat;
    glm::mat4 wandPosRoom, wandPosSpace;
    glm::mat4 headPosRoom;
	glm::mat4 txtPosSpace;
	glm::vec3 txtPos;
	int actorIndex;
	vtkSmartPointer<vtkActor> PickedActor;
    
	// There is an extra coordinate transformation we insert here. The concept is that
	// the room coordinates returned by the Oculus for the head and wand locations is to
	// be considered relative to the center of a magic carpet that we fly around with
	// the wand controls.  The carpet has a position, a *normalized* "up" vector, and an
	// angle defined relative to the z direction.  The carpetTransform() method returns a
	// rotation matrix for the current state of the carpet.
	// Use glm::rotate(carpetDirection, carpetUp.x, carpetUp.y, carpetUp.z) to get a rotation matrix.
	glm::vec3 carpetPosition, carpetUp, carpetScale;
	float carpetDirection; // (expressed in radians)

	float joystickX, joystickY;



	 DemoVRVTKApp(int argc, char** argv):
    
     MVRDemo(argc, argv) {
        movingSlide = false;
		double mm[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		slideMat = glm::mat4( mm[0],  mm[1], mm[2], mm[3],
											mm[4],  mm[5], mm[6], mm[7],
											mm[8],  mm[9],mm[10],mm[11],
											mm[12],mm[13],mm[14],mm[15]);
		lastWandPos = glm::mat4( mm[0],  mm[1], mm[2], mm[3],
											mm[4],  mm[5], mm[6], mm[7],
											mm[8],  mm[9],mm[10],mm[11],
											mm[12],mm[13],mm[14],mm[15]);
    }

	// The MinVR apparatus invokes this method whenever there is a new
	// event to process.
	void onVREvent(const MinVR::VRDataIndex &event) {
		std::string eventName = event.getName();

    
		if (eventName != "FrameStart") {
		//  std::cout << std::string("hearing event:") << eventName << std::endl;
			std::string newLine = "\n";
			std::string d = event.printStructure();
			//OutputDebugString((std::string("IN EVENT METHOD, hearing event: ") + eventName + newLine + d + "\n").c_str());
		}


		// Quit if the escape button is pressed
		if (eventName == "KbdEsc_Down") {
			shutdown();
		}


		if (eventName == "Wand0_Move") {
            const float *w = event.getValue("Transform");
            wandPosRoom = glm::mat4( w[0],  w[1], w[2], w[3],
                         w[4],  w[5], w[6], w[7],
                         w[8],  w[9], w[10], w[11],
                         w[12], w[13], w[14], w[15]);
	
            if (movingSlide) { //when slide moving
			     slideMat = (wandPosRoom / lastWandPos) * slideMat; //update the model matrix for slide
            }
      
		lastWandPos = wandPosRoom;
		} else if(eventName == "Head_Move") {
			const float *h = event.getValue("Transform");
			headPosRoom = glm::mat4(h[0], h[1], h[2], h[3],
			      h[4], h[5], h[6], h[7],
			      h[8], h[9], h[10], h[11],
			      h[12], h[13], h[14], h[15]);
		} else if (eventName == "Wand_Right_Btn_Down") {
			movingSlide = true;
		} else if (eventName == "Wand_Right_Btn_Up") {
			movingSlide = false;
		}
	}

	/// \brief Set the render context.
	///
	/// The onVRRender methods are the heart of the MinVR rendering
	/// apparatus.  Some render calls are shared among multiple views,
	/// for example a stereo view has two renders, with the same render
	/// context.
	void onVRRenderContext(const VRState &renderState) {
    
		// Check if this is the first call.  If so, do some initialization. 
		if ((int)renderState.getValue("InitRender") == 1) {
			_checkContext();
			_initializeScene();
		}

	}
	
    /// This is the heart of any graphics program, the render function.
    /// It is called each time through the main graphics loop, and
    /// re-draws the scene according to whatever has changed since the
    /// last time it was drawn.
    void onVRRenderScene(const VRState &renderState) {
        // Only draw if the application is still running.

		// Enable depth testing. Demonstrates OpenGL context being managed by external
        // application i.e. GLUT in this case.
        glEnable(GL_DEPTH_TEST);

        // Buffers being managed by external application i.e. GLUT in this case.
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Set background color to gray and opaque
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer

        glFlush();  // Render now

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);          
            
            
        // Second the load() step.  We let MinVR give us the projection
        // matrix from the render state argument to this method.
        std::vector<float> pm = renderState.getValue("ProjectionMatrix");
        glm::mat4 projMatrix = glm::mat4( pm[0],  pm[1], pm[2], pm[3],
                                        pm[4],  pm[5], pm[6], pm[7],
                                        pm[8],  pm[9],pm[10],pm[11],
                                        pm[12],pm[13],pm[14],pm[15]);

        // The draw step.  We let MinVR give us the view matrix.
        std::vector<float> vm = renderState.getValue("ViewMatrix");
        glm::mat4 viewMatrix = glm::mat4( vm[0],  vm[1], vm[2], vm[3],
                                        vm[4],  vm[5], vm[6], vm[7],
                                        vm[8],  vm[9],vm[10],vm[11],
                                        vm[12],vm[13],vm[14],vm[15]);

		viewMatrix = viewMatrix * glm::rotate(carpetDirection, carpetUp) * glm::translate(carpetPosition);

        camera = (vtkExternalOpenGLCamera *)ren->GetActiveCamera();

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

        double supernovaModel[16];
        for(int i = 0; i < 16; i++) {
            supernovaModel[i] = glm::value_ptr(slideMat)[i];
        }
            
        vtkSmartPointer<vtkMatrix4x4> sm = vtkSmartPointer<vtkMatrix4x4>::New();
        sm->DeepCopy(supernovaModel);
        sm->Transpose();
        for(int i = 0; i < NUM_ACTORS; i++) {
            actors[i]->SetUserMatrix(sm);
        }

            
        externalVTKWidget->GetRenderWindow()->Render();
        // We let MinVR swap the graphics buffers.
        // glutSwapBuffers();
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

		// Initialize the app
		DemoVRVTKApp app(argc, argv);

		// Run it.
		app.run();

		// We never get here.
		return 0;
	}