#include <unity.h>
#include "testing/network/helpers.h"

// Global test state definition
std::unique_ptr<MockNetworkService> mock_network = nullptr;

// Unity setup and teardown
void setUp()
{
    mock_network = std::make_unique<MockNetworkService>();
}

void tearDown()
{
    mock_network.reset();
}

// Test function declarations from separate .cpp files
// Credentials tests (test_network_credentials.cpp)
void test_network_set_credentials();
void test_network_load_credentials_from_env();
void test_network_get_stored_credentials();
void test_network_load_credentials_invalid_path();
void test_network_get_credentials_when_not_set();

// Connection tests (test_network_connection.cpp)
void test_network_connect_success();
void test_network_connect_failure();
void test_network_disconnect();
void test_network_is_connected();
void test_network_get_ip_address_when_connected();
void test_network_get_ip_address_when_disconnected();
void test_network_get_rssi_when_connected();
void test_network_get_rssi_when_disconnected();

// Download tests (test_network_download.cpp)
void test_network_download_success();
void test_network_download_failure();
void test_network_download_when_disconnected();
void test_network_download_chunk_handler_rejects();

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
#include <Arduino.h>

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
