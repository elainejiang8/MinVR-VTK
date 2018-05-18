#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "ActorWrapper.h"

#include <iostream>
using namespace std;

ActorWrapper::ActorWrapper(string f, const char * t, const char * a, float color[3]) {
    actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);
    
    file = f;
    title = t;
    annotation = a;
}

vtkSmartPointer<vtkActor> ActorWrapper::GetActor() {
    return actor;
}

string ActorWrapper::GetFile() {
    return file;
}

const char * ActorWrapper::GetTitle() {
    return title;
}

const char * ActorWrapper::GetAnnotation() {
    return annotation;
}