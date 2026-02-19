#include <gtest/gtest.h>
#include "XMLBusSystem.h"
#include "StringDataSource.h"

TEST(XMLBusSystemTest, SimpleTest){
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "</stops>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\">\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);
    CXMLBusSystem BusSystem(BusRouteReader,BusPathReader);

    ASSERT_EQ(BusSystem.StopCount(),2);
    EXPECT_EQ(BusSystem.RouteCount(),0);
    EXPECT_NE(BusSystem.StopByIndex(0),nullptr);
    EXPECT_NE(BusSystem.StopByIndex(1),nullptr);
    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),321);
    EXPECT_EQ(StopObj->Description(),"First");
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),311);
    EXPECT_EQ(StopObj->Description(),"second");

    std::string NewDescription = "Second Second :)";
    EXPECT_EQ(StopObj->Description(NewDescription), NewDescription);
    EXPECT_EQ(StopObj->Description(NewDescription), NewDescription);
}

TEST(XMLBusSystemTest, RouteTest){
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "   <stop id=\"3\" node=\"300\" description=\"Third\"/>\n"
                                                                "</stops>\n"
                                                                "<routes>\n"
                                                                "   <route name=\"A\">"
                                                                "       <routestop stop=\"1\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"3\" />"
                                                                "   </route>\n"
                                                                "   <route name=\"B\">"
                                                                "       <routestop stop=\"3\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"1\" />"
                                                                "   </route>\n"
                                                                "</routes>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\">\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);
    CXMLBusSystem BusSystem(BusRouteReader,BusPathReader);
    
    auto Route = BusSystem.RouteByName("A");
    EXPECT_EQ(Route->StopCount(), 3);
    EXPECT_EQ(Route->Name(), "A");
    auto StopID = Route->GetStopID(0);
    EXPECT_EQ(StopID, 1);

    auto Route2 = BusSystem.RouteByIndex(1);
    EXPECT_EQ(Route2->Name(), "B");


}

TEST(XMLBusSystemTest, PathTest){
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "   <stop id=\"3\" node=\"300\" description=\"Third\"/>\n"
                                                                "</stops>\n"
                                                                "<routes>\n"
                                                                "   <route name=\"A\">"
                                                                "       <routestop stop=\"1\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"3\" />"
                                                                "   </route>\n"
                                                                "   <route name=\"B\">"
                                                                "       <routestop stop=\"3\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"1\" />"
                                                                "   </route>\n"
                                                                "</routes>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\">\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "   <path source=\"345\" destination=\"370\">\n"
                                                                "      <node id=\"345\"/>\n"
                                                                "      <node id=\"356\"/>\n"
                                                                "      <node id=\"367\"/>\n"
                                                                "      <node id=\"370\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);
    CXMLBusSystem BusSystem(BusRouteReader,BusPathReader);
    
    auto Path = BusSystem.PathByStopIDs(321, 311);
    EXPECT_EQ(Path->StartNodeID(), 321);
    EXPECT_EQ(Path->EndNodeID(), 311);
    EXPECT_EQ(Path->NodeCount(),3);
    EXPECT_EQ(Path->GetNodeID(1), 315);
}

