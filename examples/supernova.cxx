#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataReader.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <vtkCornerAnnotation.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>
#include "ActorWrapper.h"

using namespace std;

// global variables
vector <ActorWrapper *> actorWrappers;
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
            OtherActors.push_back(actorWrappers[i]->GetActor());
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
                if(actorWrappers[i]->GetActor() != LastPickedActor) {
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

void createActorWrappers() {
    float sillColor[3] = {0.97,0.45,0.91};
    float jetsColor[3] = {0.6,0.99,0.73};
    float fekColor[3] = {0.49,0.94,0.89};
    float arColor[3] = {0.95,0.95,0.33};
    float siColor[3] = {0.26,0.59,0.94};
    float outerKnotsColor[3] = {0.94,0.32,0.4};
    float sphereColor[3] = {0.96,0.70,0.93};
    
    ActorWrapper * sill = new ActorWrapper("../data/cco-ascii.vtk", "Neutron Star", "At the center of Cas A is a neutron star, a small \nultra-dense star created by the supernova.", sillColor); // sill (purple)
    ActorWrapper * jets = new ActorWrapper("../data/newjets-ascii.vtk", "Fiducial Jets", "In green, two jets of material are seen. \nThese jets funnel material and energy \nduring and after the explosion.", jetsColor); // jets (green)
    ActorWrapper * fek = new ActorWrapper("../data/fekcorr-ascii.vtk", "FeK (Chandra Telescope)", "The light blue portions of this model \nrepresent radiation from the element \niron as seen in X-ray light from Chandra. \nIron is forged in the very core of the \nstar but ends up on the outside \nof the expanding ring of debris.", fekColor); // fek (blue)
    ActorWrapper * ar = new ActorWrapper("../data/newar-ascii.vtk", "ArII (Spitzer Telescope)", "The yellow portions of the model represent \ninfrared data from the Spitzer Space Telescope. \nThis is cooler debris that has yet to \nbe superheated by a passing shock wave", arColor); // arII (yellow)
    ActorWrapper * si = new ActorWrapper("../data/newhetg-ascii.vtk", "Si (Chandra Telescope, HETG)", "The dark blue colored elements of the model \nrepresent the outer blast wave of the \nexplosion as seen in X-rays by Chandra as well \nas optical and infrared light, much of which is silicon.", siColor); // (dark blue)
    ActorWrapper * outerKnots = new ActorWrapper("../data/newopt-ascii.vtk", "Outer Knots", "The red colored elements of the model represent \nthe outer blast wave of the explosion as seen in \nX-rays by Chandra as well as optical and infrared \nlight, much of which is silicon.", outerKnotsColor); // outer knots (red)
    ActorWrapper * sphere = new ActorWrapper("../data/newsi-ascii.vtk", "Reverse Shock Sphere", "The Cas A supernova remnant acts like a \nrelativistic pinball machine by accelerating \nelectrons to enormous energies. This \narea shows where the acceleration is taking \nplace in an expanding shock wave generated \nby the explosion.", sphereColor); // reverse shock sphere (pink)
    
    actorWrappers.push_back(sill);
    actorWrappers.push_back(jets);
    actorWrappers.push_back(fek);
    actorWrappers.push_back(ar);
    actorWrappers.push_back(si);
    actorWrappers.push_back(outerKnots);
    actorWrappers.push_back(sphere);
}

// hard code Kim's annotations
void initializeAnnotations() {
    for(int i = 0; i < NUM_ACTORS; i++) {
        annotations.push_back(actorWrappers[i]->GetAnnotation()); 
        titles.push_back(actorWrappers[i]->GetTitle());
    }
}


int main(int argc, char *argv[]) {
    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkCamera> camera =
    vtkSmartPointer<vtkCamera>::New();

    /**********************************************************/
    createActorWrappers();
    
    std::string files[7] = {actorWrappers[0]->GetFile(), actorWrappers[1]->GetFile(), actorWrappers[2]->GetFile(), actorWrappers[3]->GetFile(), actorWrappers[4]->GetFile(), actorWrappers[5]->GetFile(), actorWrappers[6]->GetFile()};
    
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
        
        vtkSmartPointer<vtkActor> actor = actorWrappers[i]->GetActor();
       // std::cout << "HI" << std::endl;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetInterpolationToFlat();
        actor->GetProperty()->SetOpacity(1);

        ren->AddActor(actor);
    }


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
