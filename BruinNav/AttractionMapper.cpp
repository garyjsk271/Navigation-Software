#include "MyMap.h"

#include "Provided.h"
#include "Support.h"

void AttractionMapperImpl::init(const MapLoader& ml)
{
    auto nSegments = ml.getNumSegments();
    auto street = StreetSegment();

    for (int i = 0; i < nSegments; ++i)
    {
        if (ml.getSegment(i, street))
        {
            auto attractionsOnSegment = street.attractionsOnThisSegment;
            for (auto address : attractionsOnSegment)
            {
                attractionMap_.associate(address.attraction, address.location);
            }
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(std::string attraction, GeoCoord& gc) const
{
    makeLowerCase(attraction);
    auto geoCoordPtr = attractionMap_.find(attraction);

    if (geoCoordPtr != nullptr)
    {
        gc = *geoCoordPtr;
        return true;
    }

    return false;
}

AttractionMapper::AttractionMapper() 
    : pImpl_(new AttractionMapperImpl) 
{
}

AttractionMapper::~AttractionMapper() 
{
    delete pImpl_; 
}

void AttractionMapper::init(const MapLoader& ml) 
{ 
    pImpl_->init(ml); 
}

bool AttractionMapper::getGeoCoord(std::string attraction, GeoCoord& gc) const
{
    return pImpl_->getGeoCoord(attraction, gc);
}