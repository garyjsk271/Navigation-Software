#pragma inline_recursion(on)

#include <cassert>
#include <functional>
#include <queue>
#include <stack>

#include "MyMap.h"
#include "Provided.h"
#include "Support.h"



bool NavigatorImpl::loadMapData(std::string mapFile)
{
    MapLoader initializer;
    if (!initializer.load(mapFile))
    {
        return false;
    }
    attractionMapper_.init(initializer);
    segmentMapper_.init(initializer);
    return true;
}

// A* Search Implementation
// https://en.wikipedia.org/wiki/A*_search_algorithm
// gScore[current] = gScore[prev] + distance(prev, current);
// hScore[current] = distance(current, goal);
// fScore[current] = gScore[current] + hScore[current];
// Reminder: location.first  == currentGeoCoord, 
//           location.second == currentStreetName
Navigator::NavResult NavigatorImpl::navigate(std::string start, std::string end,
    std::vector<NavSegment> &directions) const
{
    auto gcSrc = GeoCoord();
    auto gcDst = GeoCoord();
    // Invalid inputs
    if (!attractionMapper_.getGeoCoord(start, gcSrc))
    {
        return Navigator::NavResult::NAV_BAD_SOURCE;
    }
    if (!attractionMapper_.getGeoCoord(end, gcDst))
    {
        return Navigator::NavResult::NAV_BAD_DESTINATION;
    }

    MyMap<GeoCoord, score> scoreMap;
    auto sortByMinfScore =
        [&](const locationInfo &loc1, const locationInfo &loc2) {
            if (scoreMap.find(loc1.first) == nullptr)
            {
                scoreMap.associate(loc1.first, score());
            }
            if (scoreMap.find(loc2.first) == nullptr)
            {
                scoreMap.associate(loc2.first, score());
            }
            return  scoreMap.find(loc1.first)->f > scoreMap.find(loc2.first)->f;
        };
    auto priority = std::priority_queue<locationInfo, std::vector<locationInfo>,
                    decltype(sortByMinfScore)>(sortByMinfScore);

    MyMap<locationInfo, locationInfo> cameFrom; // u -> v, cameFrom[v] = u.
    MyMap<GeoCoord, bool> open;                 // locations not yet discarded.
    MyMap<GeoCoord, bool> closed;               // locations discarded.

    // Initialize priority queue.
    auto initialPaths = segmentMapper_.getSegments(gcSrc);
    if (size(initialPaths) == 1 and 
        initialPaths[0].segment.start != gcSrc and
        initialPaths[0].segment.end   != gcSrc)
    {
        auto pathStreetName = initialPaths[0].streetName;
        auto gc1 = initialPaths[0].segment.start;
        auto gc2 = initialPaths[0].segment.end;
        cameFrom.associate({ gc1, pathStreetName }, { gcSrc, pathStreetName });
        cameFrom.associate({ gc2, pathStreetName }, { gcSrc, pathStreetName });
        auto gScore1 = distanceEarthMiles(gcSrc, gc1);
        auto gScore2 = distanceEarthMiles(gcSrc, gc2);
        auto hScore1 = distanceEarthMiles(gc1, gcDst);
        auto hScore2 = distanceEarthMiles(gc2, gcDst);
        scoreMap.associate(gc1, score(gScore1, hScore1));
        scoreMap.associate(gc2, score(gScore1, hScore2));
        priority.emplace(gc1, pathStreetName);
        priority.emplace(gc2, pathStreetName);
        open.associate(gcSrc, false);
        closed.associate(gcSrc, true);
    }
    else
    {
        priority.emplace(gcSrc, "");
    } 

    auto fullPath = std::stack<StreetSegment>{};
    auto finalStreet = StreetSegment();
    while (!priority.empty())
    {
        auto currLocation = priority.top();
        priority.pop();
        
        if (currLocation.first == gcDst)
        {
            finalStreet.segment = GeoSegment(currLocation.first, gcDst);
            finalStreet.streetName = currLocation.second;
            fullPath.emplace(finalStreet);
            reconstructPath(cameFrom, fullPath, gcSrc);
            
            directions.clear();
            auto firstStreet = fullPath.top();
            fullPath.pop();
            auto firstNavSegment = NavSegment();
            firstNavSegment.initProceed(getTravelDirection(firstStreet.segment),
                firstStreet.streetName, distanceEarthMiles(firstStreet), firstStreet.segment);
            directions.emplace_back(firstNavSegment);      
            getNavSegments(fullPath, directions, scoreMap, firstStreet.streetName);

            return Navigator::NavResult::NAV_SUCCESS;
        }

        open.associate(currLocation.first, false);
        closed.associate(currLocation.first, true);
        
        auto connections = segmentMapper_.getSegments(currLocation.first);
        for (auto nextStreet : connections)
        {
            auto nextGeoCoord =
                currLocation.first == nextStreet.segment.start ?
                nextStreet.segment.end : nextStreet.segment.start;
            
            if (contains(closed, nextGeoCoord))
            {
                continue;
            }
                  
            auto curr_gScore   = scoreMap.find(currLocation.first)->g;
            auto distBtwnNodes = distanceEarthMiles(currLocation.first, nextGeoCoord);
            auto next_gScore   = curr_gScore + distBtwnNodes;
              
            if (!contains(open, nextGeoCoord) or
                next_gScore < scoreMap.find(nextGeoCoord)->g)
            {
                open.associate(nextGeoCoord, true);      
                cameFrom.associate({ nextGeoCoord, nextStreet.streetName },
                    currLocation);
                auto next_hScore = distanceEarthMiles(nextGeoCoord, gcDst);
                scoreMap.associate(nextGeoCoord, score(next_gScore, next_hScore));
                priority.emplace(nextGeoCoord, nextStreet.streetName);
            }      
            if (streetContainsLocation(nextStreet, gcDst))
            {
                auto final_gScore =
                    next_gScore + distanceEarthMiles(nextGeoCoord, gcDst);
                if (!contains(open, gcDst) or
                    final_gScore < scoreMap.find(gcDst)->g)
                {
                    open.associate(gcDst, true);
                    cameFrom.associate({ gcDst, nextStreet.streetName },
                        { nextGeoCoord, nextStreet.streetName });
                    scoreMap.associate(gcDst, score(final_gScore, 0));
                    priority.emplace(gcDst, nextStreet.streetName);
                }
            }
        }
    }

    return Navigator::NavResult::NAV_NO_ROUTE;
}

