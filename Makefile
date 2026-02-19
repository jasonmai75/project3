# Define the tools
AR=ar
CC=gcc
CXX=g++

# Define the directories
INC_DIR				= ./include
SRC_DIR				= ./src
TESTSRC_DIR			= ./testsrc
BIN_DIR				= ./bin
OBJ_DIR				= ./obj
LIB_DIR				= ./lib
TESTOBJ_DIR			= ./testobj
TESTBIN_DIR			= ./testbin
TESTCOVER_DIR		= ./htmlconv
TESTTMP_DIR			= ./testtmp

# Define the flags for compilation/linking
PKGS				= expat
DEFINES				= 
INCLUDE				= -I $(INC_DIR) `pkg-config --cflags $(PKGS)`
ARFLAGS				= rcs
CFLAGS				= -Wall
CPPFLAGS			= --std=c++20
LDFLAGS				= `pkg-config --libs $(PKGS)`

TEST_CFLAGS			= $(CFLAGS) -O0 -g --coverage
TEST_CPPFLAGS		= $(CPPFLAGS) -fno-inline
TEST_LDFLAGS		= $(LDFLAGS) -lgtest -lgtest_main -lpthread

# Define the object files
TEST_SVG_OBJ		= $(TESTOBJ_DIR)/svg.o
TEST_SVG_TEST_OBJ	= $(TESTOBJ_DIR)/SVGTest.o
TEST_OBJ_FILES		= $(TEST_SVG_OBJ) $(TEST_SVG_TEST_OBJ)


TEST_STRSINK_OBJ		= $(TESTOBJ_DIR)/StringDataSink.o
TEST_STRSINK_TEST_OBJ	= $(TESTOBJ_DIR)/StringDataSinkTest.o
TEST_STRSINK_OBJ_FILES	= $(TEST_STRSINK_OBJ) $(TEST_STRSINK_TEST_OBJ)

TEST_STRSRC_OBJ			= $(TESTOBJ_DIR)/StringDataSource.o
TEST_STRSRC_TEST_OBJ 	= $(TESTOBJ_DIR)/StringDataSourceTest.o
TEST_STRSRC_OBJ_FILES	= $(TEST_STRSRC_OBJ) $(TEST_STRSRC_TEST_OBJ)

TEST_SVGWRITER_OBJ		= $(TESTOBJ_DIR)/SVGWriter.o
TEST_SVGWRITER_TEST_OBJ	= $(TESTOBJ_DIR)/SVGWriterTest.o
TEST_SVGWRITER_OBJ_FILES= $(TEST_SVGWRITER_OBJ) $(TEST_SVGWRITER_TEST_OBJ) $(TEST_STRSINK_OBJ) $(STATIC_LIB)

TEST_XMLREADER_OBJ		= $(TESTOBJ_DIR)/XMLReader.o
TEST_XMLREADER_TEST_OBJ	= $(TESTOBJ_DIR)/XMLTest.o
TEST_XMLREADER_OBJ_FILES= $(TEST_XMLREADER_OBJ) $(TEST_XMLREADER_TEST_OBJ) $(TEST_STRSRC_OBJ) $(STATIC_LIB)

TEST_XMLBS_OBJ			= $(TESTOBJ_DIR)/XMLBusSystem.o
TEST_XMLBS_TEST_OBJ		= $(TESTOBJ_DIR)/XMLBusSystemTest.o
TEST_XMLBS_OBJ_FILES	= $(TEST_STRSRC_OBJ) $(TEST_XMLREADER_OBJ) $(TEST_XMLBS_OBJ) $(TEST_XMLBS_TEST_OBJ)

TEST_OSM_OBJ			= $(TESTOBJ_DIR)/OpenStreetMap.o
TEST_OSM_TEST_OBJ		= $(TESTOBJ_DIR)/OpenStreetMapTest.o
TEST_OSM_OBJ_FILES		= $(TEST_STRSRC_OBJ) $(TEST_XMLREADER_OBJ) $(TEST_OSM_OBJ) $(TEST_OSM_TEST_OBJ)

# Define the targets
TEST_TARGET			= $(TESTBIN_DIR)/testsvg

PROD_SVG_OBJ = $(OBJ_DIR)/svg.o # this is like the actual build, instead of being a test

STATIC_LIB = $(LIB_DIR)/libsvg.a # static library

MAIN_TARGET = $(BIN_DIR)/main # main

TEST_SVG_TARGET = $(TESTBIN_DIR)/testsvg

