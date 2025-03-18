#include <vtkObjectFactory.h>

#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "vtkComputeCurvature.h"

#include "../utils/DGtalVTKSurface.h"
#include "../utils/VTKToDGtalVTKImage.h"
#include "../utils/DGtalVTKAbstractContainerToVTK.h"


vtkStandardNewMacro(vtkComputeCurvature);

//----------------------------------------------------------------------------
vtkComputeCurvature::vtkComputeCurvature() 
{
  this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
vtkComputeCurvature::~vtkComputeCurvature() = default;

//----------------------------------------------------------------------------
void vtkComputeCurvature::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkComputeCurvature::SetCurvatureType(int type)
{
    curvatureType = static_cast<CurvatureType>(type);
    this->Modified();
}


//----------------------------------------------------------------------------
int vtkComputeCurvature::RequestData(vtkInformation *request,
                                   vtkInformationVector **inputVectors,
                                   vtkInformationVector *outputVector)
{
  auto params = SH3::defaultParameters() | SHG3::defaultParameters();
  
  DGtalVTKImage image = GetImageFromVtkInformation(inputVectors[0]->GetInformationObject(0));
  DGtalVTKSurface surface(image); 

  std::vector<double> curvature;
  switch(curvatureType)
  {
    case CurvatureType::MEAN_CURVATURE:
    {
      curvature = SHG3::getIIMeanCurvatures( surface.GetImage(), surface.GetSurfelRange(), params );
      break;
    }
    case CurvatureType::GAUSSIAN_CURVATURE:
    {
      curvature = SHG3::getIIGaussianCurvatures( surface.GetImage(), surface.GetSurfelRange(), params );
      break;
    }
    default:
    {
      vtkVLog(vtkLogger::VERBOSITY_ERROR, "Unknown curvatyre type");
      break;
    }      
  };

  vtkSmartPointer<vtkUnstructuredGrid> grid = GetVtkDataSetFromAbstractContainer(&surface, &curvature);
  outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), grid);
  return 1;
}