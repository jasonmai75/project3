# CBusSystem 
Abstract base class representing transit system

This class defines the interface for accessing bus stops, routes, and paths. It cannot be used directly and it must be subclassed (e.g. CXMLBusSystem) with all the pure virtual functions implemented

## Public

**using TStopID = uint64_t**
- Unique identifier type for bus stops

**Inline static constexpr TStopID InvalidStopID**
- Const static value representing an invalid or not-found stop ID. It's set to the maximum value of uint64_t

## Nested Structs
**struct SStop**
- Abstract base struct representing a single bus stop.

- Each bus stop has a unique ID, a location on the street map (via a node ID), and a human-readable description

### Pure Virtual Member Functions
**virtual TStopID ID() const noexcept = 0**
- Returns the unique stop ID for this stop.
- Returns TStopID of this stop

**virtual CStreetMap::TNodeID NodeID() const noexcept = 0**
- Returns the street map node ID where this stop is located
- Returns the TNodeID on the street map corresponding to this stop's physical location

**virtual std::string Description() const noexcept = 0**
- Returns the human- readable description of this stop

**virtual std::string Description(const std::string &description) noexcept = 0**
- Parameters
    - description: The new description string to assign to this stop
- Returns the updated description string

**struct SRoute**
- Abstract base struct representing a single bus route
- A route has a name and an ordered list of stop IDs that define the sequence of stops along the route

**Pure Virtual Member Functions**
**virtual std::string Name() const noexcept = 0;**
- Returns the name of this route
- Returns a string containing the route name (e.g. "Route 1", "A Line")

**virtual std::size_t StopCount() const noexcept = 0**
- Returns how manyt stops are on this route. (# of stops on route's stop list)

**virtual TStopID GetStopID(std::size_t index) const noexcept = 0**
- Returns the stop ID at given index in this route's stop list
- Parameters:
    - index: Zero-based position in the stop-list
- Returns the TStopID at that index, or InvalidStopID if index is out of range

**struct SPath**
- Abstract base struct representing a path between two bus stops
- A path is a sequence of street map node IDs that describe the physical route taken between a source stop and a destination stop. 

**Pure Virtual Member FUnctions**
**virtual CStreetMap::TNodeID StartNodeID() const noexcept = 0**
- Returns the node ID of the first node in the path
- Returns the starting TNodeID

**virtual CStreetMap::TNodeID EndNodeID() const noexcept = 0
- Returns the node ID of the last node in the path

**virtual std::size_t NodeCount() const noexcept = 0**
- Returns the total number of nodes in this path
- Returns the number of TNodeIDs in the path

**virtual CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept = 0**
- Returns the node ID at a given index in the path.
- Parameters:
    - index: Zero-based positon in the node list
- Returns the TNodeID at that index, or CStreetMap::InvalidNodeID if out of range

**Pure Virtual Member functions
**virtual sstd::size_t StopCount() const noexcept = 0**
- Returns the total number of stops in the bus system
- Returns the number of SStop objects loaded

**virtual std::size_t RouteCount() const noexcept = 0**
- Returns the total number of routes in the bus system
- Returns number of SRoute objects loaded

**virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0**
- Returns the stop at the given index
- Parameters:
    - index: Zero-based index into the stop list
- Returns shared pointer to the SStop, or nullptr if index is out of range

**virtual std::shared_ptr<SStop> StopByID (TStopID id) const noexcept = 0**
- Parameters:
    - id: The TStopID to search for
- Returns 
    - Shared pointer to the SStop, or nullptr if not found

**virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0**
- Returns the route at the given index
- Parameters:
    - index: Zero-based index into the route list
- Returns shared pointer to the SRoute, or nullptr if index is out of range

**virtual std::shared_ptr<SRoute> RouteByName(const stdd::string &name) const noexcept = 0**
 - Returns the route with the given name
 - Parameters:
    - name: The route name to search for
- Returns
    - Shared ptr to the SRoute, or nullptr if not found

**virtual std::shared_ptr<SPath> PathByStpIDs(TStopID start, TStopID end) const noexcept = 0**
- Returns the path connecting two stops
- Parameters:
    - start: The TStopID of the source stop
    - end: The TStpID of the destination stop
- Returns
    - Shared pointer to the SPath, or nullptr if no path exists between those stops