TEST_STRSINK_TARGET = $(TESTBIN_DIR)/teststrdatasink

TEST_STRSRC_TARGET = $(TESTBIN_DIR)/teststrdatasource

TEST_SVGWRITER_TARGET = $(TESTBIN_DIR)/testsvgwritertest

TEST_XMLREADER_TARGET = $(TESTBIN_DIR)/testxml

TEST_XMLBS_TARGET		= $(TESTBIN_DIR)/testxmlbs

TEST_OSM_TARGET		= $(TESTBIN_DIR)/testosm

# All these get ran
all: directories \
	make_svglib \
	runmain \
	xmldiff \
	run_svgtest \
	run_sinktest \
	run_srctest \
	run_svgwritertest \
	run_xmltest \
	run_xmlbstest \
	run_osmtest \
	gen_html

run_svgtest: $(TEST_SVG_TARGET)
	$(TEST_SVG_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

make_svglib: $(STATIC_LIB)

run_sinktest: $(TEST_STRSINK_TARGET)
	$(TEST_STRSINK_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_srctest: $(TEST_STRSRC_TARGET)
	$(TEST_STRSRC_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_svgwritertest: $(TEST_SVGWRITER_TARGET)
	$(TEST_SVGWRITER_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_xmltest: $(TEST_XMLREADER_TARGET)
	$(TEST_XMLREADER_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_xmlbstest: $(TEST_XMLBS_TARGET)
	$(TEST_XMLBS_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_osmtest: $(TEST_OSM_TARGET)
	$(TEST_OSM_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

gen_html:
	lcov --capture --directory . --output-file $(TESTCOVER_DIR)/coverage.info --ignore-errors inconsistent,source
	lcov --remove $(TESTCOVER_DIR)/coverage.info '*.h' '/usr/*' '*/testsrc/*' --output-file $(TESTCOVER_DIR)/coverage.info
	genhtml $(TESTCOVER_DIR)/coverage.info --output-directory $(TESTCOVER_DIR)

$(PROD_SVG_OBJ): $(SRC_DIR)/svg.c # This writes svg.0 into the object file by reading svg.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(PROD_SVG_OBJ)

$(STATIC_LIB): $(PROD_SVG_OBJ) # static library
	$(AR) $(ARFLAGS) $(STATIC_LIB) $(PROD_SVG_OBJ)

$(MAIN_TARGET): $(SRC_DIR)/main.c $(STATIC_LIB) # compile and linking the main.c with the static library created
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) $(SRC_DIR)/main.c $(STATIC_LIB) -o $(MAIN_TARGET)

runmain: $(MAIN_TARGET) # run command for main that will be run by Makefile by all:
	$(MAIN_TARGET)

# comparing generated SVG with expected SVG
xmldiff: runmain
	xmldiff expected_checkmark.svg checkmark.svg && echo "SVG files are identical" || echo "SVG files are NOT identical"

$(TEST_SVG_TARGET): $(TEST_OBJ_FILES) 
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SVG_TARGET)

$(TEST_STRSINK_TARGET): $(TEST_STRSINK_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSINK_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSINK_TARGET)

$(TEST_STRSRC_TARGET): $(TEST_STRSRC_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSRC_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSRC_TARGET)

$(TEST_SVGWRITER_TARGET): $(TEST_SVGWRITER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_SVGWRITER_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SVGWRITER_TARGET)

$(TEST_XMLREADER_TARGET): $(TEST_XMLREADER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_XMLREADER_OBJ_FILES) $(TEST_LDFLAGS) -lexpat -o $(TEST_XMLREADER_TARGET)

$(TEST_SVG_OBJ): $(SRC_DIR)/svg.c
	$(CC) $(TEST_CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(TEST_SVG_OBJ)

$(TEST_XMLBS_TARGET): $(TEST_XMLBS_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_XMLBS_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_XMLBS_TARGET)

$(TEST_OSM_TARGET): $(TEST_OSM_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OSM_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_OSM_TARGET)

$(TEST_SVG_TEST_OBJ): $(TESTSRC_DIR)/SVGTest.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $(TESTSRC_DIR)/SVGTest.cpp -o $(TEST_SVG_TEST_OBJ)

$(TESTOBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

$(TESTOBJ_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(TESTOBJ_DIR)
	mkdir -p $(TESTBIN_DIR)
	mkdir -p $(TESTCOVER_DIR)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(TESTOBJ_DIR)
	rm -rf $(TESTBIN_DIR)
	rm -rf $(TESTCOVER_DIR)
	rm -rf run_*

