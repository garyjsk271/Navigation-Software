#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "../BruinNav/Provided.h"
#include "../BruinNav/Support.h"

// static objects which will be loaded with real data.
static MapLoader        static_MapLoader;
static SegmentMapper    static_SegmentMapper;
static AttractionMapper static_AttractionMapper;
static Navigator        static_Navigator;

class MapLoaderTest : public ::testing::Test
{
protected:
    MapLoaderTest()
    {
    }

    ~MapLoaderTest() override
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

protected:
    MapLoader        mapLoader_;
};

class SegmentMapperTest : public ::testing::Test
{
protected:
    SegmentMapperTest()
    {
    }

    ~SegmentMapperTest() override
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    } 

protected:
    SegmentMapper    segmentMapper_;
};

class AttractionMapperTest : public ::testing::Test
{
protected:
    AttractionMapperTest()
    {
    }

    ~AttractionMapperTest() override
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

protected:
    AttractionMapper attractionMapper_;
};

class NavigatorTest : public ::testing::Test
{
protected:
    NavigatorTest()
    {
    }

    ~NavigatorTest() override
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

protected:
    Navigator                   navigator_;
    std::vector<NavSegment>     directions_;
};


TEST_F(MapLoaderTest, load)
{
    // load real data into static object.
    EXPECT_TRUE(static_MapLoader.load("mapdata.txt"));
    // test for loading.
    EXPECT_FALSE(mapLoader_.load(""));
    EXPECT_FALSE(mapLoader_.load(" mapdata.txt "));
    EXPECT_TRUE(mapLoader_.load("mapdata.txt"));
    EXPECT_TRUE(mapLoader_.load("MAPDATA.TXT"));
}

TEST_F(SegmentMapperTest, initAndGetSegment)
{
    // real data initialized here to be used later.
    static_SegmentMapper.init(static_MapLoader);
    // snippet of real data is used to test the SegmentMapper initialization.
    MapLoader     dummyMapLoader;
    SegmentMapper dummySegmentMapper;

    EXPECT_TRUE(dummyMapLoader.load("dummydata.txt"));
    dummySegmentMapper.init(dummyMapLoader);
    auto allStartGeoCoords = std::vector<GeoCoord>{
        GeoCoord("34.0547000", "-118.4794734"),    
        GeoCoord("34.0549825", "-118.4795629"), 
        GeoCoord("34.0552554", "-118.4796392"),    
        GeoCoord("34.0555267", "-118.4796954"),    
        GeoCoord("34.0555351", "-118.4798135"),  
        GeoCoord("34.0620596", "-118.4467237"),
        GeoCoord("34.0613323", "-118.4461140")
    };
    auto allEndGeoCoords = std::vector<GeoCoord>{
        GeoCoord("34.0544590", "-118.4801137"),
        GeoCoord("34.0547778", "-118.4802585"),
        GeoCoord("34.0550808", "-118.4803801"),
        GeoCoord("34.0555356", "-118.4798135"),
        GeoCoord("34.0554131", "-118.4804510"),
        GeoCoord("34.0613323", "-118.4461140"),
        GeoCoord("34.0609137", "-118.4457707")
    };
    auto allStreetNames = std::vector<std::string>{
        "10th Helena Drive",
        "11th Helena Drive",
        "12th Helena Drive",
        "13th Helena Drive",
        "13th Helena Drive",
        "Broxton Avenue",
        "Broxton Avenue"
    };
    auto index = 0;
    for (auto gc : allStartGeoCoords)
    {
        auto allSegments = dummySegmentMapper.getSegments(gc);
        if (index < 6)
        {
            EXPECT_EQ(size(allSegments), 1);
            EXPECT_EQ(allSegments[0].segment.start, gc);
            EXPECT_EQ(allSegments[0].segment.end, allEndGeoCoords[index]);
            EXPECT_EQ(allSegments[0].streetName, allStreetNames[index]);
        }
        else
        {
            EXPECT_EQ(size(allSegments), 2);
            EXPECT_EQ(allSegments[0].streetName, allSegments[1].streetName, "Broxton Avenue");
            if (allSegments[0].segment.start == gc)
            {
                EXPECT_TRUE(allSegments[0].segment.end   == allEndGeoCoords[index]);
                EXPECT_TRUE(allSegments[1].segment.end   == allEndGeoCoords[index - 1]);
                EXPECT_TRUE(allSegments[1].segment.start == allStartGeoCoords[index - 1]);
            }
            if (allSegments[1].segment.start == gc)
            {
                EXPECT_TRUE(allSegments[1].segment.end   == allEndGeoCoords[index]);
                EXPECT_TRUE(allSegments[0].segment.end   == allEndGeoCoords[index - 1]);
                EXPECT_TRUE(allSegments[0].segment.start == allStartGeoCoords[index - 1]);
            }
        }
        ++index;
    }
}

