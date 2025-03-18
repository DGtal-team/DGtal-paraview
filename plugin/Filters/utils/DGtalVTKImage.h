#pragma once

#include "DGtalVTKAbstractContainer.h"
#include "ShortcutsDefs.h"


#include <iterator>
#include <memory>
#include <array>

/**
 * @brief A container that holds information necessary to represent both VTK and DGtal voxel grids
 */
struct DGtalVTKImage : public DGtalVTKAbstractContainer
{
public:
	static DGtalVTKImage Empty()
	{
		DGtalVTKImage img;
		return img;
	}

	static DGtalVTKImage CreateFromBounds(
		const double imageBounds[6], 
		const double cellBounds[6], 
		const unsigned int nbCells
	) 
	{
		DGtalVTKImage image;
		image.SetBounds(imageBounds, cellBounds, nbCells);

		image.domain = DGtal::CountedPtr<SH3::Domain>( new SH3::Domain(
			SH3::Point(std::floor(imageBounds[0] / image.cellSize[0]), 
				       std::floor(imageBounds[2] / image.cellSize[1]), 
				       std::floor(imageBounds[4] / image.cellSize[2])), 
			SH3::Point(std::ceil (imageBounds[1] / image.cellSize[0]), 
				       std::ceil (imageBounds[3] / image.cellSize[1]), 
				       std::ceil (imageBounds[5] / image.cellSize[2]))
		));
		image.image = DGtal::CountedPtr<SH3::BinaryImage>(new SH3::BinaryImage(*image.domain));

		return image;
	}

	static DGtalVTKImage CreateFromImage(
		DGtal::CountedPtr<SH3::BinaryImage> im, 
		const double cellSize = 1.0
	)
	{
		DGtalVTKImage image;

		image.domain = DGtal::CountedPtr<SH3::Domain>(new SH3::Domain(im->domain()));
		image.image = im;

		auto l = image.domain->lowerBound();
		auto u = image.domain->upperBound();

		const double imgsBounds[6] = {
			l[0] * cellSize, u[0] * cellSize,
			l[1] * cellSize, u[1] * cellSize,
			l[2] * cellSize, u[2] * cellSize
		};
		const double cellBounds[6] = {0, cellSize, 0, cellSize, 0, cellSize};
		unsigned int nbCells = std::count(image.image->begin(), image.image->end(), true);
		image.SetBounds(imgsBounds, cellBounds, nbCells);

		return image;
	}

	operator bool() { return image.isValid() && domain.isValid(); }

	void SetVoxel(const SH3::Point& point)
	{
		image->setValue(point, true);
	}

	void SetVoxel(const double coords[3])
	{
		const SH3::Point p(std::round(coords[0] / cellSize[0]),
			               std::round(coords[1] / cellSize[1]),
				           std::round(coords[2] / cellSize[2]));
		SetVoxel(p);
	}


	DGtal::CountedPtr<SH3::BinaryImage> GetImage() const
	{
		return image;
	}
	

	// Iterating facilities
public:
	struct Iterator : public DgtalToVtkIterator::IteratorImpl
	{
		using BaseIterator = SH3::Domain::ConstIterator;

		Iterator(
			const DGtalVTKImage* im,
			const BaseIterator& it
		) : it(it), image(im)
		{ }

		void Increment() override
		{
			++it;
		}
		
		void Advance() override
		{
			while(it != image->domain->end())
			{
				const unsigned int idx = image->image->linearized(*it);
				
				if (image->image->at(idx)) break;
				else ++it;
			} 
		}

		bool Compare(const DgtalToVtkIterator::IteratorImpl* other) const override
		{
			const Iterator* otherIt = dynamic_cast<const Iterator*>(other);
			
			if (!otherIt) return false;
			return (it == otherIt->it);
		}

		void FillCellCoordinates(double* coords) const override
		{
			coords[0] = (*it)[0] * image->cellSize[0];
			coords[1] = (*it)[1] * image->cellSize[1]; 
			coords[2] = (*it)[2] * image->cellSize[2];
		}
		
		const DGtalVTKImage* image;
		BaseIterator it;
	};


	DGtalVTKAbstractContainer::DgtalToVtkIterator begin() const override
	{
		return DgtalToVtkIterator(new Iterator(this, domain->begin()));
	}

	DGtalVTKAbstractContainer::DgtalToVtkIterator end() const 
	{
		return DgtalToVtkIterator(new Iterator(this, domain->end()));
	}
private:
	DGtal::CountedPtr<SH3::Domain> domain;
	DGtal::CountedPtr<SH3::BinaryImage> image;
};

/*



		

		const double* operator*()
		{
			currentCell[0] = (*it)[0] * image->cellSize[0];
			currentCell[1] = (*it)[1] * image->cellSize[1]; 
			currentCell[2] = (*it)[2] * image->cellSize[2];

			return currentCell;
		}

*/