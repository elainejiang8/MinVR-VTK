#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRectilinearGridReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
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
#include <vtkProjectedTetrahedraMapper.h>
//#include <vtkHAVSVolumeMapper.h>
#include <vtkUnstructuredGridVolumeRayCastMapper.h>
#include <vtkAppendFilter.h>
#include <vtkCell.h>
#include <vtkProperty.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkThreshold.h>
#include <vtkXMLUnstructuredGridWriter.h>


std::string filename;
vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
int opacity = 20;
int NUM_ACTORS = 6;
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
    };
    
    // Read the file.
    vtkSmartPointer<vtkXMLUnstructuredGridReader> vtrReader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    vtrReader->SetFileName("../data/density_ugrid.vtu");
    vtrReader->Update();
    vtkSmartPointer<vtkUnstructuredGrid> ugrid = vtrReader->GetOutput();

//    vtkSmartPointer<vtkDataArray> scalars = grid->GetPointData()->GetArray("dens");
//    double * range = scalars->GetRange();
   // std::cout << range << std::endl;

    // Convert the vtkRectilinearGrid to vtkUnstructuredGrid.
//    vtkSmartPointer<vtkUnstructuredGrid> ugrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
//    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//    grid->GetPoints(points);
//    ugrid->SetPoints(points);
//    ugrid->GetPointData()->SetScalars(scalars);

//    vtkSmartPointer<vtkThreshold> thresh =
//    vtkSmartPointer<vtkThreshold>::New();
//    thresh->ThresholdByUpper(80);
//    thresh->AllScalarsOff();
//    thresh->SetInputConnection(vtrReader->GetOutputPort());

    //ugrid->Allocate(16581374);

//    for (unsigned int i = 0; i < grid->GetNumberOfCells(); i++){
//        vtkCell* cell = grid->GetCell(i);
//        //std::cout << i << std::endl;
//        ugrid->InsertNextCell(cell->GetCellType(), cell->GetPointIds());
//    }

    // Make sure we have only tetrahedra. (may be slow for big data. tip: save the result to a file)
    vtkSmartPointer<vtkDataSetTriangleFilter> trifilter = vtkSmartPointer<vtkDataSetTriangleFilter>::New();
    trifilter->SetInputData(ugrid);
//    
//    // Write file
//    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
//    vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
//    writer->SetFileName(std::string("trifilter_ugrid.vtu").c_str());
//    writer->SetInputData(trifilter->GetOutput());
//    writer->Write();

    // The mapper that renders the volume data.
    vtkSmartPointer<vtkUnstructuredGridVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkUnstructuredGridVolumeRayCastMapper>::New();
    volumeMapper->SetInputConnection(trifilter->GetOutputPort());

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(80.0,  0.0);
    opacityTransferFunction->AddPoint(120.0, 0.2);
    opacityTransferFunction->AddPoint(255.0, 0.2);

    // Create transfer mapping scalar value to color.
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
    vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(80.0,  0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(120.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(160.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(200.0, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 1.0, 1.0);

    // The property describes how the data will look.
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    //volumeProperty->SetScalarOpacityUnitDistance(300);
    //volumeProperty->ShadeOff();

    // Creation of the volume.
    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
//
//        vtkSmartPointer<vtkDataSetMapper> ugridMapper =
//        vtkSmartPointer<vtkDataSetMapper>::New();
//      ugridMapper->SetInputData(ugrid);
//        
//        vtkSmartPointer<vtkActor> ugridActor =
//        vtkSmartPointer<vtkActor>::New();
//      ugridActor->SetMapper(ugridMapper);
//        ugridActor->GetProperty()->SetOpacity(0.5);



    // Usual visualization.
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddVolume(volume);
    //renderer->AddActor(ugridActor);
    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->SetSize(1000, 1000);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);
    //interactor->SetInteractorStyle(style);

    window->Render();
    interactor->Start();


    return 0;

}

    