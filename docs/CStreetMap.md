# CStreetMap 
- Abstract base class representing a street map made up of nodes and ways
- This class defines the interface for accessing geographic map data. It cannot be used directly, it must be subclassed (e.g. COpenStreetMap) with all pure virtual functions implemented. The mao is structured aorund two elements: nodes (individiual points with a location) and ways (ordered sequences of nodes forming roads or paths)

### **Public**
**using TNodeID = uint64_t**
- Unique identifier type for map nodes

**using TWayID = uint64_t**
- Unique identifier type for map ways

**inline static constexpr TNodeID InvalidNodeID**
- Sentinel value representing an invalid or not-found node ID
- Set to the maximum value of uint64_t

**inline static constexpr TWayID InvalidWayID**
- Sentinel value representing an invalid or not-found way ID
- Set to the maximum value of uint64_t

## Nested Structs

### struct SLocation
- Represents a geographic coordinate with latitude and longitude
- Attributes:
    - DLatitude: Latitude in decimal degrees
    - DLongitude: Longitude in decimal degrees

**SLocation()**
- Default constructor. Creates an uninitialized location

**SLocation(const CStreetMap::SLocation &loc)**
- Copy constructor

**SLocation(double latitude, double longitude)**
- Constructs a location from explicit coordinates
- Parameters:
    - latitude: Latitude value in decimal degrees
    - longitude: Longitude value in decimal degrees

**bool operator==(const SLocation &loc) const**
- Returns true if both latitude and longitude match exactly

---

### struct SNode
- Abstract base struct representing a single point on the map
- Each node has a unique ID, a geographic location, and an optional set of key-value attributes (e.g. tags from OpenStreetMap data)

**virtual TNodeID ID() const noexcept = 0**
- Returns the unique node ID
- Returns the TNodeID of this node

**virtual SLocation Location() const noexcept = 0**
- Returns the geographic location of this node
- Returns an SLocation containing latitude and longitude

**virtual std::size_t AttributeCount() const noexcept = 0**
- Returns the number of key-value attributes on this node
- Returns the count of attribute pairs

**virtual std::string GetAttributeKey(std::size_t index) const noexcept = 0**
- Returns the attribute key at the given index
- Parameters:
    - index: Zero-based index into the attribute list
- Returns the key string at that position

**virtual bool HasAttribute(const std::string &key) const noexcept = 0**
- Returns whether an attribute with the given key exists on this node
- Parameters:
    - key: The attribute key to search for
- Returns true if the key exists, false otherwise

**virtual std::string GetAttribute(const std::string &key) const noexcept = 0**
- Returns the value of the attribute with the given key
- Parameters:
    - key: The attribute key to look up
- Returns the value string, or an empty string if the key does not exist

---

### struct SWay
- Abstract base struct representing an ordered sequence of nodes (a road, path, or boundary)
- A way has a unique ID, an ordered list of node IDs, and an optional set of key-value attributes (e.g. road type, name, speed limit)

**virtual TWayID ID() const noexcept = 0**
- Returns the unique way ID
- Returns the TWayID of this way

**virtual std::size_t NodeCount() const noexcept = 0**
- Returns the number of nodes referenced by this way
- Returns the count of node IDs in the way's list

**virtual TNodeID GetNodeID(std::size_t index) const noexcept = 0**
- Returns the node ID at the given index in this way's node list
- Parameters:
    - index: Zero-based position in the node list
- Returns the TNodeID at that index, or InvalidNodeID if index is out of range

**virtual std::size_t AttributeCount() const noexcept = 0**
- Returns the number of key-value attributes on this way
- Returns the count of attribute pairs

**virtual std::string GetAttributeKey(std::size_t index) const noexcept = 0**
- Returns the attribute key at the given index
- Parameters:
    - index: Zero-based index into the attribute list
- Returns the key string at that position

**virtual bool HasAttribute(const std::string &key) const noexcept = 0**
- Returns whether an attribute with the given key exists on this way
- Parameters:
    - key: The attribute key to search for
- Returns true if the key exists, false otherwise

**virtual std::string GetAttribute(const std::string &key) const noexcept = 0**
- Returns the value of the attribute with the given key
- Parameters:
    - key: The attribute key to look up
- Returns the value string, or an empty string if the key does not exist

## Public Member Functions

**virtual std::size_t NodeCount() const noexcept = 0**
- Returns the total number of nodes in the map
- Returns the number of SNode objects loaded

**virtual std::size_t WayCount() const noexcept = 0**
- Returns the total number of ways in the map
- Returns the number of SWay objects loaded

**virtual std::shared_ptr<SNode> NodeByIndex(std::size_t index) const noexcept = 0**
- Returns the node at the given index
- Parameters:
    - index: Zero-based index into the node list
- Returns shared pointer to the SNode, or nullptr if index is out of range

**virtual std::shared_ptr<SNode> NodeByID(TNodeID id) const noexcept = 0**
- Returns the node with the given ID
- Parameters:
    - id: The TNodeID to look up
- Returns shared pointer to the SNode, or nullptr if not found

**virtual std::shared_ptr<SWay> WayByIndex(std::size_t index) const noexcept = 0**
- Returns the way at the given index
- Parameters:
    - index: Zero-based index into the way list
- Returns shared pointer to the SWay, or nullptr if index is out of range

**virtual std::shared_ptr<SWay> WayByID(TWayID id) const noexcept = 0**
- Returns the way with the given ID
- Parameters:
    - id: The TWayID to look up
- Returns shared pointer to the SWay, or nullptr if not found