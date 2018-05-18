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

int main (int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
              << " objfile mtlfile texturepath"
              << std::endl;
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkOBJImporter> importer =
    vtkSmartPointer<vtkOBJImporter>::New();
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

  return EXIT_SUCCESS;
}

//#include <vtkOBJReader.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkSmartPointer.h>
//#include <string>
// 
//int main(int argc, char* argv[])
//{
//  // Parse command line arguments
//  if(argc != 2)
//  {
//    std::cout << "Usage: " << argv[0] << " Filename(.obj)" << std::endl;
//    return EXIT_FAILURE;
//  }
// 
//  std::string filename = argv[1];
//  vtkSmartPointer<vtkOBJReader> reader =
//    vtkSmartPointer<vtkOBJReader>::New();
//  reader->SetFileName(filename.c_str());
//  reader->Update();
// 
//  // Visualize
//  vtkSmartPointer<vtkPolyDataMapper> mapper =
//    vtkSmartPointer<vtkPolyDataMapper>::New();
//  mapper->SetInputConnection(reader->GetOutputPort());
// 
//  vtkSmartPointer<vtkActor> actor =
//    vtkSmartPointer<vtkActor>::New();
//  actor->SetMapper(mapper);
// 
//  vtkSmartPointer<vtkRenderer> renderer =
//    vtkSmartPointer<vtkRenderer>::New();
//  renderer->AddActor(actor);
//  renderer->SetBackground(0, 0, 0); // Background color green
// 
//  vtkSmartPointer<vtkRenderWindow> renderWindow =
//    vtkSmartPointer<vtkRenderWindow>::New();
//  renderWindow->AddRenderer(renderer);
// 
//  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//    vtkSmartPointer<vtkRenderWindowInteractor>::New();
//  renderWindowInteractor->SetRenderWindow(renderWindow);
// 
//  renderWindowInteractor->Start();
// 
//  return EXIT_SUCCESS;
//}