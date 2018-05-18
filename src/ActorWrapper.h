#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <iostream>
using namespace std;

class ActorWrapper {
        vtkSmartPointer<vtkActor> actor;
        string file;
        const char * title;
        const char * annotation;
    
    public:
        ActorWrapper(string f, const char * t, const char * a, float color[3]);
        vtkSmartPointer<vtkActor> GetActor();
        string GetFile();
        const char * GetTitle();
        const char * GetAnnotation();
    
};