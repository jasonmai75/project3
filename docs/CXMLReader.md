
## XMLReader.h classes and functions 

**#ifndef XMLREADER_H**
**#define XMLREADER_H**
- Header Guards prevent multiple inclusions of the same header file

**#include <memory> // For smart pointers**
**#include "XMLEntity.h" // For SXMLEntity**
**#include "DataSource.h" // For CDataSource**

**class CXMLReader{**
- The XML reader class - reads XML and produces entities

**private:
     struct SImplementation;**
- Declaring a struct called "SImplementation, but not defining it yet
- It will be defined in the cpp file
- I think this is done for cleaner interface and less messy Expat stuff

**std::unique_ptr<SImplementation> DImplementation;**
- Pointer to the hidden implementation
What is std::unique_ptr??
- Smart pointer with single ownership
- Automatically deletes when the CXMLReader is destroyed
- Cannot be copied (unique means ONE owner)
Why unique ptr?
- Frees up memory automatically, whereas a regular pointer has to be manually freed with something like delete
- Basically better for preventing memory leaks

How to use the unique ptr
std::unique_ptr<int> ptr1 = std::make_unique<int>(5); // use the unique ptr command -> put in the type -> then put the name, declare it with std::make_unique<int>(5) to make a unique ptr to 5
std::unique_ptr<int> ptr2 = ptr1;  // ❌ ERROR. Can't copy
std::unique_ptr<int> ptr2 = std::move(ptr1);  // ✅ GOOD. Transfer ownership

- Allocates an int on the heap
- Stores the value 5 there
- ptr1 owns that heap memory
- Can't copy ptr1 to ptr2 because then both ptr1 and ptr2 will both point to 5, which is contradictory to being unique ptr
- You can transfer ownership to another pointer, so instead of ptr1 owning the heap memory, ptr2 will own the heap memory.

**public:
  CXMLReader(std::shared_ptr< CDataSource > src);**
- Constructor: creates an XML reader
- The parameter "std::shared_ptr<CDataSource> src" = Where to read XML data from
Why shared pointer here?
- Multiple things might need access to the same data source
- Shared ownership ensures it doesn't get deleted while we're using it

How to use shared ptr:
std::shared_ptr<int> ptr1 = std::make_shared<int>(5); // Creates shared ptr to an integer on the heap containing 5
std::shared_ptr<int> ptr2 = ptr1; // GOOD. Shares memory

**~CXMLReader();**
- Destructor: cleans up when reader is destroyed
Why explicitly declare it?
- Destructor needs to be in the .cpp file where SImplementation is fully defined
- Otherwise, the compiler doesn't know how to delete SImplementation since it doesn't have a set size or anything since it's defined in the cpp

**bool End() const;**
- Check if done reading
- Returns:
    - true = no more entities to read
    - false = more entities avaliable
 
Example in code:
CXMLReader reader(source);
while (!reader.End()) {
    // Read more entities...
}

**bool ReadEntity(SXMLEntity &entity, bool skipcdata = false);**
- Read the next XML entity
Parameters:
- "SXMLEntity &entity" = OUTPUT parameter (gets filled in with the entity)
- "bool skipcdata = false" = Optional parameter (default is false)
    - If true, skip CharData entities (only return elements)
    - If false, return everything
Returns:
- true = Successfully read an entity
- false = No more entities (or error)

Why pass entity by reference?
- So the function can MODIFY it (fill it in)
- If we passed by value, we would only modify a copy

**#endif**
- End of header guard


### End of the CXMLReader Documentation







# In Case the SXMLEntity is required as well, since it is essential for the XML Reader to work

## XMLEntity.h classes and functions

**using TAttribute = std::pair< std::string, std::string >;**
- This is like the using name space thingy
- It's creating an alias for what std::pair will be used on which comes from the <utility> include
What is "std::pair<std::string, std::string>"?
- A pair holds TWO values together
- First value: a string (the attribute name)
- Second value: a string (the attribute value)

Like for this:
<person name="Alice" age="25">

- name="Alice" would be a pair, cus its an entire attribute
- age="25" would be a pair, cus it's an entire attribute

In Code:
TAttribute attr1 = {"name", "Alice"};
TAttribute attr2 = {"age", "25"};

// Access first element (name)
std::cout << attr1.first;   // Prints: name

// Access second element (value)
std::cout << attr1.second;  // Prints: Alice

**using TAttributes = std::vector< TAttribute >;**
- TAttributes = A list of attributes
- Its a vector of TAttribute pairs

Example in Code:
TAttributes attrs = {
    {"name", "Alice"},
    {"age", "25"},
    {"city", "Davis"}
};

// attrs[0] = {"name", "Alice"}
// attrs[1] = {"age", "25"}
// attrs[2] = {"city", "Davis"}

**struct SXMLEntity{**
- struct for SXMLEntity
- for naming convention, the S in the front of XMLEntity likely means struct

**enum class EType{StartElement, EndElement, CharData, CompleteElement};**
- "enum class" = "Scoped enumeration" (better than plain enum because much safer)
What does it represent???
It represents different types of XML "entities" you encounter when reading:
- StartElement = Opening tag like <person>
- EndElement = Closing tag like </person>
- CharData = Text between tags like the "Hello" in <msg>Hello</msg>
- CompleteElement = Self-closing tag like <br/>

Example of XML parsing:
<person name="Alice">
  Hello World
</person>

Gets broken into **entities**:
- StartElement: name="person", attributes=[("name", "Alice")]
- CharData: data="Hello World"
- EndElement: name="person"

**EType DType;**
- The entity's type - stores which kind of entity this is (start, end, chardata, complete)
- Takes the enum from before and stores whatever the XML is with the values from the enum

How to use both the enum and EType and Dtype all together in code:
SXMLEntity entity;
entity.DType = SXMLEntity::EType::StartElement;  // It's a start tag

// Check what type it is:
if (entity.DType == SXMLEntity::EType::StartElement) {
    std::cout << "This is an opening tag!";
}

**std::string DNameData;**
- This is the entity's name or data
- For StartElement/EndElement: tag name (like "person", "root")
- For CharData: the actual text content
- For CompleteElement: tag name

Example:
// For <person>
entity.DType = EType::StartElement;
entity.DNameData = "person";

// For </person>
entity.DType = EType::EndElement;
entity.DNameData = "person";

// For text "Hello"
entity.DType = EType::CharData;
entity.DNameData = "Hello";

**TAttributes DAttributes;**
- The entity's attributes
- list of attribute name-value pairs

Ex:
entity.DAttributes = {
    {"name", "Alice"},
    {"age", "25"}
};

**bool AttributeExists(const std::string &name) const{**
- Helper Function 1
- bool = Returns true or false
- AttributeExists = Function
- "(const std::string &name)" = Takes an attribute name to search for
- const at the end = Doesn't modify the entity (read-only function)

What does it do?
It checks if this entity has an attribute with the given name

**for(auto &Attribute : DAttributes){**
- "for(auto &Attribute : DAttributes)" = "For each Attribute in DAttributes"
- "auto" = Compiler figures out the type (it's TAttribute)
- "&" = Reference (don't copy, just refer to it)

Basically this:
for(size_t i = 0; i < DAttributes.size(); i++){
    TAttribute &Attribute = DAttributes[i];
    // ...
}

**if(std::get<0>(Attribute) == name){**
What is std::get<0>?
- Gets the FIRST element of a pair
- std::get<0>(Attribute) = The attribute name
- std::get<1>(Attribute) = The attribute value

The loop and this if statement is:
- Looping through all attributes
- Checking if any attribute's name matches what we're looking for
- If found, return true

**return true;   
            }
        }
        return false;**
- However, if we finish the loop without finding it, return false

Example of using it in code:
SXMLEntity entity;
entity.DAttributes = {{"name", "Alice"}, {"age", "25"}};

entity.AttributeExists("name");   // Looks for "name", finds it and returns true
entity.AttributeExists("city");   // Looks for "city", doesn't find it and returns false

**std::string AttributeValue(const std::string &name) const{**
- Helper Function 2
- Returns the VALUE of an attribute (or empty string if not found)

**for(auto &Attribute : DAttributes){
     if(std::get<0>(Attribute) == name){
        return std::get<1>(Attribute);   
     }
}
 return std::string();**
 - Quite similar to AttributeExists() however...
     - Instead of returning true, returns the VALUE (std::get<1>)
     - If it's not found, then it returns an empty string (std::string())
  
Example of it in code:
entity.DAttributes = {{"name", "Alice"}, {"age", "25"}};

std::string name = entity.AttributeValue("name");  // Returns "Alice"
std::string city = entity.AttributeValue("city");  // Returns "" (empty)

**bool SetAttribute(const std::string &name, const std::string &value){**
- Helper Function 3
- Sets an attribute to a value (creates if doesn't exist, updates if exists)

**if(name.empty()){
    return false;   
}**

- This is like a safety check, as...
   1. It doesn't allow empty attribute names
   2. name.empty() returns true if the string has no characters
   3. Return false to indicate failure
 
**for(std::vector< TAttribute >::size_type Index = 0; Index < DAttributes.size(); Index++){**
- Loop
- "std::vector<TAttribute>::size_type" = The type for vector indices (usually size_t)
- Loops through all attributes using an index

Why is it not a loop with auto in it? 
- The auto for loop is called a range-based loop
- Because we need to MODIFY the attributes, we are using the regular for loop (index-based)
    - Range-based for is better for just reading
    - Index-based for is better when you need to modify

**if(std::get<0>(DAttributes[Index]) == name){**
- Checks if this attribute name matches
- "DAttributes[Index]" = Get the attribute at position Index
- "std::get<0>(...)" = Get its name
- Compare with the name we're looking for

**DAttributes[Index] = std::make_pair(name,value);**
- If the attribute name matches/ if it's found, then this is run
- It updates the value
- "std::make_pair(name, value)" = Creates a new pair
- Replace the old attribute with the new one

**return true;
            }
        }**

- If we found and updated, then return true

**DAttributes.push_back(std::make_pair(name,value));
return true;**
- If we didn't find it...
    - Add it to the end of the attributes vector
    - push_back = Add to the end
    - Return true for success
 
### Using SetAttribute() Examples
SXMLEntity entity;

// Add new attribute
entity.SetAttribute("name", "Alice");  // Adds {"name", "Alice"}
// entity.DAttributes = {{"name", "Alice"}}

// Update existing attribute
entity.SetAttribute("name", "Bob");    // Updates to {"name", "Bob"}
// entity.DAttributes = {{"name", "Bob"}}

// Add another attribute
entity.SetAttribute("age", "25");      // Adds {"age", "25"}
// entity.DAttributes = {{"name", "Bob"}, {"age", "25"}}

// Try to add empty name (fails)
entity.SetAttribute("", "value");      // Returns false, nothing happens