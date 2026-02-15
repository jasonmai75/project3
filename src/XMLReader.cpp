#include "XMLReader.h"
#include <expat.h>
#include <queue> 

/*
Implementation for CXMLReader

This struct wraps the Expat XML parser library, handling XML parsing and entity queueing
*/
struct CXMLReader::SImplementation{
    // Data source we will read XML from (could be anything like a file, string, etc.)
    std::shared_ptr <CDataSource> DSource;
    // Expat parser object (need for actual XML parsing)
    XML_Parser DParser;
    // Queue that stores entities that Expat has found but user hasnt read yet
    std::queue <SXMLEntity> DEntityQueue;
    // Flag to track if we are at the end of an XML document
    bool DEnd; 

    // Constructor (setting up Expat)
    SImplementation(std::shared_ptr<CDataSource> src) : DSource(src), DEnd(false) {
        DParser = XML_ParserCreate(NULL); // NULL = default encoding

        // We need user data for the Handlers, "this" pointer will apply into every single object
        XML_SetUserData(DParser, this); // "this" is userData and also a pointer to this struct (SImplemntation*)
    
        // SetElementHandler and SetCharacterDataHandler are necessary as prerequisuites for their functions later
        XML_SetElementHandler(DParser, XMLStartElementCallback, XMLEndElementCallback); 
        XML_SetCharacterDataHandler(DParser, XMLCharacterDataCallback);

    }

    // Deconstructor
    ~SImplementation() {
        if(DParser) { // need to clean parser and free it
            XML_ParserFree(DParser);
        }
    }

/*
Callback called by Expat when an XML start tag is found

It creates a StartElement entity with the tag name and attribute, 
then adds it to the entity queue

Parameters: 
userData: pointer to the SImplementation
name: name of the XML element
attrs: array of attribute name-value pairs (terminated by NULL hence attrs[i] != NULL)

Need to use static void because these callbacks are member functions
*/
    static void XMLStartElementCallback(void *userData, const XML_Char *name, const XML_Char **attrs) {
        // Convert the void* back to implementation object, because userData was initially a SImplementation*, but Expat 
        // requires it to be void* in order to work with it, but after Expat works with it, it doesn't switch it back, so we need to switch it back.
        SImplementation *impl = static_cast<SImplementation*> (userData);

        // Create an entity to represent start tag
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        // Parse attributes (array will go: value1, value2, value3..., NULL)
        for (int i = 0; attrs[i] != NULL; i += 2) {
            std::string attr_name = attrs[i];  // first elements (names)
            std::string attr_value = attrs[i + 1]; // second elements (values)
            entity.DAttributes.push_back(std::make_pair(attr_name, attr_value));
        }

        // Add entity to the queue
        impl->DEntityQueue.push(entity);

    }

    /*
    Callback called by Expat when an XML closing tag is found

    Creates an EndElement entity with the tag name and adds it to the queue

    Parameters:
    userData: pointer to SImplementation
    name: name of the XML element being closed
    */
    static void XMLEndElementCallback(void *userData, const XML_Char *name) {
        // Convert to implementation object
        SImplementation *impl = static_cast<SImplementation*> (userData);

        // Create entity for closing tag
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;

        // Add entity to queue
        impl->DEntityQueue.push(entity);

    }

    /*
    Callback called by Expat when character data is found

    Creates a CharData entity with the text content and adds it to the queue

    Parameters:
    userData: point to the SImplementation
    s: pointer to the character data
    len: length of the character data
    */
    static void XMLCharacterDataCallback(void *userData, const XML_Char *s, int len) {
        // Convert to implementation object
        SImplementation *impl = static_cast<SImplementation*> (userData);

        // Extract character data as a string
        std::string data(s, len);

        // Create entity for character data
        SXMLEntity entity; 
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData = data; 

        // Add entity to queue
        impl->DEntityQueue.push(entity);


    }


};

/*
Constructs a new CXMLReader object

Parameter:
src: a data source containing XML data to parse
*/
CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique<SImplementation>(src); 
    
}

/*
Destroys the CXMLReader object
*/
CXMLReader::~CXMLReader(){
 
}

/*
Check if parsing is done

Returns true when both the data sources have no more data
and when all entities have been consumed from the queue

return true if parsing is finsihed, false otherwise
*/
bool CXMLReader::End() const{
// 1. must return when source has no more data
// 2. when all entities are consumed from queue
return DImplementation->DSource->End() && DImplementation->DEntityQueue.empty();
}

/*
Read the next XML entity from the parser

Reads from the data source and feeds data to Expat until an entity 
is avaliable in the queue. Also optionally skips character data entities with skipcdata

Parameters:
entity: Reference to store the read entity
skipcdata: If true, it will skip CharData entities and return on the opening and closing tags (element tags)

returns: true if an entity was successfully read, false if no more entities or error
*/
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    // Parse more data if queue is both empty and not at the end of the document
    while (DImplementation->DEntityQueue.empty() && !DImplementation->DEnd)
    {
        // Create buffer to read data from the source (to feed expat)
        const int bufferSize = 512;
        std::vector<char> buffer;

      /* Inside of the XML_Parse function of the Expat Library, it calls conditionals to call StartElement, or EndElement, or
     CharacterData callbacks depending on the *data passed into it. */

        // Read from data source into the buffer
        if(!DImplementation->DSource->Read(buffer, bufferSize)){

            // If no more data is avaliable, flag the end of parsing to true
            DImplementation->DEnd = true; 

            // Sets the isFinal flag to true to signal successful parsing and end of file.
            XML_Parse(DImplementation->DParser, "", 0, XML_TRUE);

            // Leave the loop and continue to the DEntityQueue popper
            break; 

        }
        
        /*
        The last function should've read everything into the buffer, now we feed it into Expat 
        to parse AND CHECK IF THERE'S AN ERROR kills program cleanly if theres an error while
        also parsing at the same time (feed buffer to Expat parser and check for errors)
        */ 
        if(XML_Parse(DImplementation->DParser, buffer.data(), buffer.size(), XML_FALSE) == XML_STATUS_ERROR) {
            DImplementation->DEnd = true;
            return false; 
        }
    }

    /*
    Have entity in queue(Not empty) -> pop them
    If skip data is true, read until non-CharData entity

    Pop entities from the queue
    */
    while (!DImplementation->DEntityQueue.empty())
    {   // Get first entity from queue
        entity = DImplementation->DEntityQueue.front();

        // After getting it -> remove it from the queue
        DImplementation->DEntityQueue.pop();

        // Skip character data entities if skipcdata is true
        if(skipcdata && entity.DType == SXMLEntity::EType::CharData){
            continue;
        }
        return true;
    }

    // No more entities to read
    return false;   
}
