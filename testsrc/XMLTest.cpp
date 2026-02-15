#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringDataSource.h"

// Testing if it could get simple opening and closing tags
TEST(XMLReaderTest, SimpleTest){
    // Test string XML
    std::string XML = "<person></person>";

    // Wrap it into a StringDataSource so XMLReader can read it
    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 

    // Create XML Reader
    CXMLReader reader(source); 

    // Create entity holder
    SXMLEntity entity;

    // Read <person> 
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");
    EXPECT_TRUE(entity.DAttributes.empty());

    // Read </person>
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    // Check if End
    EXPECT_TRUE(reader.End());
}

// Checking if it could read opening and closing tags with attributes
TEST(XMLReaderTest, ElementTest){
    // Test string XML
    std::string XML = "<person id=\"123\" name=\"Alice\"></person>";

    // Wrap it into a StringDataSource so XMLReader can read it
    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 

    // Create XML Reader
    CXMLReader reader(source); 

   // Create entity holder
    SXMLEntity entity;

    // Read starting tag
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");

    // Check for the two attributes
    ASSERT_EQ(entity.DAttributes.size(), 2); 

    // Test the AttributeExists function
    EXPECT_TRUE(entity.AttributeExists("id"));
    EXPECT_TRUE(entity.AttributeExists("name"));
    // Prove that the error checkers work
    EXPECT_FALSE(entity.AttributeExists("age")); // doesnt exist

    // Test the Attribute Value function
    EXPECT_EQ(entity.AttributeValue("id"), "123");
    EXPECT_EQ(entity.AttributeValue("name"), "Alice");
    // Test empty string error checker
    EXPECT_EQ(entity.AttributeValue("randomthing"), ""); // return empty string



    // Read </person>
    ASSERT_TRUE(reader.ReadEntity(entity)); 
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    EXPECT_TRUE(reader.End());



}

// Testing if Expat can parse typical character data including white space
TEST(XMLReaderTest, CDataTest){
    std::string XML = "<person>hello</person>";

    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 

    CXMLReader reader(source); 

    SXMLEntity entity;

    // Read starting tag
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");

    // Read "Hello World"
    ASSERT_TRUE(reader.ReadEntity(entity)); 
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "hello");

    // Read End tag
    ASSERT_TRUE(reader.ReadEntity(entity)); 
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    EXPECT_TRUE(reader.End());

    //Skip Cdata and white space
    std::string skipXML = "<person>                qwertyuiopa               sdfghjklzxcvbnm                 </person>";
    std::shared_ptr<CStringDataSource> skipSource = std::make_shared<CStringDataSource>(skipXML); 
    CXMLReader skipReader(skipSource);
    //Read start
    ASSERT_TRUE(skipReader.ReadEntity(entity, true));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");
    // Read End tag
    ASSERT_TRUE(skipReader.ReadEntity(entity, true)); 
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");
}

// Testing if Expat can parse very long strings
TEST(XMLReaderTest, LongCDataTest){
    std::string longText(1000, 'J'); // 1000 J characters (long string)

    std::string XML = "<person>" + longText + "</person>"; // create long XML string

    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 
    CXMLReader reader(source);

    SXMLEntity entity;

    // Read starting tag
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");

    // Read long text buffer chunk by buffer chunk and add them all up, so that it can be compared to the real character count
    std::string allTextCombined;
    while(reader.ReadEntity(entity) && entity.DType == SXMLEntity::EType::CharData) {
        allTextCombined += entity.DNameData;
    }

    EXPECT_EQ(allTextCombined, longText); 
    EXPECT_EQ(allTextCombined.length(), 1000); // Check if it actually got 1000 chars

    // Read End tag
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    EXPECT_TRUE(reader.End());


}

// Testing if Expat can parse special characters
TEST(XMLReaderTest, SpecialCharacterTest){
    // probabaly pretty simple, just do a special caracter wiht data char and then yeah
    std::string XML = "<person>q&gt;hello&amp;&lt;&quot;&apos;</person>";

    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 
    CXMLReader reader(source);

     SXMLEntity entity;

    // Read starting tag
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");

// Read the chunks of special characters, because for some reason they are different chunks (Expat needs to decode special chars in different callbacks)
    std::string allTextCombined;
    while(reader.ReadEntity(entity) && entity.DType == SXMLEntity::EType::CharData) {
        allTextCombined += entity.DNameData;
    }
    EXPECT_EQ(allTextCombined, "q>hello&<\"'"); 

    // Read End tag
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    EXPECT_TRUE(reader.End());
}

// if it fails, then it should fail smoothly
TEST(XMLReaderTest, InvalidXMLTest){
    // Test 1
    // Using bad xml, no start tag or end tag or <> symbols
    std::string XML = "bad xml string so that it might crash? <unclosed tag";

    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 
    CXMLReader reader(source);

    SXMLEntity entity;


// Simulate an error to check if it can handle error good  
    bool success = false;
    while(reader.ReadEntity(entity)) {
        success = true; // If able to read something, then keep it true
    }

    // After invalid XML, the reader should be true at the end
    EXPECT_TRUE(reader.End());
    EXPECT_FALSE(success);

    // Test 2
    //Reading literally nothing
    std::shared_ptr<CStringDataSource> emptySource = std::make_shared<CStringDataSource>("");
    CXMLReader emptyReader(emptySource);


    EXPECT_FALSE(emptyReader.ReadEntity(entity));
    EXPECT_TRUE(emptyReader.End());
}

// Testing what happens when the string goes past the 512 Boundary (the buffer?). 
TEST(XMLReaderTest, LongCharDataCrosses512Boundary){
    std::string longText(612, 'J'); // Past 512 characters 

    std::string XML = "<person>" + longText + "</person>"; 

    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>(XML); 
    CXMLReader reader(source);

    SXMLEntity entity;

    // Read starting tag
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "person");

    // Read long text buffer chunk by buffer chunk and add them all up, so that it can be compared to the real character count
    std::string allTextCombined;
    while(reader.ReadEntity(entity) && entity.DType == SXMLEntity::EType::CharData) {
        allTextCombined += entity.DNameData;
    }

    EXPECT_EQ(allTextCombined, longText); 
    EXPECT_EQ(allTextCombined.length(), 612); // Check if it actually got 612 chars

    // Read End tag
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "person");

    EXPECT_TRUE(reader.End());
}
