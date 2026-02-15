# CSVGWriter::SImplementation
Internal implementation structure for CSVGWriter.

This struct wraps the C SVG library, converting C++ types into C types and forwarding calls to the orignal C-based SVG functions. It cannot be used directly outside CSVGWriter.

## **Private Attributes**

### std::shared_ptr<CDataSink> DSink
Output sink where SVG data is written.

### svg_context_ptr DContext
Pointer to the SVG context from the C library.

## **Private Member Functions**

### std::string ConvertAttributes(const TAttributes &style)
Convert a list of SVG attributes into a C++‑style CSS string.

**Parameters**
- style : List of key/value attribute pairs.

**Returns**
- A formatted string

**Example**
- Attributes can be convert into a C++ string like "fill:red; stroke:black"

### static svg_return_t WriteFunction(svg_user_context_ptr user, const char *text)
Static callback used by the C SVG library to write text output.
This function writes characters into the CDataSink one at a time.

**Parameters**
- user : Pointer to the SImplementation instance.  
- text : Null‑terminated string to write.

**Returns**
- SVG_OK on success  
- SVG_ERR_NULL or SVG_ERR_STATE on failure  

### SImplementation(std::shared_ptr<CDataSink> sink, TSVGPixel width, TSVGPixel height)
Constructor. Initializes the SVG context and input the write callback.

**Parameters**
- sink : Output sink for SVG data  
- width : Width of the SVG canvas  
- height : Height of the SVG canvas  

### ~SImplementation()
Destructor. Cleans up the SVG context and add the end tag.

### bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style)
Draw a circle

**Parameters**
- center : Center point  
- radius : Circle radius  
- style : SVG styling attributes  

**Returns**
- true on success  
- false on failure

### bool Rectangle(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style)
Draw a rectangle

**Parameters**
- topleft : Top‑left corner  
- size : Width and height  
- style : SVG styling attributes  

**Returns**
- true on success  
- false on failure  

### bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style)
Draw a line segment

**Parameters**
- start : Starting point  
- end : Ending point  
- style : SVG styling attributes  

**Returns**
- true on success  
- false on failure  

### bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style)
Draw a simple path made of straight lines

**Parameters**
- points : Ordered list of points  
- style : SVG styling attributes  

**Returns**
- true on success  
- false on failure  

### bool GroupBegin(const TAttributes &style)
Begin an SVG group element

**Parameters**
- style : Attributes applied to the group  

**Returns**
- true on success  
- false on failure  

### bool GroupEnd()
End the current SVG group and add the end tag

**Returns**
- true on success  
- false on failure  

# CSVGWriter
Public interface for writing SVG content

## **Public Member Functions**

### CSVGWriter(std::shared_ptr<CDataSink> sink, TSVGPixel width, TSVGPixel height)
Construct a new CSVGWriter.

**Parameters**
- sink : Output sink  
- width : SVG canvas width  
- height : SVG canvas height  

### ~CSVGWriter()
Destructor.

### bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style)
Draw a circle.

### bool Rectangle(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style)
Draw a rectangle.

### bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style)
Draw a line segment.

### bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style)
Draw a simple path.

### bool GroupBegin(const TAttributes &attrs)
Begin an SVG group.

### bool GroupEnd()
End an SVG group.
