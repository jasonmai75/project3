#include <gtest/gtest.h>
#include "SVGWriter.h"
#include "StringDataSink.h"

// Testing basic SVG writer creation and destruction
TEST(SVGWriterTest, CreateDestroyTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
    }
    
    // Check for the XML declaration, opening tag, and closing tag
    EXPECT_NE(Sink->String().find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"), std::string::npos);
    EXPECT_NE(Sink->String().find("<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">"), std::string::npos);
    EXPECT_NE(Sink->String().find("</svg>"), std::string::npos);
}

// Test circle drawing with attributes
TEST(SVGWriterTest, CircleTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint center{50, 50};
        TSVGReal radius = 45;
        TAttributes attrs = {{"fill", "none"},{"stroke", "green"}, {"stroke-width", "2"}};
        Writer.Circle(center, radius, attrs);
    }

    // Verify circle elements exist (center, radius, style attributes)
    EXPECT_NE(Sink->String().find("<circle"), std::string::npos);
    EXPECT_NE(Sink->String().find("cx=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("cy=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("r=\"45"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:green; stroke-width:2"), std::string::npos);
}

// Test circle with no attributes
TEST(SVGWriterTest, CircleEmptyAttributeTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint center{50, 50};
        TSVGReal radius = 45;
        Writer.Circle(center, radius, {});
    }

    // Verify rectangle elements exist (top-left, dimensions, style attributes)
    EXPECT_NE(Sink->String().find("<circle"), std::string::npos);
    EXPECT_NE(Sink->String().find("cx=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("cy=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("r=\"45"), std::string::npos);
}

// Test Rectangle with attributes
TEST(SVGWriterTest, RectangleTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint topleft{25, 35};
        SSVGSize size{40, 60};
        TAttributes attrs = {{"fill", "none"}, {"stroke", "blue"}, {"stroke-width", "2"}};
        Writer.Rectangle(topleft, size, attrs);
    }
    EXPECT_NE(Sink->String().find("<rect"), std::string::npos);
    EXPECT_NE(Sink->String().find("x=\"25"), std::string::npos);
    EXPECT_NE(Sink->String().find("y=\"35"), std::string::npos);
    EXPECT_NE(Sink->String().find("width=\"40"), std::string::npos);
    EXPECT_NE(Sink->String().find("height=\"60"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:blue; stroke-width:2"), std::string::npos);
}

// Test Rectangle without attributes
TEST(SVGWriterTest, RectangleEmptyAtrributeTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint topleft{25, 35};
        SSVGSize size{40, 60};
        Writer.Rectangle(topleft, size, {});
    }
    EXPECT_NE(Sink->String().find("<rect"), std::string::npos);
    EXPECT_NE(Sink->String().find("x=\"25"), std::string::npos);
    EXPECT_NE(Sink->String().find("y=\"35"), std::string::npos);
    EXPECT_NE(Sink->String().find("width=\"40"), std::string::npos);
    EXPECT_NE(Sink->String().find("height=\"60"), std::string::npos);
}

// Test Line with attributes
TEST(SVGWriterTest, LineTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint start{20, 30};
        SSVGPoint end{50, 60};
        TAttributes attrs = {{"fill", "none"}, {"stroke", "red"}, {"stroke-width", "2"}};
        Writer.Line(start, end, attrs);
    }

    // Verify the line elements exist (start coords, end coords, style attributes)
    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"20"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"30"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"60"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:red; stroke-width:2"), std::string::npos);
}

// Test Line without attributes
TEST(SVGWriterTest, LineEmptyAttributeTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint start{20, 30};
        SSVGPoint end{50, 60};
        Writer.Line(start, end, {});
    }
    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"20"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"30"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"50"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"60"), std::string::npos);
}

// Test simple path drawing (connecting multiple points with line segments)
TEST(SVGWriterTest, SimplePathTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        std::vector<SSVGPoint> points{{6,7},{12,34},{22,44},{55,66},{78,90}};
        TAttributes attrs = {{"fill", "none"}, {"stroke", "red"}, {"stroke-width", "2"}};
        Writer.SimplePath(points,attrs);
    }

    // Verifying different line segments {6,7},{12,34},{22,44},{55,66},{78,90}
    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"6"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"7"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"12"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"34"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:red; stroke-width:2"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"12"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"34"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"22"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"44"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:red; stroke-width:2"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"22"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"44"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"55"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"66"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:red; stroke-width:2"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"55"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"66"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"78"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"90"), std::string::npos);
    EXPECT_NE(Sink->String().find("fill:none; stroke:red; stroke-width:2"), std::string::npos);
}

// Test SimplePath without attributes
TEST(SVGWriterTest, SimplePathEmptyAttributeTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        std::vector<SSVGPoint> points{{6,7},{12,34},{22,44},{55,66},{78,90}};
        Writer.SimplePath(points,{});
    }
    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"6"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"7"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"12"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"34"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"12"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"34"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"22"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"44"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"22"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"44"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"55"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"66"), std::string::npos);

    EXPECT_NE(Sink->String().find("<line"), std::string::npos);
    EXPECT_NE(Sink->String().find("x1=\"55"), std::string::npos);
    EXPECT_NE(Sink->String().find("y1=\"66"), std::string::npos);
    EXPECT_NE(Sink->String().find("x2=\"78"), std::string::npos);
    EXPECT_NE(Sink->String().find("y2=\"90"), std::string::npos);
}

// Test SVG group creation with attributes
TEST(SVGWriterTest, GroupTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        TAttributes attrs = {{"id", "hello"}, {"transform", "rotate(45)"}};
        Writer.GroupBegin(attrs);
        Writer.GroupEnd();
    }

    // Verify group elements exist (attributes, closing tag)
    EXPECT_NE(Sink->String().find("<g"), std::string::npos);
    EXPECT_NE(Sink->String().find("id=\"hello\""), std::string::npos);
    EXPECT_NE(Sink->String().find("transform=\"rotate(45)\""), std::string::npos);
    EXPECT_NE(Sink->String().find("</g>"), std::string::npos);
}

// Test SVG group creation without attributes
TEST(SVGWriterTest, GroupEmptyAttributeTest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        Writer.GroupBegin({});
        Writer.GroupEnd();
    }
    EXPECT_NE(Sink->String().find("<g"), std::string::npos);
    EXPECT_NE(Sink->String().find("</g>"), std::string::npos);
}

class CFailingSink : public CDataSink{
    public:
        int DValidCalls = 0;
        virtual ~CFailingSink(){};
        bool Put(const char &ch) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }

        bool Write(const std::vector<char> &buf) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }
};

// Test error handling
TEST(SVGWriterTest, ErrorTests){
    //Check if the write function call back return false once 10 succesful writes are passed
    bool testA, testB;
    std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
    {
        CSVGWriter Writer(Sink,100,50);
        SSVGPoint center{50, 50};
        TSVGReal radius = 45;
        TAttributes attrs = {{"fill", "none"},{"stroke", "green"}, {"stroke-width", "2"}};

        // Allow 10 successful writes, then fail
        Sink->DValidCalls = 10;
        testA = Writer.Circle(center, radius, attrs);
        SSVGPoint start{20, 30};
        SSVGPoint end{50, 60};

        // Fail
        Sink->DValidCalls = 0;
        testB = Writer.Line(start, end, attrs);
    }

    // Both should be false
    EXPECT_EQ(testA, false);
    EXPECT_EQ(testB, false);
}
