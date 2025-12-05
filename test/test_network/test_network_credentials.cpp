#include "test_network_helpers.h"

// ============================================================================
// Credentials Management Tests
// ============================================================================

void test_network_set_credentials()
{
    mock_network->set_credentials("TestSSID", "TestPassword");

    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();

    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_EQUAL_STRING("TestSSID", ssid_result.value().c_str());
    TEST_ASSERT_EQUAL_STRING("TestPassword", password_result.value().c_str());
}

void test_network_load_credentials_from_env()
{
#ifdef NATIVE_TEST
    // For native tests, use the project directory from the build system
    const char *env_path = PROJECT_DIR "/.env";
#else
    // For embedded tests, this test will be skipped as filesystem operations
    // are not available on the embedded platform
    const char *env_path = "/.env";
#endif

    auto load_result = mock_network->load_credentials_from_env(env_path);
    TEST_ASSERT_TRUE(load_result.succeed());

    // Verify credentials were loaded
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();

    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_FALSE(ssid_result.value().empty());
    TEST_ASSERT_FALSE(password_result.value().empty());
}

void test_network_get_stored_credentials()
{
    mock_network->set_credentials("MyNetwork", "MyPassword");

    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();

    TEST_ASSERT_TRUE(ssid_result.succeed());
    TEST_ASSERT_TRUE(password_result.succeed());
    TEST_ASSERT_EQUAL_STRING("MyNetwork", ssid_result.value().c_str());
    TEST_ASSERT_EQUAL_STRING("MyPassword", password_result.value().c_str());
}

void test_network_load_credentials_invalid_path()
{
    auto load_result = mock_network->load_credentials_from_env("/nonexistent/path/.env");
    TEST_ASSERT_TRUE(load_result.failed());
}

void test_network_get_credentials_when_not_set()
{
    // Don't set any credentials
    auto ssid_result = mock_network->get_stored_ssid();
    auto password_result = mock_network->get_stored_password();

    TEST_ASSERT_TRUE(ssid_result.failed());
    TEST_ASSERT_TRUE(password_result.failed());
}
