#include "svg.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <climits> // adds the INT_MAX because testing maximum integer is probabaly good

// Helper structure to capture written SVG strings
struct STestOutput{
    std::vector<std::string> DLines; // list of strings
    bool DDestroyed = false; // A flag that that just closes the output or just serves to be a boolean for when you destroy the output (its likely useful for the double destruction edge cases)
    std::string JoinOutput(){ // Combines all strings into one big string
        std::string Result;

        for (const auto& line : DLines){ 
            Result += line;
        }
        return Result; // combined strings
    }
};

// Callback to capture SVG output (like write function)
svg_return_t write_callback(svg_user_context_ptr user, const char* text){
    if(!user || !text){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    OutPtr->DLines.push_back(text);
    return SVG_OK;
}

// Cleanup callback (just returns OK for testing) (like cleaning up canvas function)
svg_return_t cleanup_callback(svg_user_context_ptr user){
    if(!user){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    if(OutPtr->DDestroyed){
        return SVG_ERR_STATE;
    }
    OutPtr->DDestroyed = true;
    return SVG_OK;
}

// --- TEST FIXTURE ---
class SVGTest : public ::testing::Test{
    protected:
        STestOutput DOutput;
        svg_context_ptr DContext = nullptr;

        void SetUp() override{
            DContext = svg_create(write_callback, cleanup_callback, &DOutput, 100, 100);
            ASSERT_NE(DContext, nullptr);
        }

        void TearDown() override{
            if(DContext){
                svg_destroy(DContext);
                DContext = nullptr;
            }
        }
};

// --- BASIC CREATION TEST ---
TEST_F(SVGTest, CreateAndDestroy){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);
    EXPECT_NE(context,nullptr);
    EXPECT_EQ(svg_destroy(context), SVG_OK);
}

// --- INVALID INPUT TESTS ---
// Tests if the NULL pointer context parameter smoothly runs the error or not
TEST_F(SVGTest, NullContextFunctions){
    // Need to create test values from svg.h structs so this can test against itself
    svg_point_t point = {10, 10};
    svg_size_t size = {20, 20};

    // Use EXPECT_EQ(actual value, expected value) with the functions and null errors
    EXPECT_EQ(svg_circle(nullptr, &point, 5, "fill:red"), SVG_ERR_NULL);
    EXPECT_EQ(svg_rect(nullptr, &point, &size, "fill:red"), SVG_ERR_NULL);
    EXPECT_EQ(svg_line(nullptr, &point, &point, "stroke:red"), SVG_ERR_NULL);
    EXPECT_EQ(svg_group_begin(nullptr, "something"), SVG_ERR_NULL);
    EXPECT_EQ(svg_group_end(nullptr), SVG_ERR_NULL);
    EXPECT_EQ(svg_destroy(nullptr), SVG_ERR_NULL);
}

// --- DRAWING TESTS --- (do they work?)
TEST_F(SVGTest, Circle){
    // This is the test value
    svg_point_t center = {50, 50};
    
    // Uses DContext as the mock context for the test and compares the function to SVG_OK to prove that the function is OK and works.
    EXPECT_EQ(svg_circle(DContext, &center, 45, "fill:none; stroke:green; stroke-width:2"), SVG_OK);

    /* Standard string command utilizes STestOutput in order to take all the strings saved
    like the XML commands so that I am able to look through it with .find since this command puts them all into a big string*/
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the stirng is found or not because "npo" means not found in C++
    // So this is comparing every XML portion of the command with Not Found to check if they arent equal
    EXPECT_NE(output.find("<circle"), std::string::npos);
    EXPECT_NE(output.find("cx=\"50"), std::string::npos);
    EXPECT_NE(output.find("cy=\"50"), std::string::npos);
    EXPECT_NE(output.find("r=\"45"), std::string::npos);
    EXPECT_NE(output.find("fill:none; stroke:green; stroke-width:2"), std::string::npos);
}

TEST_F(SVGTest, CircleWithoutStyle){
    // This is the test value
    svg_point_t center = {50, 50};
    
    // Uses DContext as the mock context for the test and compares the function to SVG_OK to prove that the function is OK and works.
    EXPECT_EQ(svg_circle(DContext, &center, 45, nullptr), SVG_OK);

    /* Standard string command utilizes STestOutput in order to take all the strings saved
    like the XML commands so that I am able to look through it with .find since this command puts them all into a big string*/
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the stirng is found or not because "npo" means not found in C++
    // So this is comparing every XML portion of the command with Not Found to check if they arent equal
    EXPECT_NE(output.find("<circle"), std::string::npos);
    EXPECT_NE(output.find("cx=\"50"), std::string::npos);
    EXPECT_NE(output.find("cy=\"50"), std::string::npos);
    EXPECT_NE(output.find("r=\"45"), std::string::npos);
}

TEST_F(SVGTest, Rectangle){
    // Test values for the topleft and size of rectangle
    svg_point_t topleft = {25, 35};
    svg_size_t size = {40, 60};

    // Uses DContext as the mock context for the test and compares to SVG_OK
    EXPECT_EQ(svg_rect(DContext, &topleft, &size, "fill:none; stroke:blue; stroke-width:2"), SVG_OK);

    // Getting all the strings and adding them up so that I could look through them later
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the portions of the XML string are found or not (Infleunced from circle test)
    EXPECT_NE(output.find("<rect"), std::string::npos);
    EXPECT_NE(output.find("x=\"25"), std::string::npos);
    EXPECT_NE(output.find("y=\"35"), std::string::npos);
    EXPECT_NE(output.find("width=\"40"), std::string::npos);
    EXPECT_NE(output.find("height=\"60"), std::string::npos);
    EXPECT_NE(output.find("fill:none; stroke:blue; stroke-width:2"), std::string::npos);

}

TEST_F(SVGTest, RectangleWithoutStyle){
    // Test values for the topleft and size of rectangle
    svg_point_t topleft = {25, 35};
    svg_size_t size = {40, 60};

    // Uses DContext as the mock context for the test and compares to SVG_OK
    EXPECT_EQ(svg_rect(DContext, &topleft, &size, nullptr), SVG_OK);

    // Getting all the strings and adding them up so that I could look through them later
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the portions of the XML string are found or not (Infleunced from circle test)
    EXPECT_NE(output.find("<rect"), std::string::npos);
    EXPECT_NE(output.find("x=\"25"), std::string::npos);
    EXPECT_NE(output.find("y=\"35"), std::string::npos);
    EXPECT_NE(output.find("width=\"40"), std::string::npos);
    EXPECT_NE(output.find("height=\"60"), std::string::npos);

}

TEST_F(SVGTest, Line){
    // Creating Test values for the start and end points of the line
    svg_point_t start = {20, 30}; 
    svg_point_t end = {50, 60};

    // Uses DContext as the mock context for the test and compares to SVG_OK
    EXPECT_EQ(svg_line(DContext, &start, &end, "fill:none; stroke:red; stroke-width:2"), SVG_OK);

    // Getting all the strings and adding them up so that I could look through them later with .find
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the portions of the XML string are found or not (Influenced from the Rectangle test)
    EXPECT_NE(output.find("<line"), std::string::npos);
    EXPECT_NE(output.find("x1=\"20"), std::string::npos);
    EXPECT_NE(output.find("y1=\"30"), std::string::npos);
    EXPECT_NE(output.find("x2=\"50"), std::string::npos);
    EXPECT_NE(output.find("y2=\"60"), std::string::npos);
    EXPECT_NE(output.find("fill:none; stroke:red; stroke-width:2"), std::string::npos);
    
}

TEST_F(SVGTest, LineWithoutStyle){
    // Creating Test values for the start and end points of the line
    svg_point_t start = {20, 30}; 
    svg_point_t end = {50, 60};

    // Uses DContext as the mock context for the test and compares to SVG_OK
    EXPECT_EQ(svg_line(DContext, &start, &end, nullptr), SVG_OK);

    // Getting all the strings and adding them up so that I could look through them later with .find
    std::string output = DOutput.JoinOutput();

    // Looking through the string to see if the portions of the XML string are found or not (Influenced from the Rectangle test)
    EXPECT_NE(output.find("<line"), std::string::npos);
    EXPECT_NE(output.find("x1=\"20"), std::string::npos);
    EXPECT_NE(output.find("y1=\"30"), std::string::npos);
    EXPECT_NE(output.find("x2=\"50"), std::string::npos);
    EXPECT_NE(output.find("y2=\"60"), std::string::npos);
    
}

// --- GROUPING TEST --- (does grouping work?)
TEST_F(SVGTest, Grouping){
    // No Test Values needed for grouping because they are in string format or like style/attrs format

    // Uses DContext as the mock context for the test and a group modifier like an XML id as attrs in order to compare to SVG_OK
    EXPECT_EQ(svg_group_begin(DContext, "id=\"hello\" transform=\"rotate(45)\""), SVG_OK);
    EXPECT_EQ(svg_group_end(DContext), SVG_OK);

    
    // This places all little strings within a big string that will be searched through by .find
    std::string output = DOutput.JoinOutput();

    // Finding every attribute given to this test and making sure it's all correct and nothing equals to npo
    EXPECT_NE(output.find("<g"), std::string::npos);
    EXPECT_NE(output.find("id=\"hello\""), std::string::npos);
    EXPECT_NE(output.find("transform=\"rotate(45)\""), std::string::npos);
    EXPECT_NE(output.find("</g>"), std::string::npos); 
    
}

    TEST_F(SVGTest, GroupingWithoutAttr){
    // No Test Values needed for grouping because they are in string format or like style/attrs format

    // Uses DContext as the mock context for the test and a group modifier like an XML id as attrs in order to compare to SVG_OK
    EXPECT_EQ(svg_group_begin(DContext, nullptr), SVG_OK);
    EXPECT_EQ(svg_group_end(DContext), SVG_OK);

    
    // This places all little strings within a big string that will be searched through by .find
    std::string output = DOutput.JoinOutput();

    // Finding every attribute given to this test and making sure it's all correct and nothing equals to npo
    EXPECT_NE(output.find("<g"), std::string::npos);
    EXPECT_NE(output.find("</g>"), std::string::npos); 
}



// --- EDGE CASES ---
// I believe we need this because in case you crash the program with 0 values like dividing by zero or something and maybe putting in 0 dimensions was actually the intended format for some reason
// This test allows for 0x0 to be a valid format?? I'm not sure but I think I know how to create it relatively
// Trying to create an empty canvas and empty shapes
TEST_F(SVGTest, ZeroDimensions){
    // Can't use DOutput because it provides a set canvas dimension when I want a 0x0 dimension canvas, so STestOutput lets me create my own
    STestOutput output; 

    // 0 Dimensions
    svg_point_t zeroPoint = {0, 0}; 
    svg_size_t zeroSize = {0, 0};

    // Creating the context parameter and canvas for my svg_create function for when I need to create the 0x0 board
    // requires the writing fn, then a cleanup fn, then a place to put output and the zero dimensions height and width
    svg_context_ptr context = svg_create(write_callback, cleanup_callback, &output, 0, 0);

    // Seeing if context isn't NULL when given a zero dimension and whether the input is actually valid
    EXPECT_NE(context, nullptr);

    // Zero out circle
    svg_point_t center = {50, 50};
    EXPECT_EQ(svg_circle(context, &center, 0, "fill:red"), SVG_OK);

    // Zero Rectangle
    EXPECT_EQ(svg_rect(context, &zeroPoint, &zeroSize, "fill:blue"), SVG_OK);

    // Zero Line
    EXPECT_EQ(svg_line(context, &zeroPoint, &zeroPoint, "stroke:green"), SVG_OK);

    // Verify they all contain the shape
    std::string fullOutput = output.JoinOutput();
    EXPECT_NE(fullOutput.find("<circle"), std::string::npos);
    EXPECT_NE(fullOutput.find("<rect"), std::string::npos);
    EXPECT_NE(fullOutput.find("<line"), std::string::npos);

    // Free the context to prevent memory leaks
    svg_destroy(context);

}

// What happens when NUll is passed into the pointer parameters of the shape functions like center, top_left, size, start, or end
TEST_F(SVGTest, NullPointPointer){
    // Side Note: We don't need to test *context because we already tested it in the NullContextFunctions, we just need to do the 3 shapes since thye contain the other pointer params

    // Create Mock Values 
    // For the circle and rectangle
    svg_size_t size = {50, 60};
    svg_point_t point = {10, 20};

    // For the line
    svg_point_t start = {20, 30};
    svg_point_t end = {60, 70};

    // we expect the NULLs to properly give a NULL error smoothly rather than crash, so we need to EXPECT_EQ shapes with the nullptrs
    // in place of all the pointer parameters like center, size and etc.

    // Circle *center param
    EXPECT_EQ(svg_circle(DContext, nullptr, 10, "fill:red"), SVG_ERR_NULL); // tests *center param
   
    // Rectangle *top_left and *size params
    EXPECT_EQ(svg_rect(DContext, nullptr, &size, "fill:red"), SVG_ERR_NULL); // tests the *top_left param
    EXPECT_EQ(svg_rect(DContext, &point, nullptr, "fill:red"), SVG_ERR_NULL); // tests the *size param

    // Line *start and *end params
    EXPECT_EQ(svg_line(DContext, nullptr, &end, "stroke:red"), SVG_ERR_NULL); // tests the *start param
    EXPECT_EQ(svg_line(DContext, &start, nullptr, "stroke:red"), SVG_ERR_NULL); // tests the *end param
    
}

// Create Edge Cases
TEST_F(SVGTest, CreateEdgeCases){
    // Creating new original output, since I can't use DOutput to test svg_create function which means consequently I can't use DContext either
    STestOutput output;
    svg_context_ptr context = nullptr;

    // I am going to create my tests in the read me
    // This is the NULL write function (it should fail)
    EXPECT_EQ(svg_create(nullptr, cleanup_callback, &output, 100, 100), nullptr);

    // both cleanup and write fn being NULL (should fail)
    EXPECT_EQ(svg_create(nullptr, nullptr, &output, 100, 100), nullptr);

    // NULL user pointer (should fail)
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, nullptr, 100, 100), nullptr);

    // negative width (should fail)
     EXPECT_EQ(svg_create(write_callback, cleanup_callback, &output, -100, 100), nullptr);

    // negative height (should fail)
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &output, 100, -100), nullptr);

    // negative height and width (should fail)
    EXPECT_EQ(svg_create(write_callback, cleanup_callback, &output, -100, -100), nullptr);

    // Need to format successes differently because they will actually create things within the output that needs to be destroyed so that it can be reused

    // Very large dimensions (should succeed)
    context = svg_create(write_callback, cleanup_callback, &output, 999999999, 999999999); 
    EXPECT_NE(context, nullptr);
    if (context != nullptr) {
        svg_destroy(context);
    }


    // Maximum integer dimensions with INT_MAX (should succeed)
    context = svg_create(write_callback, cleanup_callback, &output, INT_MAX, INT_MAX); 
    EXPECT_NE(context, nullptr);
    if (context != nullptr) {
        svg_destroy(context);
    }
    
}