inline bool NavigatorImpl::contains(const MyMap<GeoCoord, bool> &container,
        const GeoCoord &gc) const
{
    
    return container.find(gc) != nullptr and *container.find(gc) == true;
}

inline bool NavigatorImpl::streetContainsLocation(const StreetSegment &seg,
        const GeoCoord &locaton) const
{

    for (auto attraction : seg.attractionsOnThisSegment)
    {
        if (attraction.location == locaton)
        {
            return true;
        }
    }
    return false;
}

inline void NavigatorImpl::reconstructPath(
        const MyMap<locationInfo, locationInfo> &cameFrom,
        std::stack<StreetSegment> &fullPath,
        const GeoCoord &src) const
{
    auto current = fullPath.top();
    if (current.segment.start == src)
    {
        return;
    }
  
    auto prevLoc = *cameFrom.find({ current.segment.start, current.streetName });
    auto toBeInserted       = StreetSegment();
    toBeInserted.segment    = GeoSegment(prevLoc.first, current.segment.start);
    toBeInserted.streetName = prevLoc.second;
    fullPath.emplace(toBeInserted);

    reconstructPath(cameFrom, fullPath, src);
}

void NavigatorImpl::getNavSegments(std::stack<StreetSegment> &fullPath,
                                   std::vector<NavSegment> &result, 
                                   const MyMap<GeoCoord, score> &scoreMap,
                                   const std::string &prevStreetName) const
{

    if (size(fullPath) == 1)
    {
        return;
    }

    auto currStreet     = fullPath.top();
    fullPath.pop();

    auto prevNavSeg      = result.back();  
    auto travelDirection = getTravelDirection(currStreet.segment);

    NavSegment nextNavSeg;
    if (prevStreetName != currStreet.streetName)
    {
        auto turnDirection = getTurnDirection(prevNavSeg.getSegment(), currStreet.segment);
        nextNavSeg.initTurn(turnDirection, currStreet.streetName);
        result.emplace_back(NavSegment(nextNavSeg));
    }

    auto distanceTraveled = distanceEarthMiles(currStreet);

    nextNavSeg.initProceed(travelDirection, currStreet.streetName, 
        distanceTraveled,currStreet.segment);
    
    result.emplace_back(nextNavSeg);
    
    getNavSegments(fullPath, result, scoreMap, currStreet.streetName);
}

Navigator::Navigator() : pImpl_(new NavigatorImpl) {}

Navigator::~Navigator() { delete pImpl_; }

bool Navigator::loadMapData(std::string mapFile)
{
    return pImpl_->loadMapData(mapFile);
}

Navigator::NavResult Navigator::navigate(std::string start, std::string end,
    std::vector<NavSegment> &directions) const
{
    return pImpl_->navigate(start, end, directions);
}
