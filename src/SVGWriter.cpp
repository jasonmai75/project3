#include "SVGWriter.h"
#include "svg.h"
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief Internal implementation structure for CSVGWriter.
 *
 * This struct wraps the C SVG library, converting C++ types into C types
 * and forwarding calls to the orignal C-based SVG functions.
 * It cannot be used directly outside CSVGWriter.
 */

struct CSVGWriter::SImplementation{
    /**
     * @brief Output sink where SVG data is written.
     */
    std::shared_ptr< CDataSink > DSink;
    /**
     * @brief Pointer to the SVG context from the C library.
     */
    svg_context_ptr DContext;

    /**
     * @brief Convert a list of SVG attributes into a C++ style CSS string.
     *
     * @param style List of key/value attribute pairs.
     * @return A formatted C++ style CSS string
     * @example Attributes can be convert into a C++ style CSS string like "fill:red; stroke:black"
     */
    std::string ConvertAttributes(const TAttributes &style ){
        std::string styleStr;

        // Create combined strings from attribute pairs
        for(const TAttribute &Attr : style){
            auto key = std::get<0>(Attr);
            auto value = std::get<1>(Attr);
            styleStr += key + ":" + value + "; ";
        }
        // remove last space formatting at the end
        if (!styleStr.empty()) {
            styleStr.pop_back(); 
        }
        return styleStr;
    }
    /**
     * @brief Static callback used by the C SVG library to write text output.
     *
     * This function writes characters into the CDataSink one at a time.
     *
     * @param user Pointer to SImplementation instance.
     * @param text Null-terminated string to write.
     * @return SVG_OK on success, SVG_ERR_NULL or SVG_ERR_STATE on failure.
     */
    static svg_return_t WriteFunction(svg_user_context_ptr user, const char *text){
        // Input validation
        if(!user || !text){
            return SVG_ERR_NULL;
        }

        SImplementation *Implementation = (SImplementation *)user;

        // Writing characters to DSink
        while(*text){
            bool result = Implementation->DSink->Put(*text);
            if(!result){
                return SVG_ERR_STATE;
            }
            text++;
        }
        return SVG_OK;   
    }

    /**
     * @brief Construct a new SImplementation object.
     *
     * Initializes the SVG context and input the write callback.
     *
     * @param sink Output sink for SVG data.
     * @param width Width of the SVG canvas.
     * @param height Height of the SVG canvas.
     */
    SImplementation(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){

        DSink = sink;

        // Create context
        DContext = svg_create(WriteFunction,nullptr,this,width,height);
    }
    /**
     * @brief Destroy the SImplementation object.
     *
     * Add the end tag and cleans up the SVG context.
     */
    ~SImplementation(){
        if (DContext) {
            svg_destroy(DContext);
            DContext = nullptr;
        }
    }
    
    //C++ wrapper for SVG shape and line functions in svg.c
    //Generally just convert c++ types into c types for the parameters of the original C function

    /**
     * @brief Draw a circle in the SVG output.
     *
     * @param center Center point of the circle.
     * @param radius Radius of the circle.
     * @param style SVG styling attributes.
     * @return true if successful, false if failed.
     */
    bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
        // Convert C++ point to C struct
        svg_point_t myCenter{center.DX, center.DY};
        std::string styleStr = ConvertAttributes(style);

        // Call the C function
        svg_return_t result = svg_circle(DContext, &myCenter, radius, styleStr.c_str());

