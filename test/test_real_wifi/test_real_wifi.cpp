#include <unity.h>
#include "testing/libs/plant_nanny/services/network/MockINetworkService.h"

void setUp(void)
{
    // Set up code here
}

void tearDown(void)
{
    // Clean up code here
}

void test_load_wifi_credentials_from_env()
{
    testing::mocks::MockNetworkService network_service;

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load credentials from .env file
    auto load_result = network_service.load_credentials_from_env(env_path);

    TEST_ASSERT_TRUE_MESSAGE(load_result.succeed(), "Failed to load credentials from .env file");

    // Verify credentials were loaded
    auto ssid_result = network_service.get_stored_ssid();
    auto password_result = network_service.get_stored_password();

    TEST_ASSERT_TRUE_MESSAGE(ssid_result.succeed(), "Failed to get SSID");
    TEST_ASSERT_TRUE_MESSAGE(password_result.succeed(), "Failed to get password");
    TEST_ASSERT_FALSE_MESSAGE(ssid_result.value().empty(), "SSID should not be empty");
    TEST_ASSERT_FALSE_MESSAGE(password_result.value().empty(), "Password should not be empty");

    // Print loaded credentials for verification (remove in production)
    printf("Loaded SSID: %s\n", ssid_result.value().c_str());
    printf("Password length: %zu\n", password_result.value().length());
}

void test_connect_with_env_credentials()
{
    testing::mocks::MockNetworkService network_service;

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load credentials from .env file
    auto load_result = network_service.load_credentials_from_env(env_path);
    TEST_ASSERT_TRUE_MESSAGE(load_result.succeed(), "Failed to load credentials from .env file");

    // Attempt to connect (mock will succeed by default)
    auto result = network_service.connect();

    TEST_ASSERT_TRUE_MESSAGE(result.succeed(), "Connection should succeed");
    TEST_ASSERT_TRUE_MESSAGE(network_service.is_connected(), "Should be connected");
}

void test_connect_failure()
{
    // Create service configured to fail connections
    testing::mocks::MockNetworkConfig config;
    config.connect_should_succeed = false;
    testing::mocks::MockNetworkService network_service(config);

    // Attempt to connect
    auto result = network_service.connect();

    TEST_ASSERT_TRUE_MESSAGE(result.failed(), "Connection should fail");
    TEST_ASSERT_FALSE_MESSAGE(network_service.is_connected(), "Should not be connected");
}

void test_get_network_info()
{
    testing::mocks::MockNetworkService network_service;

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load credentials and connect
    network_service.load_credentials_from_env(env_path);
    network_service.connect();

    // Get IP address
    auto ip_result = network_service.get_ip_address();
    TEST_ASSERT_TRUE_MESSAGE(ip_result.succeed(), "Should get IP address");

    // Get RSSI
    auto rssi_result = network_service.get_rssi();
    TEST_ASSERT_TRUE_MESSAGE(rssi_result.succeed(), "Should get RSSI");

    printf("IP Address: %s\n", ip_result.value().c_str());
    printf("RSSI: %d dBm\n", rssi_result.value());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_load_wifi_credentials_from_env);
    RUN_TEST(test_connect_with_env_credentials);
    RUN_TEST(test_connect_failure);
    RUN_TEST(test_get_network_info);

    return UNITY_END();
}
