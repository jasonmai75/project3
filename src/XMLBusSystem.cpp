#include "XMLBusSystem.h"
#include <vector>
#include <unordered_map>
#include <iostream>
using std::cout;
using std::endl;

struct CXMLBusSystem::SImplementation{
    // Tag and Attributes
    const std::string DBusSystemTag = "bussystem";
    const std::string DStopsTag = "stops";
    const std::string DStopTag = "stop";
    const std::string DStopIDAttr = "id";
    const std::string DStopNodeAttr = "node";
    const std::string DStopDescAttr = "description";

    const std::string DRoutesTag = "routes";
    const std::string DRouteTag = "route";
    const std::string DRouteNameAttr = "name";
    const std::string DRouteStopTag = "routestop";

    const std::string DPathsTag = "paths";
    const std::string DPathTag = "path";
    const std::string DPathSourceAttr = "source";
    const std::string DPathDestAttr = "destination";
    const std::string DNodeTag = "node";
    const std::string DNodeIDAttr = "id";

    // Represents bus stop with ID, location node, and description
    struct SStop : public CBusSystem::SStop{
        TStopID DID;                    // Stop ID
        CStreetMap::TNodeID DNodeID;    // Node ID on street map
        std::string DDescription;       // Readable Description 

        // Constructor
        SStop(TStopID id, CStreetMap::TNodeID nodeid, const std::string &description){
            DID = id;
            DNodeID = nodeid;
            DDescription = description;
        }

        // Destructor
        ~SStop(){};

        // Returns the stop ID
        TStopID ID() const noexcept override{
            return DID;
        }

        // Returns the node ID where this stop is located
        CStreetMap::TNodeID NodeID() const noexcept override{
            return DNodeID;
        }

        // Returns the stop description (getter)
        std::string Description() const noexcept override{
            return DDescription;
        }

        // Sets and returns the stop description (setter)
        std::string Description(const std::string &description) noexcept override{
            DDescription = description;
            return DDescription;
        }
    };

    // Represents bus route with name and list of stops
    struct SRoute : public CBusSystem::SRoute {
        std::string DName;                  // Route Name
        std::vector<TStopID> DStopIDs;      // Ordered list of stop IDs on this route

        // Constructor
        SRoute(const std::string &name) : DName(name) {}

        // Destructor
        ~SRoute() {}

        // Returns route name
        std::string Name() const noexcept override{
            return DName;
        }

        // Returns how many stops are on this route
        std::size_t StopCount() const noexcept override {
            return DStopIDs.size();
        }

        // Returns stop ID at given index
        TStopID GetStopID(std::size_t index) const noexcept override {
            return DStopIDs[index];
        }
    };
    
    struct SPath : public CBusSystem::SPath {
        CStreetMap::TNodeID DStartNodeID;
        CStreetMap::TNodeID DEndNodeID;
        std::vector<CStreetMap::TNodeID> DNodeIDs;

        SPath() : DStartNodeID(CStreetMap::InvalidNodeID), DEndNodeID(CStreetMap::InvalidNodeID) {}

        ~SPath(){}

        // Return starting node ID
        CStreetMap::TNodeID StartNodeID() const noexcept override {
            return DStartNodeID;
        }

        // Return ending node ID
        CStreetMap::TNodeID EndNodeID() const noexcept override {
            return DEndNodeID;
        }

        // Returns total num of nodes in path
        std::size_t NodeCount() const noexcept override {
            return DNodeIDs.size();
        }

        // Returns node ID at given index
        CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept override {
            return DNodeIDs[index];
        }

    };

    // Helper Functions

    // Searches XML stream for specific opening tag and returns true if found and false if it reaches the end without finding it
    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){  // true = skip char data
            // If found, then return true
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false; // Otherwise, return false
    }

    // Same as last function, however with the end tag
    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    // Data Storage

    // Stop storage: by index (is fastest lookup) and by ID (for fast lookup)
    std::vector<std::shared_ptr<SStop> > DStopsByIndex;
    std::unordered_map<TStopID,std::shared_ptr<SStop> > DStopsByID;

    // Route storage: should be by index (fast iteration) and name (fast lookup)
    std::vector<std::shared_ptr<SRoute>> DRoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> DRoutesByName;

    // Path storage: nested map [start_stop][end_stop] -> path
    std::unordered_map<TStopID, std::unordered_map<TStopID, std::shared_ptr<SPath>>> DPathsByStopIDs;

    // Parsing Functions

    void ParseStop(std::shared_ptr< CXMLReader > systemsource, const SXMLEntity &stop){
        TStopID StopID = std::stoull(stop.AttributeValue(DStopIDAttr));
        CStreetMap::TNodeID NodeID = std::stoull(stop.AttributeValue(DStopNodeAttr));
        auto NewStop = std::make_shared<SStop>(StopID, NodeID, stop.AttributeValue(DStopDescAttr));
        DStopsByIndex.push_back(NewStop);
        cout<<"DStopsByIndex "<<DStopsByIndex.size()<<endl;
        DStopsByID[StopID] = NewStop;
        FindEndTag(systemsource,DStopTag);
    }

    void ParseStops(std::shared_ptr< CXMLReader > systemsource){
        SXMLEntity TempEntity;

        do{
            if(!systemsource->ReadEntity(TempEntity,true)){

                return;
            }
            cout<<int(TempEntity.DType)<<" '"<<TempEntity.DNameData<<"'"<<endl;
            if((TempEntity.DType == SXMLEntity::EType::StartElement) &&(TempEntity.DNameData == DStopTag)){
                ParseStop(systemsource,TempEntity);
            }

        }while((TempEntity.DType != SXMLEntity::EType::EndElement)||(TempEntity.DNameData != DStopsTag));
    }

    void ParseRoute(std::shared_ptr< CXMLReader > systemsource){

    }

    void ParseRoutes(std::shared_ptr< CXMLReader > systemsource){

    }

    

    void ParseBusSystem(std::shared_ptr< CXMLReader > systemsource){
        SXMLEntity TempEntity;
        if(!FindStartTag(systemsource,DBusSystemTag)){
            cout<<"Start tag bussystem not found"<<endl;
            return;
        }
        if(!FindStartTag(systemsource,DStopsTag)){
            cout<<"Start tag stop not found"<<endl;
            return;
        }
        ParseStops(systemsource);

    }

    SImplementation(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
        ParseBusSystem(systemsource);
        
    }

    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }

    std::size_t RouteCount() const noexcept{
        return 0;
    }
    
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        return DStopsByIndex[index];
    }
    
    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        return nullptr;
    }
    
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{

    }
    
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{

    }
    
    std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept{

    }
    
};

CXMLBusSystem::CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
    DImplementation = std::make_unique<SImplementation>(systemsource,pathsource);
}
    
CXMLBusSystem::~CXMLBusSystem(){

}
    
std::size_t CXMLBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}
    
std::size_t CXMLBusSystem::RouteCount() const noexcept{
    return 0;
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}

std::shared_ptr<CBusSystem::SPath> CXMLBusSystem::PathByStopIDs(TStopID start, TStopID end) const noexcept{
    return DImplementation->PathByStopIDs(start, end);
}
