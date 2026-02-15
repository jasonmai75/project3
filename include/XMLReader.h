#ifndef XMLREADER_H
#define XMLREADER_H

#include <memory> // For smart pointers
#include "XMLEntity.h" // For SXMLEntity
#include "DataSource.h" // For CDataSource

class CXMLReader{
    private: // Private members (hidden)
        struct SImplementation; // Forward declaration
        std::unique_ptr<SImplementation> DImplementation; // Unique Pointer
        
    public: // Public interface (visible)
        CXMLReader(std::shared_ptr< CDataSource > src); // Constructor
        ~CXMLReader(); // Deconstructor
        
        bool End() const; // Check if done
        bool ReadEntity(SXMLEntity &entity, bool skipcdata = false); // Read next entity
};

#endif