TEST_F(SVGTest, DestroyEdgeCases){
    // Create a place to output
    STestOutput output;
    
    // Tests if SVG Destroy handles destroying an empty NULL value properly and smoothly
    EXPECT_EQ(svg_destroy(nullptr), SVG_ERR_NULL);

    // Creates the context and destroys so that I could simulate a double destroy test
    svg_context_ptr context = svg_create(write_callback, cleanup_callback, &output, 100, 100);

    // Test to see if DDestroyed from output2 is still false like it should be before destruction
    EXPECT_FALSE(output.DDestroyed);

    // Destroy it so that the stauts of output2 should change
    svg_destroy(context);

    // Now test to see if DDestroyed from output2 has changed from False to True like it's supposed to
    EXPECT_TRUE(output.DDestroyed);

    // Test double cleanup
    STestOutput output2; // Creates another output since the last one can't be used since it's been destroyed
    output2.DDestroyed = true;  // Simulate already destroyed by manually setting it to true
    svg_context_ptr context2 = svg_create(write_callback, cleanup_callback, &output2, 100, 100); // creating a context for it
    EXPECT_EQ(svg_destroy(context2), SVG_ERR_STATE); // destroying it to see if it reaches "SVG_ERR_STATE"

}

// Callback to capture SVG output
/*
It takes an int* as the user context (not a STestOutput*)
It has a failure counter
If the counter is > 0, it decrements and returns SVG_OK (success)
If the counter is 0, it returns SVG_ERR_IO (failure)
*/
svg_return_t write_error_callback(svg_user_context_ptr user, const char* text){
    int *FailureCount = (int *)user;
    if(*FailureCount){
        (*FailureCount)--;
        return SVG_OK;    
    }
    return SVG_ERR_IO;
}