        return result == SVG_OK;
    }

    /**
     * @brief Draw a rectangle in the SVG output.
     *
     * @param topleft Top-left corner of the rectangle.
     * @param size Width and height of the rectangle.
     * @param style SVG styling attributes.
     * @return true if successful, false if failed.
     */
    bool Rectangle(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){
        // Convert C++ point to C struct
        svg_point_t myTopLeft{topleft.DX, topleft.DY};
        svg_size_t mySize{size.DWidth, size.DHeight};
        std::string styleStr = ConvertAttributes(style);

        // Call the C function
        svg_return_t result = svg_rect(DContext, &myTopLeft, &mySize, styleStr.c_str());

        return result == SVG_OK;
    }

    /**
     * @brief Draw a line segment in the SVG output.
     *
     * @param start Starting point.
     * @param end Ending point.
     * @param style SVG styling attributes.
     * @return true if successful, false if failed.
     */
    bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){
        // Convert C++ point to C struct
        svg_point_t myStart{start.DX, start.DY};
        svg_point_t myEnd{end.DX, end.DY};
        std::string styleStr = ConvertAttributes(style);

        // Call the C function
        svg_return_t result = svg_line(DContext, &myStart, &myEnd, styleStr.c_str());

        return result == SVG_OK;
    }
    //A simple path made out of single lines
    //Use svg line and connect the previous point with the current point
    /**
     * @brief Draw a simple path made of straight lines.
     *
     * @param points Ordered list of points forming the path.
     * @param style SVG styling attributes.
     * @return true if successful, false if failed.
     */
    bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){
        bool firstPt = true;
        svg_point_t prevPt;

        // Connect consecutive points with lines
        for(const SSVGPoint &singlePoint : points){
            // If it's the first point, skip it and still store it
            if(firstPt){
                svg_point_t myPrevPt{singlePoint.DX,singlePoint.DY};
                prevPt = myPrevPt;
                firstPt = false;
                continue;
            }
            
            // Draw line from previous point to current point
            svg_point_t myCurPt{singlePoint.DX, singlePoint.DY};
            std::string styleStr = ConvertAttributes(style);

            svg_return_t result = svg_line(DContext, &prevPt, &myCurPt, styleStr.c_str());

            if(result != SVG_OK){return false;}

            // Update previous point for next iteration
            prevPt = myCurPt;
        }

        return true;
    }
    /**
     * @brief Begin an SVG group element.
     *
     * @param style Attributes applied to the group.
     * @return true if successful, false if failed.
     */
    bool GroupBegin(const TAttributes &style){
        // Build attribute string in XML format
        std::string styleStr;
        for(const TAttribute &Attr : style){
            auto key = std::get<0>(Attr);
            auto value = std::get<1>(Attr);
            styleStr += key + "=" + "\"" + value + "\"" +" ";
        }
        // remove last space formatting at the end
        if (!styleStr.empty()) {
            styleStr.pop_back(); 
        }

        // C function to open group tag
        svg_return_t result = svg_group_begin(DContext, styleStr.c_str());

        return result == SVG_OK;
    }
    /**
     * @brief End the current SVG group.
     *
     * Add the ending tag
     * 
     * @return true if successful, false if failed.
     */
    bool GroupEnd(){
        // Call C function to close group tag
        svg_return_t result = svg_group_end(DContext);
        
        return result == SVG_OK;
    }

};
//CSVGWriter public interface
//Can be use outside

/**
 * @brief Construct a new CSVGWriter.
 *
 * @param sink Output sink for SVG data.
 * @param width Width of the SVG canvas.
 * @param height Height of the SVG canvas.
 */
CSVGWriter::CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){
    DImplementation = std::make_unique<SImplementation>(sink,width,height);
}

/**
 * @brief Destroy the CSVGWriter object.
 */
CSVGWriter::~CSVGWriter(){

}

/**
 * @brief Draw a circle.
 */
bool CSVGWriter::Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
    return DImplementation->Circle(center, radius,style);
}

/**
 * @brief Draw a rectangle.
 */
bool CSVGWriter::Rectangle(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){
    return DImplementation->Rectangle(topleft, size, style);
}

/**
 * @brief Draw a line segment.
 */
bool CSVGWriter::Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){
    return DImplementation->Line(start, end, style);
}

/**
 * @brief Draw a simple path.
 */
bool CSVGWriter::SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){
    return DImplementation->SimplePath(points, style);
}

/**
 * @brief Begin an SVG group.
 */
bool CSVGWriter::GroupBegin(const TAttributes &attrs){
    return DImplementation->GroupBegin(attrs);
}


/**
 * @brief End an SVG group.
 */
bool CSVGWriter::GroupEnd(){
    return DImplementation->GroupEnd();
}