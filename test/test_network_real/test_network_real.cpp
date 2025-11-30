#include <unity.h>
#include "mocks/MockNetworkService.h"
#include <fstream>
#include <string>

bool load_wifi_credentials(std::string &ssid, std::string &password)
{
#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif
    std::ifstream env_file(env_path);
    if (!env_file.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(env_file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            if (key == "WIFI_SSID")
            {
                ssid = value;
            }
            else if (key == "WIFI_PASSWORD")
            {
                password = value;
            }
        }
    }

    return !ssid.empty() && !password.empty();
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_real_load_wifi_credentials()
{
    std::string ssid, password;
    bool loaded = load_wifi_credentials(ssid, password);

    TEST_ASSERT_TRUE_MESSAGE(loaded, "Failed to load WiFi credentials from .env");
    TEST_ASSERT_FALSE_MESSAGE(ssid.empty(), "SSID should not be empty");
    TEST_ASSERT_FALSE_MESSAGE(password.empty(), "Password should not be empty");

    printf("Loaded WiFi credentials:\n");
    printf("  SSID: %s\n", ssid.c_str());
    printf("  Password length: %zu characters\n", password.length());
}

void test_real_mock_with_env_credentials()
{
    // Create service with real connection mode enabled
    test::mocks::MockNetworkConfig config;
    config.use_real_connection = true;
    test::mocks::MockNetworkService network_service(config);

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load real credentials
    auto load_result = network_service.load_credentials_from_env(env_path);
    TEST_ASSERT_TRUE_MESSAGE(load_result.succeed(), "Failed to load credentials");

    // Verify they were stored
    auto ssid_result = network_service.get_stored_ssid();
    auto password_result = network_service.get_stored_password();

    TEST_ASSERT_TRUE_MESSAGE(ssid_result.succeed(), "Failed to get SSID");
    TEST_ASSERT_TRUE_MESSAGE(password_result.succeed(), "Failed to get password");

    printf("Mock stored credentials:\n");
    printf("  SSID: %s\n", ssid_result.value().c_str());
    printf("  Password length: %zu\n", password_result.value().length());

    // Attempt real connection with loaded credentials
    auto connect_result = network_service.connect();
    TEST_ASSERT_TRUE_MESSAGE(connect_result.succeed(), "Real connection should succeed");
    TEST_ASSERT_TRUE(network_service.is_connected());

    // Get network info (mock values in native, real values on embedded)
    auto ip_result = network_service.get_ip_address();
    auto rssi_result = network_service.get_rssi();

    TEST_ASSERT_TRUE(ip_result.succeed());
    TEST_ASSERT_TRUE(rssi_result.succeed());

    printf("Network info (real connection mode):\n");
    printf("  IP: %s\n", ip_result.value().c_str());
    printf("  RSSI: %d dBm\n", rssi_result.value());
}

void test_real_connection_error_handling()
{
    test::mocks::MockNetworkService network_service;

    // Test with invalid path
    auto load_result = network_service.load_credentials_from_env("/invalid/path/.env");
    TEST_ASSERT_TRUE_MESSAGE(load_result.failed(), "Should fail with invalid path");

    // Test getting credentials when not set
    auto ssid_result = network_service.get_stored_ssid();
    auto password_result = network_service.get_stored_password();

    TEST_ASSERT_TRUE_MESSAGE(ssid_result.failed(), "Should fail when SSID not set");
    TEST_ASSERT_TRUE_MESSAGE(password_result.failed(), "Should fail when password not set");

    printf("Error messages:\n");
    printf("  Load error: %s\n", load_result.error().message().c_str());
    printf("  SSID error: %s\n", ssid_result.error().message().c_str());
    printf("  Password error: %s\n", password_result.error().message().c_str());
}

void test_real_simulated_connection_failure()
{
    // Create service with connection failure configured
    test::mocks::MockNetworkConfig config;
    config.connect_should_succeed = false;
    test::mocks::MockNetworkService network_service(config);

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load real credentials
    network_service.load_credentials_from_env(env_path);

    // Try to connect (should fail)
    auto connect_result = network_service.connect();

    TEST_ASSERT_TRUE_MESSAGE(connect_result.failed(), "Connection should fail when configured to fail");
    TEST_ASSERT_FALSE(network_service.is_connected());

    printf("Simulated connection failure error: %s\n", connect_result.error().message().c_str());
}

void test_real_connection_without_credentials()
{
    // Create service with real connection mode enabled
    test::mocks::MockNetworkConfig config;
    config.use_real_connection = true;
    test::mocks::MockNetworkService network_service(config);

    // Try to connect without setting credentials
    auto connect_result = network_service.connect();

    TEST_ASSERT_TRUE_MESSAGE(connect_result.failed(), "Connection should fail without credentials");
    TEST_ASSERT_FALSE(network_service.is_connected());

    printf("Real connection without credentials error: %s\n", connect_result.error().message().c_str());
}

void test_real_reconnection_scenario()
{
    // Create service with real connection mode enabled
    test::mocks::MockNetworkConfig config;
    config.use_real_connection = true;
    test::mocks::MockNetworkService network_service(config);

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load real credentials and connect
    network_service.load_credentials_from_env(env_path);
    auto connect_result = network_service.connect();
    TEST_ASSERT_TRUE(connect_result.succeed());
    TEST_ASSERT_TRUE(network_service.is_connected());

    // Disconnect
    network_service.disconnect();
    TEST_ASSERT_FALSE(network_service.is_connected());

    // Reconnect
    auto reconnect_result = network_service.connect();
    TEST_ASSERT_TRUE(reconnect_result.succeed());
    TEST_ASSERT_TRUE(network_service.is_connected());

    printf("Real mode reconnection successful\n");
}

void test_real_network_operations()
{
    // Create service with real connection mode enabled
    test::mocks::MockNetworkConfig config;
    config.use_real_connection = true;
    test::mocks::MockNetworkService network_service(config);

#ifdef NATIVE_TEST
    std::string env_path = std::string(PROJECT_DIR) + "/.env";
#else
    std::string env_path = "/.env";
#endif

    // Load and connect
    auto load_result = network_service.load_credentials_from_env(env_path);
    TEST_ASSERT_TRUE(load_result.succeed());

    auto connect_result = network_service.connect();
    TEST_ASSERT_TRUE(connect_result.succeed());

    // Test getting network info
    auto ip_result = network_service.get_ip_address();
    TEST_ASSERT_TRUE(ip_result.succeed());
    printf("  Real mode - Connected with IP: %s\n", ip_result.value().c_str());

    auto rssi_result = network_service.get_rssi();
    TEST_ASSERT_TRUE(rssi_result.succeed());
    printf("  Real mode - Signal strength: %d dBm\n", rssi_result.value());

    // Test download simulation (still mocked even in real connection mode)
    bool download_completed = false;
    auto download_result = network_service.download_file(
        "http://example.com/test.bin",
        [&download_completed](const uint8_t *data, size_t length) -> bool
        {
            download_completed = true;
            return true;
        });

    TEST_ASSERT_TRUE(download_result.succeed());
    TEST_ASSERT_TRUE(download_completed);
    printf("  Real mode - Download test completed\n");
}

#ifdef NATIVE_TEST
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_real_load_wifi_credentials);
    RUN_TEST(test_real_mock_with_env_credentials);
    RUN_TEST(test_real_connection_error_handling);
    RUN_TEST(test_real_simulated_connection_failure);
    RUN_TEST(test_real_connection_without_credentials);
    RUN_TEST(test_real_reconnection_scenario);
    RUN_TEST(test_real_network_operations);

    return UNITY_END();
}
#else
void setup()
{
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_real_load_wifi_credentials);
    RUN_TEST(test_real_mock_with_env_credentials);
    RUN_TEST(test_real_connection_error_handling);
    RUN_TEST(test_real_simulated_connection_failure);
    RUN_TEST(test_real_connection_without_credentials);
    RUN_TEST(test_real_reconnection_scenario);
    RUN_TEST(test_real_network_operations);

    UNITY_END();
}

void loop() {}
#endif
