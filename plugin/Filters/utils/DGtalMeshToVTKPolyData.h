#pragma once

#include "ShortcutsDefs.h"

#include <vtkUnstructuredGrid.h>
#include <vtkTypeInt64Array.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPoints.h>
#include <vtkLogger.h>
#include <vtkPolyData.h>

inline vtkSmartPointer<vtkPolyData> GetVtkPolyDataFromDGtalTriangularMesh(const DGtal::CountedPtr<SH3::TriangulatedSurface>& meshptr) 
{
	using Index = typename SH3::TriangulatedSurface::Index;
	const SH3::TriangulatedSurface& mesh = *meshptr;
	size_t numTriangles = mesh.nbFaces();
	size_t numPoints = mesh.nbVertices();

	vtkDoubleArray* vtkPointsArray = vtkDoubleArray::New();
    vtkPointsArray->SetNumberOfComponents(3); 
    
    double* pointsPtr = ((vtkDoubleArray*)vtkPointsArray)->WritePointer(0, numPoints * 3);
	for (Index i = 0; i < numPoints; ++i)
	{
		auto pos = mesh.position(i);
		for (size_t j = 0; j < 3; j++)
			pointsPtr[j + i * 3] = pos[j];
	}

	vtkSmartPointer<vtkIdTypeArray> vtkConnArray = vtkSmartPointer<vtkIdTypeArray>::New();
	vtkConnArray->SetNumberOfComponents(1);
	vtkConnArray->SetNumberOfTuples(3 * numTriangles);

	vtkSmartPointer<vtkTypeInt64Array> vtkOffsetArray = vtkSmartPointer<vtkTypeInt64Array>::New();
    vtkOffsetArray->SetNumberOfComponents(1);
    vtkOffsetArray->SetNumberOfTuples(numTriangles + 1);

	auto vtkConnPtr = vtkConnArray->GetPointer(0);
    auto vtkOffsetPtr = vtkOffsetArray->GetPointer(0);

	for (Index i = 0; i < numTriangles; ++i)
	{
		vtkOffsetPtr[i] = 3 * i;
		auto vertices = mesh.verticesAroundFace(i);
		
		for (size_t j = 0; j < 3; j++)
			vtkConnPtr[j + i * 3] = vertices[j];
	}

	vtkOffsetPtr[numTriangles] = 3 * numTriangles;

	vtkSmartPointer<vtkCellArray> vtkCells = vtkSmartPointer<vtkCellArray>::New();
    vtkCells->SetData(vtkOffsetArray, vtkConnArray);
    vtkCells->Register(nullptr);

    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetData(vtkPointsArray);

	vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkMesh->SetPoints(pts);
    vtkMesh->SetPolys(vtkCells);
    return vtkMesh;
}