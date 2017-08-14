#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkCamera.h>

std::string filename;
vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
int opacity = 20;

void draw(int opacity) {
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(opacity, 0.0);
    opacityTransferFunction->AddPoint(255, 0.2);
    
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    renWin->Render();
}

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
                opacity += 20;
                std::cout << "Opacity level: " << opacity << std::endl;
                draw(opacity);
            }
            if(key == "Down") {
                opacity -= 20;
                draw(opacity);
                std::cout << "Opacity level: " << opacity << std::endl;
            }
            
            // forward events
            vtkInteractorStyleTrackballCamera::OnKeyPress();
        }
};
vtkStandardNewMacro(KeyPressInteractorStyle);




int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr <<"Required arguments: vtkFile" << std::endl;
        return EXIT_FAILURE;
    }
    
    filename = argv[1]; // "/Data/ironProt.vtk;
    
     // create the renderers, render window, and interactor
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    renWin->AddRenderer(ren);
    
    vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();
    
    
    /* default interactor: responds to the following events:
     *     keypress j/keypress t - toggle between joystick (position sensitive) and trackball     *                             (motion sensitive) styles.
     *     keypress c/keypress a - toggle between camera and actor (object) modes. In camera     *                             mode, mouse events affect the camera position and focal point. *                             In object mode, mouse events affect the actor that is under   *                             the mouse pointer.
     *     button 1 - rotate camera around its focal point (if camera mode) or rotate actor       *                around its origin (if actor mode)
     *     button 2 - pan the camera or translate the actor (<shift>-button 1)
     *     button 3 - zoom the camera or scale the actor
     */
    
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);
    
    // read the data from a vtk file
    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName(filename.c_str());
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
    
    // a keypress interactor
    vtkSmartPointer<KeyPressInteractorStyle> style = vtkSmartPointer<KeyPressInteractorStyle>::New();
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(ren);
    
    ren->AddVolume(volume);
    ren->SetActiveCamera(camera);
    ren->ResetCamera();
    
//    for(int i = 0; i < 3; i++) {
//        std::cout << ren->GetActiveCamera()->GetPosition()[i] << std::endl;
//    }
    
    ren->SetBackground(0.87, 0.88, 0.91);
    renWin->SetSize(1000, 1000);
    renWin->Render();
    iren->Start();
    
    
    return 0;
    
}