#include <vtkObjectFactory.h>

#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "DGtalDigitalSurfaceToMesh.h"

#include "../utils/DGtalVTKSurface.h"
#include "../utils/VTKToDGtalVTKImage.h"
#include "../utils/DGtalMeshToVTKPolyData.h"


vtkStandardNewMacro(DGtalDigitalSurfaceToMesh);

//----------------------------------------------------------------------------
DGtalDigitalSurfaceToMesh::DGtalDigitalSurfaceToMesh() 
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

int DGtalDigitalSurfaceToMesh::FillOutputPortInformation(int port, vtkInformation* info)
{
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    return 1;
}

//----------------------------------------------------------------------------
DGtalDigitalSurfaceToMesh::~DGtalDigitalSurfaceToMesh() = default;

//----------------------------------------------------------------------------
void DGtalDigitalSurfaceToMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int DGtalDigitalSurfaceToMesh::RequestData(vtkInformation *request,
                                         vtkInformationVector **inputVectors,
                                         vtkInformationVector *outputVector)
{
  auto params = SH3::defaultParameters() | SHG3::defaultParameters();
  
  DGtalVTKImage image = GetImageFromVtkInformation(inputVectors[0]->GetInformationObject(0));
  // DGtalVTKSurface surface(image); 

  auto grayImage = SH3::makeGrayScaleImage(image.GetImage());
  auto trisurf = SH3::makeTriangulatedSurface(grayImage, params);
  
  auto mesh = GetVtkPolyDataFromDGtalTriangularMesh(trisurf);
  outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), mesh);
  return 1;
}
