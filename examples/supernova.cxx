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
#include <vtkCornerAnnotation.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>

using namespace std;

// global variables
vector <vtkSmartPointer<vtkActor>> actors;
vector <const char *> titles;
vector <const char *> annotations;
vtkSmartPointer<vtkRenderer> ren =
    vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkCornerAnnotation> titleAnnotation = 
    vtkSmartPointer<vtkCornerAnnotation>::New();
vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation = 
    vtkSmartPointer<vtkCornerAnnotation>::New();
static int NUM_ACTORS = 7;

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
        vector <vtkSmartPointer<vtkActor>> OtherActors;
        vector <vtkSmartPointer<vtkProperty>> OtherProperties;
};

vtkStandardNewMacro(MouseInteractorHighLightActor);

// display cube axes
void displayAxes() {
    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
    vtkSmartPointer<vtkCubeAxesActor>::New();
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


int main(int argc, char *argv[]) {
    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();
    
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
    
    // Set the custom type to use for interaction.
    vtkSmartPointer<MouseInteractorHighLightActor> style =
    vtkSmartPointer<MouseInteractorHighLightActor>::New();
    style->SetDefaultRenderer(ren);
    
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);
    
    /**********************************************************/
    initializeAnnotations();
    displayAnnotations();
    displayAxes();
    
    ren->SetBackground(0.87, 0.88, 0.91);
    ren->SetActiveCamera(camera);

    ren->ResetCamera();

    renderWindow->AddRenderer(ren);
    renderWindow->SetSize(2000, 2000);
    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}