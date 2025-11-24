#include <unity.h>
#include "mocks/MockNetworkService.h"
#include <memory>

using namespace plant_nanny::services;
using namespace test::mocks;

std::unique_ptr<MockNetworkService> mock_network = nullptr;

void setUp(void) {
    mock_network = std::make_unique<MockNetworkService>();
}

void tearDown(void) {
    mock_network.reset();
}

// Basic network service tests
void test_network_set_credentials() {
    mock_network->set_credentials("TestSSID", "TestPassword");
    
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();
    
    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_EQUAL_STRING("TestSSID", ssid_result.value().c_str());
    TEST_ASSERT_EQUAL_STRING("TestPassword", password_result.value().c_str());
}

void test_network_load_credentials_from_env() {
    auto load_result = mock_network->load_credentials_from_env("/home/hurel/repo/IOT/.env");
    TEST_ASSERT_TRUE(load_result.succeed());
    
    // Verify credentials were loaded
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();
    
    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_FALSE(ssid_result.value().empty());
    TEST_ASSERT_FALSE(password_result.value().empty());
}

void test_network_get_stored_credentials() {
    mock_network->set_credentials("MyNetwork", "MyPassword");
    
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();
    
    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_EQUAL_STRING("MyNetwork", ssid_result.value().c_str());
    TEST_ASSERT_EQUAL_STRING("MyPassword", password_result.value().c_str());
}

void test_network_load_credentials_invalid_path() {
    auto load_result = mock_network->load_credentials_from_env("/nonexistent/path/.env");
    TEST_ASSERT_TRUE(load_result.failed());
}

void test_network_get_credentials_when_not_set() {
    // Don't set any credentials
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();
    
    TEST_ASSERT_TRUE(ssid_result.failed());
    TEST_ASSERT_TRUE(password_result.failed());
}

void test_network_connect_success() {
    test::mocks::MockNetworkConfig config;
    config.connect_should_succeed = true;
    mock_network->test_update_config(config);
    auto result = mock_network->connect();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(mock_network->is_connected());
}

void test_network_connect_failure() {
    test::mocks::MockNetworkConfig config;
    config.connect_should_succeed = false;
    mock_network->test_update_config(config);
    auto result = mock_network->connect();
    TEST_ASSERT_TRUE(result.failed());
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_disconnect() {
    test::mocks::MockNetworkConfig config;
    config.connect_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->connect();
    TEST_ASSERT_TRUE(mock_network->is_connected());
    
    mock_network->disconnect();
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_is_connected() {
    TEST_ASSERT_FALSE(mock_network->is_connected());
    
    mock_network->test_set_connected(true);
    TEST_ASSERT_TRUE(mock_network->is_connected());
    
    mock_network->test_set_connected(false);
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_get_ip_address_when_connected() {
    test::mocks::MockNetworkConfig config;
    config.mock_ip = "192.168.1.100";
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);
    
    auto result = mock_network->get_ip_address();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL_STRING("192.168.1.100", result.value().c_str());
}

void test_network_get_ip_address_when_disconnected() {
    mock_network->test_set_connected(false);
    
    auto result = mock_network->get_ip_address();
    TEST_ASSERT_TRUE(result.failed());
}

void test_network_get_rssi_when_connected() {
    test::mocks::MockNetworkConfig config;
    config.mock_rssi = -50;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);
    
    auto result = mock_network->get_rssi();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL(-50, result.value());
}

void test_network_get_rssi_when_disconnected() {
    mock_network->test_set_connected(false);
    
    auto result = mock_network->get_rssi();
    TEST_ASSERT_TRUE(result.failed());
}

void test_network_download_success() {
    test::mocks::MockNetworkConfig config;
    config.download_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);
    
    bool chunk_received = false;
    bool progress_called = false;
    
    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [&chunk_received](const uint8_t* data, size_t length) -> bool {
            chunk_received = true;
            TEST_ASSERT_NOT_NULL(data);
            TEST_ASSERT_GREATER_THAN(0, length);
            return true;
        },
        [&progress_called](size_t downloaded, size_t total) {
            progress_called = true;
        }
    );
    
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(chunk_received);
    TEST_ASSERT_TRUE(progress_called);
}

void test_network_download_failure() {
    test::mocks::MockNetworkConfig config;
    config.download_should_succeed = false;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);
    
    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t* data, size_t length) -> bool { return true; }
    );
    
    TEST_ASSERT_TRUE(result.failed());
}

void test_network_download_when_disconnected() {
    mock_network->test_set_connected(false);
    
    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t* data, size_t length) -> bool { return true; }
    );
    
    TEST_ASSERT_TRUE(result.failed());
}

void test_network_download_chunk_handler_rejects() {
    test::mocks::MockNetworkConfig config;
    config.download_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);
    
    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t* data, size_t length) -> bool {
            return false; // Reject the chunk
        }
    );
    
    TEST_ASSERT_TRUE(result.failed());
}

#ifdef NATIVE_TEST
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    
    // Basic network service tests
    RUN_TEST(test_network_set_credentials);
    RUN_TEST(test_network_load_credentials_from_env);
    RUN_TEST(test_network_get_stored_credentials);
    RUN_TEST(test_network_load_credentials_invalid_path);
    RUN_TEST(test_network_get_credentials_when_not_set);
    RUN_TEST(test_network_connect_success);
    RUN_TEST(test_network_connect_failure);
    RUN_TEST(test_network_disconnect);
    RUN_TEST(test_network_is_connected);
    RUN_TEST(test_network_get_ip_address_when_connected);
    RUN_TEST(test_network_get_ip_address_when_disconnected);
    RUN_TEST(test_network_get_rssi_when_connected);
    RUN_TEST(test_network_get_rssi_when_disconnected);
    
    // Download tests
    RUN_TEST(test_network_download_success);
    RUN_TEST(test_network_download_failure);
    RUN_TEST(test_network_download_when_disconnected);
    RUN_TEST(test_network_download_chunk_handler_rejects);
    
    return UNITY_END();
}
#else
void setup()
{
    delay(2000);
    UNITY_BEGIN();
    
    // Basic network service tests
    RUN_TEST(test_network_set_credentials);
    RUN_TEST(test_network_load_credentials_from_env);
    RUN_TEST(test_network_get_stored_credentials);
    RUN_TEST(test_network_load_credentials_invalid_path);
    RUN_TEST(test_network_get_credentials_when_not_set);
    RUN_TEST(test_network_connect_success);
    RUN_TEST(test_network_connect_failure);
    RUN_TEST(test_network_disconnect);
    RUN_TEST(test_network_is_connected);
    RUN_TEST(test_network_get_ip_address_when_connected);
    RUN_TEST(test_network_get_ip_address_when_disconnected);
    RUN_TEST(test_network_get_rssi_when_connected);
    RUN_TEST(test_network_get_rssi_when_disconnected);
    
    // Download tests
    RUN_TEST(test_network_download_success);
    RUN_TEST(test_network_download_failure);
    RUN_TEST(test_network_download_when_disconnected);
    RUN_TEST(test_network_download_chunk_handler_rejects);
    
    UNITY_END();
}

void loop() {}
#endif
