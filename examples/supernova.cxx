#include <vtkSmartPointer.h>
#include <vtkVersion.h>
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
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataReader.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmoothPolyDataFilter.h>

#include <vtkMath.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>

// Handle mouse events
class MouseInteractorHighLightActor : public vtkInteractorStyleTrackballCamera
{
public:
  static MouseInteractorHighLightActor* New();
  vtkTypeMacro(MouseInteractorHighLightActor, vtkInteractorStyleTrackballCamera);

  MouseInteractorHighLightActor() {
    LastPickedActor = NULL;
    LastPickedProperty = vtkProperty::New();
  }
  
  virtual ~MouseInteractorHighLightActor() {
    LastPickedProperty->Delete();
  }
  
  virtual void OnLeftButtonDown() VTK_OVERRIDE{
    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkSmartPointer<vtkPropPicker>  picker =
      vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

    // If we picked something before, reset its property
    if (this->LastPickedActor)
    {
      this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);
    }
    this->LastPickedActor = picker->GetActor();
    if (this->LastPickedActor)
    {
      // Save the property of the picked actor so that we can
      // restore it next time
      this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());
      // Highlight the picked actor by changing its properties
      this->LastPickedActor->GetProperty()->SetOpacity(1);
      this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
      this->LastPickedActor->GetProperty()->SetSpecular(0.0);
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }

private:
  vtkActor    *LastPickedActor;
  vtkProperty *LastPickedProperty;
};

vtkStandardNewMacro(MouseInteractorHighLightActor);


int main(int argc, char *argv[])
{
    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();

    vtkSmartPointer<vtkRenderer> ren =
    vtkSmartPointer<vtkRenderer>::New();
    
    /**********************************************************/
    
    // color and opacity transformations here
    
    // create transfer mapping scalar value to color
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(-1.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(-0.5, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(0.5, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(1, 0.0, 0.2, 0.0);

    /**********************************************************/
    
    std::string files[7] = {"../data/newsi-ascii.vtk", "../data/newjets-ascii.vtk", "../data/fekcorr-ascii.vtk", "../data/newar-ascii.vtk", "../data/newhetg-ascii.vtk", "../data/newopt-ascii.vtk", "../data/newsi-ascii.vtk"};
    
    vtkSmartPointer<vtkActor> actors[7];
    
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
        mapper->ScalarVisibilityOff();
        
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
    
    // Set the custom type to use for interaction.
    vtkSmartPointer<MouseInteractorHighLightActor> style =
    vtkSmartPointer<MouseInteractorHighLightActor>::New();
    style->SetDefaultRenderer(ren);
    
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
    //interactor->SetInteractorStyle(style);
    
    /**********************************************************/
    
    ren->SetBackground(0.87, 0.88, 0.91);
    ren->SetActiveCamera(camera);

    ren->ResetCamera();

    renderWindow->AddRenderer(ren);
    renderWindow->SetSize(2000, 2000);
    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}