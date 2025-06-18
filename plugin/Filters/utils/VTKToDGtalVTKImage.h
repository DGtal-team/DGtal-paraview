#pragma once

#include "DGtalVTKImage.h"

#include <vtkCellIterator.h>
#include <vtkGenericCell.h>
#include <vtkDataSet.h>
#include <vtkLogger.h>


DGtalVTKImage GetImageFromVtkDataSet(vtkDataSet* dataset);

inline DGtalVTKImage GetImageFromVtkInformation(vtkInformation* info)
{
	// No information
	if (info == nullptr) 
	{
		vtkVLog(vtkLogger::VERBOSITY_ERROR, "No information to convert from !");
		return DGtalVTKImage::Empty();
	}
	
	// Can not deduce underlying data type
    auto input = vtkDataObject::GetData(info);
	if (!input)
	{
		vtkVLog(vtkLogger::VERBOSITY_ERROR, "Can not get data from input");
		return DGtalVTKImage::Empty();
	}

	if (input->IsA("vtkDataSet")) // Lowest supported model
		return GetImageFromVtkDataSet(vtkDataSet::GetData(info));

	vtkVLog(vtkLogger::VERBOSITY_ERROR, "Unknown type to convert from \"" + std::string(input->GetClassName()) + "\"");
	return DGtalVTKImage::Empty();
}


inline DGtalVTKImage GetImageFromVtkDataSet(vtkDataSet* dataset)
{	
	vtkVLog(vtkLogger::VERBOSITY_INFO, "Getting image from vtkUnstructuredGrid");
	if (!dataset)
	{
		vtkVLog(vtkLogger::VERBOSITY_ERROR, "No grid to convert from !");
		return DGtalVTKImage::Empty();
	}

	vtkVLog(vtkLogger::VERBOSITY_INFO, "Number of cells: " + std::to_string(dataset->GetNumberOfCells()));

	// Get grid information
	double imageBounds[6], cellBounds[6];
	dataset->GetBounds(imageBounds);
	dataset->GetCell(0)->GetBounds(cellBounds); // Assume all cells are equivalents

	auto dgtalImage = DGtalVTKImage::CreateFromBounds(imageBounds, cellBounds, dataset->GetNumberOfCells());
	
	// Use generic model of cell to be compatible with most models
	int subId = 0; /* unused: assume there are no composite cells */
	double interpolationWeights[8] = { 0., 0., 0., 0., 0., 0., 0., 0.}; /* unused */
	const double parametricCoords[3] = {0.5, 0.5, 0.5}; /* center of cell */
	
	// Iterating values
	auto it = dataset->NewCellIterator();
	vtkGenericCell* cell = vtkGenericCell::New();
	
	vtkVLog(vtkLogger::VERBOSITY_INFO, "Filling data");
	size_t i = 0;
	for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
	{
		vtkVLog(vtkLogger::VEBOSITY_INFO, "Cell " + std::to_string(i++));
		double rawCoordinates[3] = {0., 0., 0.};

		if (it->GetCellType() != VTK_VOXEL) continue;
		
		it->GetCell(cell);
		cell->EvaluateLocation(subId, parametricCoords, rawCoordinates, interpolationWeights);
		
		vtkVLog(vtkLogger::VERBOSITY_INFO, "Coordinates: " + std::to_string(rawCoordinates[0]) + ", " + std::to_string(rawCoordinates[1]) + ", " + std::to_string(rawCoordinates[2]));
		dgtalImage.SetVoxel(rawCoordinates);
	}
	vtkVLog(vtkLogger::VERBOSITY_INFO, "Done filling");
	
	cell->Delete();
	return dgtalImage;
}
