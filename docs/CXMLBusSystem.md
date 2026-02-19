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
