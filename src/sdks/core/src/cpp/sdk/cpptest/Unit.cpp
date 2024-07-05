#include "gtest/gtest.h"
#include "CoreSDKTest.h"

class MockTransportTest : public ::testing::Test {
};

TEST_F(MockTransportTest, TestAccountIDPositive) {
    std::cout << ">>> Inside AccountID unit test 1" << std::endl;
    Firebolt::Error error = Firebolt::Error::None;
    const std::string id = Firebolt::IFireboltAccessor::Instance().AccountInterface().id(&error);
    std::cout << "Unit test : Received Account ID = " << id.c_str() << std::endl;
    EXPECT_EQ(id, "000000");
}

int main(int argc, char** argv) {
    std::cout << ">>> Inside Unit Test Main" << std::endl;
    std::string url = "ws://localhost:9998";
    CoreSDKTest::CreateFireboltInstance(url);
    if (CoreSDKTest::WaitOnConnectionReady() == true) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
}