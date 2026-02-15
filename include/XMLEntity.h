#ifndef XMLENTITY_H
#define XMLENTITY_H

#include <utility>
#include <string>
#include <vector>

using TAttribute = std::pair< std::string, std::string >;
using TAttributes = std::vector< TAttribute >;

struct SXMLEntity{    
    enum class EType{StartElement, EndElement, CharData, CompleteElement}; // What type of XML thing is it
    EType DType; // The variable to store the XML type
    std::string DNameData; // Stores the entity (like the tags, like <person> or smth)
    TAttributes DAttributes; // Stores a vector of entities
    
    bool AttributeExists(const std::string &name) const{ // Does the attribute exist?
        for(auto &Attribute : DAttributes){ // Loops through every attribute in DAttributes (auto figures out the type automatically)
            if(std::get<0>(Attribute) == name){ // Gets first element of pair if found
                return true;   
            }
        }
        return false; // If nothing found, return false for nothing found
    };
    
    std::string AttributeValue(const std::string &name) const{ // Basically the same as above, but returns the attribute value
        for(auto &Attribute : DAttributes){ // Looks through all attributes in DAttributes
            if(std::get<0>(Attribute) == name){ // Finds the first element of pair
                return std::get<1>(Attribute);   // Returns the second element of the pair
            }
        }
        return std::string(); // If nothing is found, return an empty string " "
    };
    
    bool SetAttribute(const std::string &name, const std::string &value){ // Sets an attribute to a value (creates if not exist, updates if exists)
        if(name.empty()){ // Safety check for empty attribute names
            return false;  
        }
        for(std::vector< TAttribute >::size_type Index = 0; Index < DAttributes.size(); Index++){ // Loops through all vector indices/ all attributes in an index
            if(std::get<0>(DAttributes[Index]) == name){  // Checks if attribute name matches, gets the attribute at position Index and gets it's name and compares to name we looking for
                DAttributes[Index] = std::make_pair(name,value); // If attribute name matches, update the value and create a new pair and then replace old attribute with new attribute
                return true; // If found and updated, return true
            }
        }
        DAttributes.push_back(std::make_pair(name,value)); // If not found, add it to the end of the attributes vector
        return true; // Return for success
    };
};
   
#endif
