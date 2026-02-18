#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

TEST(OpenStreetMapTest, SimpleTest){
    auto OSMSource = std::make_shared<CStringDataSource>(  "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"1000\">\n"
                                                                "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "  </way>\n"
                                                                "  <way id=\"1234\">\n"
                                                                "    <nd ref=\"3\"/>\n"
		                                                        "    <nd ref=\"4\"/>\n"
                                                                "    <nd ref=\"5\"/>\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                            );
    auto OSMReader = std::make_shared< CXMLReader >(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.NodeCount(), 2);
    auto Node = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(Node,nullptr);
    EXPECT_EQ(Node->ID(),1);
    auto Location = CStreetMap::SLocation{38.5,-121.7};
    EXPECT_EQ(Node->Location(),Location);

    auto Node2 = OpenStreetMap.NodeByID(2);
    ASSERT_NE(Node2,nullptr);
    EXPECT_EQ(Node2->ID(),2);
    auto Location2 = CStreetMap::SLocation{38.5,-121.8};
    EXPECT_EQ(Node2->Location(),Location2);
    
    ASSERT_EQ(OpenStreetMap.WayCount(), 2);
    auto Way = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(Way, nullptr);
    EXPECT_EQ(Way->ID(), 1000);
    EXPECT_EQ(Way->NodeCount(), 2);
    EXPECT_EQ(Way->GetNodeID(0), 1);
    EXPECT_EQ(Way->GetNodeID(1), 2);

    auto Way2 = OpenStreetMap.WayByID(1234);
    ASSERT_NE(Way2, nullptr);
    EXPECT_EQ(Way2->ID(), 1234);
    EXPECT_EQ(Way2->NodeCount(), 3);
    EXPECT_EQ(Way2->GetNodeID(0), 3);
    EXPECT_EQ(Way2->GetNodeID(1), 4);
    EXPECT_EQ(Way2->GetNodeID(2), 5);
}

TEST(OpenStreetMapTest, NodeTest){
    auto OSMSource = std::make_shared<CStringDataSource>(  "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "   <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                            "   <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                            "   <node id=\"5229979989\" lat=\"38.5902119\" lon=\"-121.7650771\"/>\n"
                                                            "   <node id=\"5603430194\" lat=\"38.5631819\" lon=\"-121.6386001\"/>\n"
                                                            "   <node id=\"5603430199\" lat=\"38.5612363\" lon=\"-121.643647\">\n"
                                                            "       <tag k=\"highway\" v=\"turning_circle\"/>\n"
                                                            "   </node>\n"
                                                            "   <node id=\"5603430200\" lat=\"38.5612313\" lon=\"-121.6437999\"/>\n"
                                                            "   <node id=\"5603430201\" lat=\"38.5612887\" lon=\"-121.6442515\"/>\n"
                                                            "</osm>"
                                                            );
    auto OSMReader = std::make_shared< CXMLReader >(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    
    ASSERT_EQ(OpenStreetMap.NodeCount(), 7);
    auto Node = OpenStreetMap.NodeByID(5603430199);
    ASSERT_NE(Node,nullptr);
    EXPECT_EQ(Node->ID(),5603430199);
    EXPECT_EQ(Node->AttributeCount(),1);
    std::string Key = Node->GetAttributeKey(0);
    EXPECT_EQ(Key, "highway");
    EXPECT_EQ(Node->HasAttribute(Key), true);
    EXPECT_EQ(Node->GetAttribute(Key), "turning_circle");
}

TEST(OpenStreetMapTest, WayTest){
    auto OSMSource = std::make_shared<CStringDataSource>(  "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "   <node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                            "   <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                            "   <node id=\"5229979989\" lat=\"38.5902119\" lon=\"-121.7650771\"/>\n"
                                                            "   <node id=\"5603430194\" lat=\"38.5631819\" lon=\"-121.6386001\"/>\n"
                                                            "   <node id=\"5603430199\" lat=\"38.5612363\" lon=\"-121.643647\"/>\n"
                                                            "   <node id=\"5603430200\" lat=\"38.5612313\" lon=\"-121.6437999\"/>\n"
                                                            "   <node id=\"5603430201\" lat=\"38.5612887\" lon=\"-121.6442515\"/>\n"
                                                            "   <way id=\"8700118\">\n"
                                                            "       <nd ref=\"62232638\"/>\n"
                                                            "       <nd ref=\"62232636\"/>\n"
                                                            "       <nd ref=\"62232634\"/>\n"
                                                            "       <nd ref=\"62232633\"/>\n"
                                                            "       <tag k=\"lanes\" v=\"2\"/>\n"
                                                            "       <tag k=\"oneway\" v=\"yes\"/>\n"
                                                            "       <tag k=\"bicycle\" v=\"no\"/>\n"
                                                            "       <tag k=\"highway\" v=\"motorway_link\"/>\n"
                                                            "   </way>\n"
                                                            "   <way id=\"8701218\">\n"
                                                            "       <nd ref=\"62248792\"/>\n"
                                                            "       <nd ref=\"2549420729\"/>\n"
                                                            "       <tag k=\"layer\" v=\"1\"/>\n"
                                                            "       <tag k=\"bridge\" v=\"yes\"/>\n"
                                                            "       <tag k=\"highway\" v=\"residential\"/>\n"
                                                            "   </way>\n"
                                                            "   <way id=\"8706922\">\n"
                                                            "       <nd ref=\"62321892\"/>\n"
                                                            "       <nd ref=\"62225731\"/>\n"
                                                            "       <nd ref=\"591391496\"/>\n"
                                                            "       <tag k=\"highway\" v=\"residential\"/>\n"
                                                            "   </way>\n"
                                                            "</osm>"
                                                            );
    auto OSMReader = std::make_shared< CXMLReader >(OSMSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    
    ASSERT_EQ(OpenStreetMap.WayCount(), 3);
    auto Way = OpenStreetMap.WayByIndex(1);
    ASSERT_NE(Way, nullptr);
    EXPECT_EQ(Way->ID(), 8701218);
    EXPECT_EQ(Way->NodeCount(), 2);
    EXPECT_EQ(Way->GetNodeID(0), 62248792);
    EXPECT_EQ(Way->GetNodeID(1), 2549420729);
}