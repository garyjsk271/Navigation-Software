#include "Provided.h"
#include "Support.h"

void makeLowerCase(std::string &toBeConverted)
{
    for (auto& letter : toBeConverted)
    {
        letter = tolower(letter);
    }
}

void extractGeoCoords(const std::string &geoCoordInfo, int currentIndex,
        std::vector<GeoCoord> &result)
{
    if (currentIndex >= size(geoCoordInfo) or currentIndex == std::string::npos)
    {
        return;
    }

    while (geoCoordInfo[currentIndex] == ',' or geoCoordInfo[currentIndex] == ' ')
    {
        ++currentIndex;
    }

    auto endOfLatitude      = geoCoordInfo.find(',', currentIndex);
    auto sLatitude          = geoCoordInfo.substr(currentIndex, endOfLatitude - currentIndex);
    
    auto startOfLongitude   = endOfLatitude + 1;
    while (geoCoordInfo[startOfLongitude] == ',' or geoCoordInfo[startOfLongitude] == ' ')
    {
        ++startOfLongitude;
    }

    auto endOfLongitude     = geoCoordInfo.find(' ', startOfLongitude);
    auto sLongitude         = geoCoordInfo.substr(startOfLongitude, 
        endOfLongitude - startOfLongitude);

    result.emplace_back(GeoCoord(sLatitude, sLongitude));
    extractGeoCoords(geoCoordInfo, endOfLongitude, result);
}

void extractAttraction(const std::string &attractionInfo, Address &attraction)
{
    auto delimPos = attractionInfo.find('|');
    auto attractionName = attractionInfo.substr(0, delimPos);
    auto parsedGeoCoords = std::vector<GeoCoord>{};
    extractGeoCoords(attractionInfo, delimPos + 1, parsedGeoCoords);
    attraction.Address::attraction = attractionName;
    attraction.location = parsedGeoCoords[0];
}


std::string getTravelDirection(const GeoSegment &gs)
{
    auto travelAngle = angleOfLine(gs);

    auto direction =
        travelAngle <= 22.5  ? "east"      :
        travelAngle <= 67.5  ? "northeast" :
        travelAngle <= 112.5 ? "north"     :
        travelAngle <= 157.5 ? "northwest" :
        travelAngle <= 202.5 ? "west"      :
        travelAngle <= 247.5 ? "southwest" :
        travelAngle <= 292.5 ? "south"     :
        travelAngle <= 337.5 ? "southeast" : "east";

    return direction;
}

std::string getTurnDirection(const GeoSegment &gs1, const GeoSegment &gs2)
{
    return angleBetween2Lines(gs1, gs2) < 180.0 ? "left" : "right";
}

double distanceEarthMiles(const StreetSegment &street)
{
    return distanceEarthMiles(street.segment.start, street.segment.end);
}

bool operator==(const GeoCoord &gc1, const GeoCoord &gc2) 
{
    return gc1.sLatitude  == gc2.sLatitude and
           gc1.sLongitude == gc2.sLongitude;
}

bool operator!=(const GeoCoord &gc1, const GeoCoord &gc2)
{
    return !(gc1 == gc2);
}

bool operator<(const GeoCoord &gc1, const GeoCoord &gc2)
{
    if (gc1.sLatitude == gc2.sLatitude)
    {
        return gc1.sLongitude < gc2.sLongitude;
    }
    return gc1.sLatitude < gc2.sLatitude;
}

bool operator>(const GeoCoord &gc1, const GeoCoord &gc2)
{
    if (gc1.sLatitude == gc2.sLatitude)
    {
        return gc1.sLongitude > gc2.sLongitude;
    }
    return gc1.sLatitude > gc2.sLatitude;
}

bool operator==(const StreetSegment &seg1, const StreetSegment &seg2)
{
    return seg1.segment.start == seg2.segment.start and
           seg1.segment.end == seg2.segment.end   and
           seg1.streetName == seg2.streetName;
}

bool operator!=(const StreetSegment &seg1, const StreetSegment &seg2)
{
    return !(seg1 == seg2);
}

bool operator<(const StreetSegment &seg1, const StreetSegment &seg2)
{
    return seg1.segment.start < seg2.segment.start;
}

bool operator>(const StreetSegment &seg1, const StreetSegment &seg2)
{
    return seg1.segment.start > seg2.segment.start;
}
