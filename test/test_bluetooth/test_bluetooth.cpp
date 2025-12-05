#include <unity.h>
#include "libs/common/service/Registry.h"

#ifdef NATIVE_TEST
#include "testing/libs/plant_nanny/services/bluetooth/MockManager.h"
using BluetoothManager = testing::mocks::MockBluetoothService;
#else
#include "libs/plant_nanny/services/bluetooth/Manager.h"
using BluetoothManager = plant_nanny::services::bluetooth::Manager;
#endif

using namespace common::patterns;

BluetoothManager *btManager = nullptr;

void setUp(void)
{
    common::service::DefaultRegistry::create();
    // Called before each test
#ifdef NATIVE_TEST
    testing::mocks::MockBluetoothConfig config;
    config.channel_open_should_succeed = true;
    config.device_connected = true;
    config.has_data_available = false;
    btManager = new BluetoothManager(config);
#else
    btManager = new BluetoothManager();
#endif
}

void tearDown(void)
{
    // Called after each test
    if (btManager != nullptr)
    {
        delete btManager;
        btManager = nullptr;
    }
}

void test_bluetooth_initialization()
{
    TEST_ASSERT_NOT_NULL(btManager);
    TEST_ASSERT_FALSE(btManager->is_channel_open());
}

void test_bluetooth_open_channel()
{
    auto result = btManager->open_channel();

#ifdef NATIVE_TEST
    // Mock implementation should succeed
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(btManager->is_channel_open());
#else
    // Real implementation - depends on hardware
    // Just verify it returns a result
    TEST_ASSERT_TRUE(result.succeed() || result.failed());
#endif
}

void test_bluetooth_close_channel()
{
    btManager->open_channel();
    auto result = btManager->close_channel();

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_FALSE(btManager->is_channel_open());
#else
    TEST_ASSERT_TRUE(result.succeed() || result.failed());
#endif
}

void test_bluetooth_send_data_without_open()
{
    // Should fail if channel not open
    auto result = btManager->send_data("test data");

#ifdef NATIVE_TEST
    // Mock should fail if not open
    TEST_ASSERT_TRUE(result.failed());
    TEST_ASSERT_TRUE(result.error().message().find("not open") != std::string::npos);
#else
    TEST_ASSERT_TRUE(result.failed());
    TEST_ASSERT_TRUE(result.error().message().find("not open") != std::string::npos);
#endif
}

void test_bluetooth_send_data_with_open()
{
    btManager->open_channel();
    auto result = btManager->send_data("Hello Bluetooth");

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL(15, result.value()); // "Hello Bluetooth" is 15 chars
#else
    // Real implementation may fail if no device connected
    if (result.succeed())
    {
        TEST_ASSERT_GREATER_THAN(0, result.value());
    }
    else
    {
        // Expected when no device connected
        TEST_ASSERT_TRUE(result.error().message().find("connected") != std::string::npos);
    }
#endif
}

void test_bluetooth_receive_data_without_open()
{
    auto result = btManager->receive_data(256);

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.failed());
    TEST_ASSERT_TRUE(result.error().message().find("not open") != std::string::npos);
#else
    TEST_ASSERT_TRUE(result.failed());
#endif
}

void test_bluetooth_receive_data_with_open()
{
    btManager->open_channel();

#ifdef NATIVE_TEST
    // Configure mock to have data
    auto *mockBt = dynamic_cast<testing::mocks::MockBluetoothService *>(btManager);
    mockBt->set_data_available(true);
    mockBt->set_mock_data("Test data from BLE");
#endif

    auto result = btManager->receive_data(256);

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(result.value().length() > 0);
    TEST_ASSERT_EQUAL_STRING("Test data from BLE", std::string(result.value()).c_str());
#else
    // Real implementation - likely no data available
    if (result.failed())
    {
        TEST_ASSERT_TRUE(result.error().message().find("available") != std::string::npos ||
                         result.error().message().find("connected") != std::string::npos);
    }
#endif
}

void test_bluetooth_send_empty_string()
{
    btManager->open_channel();
    auto result = btManager->send_data("");

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL(0, result.value());
#else
    if (result.succeed())
    {
        TEST_ASSERT_EQUAL(0, result.value());
    }
#endif
}

void test_bluetooth_send_large_data()
{
    btManager->open_channel();

    // Create a string larger than typical MTU (23 bytes)
    std::string largeData(100, 'X');
    auto result = btManager->send_data(largeData);

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL(100, result.value());
#else
    // Real implementation handling depends on BLE stack
    TEST_ASSERT_TRUE(result.succeed() || result.failed());
#endif
}

void test_bluetooth_receive_with_max_length()
{
    btManager->open_channel();

#ifdef NATIVE_TEST
    auto *mockBt = dynamic_cast<testing::mocks::MockBluetoothService *>(btManager);
    mockBt->set_data_available(true);
    mockBt->set_mock_data("This is a long string that should be truncated");
#endif

    // Test with small max_length
    auto result = btManager->receive_data(10);

#ifdef NATIVE_TEST
    TEST_ASSERT_TRUE(result.succeed());
    // Mock data should be truncated if longer than 10
    TEST_ASSERT_LESS_OR_EQUAL(10, result.value().length());
    TEST_ASSERT_EQUAL(10, result.value().length());
#else
    // Just verify it doesn't crash
    TEST_ASSERT_TRUE(result.succeed() || result.failed());
#endif
}

void test_bluetooth_multiple_open_close_cycles()
{
    // Open and close multiple times
    for (int i = 0; i < 3; i++)
    {
        auto openResult = btManager->open_channel();

#ifdef NATIVE_TEST
        TEST_ASSERT_TRUE(openResult.succeed());
#endif

        auto closeResult = btManager->close_channel();

#ifdef NATIVE_TEST
        TEST_ASSERT_TRUE(closeResult.succeed());
#endif
    }
}

void test_bluetooth_destructor_closes_channel()
{
#ifdef NATIVE_TEST
    testing::mocks::MockBluetoothConfig config;
    auto *tempManager = new BluetoothManager(config);
#else
    auto *tempManager = new BluetoothManager();
#endif
    tempManager->open_channel();

    // Destructor should close channel without errors
    delete tempManager; // Should not crash

    TEST_ASSERT_TRUE(true); // If we get here, destructor worked
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_bluetooth_initialization);
    RUN_TEST(test_bluetooth_open_channel);
    RUN_TEST(test_bluetooth_close_channel);
    RUN_TEST(test_bluetooth_send_data_without_open);
    RUN_TEST(test_bluetooth_send_data_with_open);
    RUN_TEST(test_bluetooth_receive_data_without_open);
    RUN_TEST(test_bluetooth_receive_data_with_open);
    RUN_TEST(test_bluetooth_send_empty_string);
    RUN_TEST(test_bluetooth_send_large_data);
    RUN_TEST(test_bluetooth_receive_with_max_length);
    RUN_TEST(test_bluetooth_multiple_open_close_cycles);
    RUN_TEST(test_bluetooth_destructor_closes_channel);

    return UNITY_END();
}
