#include "testing/network/helpers.h"

// ============================================================================
// Connection Management Tests
// ============================================================================

void test_network_connect_success()
{
    testing::mocks::MockNetworkConfig config;
    config.connect_should_succeed = true;
    mock_network->test_update_config(config);
    auto result = mock_network->connect();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(mock_network->is_connected());
}

void test_network_connect_failure()
{
    testing::mocks::MockNetworkConfig config;
    config.connect_should_succeed = false;
    mock_network->test_update_config(config);
    auto result = mock_network->connect();
    TEST_ASSERT_TRUE(result.failed());
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_disconnect()
{
    testing::mocks::MockNetworkConfig config;
    config.connect_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->connect();
    TEST_ASSERT_TRUE(mock_network->is_connected());

    mock_network->disconnect();
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_is_connected()
{
    TEST_ASSERT_FALSE(mock_network->is_connected());

    mock_network->test_set_connected(true);
    TEST_ASSERT_TRUE(mock_network->is_connected());

    mock_network->test_set_connected(false);
    TEST_ASSERT_FALSE(mock_network->is_connected());
}

void test_network_get_ip_address_when_connected()
{
    testing::mocks::MockNetworkConfig config;
    config.mock_ip = "192.168.1.100";
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);

    auto result = mock_network->get_ip_address();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL_STRING("192.168.1.100", result.value().c_str());
}

void test_network_get_ip_address_when_disconnected()
{
    mock_network->test_set_connected(false);

    auto result = mock_network->get_ip_address();
    TEST_ASSERT_TRUE(result.failed());
}

void test_network_get_rssi_when_connected()
{
    testing::mocks::MockNetworkConfig config;
    config.mock_rssi = -50;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);

    auto result = mock_network->get_rssi();
    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_EQUAL(-50, result.value());
}

void test_network_get_rssi_when_disconnected()
{
    mock_network->test_set_connected(false);

    auto result = mock_network->get_rssi();
    TEST_ASSERT_TRUE(result.failed());
}
