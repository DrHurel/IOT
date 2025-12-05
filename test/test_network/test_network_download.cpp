#include "test_network_helpers.h"

// ============================================================================
// File Download Tests
// ============================================================================

void test_network_download_success()
{
    testing::mocks::MockNetworkConfig config;
    config.download_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);

    bool chunk_received = false;
    bool progress_called = false;

    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [&chunk_received](const uint8_t *data, size_t length) -> bool
        {
            chunk_received = true;
            TEST_ASSERT_NOT_NULL(data);
            TEST_ASSERT_GREATER_THAN(0, length);
            return true;
        },
        [&progress_called](size_t downloaded, size_t total)
        {
            progress_called = true;
        });

    TEST_ASSERT_TRUE(result.succeed());
    TEST_ASSERT_TRUE(chunk_received);
    TEST_ASSERT_TRUE(progress_called);
}

void test_network_download_failure()
{
    testing::mocks::MockNetworkConfig config;
    config.download_should_succeed = false;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);

    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t *data, size_t length) -> bool
        { return true; });

    TEST_ASSERT_TRUE(result.failed());
}

void test_network_download_when_disconnected()
{
    mock_network->test_set_connected(false);

    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t *data, size_t length) -> bool
        { return true; });

    TEST_ASSERT_TRUE(result.failed());
}

void test_network_download_chunk_handler_rejects()
{
    testing::mocks::MockNetworkConfig config;
    config.download_should_succeed = true;
    mock_network->test_update_config(config);
    mock_network->test_set_connected(true);

    auto result = mock_network->download_file(
        "http://example.com/firmware.bin",
        [](const uint8_t *data, size_t length) -> bool
        {
            return false; // Reject the chunk
        });

    TEST_ASSERT_TRUE(result.failed());
}
