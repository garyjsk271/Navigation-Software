#include <algorithm>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include "Provided.h"
#include "Support.h"

bool MapLoaderImpl::load(std::string mapFile)
{
    std::ifstream filestream;
    filestream.open(mapFile);

    if (filestream.fail())
    {
        filestream.close();
        return false;
    }

    std::string locationName;
    std::string geoCoordInfo;
    std::string nAttractionsAsString;

    while (!filestream.eof())
    {
        std::getline(filestream, locationName);
        std::getline(filestream, geoCoordInfo);
        std::getline(filestream, nAttractionsAsString);

        if (empty(locationName) or empty(geoCoordInfo) or empty(nAttractionsAsString) )
        {
            break;
        }

        auto extractedGeoCoords = std::vector<GeoCoord>{};
        extractGeoCoords(geoCoordInfo, 0, extractedGeoCoords);
        assert(size(extractedGeoCoords) == 2);

        auto nAttractions       = stoi(nAttractionsAsString);
        auto toBeInserted       = StreetSegment();
        toBeInserted.streetName = locationName;
        toBeInserted.segment    = GeoSegment(extractedGeoCoords[0], 
                                             extractedGeoCoords[1]);
           
        toBeInserted.attractionsOnThisSegment.resize(nAttractions);
        for (int i = 0; i < nAttractions; ++i)
        {
            std::string attractionInfo;
            std::getline(filestream, attractionInfo);
            makeLowerCase(attractionInfo);
            extractAttraction(attractionInfo, toBeInserted.attractionsOnThisSegment[i]);
        }

        allStreets_.emplace_back(std::move(toBeInserted));    
    }

    nStreets_ = size(allStreets_);

    filestream.close();
    
    return true;
}

inline size_t MapLoaderImpl::getNumSegments() const
{
    return static_cast<size_t>(nStreets_);
}

inline bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if (segNum < nStreets_)
    {
        seg = allStreets_[segNum];
        return true;
    }
    return false;
}


MapLoader::MapLoader() 
    : pImpl_(new MapLoaderImpl) 
{
}

MapLoader::~MapLoader()
{
    delete pImpl_;
}

bool MapLoader::load(std::string mapFile)
{
    return pImpl_->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return pImpl_->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment &seg) const
{
    return pImpl_->getSegment(segNum, seg);
}
