#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRectilinearGridReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkOpenGLProjectedTetrahedraMapper.h>
#include <vtkUnstructuredGridVolumeMapper.h>
#include <vtkAppendFilter.h>

#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkExtractGrid.h>
#include <vtkLookupTable.h>
#include <vtkStripper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTubeFilter.h>
#include <vtkNamedColors.h>

std::string filename;
vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
int opacity = 20;
int NUM_ACTORS = 4;
std::vector <vtkSmartPointer<vtkActor>> actors;

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
    if(argc < 1) {
        std::cerr <<"Required arguments: vtkFile" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::vector<std::string> files;
    files.push_back(std::string("../data/dens_0012.vtk"));
    files.push_back(std::string("../data/dens_0030.vtk"));
    files.push_back(std::string("../data/dens_ism_0012.vtk"));
    files.push_back(std::string("../data/dens_ism_0030.vtk"));
    
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
    
    for(int i = 0; i < NUM_ACTORS; i++) {
    
        // read the data from a vtk file
        vtkSmartPointer<vtkRectilinearGridReader> reader = vtkSmartPointer<vtkRectilinearGridReader>::New();
        reader->SetFileName(files[i].c_str());
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
        vtkSmartPointer<vtkDataSetMapper> volumeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        volumeMapper->SetInputConnection(reader->GetOutputPort());

        // the volume holds the mapper and the property and can be used to position/orient the volume
        vtkSmartPointer<vtkActor> volume = vtkSmartPointer<vtkActor>::New();
        volume->SetMapper(volumeMapper);
        //volume->SetProperty(volumeProperty);
        volume->SetOrigin(0, 0, 0);
        volume->SetPosition(0, 0, 0);

        ren->AddVolume(volume);
    }
    
    // a keypress interactor
    vtkSmartPointer<KeyPressInteractorStyle> style = vtkSmartPointer<KeyPressInteractorStyle>::New();
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(ren);
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

// Perform psuedo volume rendering in a structured grid by compositing
// translucent cut planes. This same trick can be used for unstructured
// grids. Note that for better results, more planes can be created. Also,
// if your data is vtkImageData, there are much faster methods for volume
// rendering.

//int main (int argc, char *argv[])
//{
////  if (argc < 1)
////  {
////    std::cout << "Usage: " << argv[0] << " combxyz.bin combq.bin" << std::endl;
////    return EXIT_FAILURE;
////  }
//    
//    std::vector<std::string> files;
//    files.push_back(std::string("../data/dens_0012.vtk"));
//    files.push_back(std::string("../data/dens_0030.vtk"));
//    files.push_back(std::string("../data/dens_ism_0012.vtk"));
//    files.push_back(std::string("../data/dens_ism_0030.vtk"));
//
//    // Read the file.
//    vtkSmartPointer<vtkRectilinearGridReader> vtrReader = vtkSmartPointer<vtkRectilinearGridReader>::New();
//    vtrReader->SetFileName(files[1].c_str());
//    vtrReader->Update();
//    vtkSmartPointer<vtkRectilinearGrid> grid = vtrReader->GetOutput();
//    //vtkSmartPointer<vtkDataArray> scalars = grid->GetPointData()->GetArray("temperatures");
//
//    // Convert the vtkRectilinearGrid to vtkUnstructuredGrid.
//    vtkSmartPointer<vtkStructuredGrid> sgrid = vtkSmartPointer<vtkStructuredGrid>::New();
//    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//    grid->GetPoints(points);
//    sgrid->SetPoints(points);
//    //sgrid->GetPointData()->SetScalars(scalars);
//
////    for (unsigned int i = 0; i < grid->GetNumberOfCells(); i++){
////        vtkCell* cell = grid->GetCell(i);
////        sgrid->InsertNextCell(cell->GetCellType(), cell->GetPointIds());
////    }
//
//    // Make sure we have only tetrahedra. (may be slow for big data. tip: save the result to a file)
//    //vtkSmartPointer<vtkDataSetTriangleFilter> trifilter = vtkSmartPointer<vtkDataSetTriangleFilter>::New();
//    //trifilter->SetInputData(ugrid);
//    
//    vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
//    appendFilter->AddInputData(sgrid);
//    appendFilter->Update();
//
//    // The mapper that renders the volume data.
//    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
//    volumeMapper->SetInputConnection(appendFilter->GetOutputPort());
//
//    // Create transfer mapping scalar value to opacity.
//    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
//    opacityTransferFunction->AddPoint(20, 0.05);
//    opacityTransferFunction->AddPoint(255, 0.5);
//
//    // Create transfer mapping scalar value to color.
//    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
//    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 1.0);
//    colorTransferFunction->AddRGBPoint(64, 1.0, 0.0, 0.0);
//
//    // The property describes how the data will look.
//    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
//    volumeProperty->SetColor(colorTransferFunction);
//    volumeProperty->SetScalarOpacity(opacityTransferFunction);
//    volumeProperty->SetScalarOpacityUnitDistance(300);
//    volumeProperty->ShadeOff();
//
//    // Creation of the volume.
//    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
//    volume->SetMapper(volumeMapper);
//    volume->SetProperty(volumeProperty);
//
//    // Usual visualization.
//    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
//    //renderer->AddVolume(volume);
//    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
//    window->Render();
//    
//    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//    vtkSmartPointer<vtkRenderWindowInteractor> interactor = 
//    
//    vtkRenderWindowInteractor::New();
//    interactor->SetRenderWindow(window);
//    interactor->SetInteractorStyle(style);
//    interactor->Initialize();
//    interactor->Start();
//
//    return EXIT_SUCCESS;
}

    