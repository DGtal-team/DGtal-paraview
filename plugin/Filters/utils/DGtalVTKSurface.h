#pragma once

// Defines SH3
#include "ShortcutsDefs.h"

#include "DGtalVTKImage.h"
#include "DGtalVTKAbstractContainer.h"

#include "DGtal/helpers/Parameters.h"


class DGtalVTKSurface : public DGtalVTKAbstractContainer
{
public:
    DGtalVTKSurface(const DGtalVTKImage& im) :
        image(im.GetImage())
    { 
        auto params = SH3::defaultParameters() | SHG3::defaultParameters();
        auto K = SH3::getKSpace(image, params);

        surface    = SH3::makeLightDigitalSurface( image, K, params );
        range      = SH3::getSurfelRange(surface, params);
        embedder   = SH3::getCellEmbedder(K);
        scembedder = SH3::getSCellEmbedder(K);
    
        SetBounds(im);
        nbCells = range.size();
    }

    DGtal::CountedPtr<SH3::BinaryImage> GetImage() const
    {
        return image;
    }

    const SH3::SurfelRange& GetSurfelRange() const
    {
        return range;
    }

public:
    struct Iterator : public DgtalToVtkIterator::IteratorImpl
    {
        using BaseIterator = SH3::SurfelRange::const_iterator;

        Iterator(
            const DGtalVTKSurface* surf, 
            const BaseIterator& it
        ) : it(it), surf(surf)
        { }

        void Increment() override { ++it; }

        void Advance() override { }

        bool Compare(const DgtalToVtkIterator::IteratorImpl* other) const override
        {
            const Iterator* otherIt = dynamic_cast<const Iterator*>(other);
            
            if (!otherIt) return false;
            return (it == otherIt->it);
        }

        void FillCellCoordinates(double* coords) const override
        {
            // auto pt = surf->embedder(*it);
            // #FIXME: Simply multiply by 0.5. Otherwise, it triggers an
            // invalid cell assert... 
            std::array<double, 3> pt = {
                0.5 * it->preCell().coordinates[0], 
                0.5 * it->preCell().coordinates[1], 
                0.5 * it->preCell().coordinates[2]
            };
            coords[0] = pt[0] * surf->cellSize[0];
            coords[1] = pt[1] * surf->cellSize[1]; 
            coords[2] = pt[2] * surf->cellSize[2];
        }
        
        const DGtalVTKSurface* surf;
        BaseIterator it;
    };


    DGtalVTKAbstractContainer::DgtalToVtkIterator begin() const override
    {
        return DgtalToVtkIterator(new Iterator(this, range.begin()));
    }

    DGtalVTKAbstractContainer::DgtalToVtkIterator end() const 
    {
        return DgtalToVtkIterator(new Iterator(this, range.end()));
    }

private:
    DGtal::CountedPtr<SH3::BinaryImage> image; // Hold pointer to avoid early destruction
    DGtal::CountedPtr<SH3::LightDigitalSurface> surface; 
    DGtal::CanonicSCellEmbedder<SH3::KSpace> scembedder;
    DGtal::CanonicCellEmbedder<SH3::KSpace> embedder;
    SH3::SurfelRange range;
};
