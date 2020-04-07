#include <iostream>
#include <vector>

#include "MyMap.h"
#include "Provided.h"
#include "Support.h"

inline void SegmentMapperImpl::
    insertOrAppend(const GeoCoord &geoCoord, const StreetSegment &segment)
{
    auto findCoord = geoCoordMap_.find(geoCoord);
    if (findCoord == nullptr)
    {
        geoCoordMap_.associate(geoCoord, std::vector<StreetSegment>{segment});
    }
    else
    {
        findCoord->emplace_back(segment);
    }
}

void SegmentMapperImpl::init(const MapLoader &ml)
{
    auto nStreetSegments = ml.getNumSegments();
    auto toBeInserted = StreetSegment();
    for (int i = 0; i < nStreetSegments; ++i)
    {
        if (ml.getSegment(i, toBeInserted))
        {
            insertOrAppend(toBeInserted.segment.start, toBeInserted);
            insertOrAppend(toBeInserted.segment.end, toBeInserted);
            for (auto attraction : toBeInserted.attractionsOnThisSegment)
            {
                if (attraction.location == toBeInserted.segment.start)
                {
                    continue;
                }
                
                insertOrAppend(attraction.location, toBeInserted);
            }
        }
    }
}

inline std::vector<StreetSegment> SegmentMapperImpl::
    getSegments(const GeoCoord &gc) const
{

    auto segmentsFound = geoCoordMap_.find(gc);
    return segmentsFound == nullptr ? std::vector<StreetSegment>{} : *segmentsFound;
}

// SegmentMapper
SegmentMapper::SegmentMapper()
    : pImpl_(new SegmentMapperImpl) 
{
}

SegmentMapper::~SegmentMapper()
{
    delete pImpl_;
}

void SegmentMapper::init(const MapLoader &ml)
{
    pImpl_->init(ml);
}

std::vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord &gc) const
{
    return pImpl_->getSegments(gc);
}