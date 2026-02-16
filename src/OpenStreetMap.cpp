#include "OpenStreetMap.h"
#include <unordered_map>

struct COpenStreetMap::SImplementation{
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DWayTag = "way";
    const std::string DNodeReferenceTag = "nd";


    struct SNode: public CStreetMap::SNode{
        const std::string DNodeIDAttr = "id";
        const std::string DNodeLatAttr = "lat";
        const std::string DNodeLonAttr = "lon";
        TNodeID DID;
        SLocation DLocation;

        TAttributes DAttributes;

        SNode(const SXMLEntity &entity){
            auto NodeID = std::stoull(entity.AttributeValue(DNodeIDAttr));
            auto NodeLat = std::stod(entity.AttributeValue(DNodeLatAttr));
            auto NodeLon = std::stod(entity.AttributeValue(DNodeLonAttr));
            DID = NodeID;
            DLocation = SLocation{NodeLat,NodeLon};
            DAttributes = entity.DAttributes;
        }
        ~SNode(){

        }

        TNodeID ID() const noexcept override{
            return DID;
        }
        
        SLocation Location() const noexcept override{
            return DLocation;
        }
        
        std::size_t AttributeCount() const noexcept override{
            return DAttributes.size();
        }
        
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            return DAttributes[index].first;
        }
        
        bool HasAttribute(const std::string &key) const noexcept override{
            for(auto &Attribute : DAttributes){ // Looks through all attributes in DAttributes
                if(std::get<0>(Attribute) == key){ // Finds the first element of pair
                    return true;   // Returns true if found
                }
            }
            return false; // If nothing is found, return false
        }
        
        std::string GetAttribute(const std::string &key) const noexcept override{
            for(auto &Attribute : DAttributes){ // Looks through all attributes in DAttributes
                if(std::get<0>(Attribute) == key){ // Finds the key of pair
                    return std::get<1>(Attribute);   // Returns the value of attribute
                }
            }
            return std::string(); // If nothing is found, return an empty string " "
        }
        
    };

    struct SWay: public CStreetMap::SWay{
        const std::string DWayIDAttr = "id";
        TWayID DID;
        TAttributes DAttributes;
        std::vector<TNodeID> DNodeReferences;

        SWay(const SXMLEntity &entity){
            auto WayID = std::stoull(entity.AttributeValue(DWayIDAttr));
            DID = WayID;
            DAttributes = entity.DAttributes;    
        }

        ~SWay(){

        }

        TWayID ID() const noexcept override{
            return DID;
        }
        
        std::size_t NodeCount() const noexcept override{
            return DNodeReferences.size();
        }
        
        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if(index >= NodeCount()){
                return InvalidNodeID;
            }

            return DNodeReferences[index];
        }
        
        std::size_t AttributeCount() const noexcept override{
            return DAttributes.size();
        }
        
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            return DAttributes[index].first;
        }
        
        bool HasAttribute(const std::string &key) const noexcept override{
            for(auto &Attribute : DAttributes){ // Looks through all attributes in DAttributes
                if(std::get<0>(Attribute) == key){ // Finds the first element of pair
                    return true;   // Returns true if found
                }
            }
            return false; // If nothing is found, return false
        }
        
        std::string GetAttribute(const std::string &key) const noexcept override{
            for(auto &Attribute : DAttributes){ // Looks through all attributes in DAttributes
                if(std::get<0>(Attribute) == key){ // Finds the key of pair
                    return std::get<1>(Attribute);   // Returns the value of attribute
                }
            }
            return std::string(); // If nothing is found, return an empty string " "
        }
        
    };

    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;

    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TNodeID,std::shared_ptr<SWay>> DWaysByID;

    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity,true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    bool ParseOSM(std::shared_ptr<CXMLReader> src){
        SXMLEntity TempEntity;
        if(!FindStartTag(src,DOSMTag)){
            return false;
        }
        while(src->ReadEntity(TempEntity)){
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag){
                auto NewNode = std::make_shared<SNode>(TempEntity);
                DNodesByIndex.push_back(NewNode);
                DNodesByID[NewNode->ID()] = NewNode;
                FindEndTag(src,DNodeTag);
            }

            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){
                auto NewWay = std::make_shared<SWay>(TempEntity);
                //Find and read the <nd> between way
                while(src->ReadEntity(TempEntity,true)){
                    //stop if see end tag
                    if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == DWayTag)){
                        break;
                    }
                    if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == DNodeReferenceTag)){
                        auto NodeRef = std::stoull(TempEntity.AttributeValue("ref"));
                        NewWay->DNodeReferences.push_back(NodeRef);
                    }
                }
                DWaysByIndex.push_back(NewWay);
                DWaysByID[NewWay->ID()] = NewWay;
                FindEndTag(src, DWayTag);
            }
        }


    }

    SImplementation(std::shared_ptr<CXMLReader> src){

    }

    std::size_t NodeCount() const noexcept{
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept{
        return DWaysByIndex.size();
    }

    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{
        if(index < DNodesByIndex.size()){
            return DNodesByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{
        auto Search = DNodesByID.find(id);
        if(Search != DNodesByID.end()){
            return Search->second;
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{
        if(index < DWaysByIndex.size()){
            return DWaysByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{
        auto Search = DWaysByID.find(id);
        if(Search != DWaysByID.end()){
            return Search->second;
        }
        return nullptr;
    }

};


COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap(){

}

std::size_t COpenStreetMap::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}

std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}

