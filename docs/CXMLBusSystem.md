# CXMLBusSystem
- Concrete implementation of CBusSystem that loads bus stop, route, and path data from XML files

- This class parses two XML sources: a bus system file (containing stops and routes) and a paths file (containing node sequences between stops). It inherits from CBusSystem and provides a fully working implementation of all its pure virtual functions

## Constructor

**CXMLBusSystem(std::shared_ptr<CXMLReader> systemsource, std::shared_ptr<CXMLReader> pathsource)
- Constructs the bus system by parsing both XML sources
- Parameters:
    - systemsource: XMLReader pointed at the bus system file(contains <bussystem>, <stops> and <routes> sections)
    - pathsource: XML reader pointed at the paths file (contains <paths> with node sequences)

## Destructor
**~CXMLBusSystem()**
- Destructor. Cleans up the internal implementation

##  Public Member Functions
**std::size_t StopCount() const noexcept override**
- Returns the totla number of stops loaded from the XML.
- Returns number of <stop> elements parsed

**std::size_t RouteCount() const noexcept override
- Returns the total number of routes loaded from the XML
- Returns number of <route> elements parsed

**std::shared_ptr<CBusSystem::SStop> StopByIndex(std::size_t index) const noexcept override**
- Returns the stop at the given index in load order
- Parameters:
    - index: Zero-based index into the stop list
- Returns shared pointer to the SStop, or nullptr if index is out of range

**std::shared_ptr<CBusSystem::SStop> StopByID(TStopID id) const noexcept override
- Returns the stop with the given ID
- Parameters:
    - id: The TStopID to look up
- Returns shared pointer to the SStop, or nullptr if no stop with that ID exists

**std::shared_ptr<CBusSYstem::SRoute> RouteByIndex(std::size_t index) const noexcept override**
- Returns the route at the given index in load order
- Parameters:
    - index: Zero-based index with the route list
- Returns shared pointer to the SRoute, or nullptr if index is out of range

**std::shared_ptr<CBusSystem::SRoute> RouteByName(const std::string &name) const noexcept override
- Returns the route with the given name
- Parameters:
    - name: The route name string to search for
- Returns
    - Shared pointer to the SRoute, or nullptr if no route with that name exists

**std::shared_ptr<CBusSYstem::SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept override**
- Returns the path connecting two stops by their IDs
- Parameters:
    - start: The TStopID of the starting stop
    - end: The TStopID of the destination stop
- Returns shared pointer to the SPath, or nullptr if no path exists between those two stops

## XML Format Expected
**Bus System File (systemsource)**
<bussystem>
  <stops>
    <stop id="1001" node="500001" description="Main St & 1st Ave"/>
    <stop id="1002" node="500002" description="2nd Ave & Oak Rd"/>
  </stops>
  <routes>
    <route name="Route 1">
      <routestop id="1001"/>
      <routestop id="1002"/>
    </route>
  </routes>
</bussystem>

**Paths FIle(pathsource)**
<paths>
  <path source="1001" destination="1002">
    <node id="500001"/>
    <node id="500010"/>
    <node id="500002"/>
  </path>
</paths>

**Attribute meanings**
- stop id = TStopID used to look up the stop
- stop node = The CStreetMap TNodeID where the stop is physically located
- stop description = Human-readable label for the stop
- route name = The string used to look up the route by name
- routestop id = References a stop's TStopID to build the route's stop list
- path source / path destination = TStopIDs defining which two stops the path connects
- node id = CStreetMap TNodeID - each one is a waypoint along the path