# COpenStreetMap
- Concrete implementation of CStreetMap that loads map data from an OpenStreetMap XML (.osm) file.
- This class parses an OSM-format XML source and provides a fully working implementation of all CStreetMap pure virtual functions. Nodes and ways are loaded into memory and accessible by both index (for fast iteration) and ID(for fast look up)

## Constructor

**COpenStreetMap(std::shared_ptr<CXMLReader> src)**
- Constructs the street map by parsing the OSM XML source
- Parameters:
    - src: XML reader pointed at an OSM-format data source

## Destructor

**~COpenStreetMap()**
- Destructor. Cleans up the internal implementation

## Public Member Functions

**std::size_t NodeCount() const noexcept override**
- Returns the total number of nodes loaded from the OSM file
- Returns the number of <node> elements parsed

**std::size_t WayCount() const noexcept override**
- Returns the total number of ways loaded from the OSM file
- Returns the number of <way> elements parsed

**std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept override**
- Returns the node at the given index in load order
- Parameters:
    - index: Zero-based index into the node list
- Returns shared pointer to the SNode, or nullptr if index is out of range

**std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept override**
- Returns the node with the given ID
- Parameters:
    - id: The TNodeID to look up
- Returns shared pointer to the SNode, or nullptr if no node with that ID exists

**std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept override**
- Returns the way at the given index in load order
- Parameters:
    - index: Zero-based index into the way list
- Returns shared pointer to the SWay, or nullptr if index is out of range

**std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept override**
- Returns the way with the given ID
- Parameters:
    - id: The TWayID to look up
- Returns shared pointer to the SWay, or nullptr if no way with that ID exists

## OSM XML Format Expected

**OSM File (src)**
```xml
<osm>
  <node id="500001" lat="38.5382" lon="-121.7617">
    <tag k="name" v="Main & 1st"/>
  </node>
  <node id="500002" lat="38.5391" lon="-121.7625"/>

  <way id="200001">
    <nd ref="500001"/>
    <nd ref="500002"/>
    <tag k="highway" v="residential"/>
    <tag k="name" v="Main Street"/>
  </way>
</osm>
```

**Attribute meanings**
- node id = The TNodeID used to look up the node
- node lat = Latitude in decimal degrees stored in SLocation.DLatitude
- node lon = Longitude in decimal degrees stored in SLocation.DLongitude
- way id = The TWayID used to look up the way
- nd ref = References a node's TNodeID to build the way's ordered node list
- tag k / tag v = Key-value attribute pairs accessible via HasAttribute and GetAttribute
