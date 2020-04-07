#pragma once

#include <string>
#include <vector>

#define pi 3.14159265358979323846
#define earthRadiusKm 6371.0

class MapLoaderImpl;
class SegmentMapperImpl;
class AttractionMapperImpl;
class NavigatorImpl;

struct GeoCoord
{
    GeoCoord(const std::string &lat = "0", const std::string &lon = "0")
    {
        latitude = atof(lat.c_str());
        longitude = atof(lon.c_str());

        sLatitude = lat;
        sLongitude = lon;
    }

    double      latitude;
    double      longitude;
    std::string sLatitude;
    std::string sLongitude;
};

struct GeoSegment
{
    GeoSegment(const GeoCoord &s, const GeoCoord &e)
        : start(s), end(e)
    {
    }

    GeoSegment()
    {
    }

    GeoCoord start;
    GeoCoord end;
};

struct Address
{
    std::string attraction;
    GeoCoord    location;
};

struct StreetSegment
{
    std::string             streetName;
    GeoSegment              segment;
    std::vector<Address>    attractionsOnThisSegment;
};


inline double angleBetween2Lines(const GeoSegment &line1, const GeoSegment &line2)
{
    /*
    double angle1 = atan2(line1.start.latitude - line1.end.latitude, line1.start.longitude - line1.end.longitude);
    double angle2 = atan2(line2.start.latitude - line2.end.latitude, line2.start.longitude - line2.end.longitude);
    */
    double angle1 = atan2(
        line1.end.latitude - line1.start.latitude,
        line1.end.longitude - line1.start.longitude);
    double angle2 = atan2(
        line2.end.latitude - line2.start.latitude,
        line2.end.longitude - line2.start.longitude);
    double result = (angle2 - angle1) * 180 / 3.14;
    if (result < 0)
    {
        result += 360;
    }
    return result;
}

inline double angleOfLine(const GeoSegment &line1)
{
    double angle = atan2(
        line1.end.latitude - line1.start.latitude,
        line1.end.longitude - line1.start.longitude);
    double result = angle * 180 / 3.14;
    if (result < 0)
    {
        result += 360;
    }
    return result;
}

// This function converts decimal degrees to radians
inline double deg2rad(double deg) { return (deg * pi / 180); }

//  This function converts radians to decimal degrees
inline double rad2deg(double rad) { return (rad * 180 / pi); }

/**
* Returns the distance between two points on the Earth.
* Direct  translation from http://en.wikipedia.org/wiki/Haversine_formula
* @param  lat1d Latitude of the first point in degrees
* @param  lon1d Longitude of the first point in degrees
* @param  lat2d Latitude of the second point in degrees
* @param  lon2d Longitude of the second point in degrees
* @return The distance between the two points in kilometers
*/
inline double distanceEarthKM(const GeoCoord &g1, const GeoCoord &g2)
{
    double lat1r, lon1r, lat2r, lon2r, u, v;
    lat1r = deg2rad(g1.latitude);
    lon1r = deg2rad(g1.longitude);
    lat2r = deg2rad(g2.latitude);
    lon2r = deg2rad(g2.longitude);
    u = sin((lat2r - lat1r) / 2);
    v = sin((lon2r - lon1r) / 2);
    return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

inline double distanceEarthMiles(const GeoCoord &g1, const GeoCoord &g2)
{
    const double milesPerKm = 0.621371;
    return distanceEarthKM(g1, g2) * milesPerKm;
}

// Pointer to Implementation
class MapLoader
{
public:
    MapLoader();
    ~MapLoader();
    bool load(std::string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment &seg) const;

private:
    MapLoaderImpl *pImpl_;
};

// Pointer to Implementation
class SegmentMapper
{
public:
    SegmentMapper();
    ~SegmentMapper();
    void init(const MapLoader &ml);
    std::vector<StreetSegment> getSegments(const GeoCoord &gc) const;

private:
    SegmentMapperImpl *pImpl_;
};

// Pointer to Implementation
class AttractionMapper
{
public:
    AttractionMapper();
    ~AttractionMapper();
    void init(const MapLoader &ml);
    bool getGeoCoord(std::string attraction, GeoCoord &gc) const;

private:
    AttractionMapperImpl *pImpl_;
};

class NavSegment
{
public:

    enum NAV_COMMAND { invalid, turn, proceed };

    NavSegment()
    {
        m_command = invalid;
    }

    void initTurn(const std::string &turnDirection, const std::string &streetName)
    {
        m_command = turn;
        m_streetName = streetName;
        m_direction = turnDirection;
        m_distance = 0;
    }

    void initProceed(const std::string &direction, const std::string &streetName,
        double distance, const GeoSegment &gs)
    {
        m_command = proceed;
        m_direction = direction;
        m_streetName = streetName;
        m_distance = distance;
        m_gs = gs;
    }

    NAV_COMMAND getCommandType() const
    {
        return m_command;
    }

    std::string getDirection() const
    {
        return m_direction;
    }

    std::string getStreet() const
    {
        return m_streetName;
    }

    double getDistance() const
    {
        return m_distance;
    }

    void setDistance(double dist)
    {
        m_distance = dist;
    }

    GeoSegment getSegment() const
    {
        return m_gs;
    }


private:
    NAV_COMMAND		m_command;	    // turn left, turn right, proceed
    std::string		m_streetName;	// Westwood Blvd
    std::string		m_direction;	// "left" for turn or "northeast" for proceed
    double			m_distance;		// 3.2 //KM
    GeoSegment		m_gs;
};

// Pointer to Implementation
class Navigator
{
public:
    enum NavResult
    {
        NAV_SUCCESS,
        NAV_BAD_SOURCE,
        NAV_BAD_DESTINATION,
        NAV_NO_ROUTE
    };
    Navigator();
    ~Navigator();
    bool loadMapData(std::string mapFile);
    NavResult navigate(std::string start, std::string end,
        std::vector<NavSegment>& directions) const;

private:
    NavigatorImpl* pImpl_;
};