TEST(XMLBusSystemTest, ParsingErrorTest){
    //reading nothing 
    auto EmptySource = std::make_shared<CStringDataSource>("");
    auto EmptyReader = std::make_shared< CXMLReader >(EmptySource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\">\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "   <path source=\"345\" destination=\"370\">\n"
                                                                "      <node id=\"345\"/>\n"
                                                                "      <node id=\"356\"/>\n"
                                                                "      <node id=\"367\"/>\n"
                                                                "      <node id=\"370\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);

    
    CXMLBusSystem EmptyBusSystem(EmptyReader,BusPathReader);

    ASSERT_EQ(EmptyBusSystem.StopCount(),0);
    ASSERT_EQ(EmptyBusSystem.RouteCount(),0);
    
    //No stops at all
    auto NoStopSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<routes>\n"
                                                                "   <route name=\"A\">"
                                                                "       <routestop stop=\"1\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"3\" />"
                                                                "   </route>\n"
                                                                "   <route name=\"B\">"
                                                                "       <routestop stop=\"3\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"1\" />"
                                                                "   </route>\n"
                                                                "</routes>\n"
                                                                "</bussystem>");
    auto NoStopReader = std::make_shared< CXMLReader >(NoStopSource);
    CXMLBusSystem NoStopBusSystem(NoStopReader,BusPathReader);

    ASSERT_EQ(NoStopBusSystem.StopCount(),0);
    
    //No end tag for stops
    auto OnlyErrorStopsTagSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                    "<stops>\n"                                                                 
                                                                    "<routes>\n"
                                                                    "   <route name=\"A\">"
                                                                    "       <routestop stop=\"1\" />"
                                                                    "       <routestop stop=\"2\" />"
                                                                    "       <routestop stop=\"3\" />"
                                                                    "   </route>\n"
                                                                    "   <route name=\"B\">"
                                                                    "       <routestop stop=\"3\" />"
                                                                    "       <routestop stop=\"2\" />"
                                                                    "       <routestop stop=\"1\" />");
    auto OnlyErrorStopsTagReader = std::make_shared< CXMLReader >(OnlyErrorStopsTagSource);
    CXMLBusSystem OnlyErrorStopsTagBusSystem(OnlyErrorStopsTagReader,BusPathReader);

    ASSERT_EQ(OnlyErrorStopsTagBusSystem.StopCount(),0);
    
    //Have stop start tag but no end tag />
    auto ErrorEndTagSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\">\n"                                                                 
                                                                );
    auto ErrorEndTagReader = std::make_shared< CXMLReader >(ErrorEndTagSource);
    CXMLBusSystem ErrorEndTagBusSystem(ErrorEndTagReader,BusPathReader);

    ASSERT_EQ(ErrorEndTagBusSystem.StopCount(),1);
    ASSERT_EQ(ErrorEndTagBusSystem.RouteCount(), 0);

    //No path
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "   <stop id=\"3\" node=\"300\" description=\"Third\"/>\n"
                                                                "</stops>\n"
                                                                "<routes>\n"
                                                                "   <route name=\"A\">"
                                                                "       <routestop stop=\"1\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"3\" />"
                                                                "   </route>\n"
                                                                "   <route name=\"B\">"
                                                                "       <routestop stop=\"3\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"1\" />"
                                                                "   </route>\n"
                                                                "</routes>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto NoPathSource = std::make_shared<CStringDataSource>(" :) ");
    auto NoPathReader = std::make_shared< CXMLReader >(NoPathSource);
    CXMLBusSystem NoPathBusSystem(BusRouteReader,NoPathReader);
}

TEST(XMLBusSystemTest, InvalidIDIndexTest){
    auto BusRouteSource = std::make_shared<CStringDataSource>(  "<bussystem>\n"
                                                                "<stops>\n"
                                                                "   <stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                "   <stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "   <stop id=\"3\" node=\"300\" description=\"Third\"/>\n"
                                                                "</stops>\n"
                                                                "<routes>\n"
                                                                "   <route name=\"A\">"
                                                                "       <routestop stop=\"1\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"3\" />"
                                                                "   </route>\n"
                                                                "   <route name=\"B\">"
                                                                "       <routestop stop=\"3\" />"
                                                                "       <routestop stop=\"2\" />"
                                                                "       <routestop stop=\"1\" />"
                                                                "   </route>\n"
                                                                "</routes>\n"
                                                                "</bussystem>");
    auto BusRouteReader = std::make_shared< CXMLReader >(BusRouteSource);
    auto BusPathSource = std::make_shared<CStringDataSource>(  "<paths>\n"
                                                                "   <path source=\"321\" destination=\"311\">\n"
                                                                "      <node id=\"321\"/>\n"
                                                                "      <node id=\"315\"/>\n"
                                                                "      <node id=\"311\"/>\n"
                                                                "   </path>\n"
                                                                "   <path source=\"345\" destination=\"370\">\n"
                                                                "      <node id=\"345\"/>\n"
                                                                "      <node id=\"356\"/>\n"
                                                                "      <node id=\"367\"/>\n"
                                                                "      <node id=\"370\"/>\n"
                                                                "   </path>\n"
                                                                "</paths>");
    auto BusPathReader = std::make_shared< CXMLReader >(BusPathSource);
    CXMLBusSystem BusSystem(BusRouteReader,BusPathReader);
    //Invalid index
    auto Path = BusSystem.PathByStopIDs(321, 311);
    EXPECT_EQ(Path->GetNodeID(123), CStreetMap::InvalidNodeID);
    
    auto Route = BusSystem.RouteByName("B");
    EXPECT_EQ(Route->GetStopID(123), BusSystem.InvalidStopID);

    EXPECT_EQ(BusSystem.StopByIndex(123), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(123), nullptr);
    
    //Invalid ID

    EXPECT_EQ(BusSystem.RouteByName("XYZ"), nullptr);
    EXPECT_EQ(BusSystem.StopByID(789), nullptr);
    EXPECT_EQ(BusSystem.PathByStopIDs(789,456), nullptr);
}