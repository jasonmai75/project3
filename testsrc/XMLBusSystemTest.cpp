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
    
}


