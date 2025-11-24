#include <unity.h>
#include "libs/plant_nanny/services/ota/OTAState.h"
#include "libs/common/utils/EspError.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

using namespace plant_nanny::services::ota;

void setUp(void) {}
void tearDown(void) {}

// OTAState tests - no service registry needed
void test_ota_state_initial_state()
{
    OTAState state;
    TEST_ASSERT_FALSE(state.is_updating());
    TEST_ASSERT_EQUAL(0, state.bytes_written());
    TEST_ASSERT_NULL(state.partition());
}

void test_ota_state_begin_update()
{
    OTAState state;
    const esp_partition_t* mock_partition = (const esp_partition_t*)0x1234;
    esp_ota_handle_t mock_handle = 0x5678;
    
    state.begin_update(mock_partition, mock_handle);
    
    TEST_ASSERT_TRUE(state.is_updating());
    TEST_ASSERT_EQUAL(mock_partition, state.partition());
    TEST_ASSERT_EQUAL(mock_handle, state.handle());
}

void test_ota_state_add_bytes()
{
    OTAState state;
    const esp_partition_t* mock_partition = (const esp_partition_t*)0x1234;
    esp_ota_handle_t mock_handle = 0x5678;
    
    state.begin_update(mock_partition, mock_handle);
    state.add_bytes(100);
    TEST_ASSERT_EQUAL(100, state.bytes_written());
    
    state.add_bytes(50);
    TEST_ASSERT_EQUAL(150, state.bytes_written());
}

void test_ota_state_reset()
{
    OTAState state;
    const esp_partition_t* mock_partition = (const esp_partition_t*)0x1234;
    esp_ota_handle_t mock_handle = 0x5678;
    
    state.begin_update(mock_partition, mock_handle);
    state.add_bytes(1000);
    TEST_ASSERT_TRUE(state.is_updating());
    
    state.reset();
    TEST_ASSERT_FALSE(state.is_updating());
    TEST_ASSERT_EQUAL(0, state.bytes_written());
}

// EspError tests
void test_esp_error_is_ok()
{
    TEST_ASSERT_TRUE(common::utils::EspError::is_ok(ESP_OK));
    TEST_ASSERT_FALSE(common::utils::EspError::is_ok(ESP_FAIL));
}

void test_esp_error_to_result()
{
    auto success = common::utils::EspError::to_result(ESP_OK, "Test");
    TEST_ASSERT_TRUE(success.succeed());
    
    auto failure = common::utils::EspError::to_result(ESP_FAIL, "Test");
    TEST_ASSERT_TRUE(failure.failed());
}

void test_esp_error_name()
{
    std::string name = common::utils::EspError::error_name(ESP_OK);
    TEST_ASSERT_FALSE(name.empty());
}

#ifdef NATIVE_TEST
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    
    RUN_TEST(test_ota_state_initial_state);
    RUN_TEST(test_ota_state_begin_update);
    RUN_TEST(test_ota_state_add_bytes);
    RUN_TEST(test_ota_state_reset);
    RUN_TEST(test_esp_error_is_ok);
    RUN_TEST(test_esp_error_to_result);
    RUN_TEST(test_esp_error_name);
    
    return UNITY_END();
}
#else
void setup()
{
    delay(2000);
    UNITY_BEGIN();
    
    RUN_TEST(test_ota_state_initial_state);
    RUN_TEST(test_ota_state_begin_update);
    RUN_TEST(test_ota_state_add_bytes);
    RUN_TEST(test_ota_state_reset);
    RUN_TEST(test_esp_error_is_ok);
    RUN_TEST(test_esp_error_to_result);
    RUN_TEST(test_esp_error_name);
    
    UNITY_END();
}

void loop() {}
#endif