// --- IO Errors ---
// Testing what happens when writing to the file fails because of possible mishaps like disk being full or file permissions change or something like that
// Basically, when the file can't be written inside of
TEST_F(SVGTest, IOErrorTest){
// Need to somehow integrate write_error_callback for sure

// Requires a failure counter
int failureCounter = 0;

// Instead of using the actual write_callback, perhaps I use the write_error_callback instead to simulate and error in file reading and try to match it with NULL
svg_context_ptr context = svg_create(write_error_callback, cleanup_callback, &failureCounter, 100, 100);

// Since the writing failed, svg_create should return NULL so now we can test the product of create (context) to null
EXPECT_EQ(context, nullptr);

// Allows one correct write
failureCounter = 1;

// Creating context
context = svg_create(write_error_callback, cleanup_callback, &failureCounter, 100, 100);

// Since only writing fn succeeded but the second one results in failure, this will not have a closing tab and will fail and we can test the error for that
EXPECT_EQ(context, nullptr);

// Test 2, testing the shapes
failureCounter = 2; // allows for the first two writes to succeed so that create can create a proper context, so I could test the actual drawing function

// Creating the context
context = svg_create(write_error_callback, cleanup_callback, &failureCounter, 100, 100);

// Creatin the circle and making sure to IO failure message is correct for drawing functions (SVG_ERR_IO)
svg_point_t center = {50, 50};
// We are basically purposely failing on the 3rd write, so the drawing function purposely fails and then we can get our error and it shows us whether or not the test is good or not
EXPECT_EQ(svg_circle(context, &center, 50, "fill:red"), SVG_ERR_IO);

// Free the memory to prevent memory leaks
svg_destroy(context);

}