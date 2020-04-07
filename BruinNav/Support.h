#pragma once

#include <stack>
#include <string>
#include <vector>

#include "MyMap.h"
#include "Provided.h"

using locationInfo = std::pair<GeoCoord, std::string>;

struct score
{
    score() {}

    score(double gScore, double hScore)
        : g(gScore), h(hScore)
    {
        f = g + h;
    }

    double f = std::numeric_limits<double>::max();
    double g = std::numeric_limits<double>::max();
    double h;
};

// Implementation defined in MapLoader.cpp
class MapLoaderImpl
{
public:
    MapLoaderImpl() = default;
    ~MapLoaderImpl() = default;
    bool load(std::string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment &seg) const;

private:
    int                             nStreets_ = 0;
    std::vector<StreetSegment>      allStreets_ = {};
};

// Implementation defined in SegmentMapper.cpp
class SegmentMapperImpl
{
public:
    SegmentMapperImpl()  = default;
    ~SegmentMapperImpl() = default;

public:
    inline void insertOrAppend(const GeoCoord &geoCoord, const StreetSegment &segment);

    void init(const MapLoader &ml);

    inline std::vector<StreetSegment> getSegments(const GeoCoord &gc) const;

private:
    MyMap<GeoCoord, std::vector<StreetSegment>> geoCoordMap_;
};

// Implementation defined in AttractionMapper.cpp
class AttractionMapperImpl
{
public:
    AttractionMapperImpl()  = default;
    ~AttractionMapperImpl() = default;
    void init(const MapLoader& ml);
    bool getGeoCoord(std::string attraction, GeoCoord& gc) const;

private:
    MyMap<std::string, GeoCoord> attractionMap_;
};

// Implementation defined in Navigator.cpp
class NavigatorImpl
{
public:
    NavigatorImpl()  = default;
    ~NavigatorImpl() = default;
    bool loadMapData(std::string mapFile);
    Navigator::NavResult navigate(std::string start, std::string end,
                                  std::vector<NavSegment>& directions) const;

private:
    inline bool contains(const MyMap<GeoCoord, bool> &container,
                         const GeoCoord &gc) const;

    inline bool streetContainsLocation(const StreetSegment &seg, 
                                       const GeoCoord &locaton) const;
    
    inline void reconstructPath(const MyMap<locationInfo, locationInfo> &cameFrom,
                                std::stack<StreetSegment> &fullPath,
                                const GeoCoord &src) const;

    void getNavSegments(std::stack<StreetSegment> &fullPath,
                        std::vector<NavSegment> &result, 
                        const MyMap<GeoCoord, score> &scoreMap,
                        const std::string &prevStreetName) const;

private:
    AttractionMapper    attractionMapper_;
    SegmentMapper       segmentMapper_;
};

/**
 *  @param toBeConverted the string to be converted to lowercase
 *  @post  all characters of toBeConverted shall be in lowercase alphabet
 *         (other characters shall be untouched).
 */
void makeLowerCase(std::string &toBeConverted);

/**
 * @param geoCoordInfo the string containing at least one set of latitudes, longitudes.
 * @param currentIndex the position of the string to begin parsing.
 * @param result       the container in which the extracted GeoCoord is to be emplaced.
 */
void extractGeoCoords(const std::string &geoCoordInfo, int currentIndex,
        std::vector<GeoCoord> &result);

/**
 * @param attractionInfo the string containing the name of attraction, latitude, longitude.
 * @param attraction     the Address reference to which the parsed info is to be emplaced.
 */
void extractAttraction(const std::string &attractionInfo, Address &attraction);

std::string getTravelDirection(const GeoSegment &gs);

std::string getTurnDirection(const GeoSegment &gs1, const GeoSegment &gs2);

double distanceEarthMiles(const StreetSegment &street);

// basic comparison operators to check for uniqueness.
bool operator==(const GeoCoord &gc1, const GeoCoord &gc2);

bool operator!=(const GeoCoord &gc1, const GeoCoord &gc2);

bool operator<(const GeoCoord &gc1, const GeoCoord &gc2);

bool operator>(const GeoCoord &gc1, const GeoCoord &gc2);

bool operator==(const StreetSegment &seg1, const StreetSegment &seg2);

bool operator!=(const StreetSegment &seg1, const StreetSegment &seg2);

bool operator<(const StreetSegment &seg1, const StreetSegment &seg2);

bool operator>(const StreetSegment &seg1, const StreetSegment &seg2);