TEST_F(AttractionMapperTest, initAndGetGeoCoord)
{
    static_AttractionMapper.init(static_MapLoader);
    GeoCoord gc;
    EXPECT_EQ(static_AttractionMapper.getGeoCoord("Robertson Playground", gc),
              static_AttractionMapper.getGeoCoord("robertSON playGROUND", gc));
    EXPECT_EQ(gc, GeoCoord("34.0488027", "-118.3847314"));

    EXPECT_TRUE(static_AttractionMapper.getGeoCoord("1061 Broxton Avenue", gc));
    EXPECT_EQ(gc, GeoCoord("34.0613269", "-118.4462765"));

    EXPECT_TRUE(static_AttractionMapper.getGeoCoord("Drake Stadium", gc));
    EXPECT_EQ(gc, GeoCoord("34.0711829", "-118.4492444"));
}

TEST_F(NavigatorTest, loadMapData)
{
    EXPECT_TRUE(static_Navigator.loadMapData("mapdata.txt"));
}

// Test for navigating under these conditions:
// - start at attraction in the middle of a street.
// - end at attraction in the middle of a street 1 segment away.
TEST_F(NavigatorTest, navigate1)
{  
    auto dummyNavSeg = NavSegment();
    dummyNavSeg.initProceed("THIS SHOULD BE CLEARED!", "THIS SHOULD BE CLEARED!", -1.0, GeoSegment());
    directions_.emplace_back(dummyNavSeg);
    EXPECT_EQ(static_Navigator.navigate("1031 Broxton Avenue", "1073 Broxton Avenue", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 2);
}

// Test for navigating under these conditions:
// - start at attraction in the middle of a street.
// - end at attraction in the middle of the same street.
TEST_F(NavigatorTest, navigate2)
{
    EXPECT_EQ(static_Navigator.navigate("1031 Broxton Avenue", "1037 Broxton Avenue", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 1);
}

// Test to make sure that shortest path is evaluated.
TEST_F(NavigatorTest, manySmallStepsIsShorterThanOneLongStep)
{
    navigator_.loadMapData("dummydata1.txt");
    EXPECT_EQ(navigator_.navigate("Attraction B", "Attraction X", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 5);
}

// When a starting location splits off into multiple streets...
//       A
//      /
// C-- start -- end -- D
//      \
//       B
// And the destination is within one of those streets...
// Test that the correct direction (only 1 command) is evaluated.
TEST_F(NavigatorTest, edgeCase1)
{
    navigator_.loadMapData("dummydata1.txt");
    EXPECT_EQ(navigator_.navigate("Attraction A", "Attraction Edge1", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 1);
}
TEST_F(NavigatorTest, edgeCase2)
{
    navigator_.loadMapData("dummydata1.txt");
    EXPECT_EQ(navigator_.navigate("Attraction A", "Attraction Edge2", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 1);
}

TEST_F(NavigatorTest, turnTests)
{
    navigator_.loadMapData("dummydata1.txt");
    EXPECT_EQ(navigator_.navigate("Attraction Edge1", "Attraction Left", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 4);
    EXPECT_EQ(directions_[2].getCommandType(), NavSegment::NAV_COMMAND::turn);
}

TEST_F(NavigatorTest, testFromSpecs)
{
    EXPECT_EQ(static_Navigator.navigate("1061 Broxton Avenue", "Headlines", directions_),
              Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(static_Navigator.navigate("1000 Gayley Avenue", "Novel Cafe Westwood", directions_),
        Navigator::NavResult::NAV_SUCCESS);
    EXPECT_EQ(size(directions_), 7);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_EQ(directions_[i].getStreet(), "Broxton Avenue");
    }
    EXPECT_EQ(directions_[5].getCommandType(), NavSegment::NAV_COMMAND::turn);
    for (int i = 6; i < 7; ++i)
    {
        EXPECT_EQ(directions_[i].getStreet(), "Kinross Avenue");
    }
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
