#include <gtest/gtest.h>
#include <uutid.hpp>
#include <thread>
#include <chrono>
#include <set>
#include <sstream>
#include <cmath> 

class UUTIDTest : public ::testing::Test {
protected:
    static std::chrono::system_clock::time_point truncateToSeconds(std::chrono::system_clock::time_point tp) {
        auto duration = tp.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        return std::chrono::system_clock::time_point(seconds);
    }
};

TEST_F(UUTIDTest, NewGeneratesValidID) {
    auto now = std::chrono::system_clock::now();
    
    auto uutid = UUTID::new_id();
    EXPECT_NE(uutid.to_string(), UUTID().to_string());
    
    auto uutid_time = uutid.time();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(
        uutid_time - now).count();
    
    EXPECT_GE(diff, -5);
    EXPECT_LE(diff, 5);
}

TEST_F(UUTIDTest, NewWithTimeNow) {
    auto now = std::chrono::system_clock::now();
    auto uutid = UUTID::new_with_time(now);
    
    EXPECT_NE(uutid.to_string(), UUTID().to_string());
    
    auto uutid_time = uutid.time();
    // Fix the time difference calculation
    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(
        uutid_time - now).count();
    EXPECT_LE(std::abs(diff), 100);  // Use std::abs instead of std::chrono::abs
}

TEST_F(UUTIDTest, NewWithCustomTime) {
    auto test_time = std::chrono::system_clock::from_time_t(1610845510); // 2021-01-17 01:05:10 UTC
    test_time += std::chrono::nanoseconds(123456900);
    
    auto uutid = UUTID::new_with_time(test_time);
    EXPECT_NE(uutid.to_string(), UUTID().to_string());
    
    auto uutid_time = uutid.time();
    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(
        test_time - uutid_time).count();
    
    EXPECT_EQ(diff, 0);
    
    std::string uuid = uutid.to_uuid_string();
    EXPECT_EQ(uuid.substr(0, 18), "60038d46-1d6f-4361");
}

TEST_F(UUTIDTest, UUIDTimestampExtraction) {
    auto test_time = std::chrono::system_clock::from_time_t(1610845510);
    test_time += std::chrono::nanoseconds(123456900);
    
    auto uutid = UUTID::new_with_time(test_time);
    std::string uuid = uutid.to_uuid_string();
    
    std::string timestamp_hex = uuid.substr(0, 8);
    uint32_t timestamp;
    std::stringstream ss;
    ss << std::hex << timestamp_hex;
    ss >> timestamp;
    
    auto expected_time = truncateToSeconds(test_time);
    auto actual_time = std::chrono::system_clock::from_time_t(timestamp);
    
    EXPECT_EQ(actual_time, expected_time);
}

TEST_F(UUTIDTest, SpecificTimestamp) {
    auto test_time = std::chrono::system_clock::from_time_t(7952935226LL) + 
                     std::chrono::nanoseconds(782162000);
    
    auto uutid = UUTID::new_with_time(test_time);
    std::string uuid = uutid.to_uuid_string();
    
    EXPECT_EQ(uuid.substr(0, 18), "da08293a-ba7b-4614");
}

TEST_F(UUTIDTest, CustomRandomGenerator) {
    auto custom_rng = std::make_unique<std::mt19937_64>(42);
    UUTID::set_rand(std::move(custom_rng));
    
    auto id1 = UUTID::new_id();
    auto id2 = UUTID::new_id();
    
    EXPECT_NE(id1.to_string(), id2.to_string());
    
    UUTID::set_rand(nullptr);
}

TEST_F(UUTIDTest, Version) {
    UUTID::set_version(5);
    auto uutid = UUTID::new_id();
    
    EXPECT_THROW(UUTID::set_version(10), std::runtime_error);
    EXPECT_THROW(UUTID::set_version(-1), std::runtime_error);
    
    UUTID::set_version(4);
}

TEST_F(UUTIDTest, StringConversions) {
    auto test_time = std::chrono::system_clock::from_time_t(1610845510);
    auto test_id = UUTID::new_with_time(test_time);
    
    // Base64
    std::string base64 = test_id.base64();
    EXPECT_EQ(base64.length(), 22);
    auto from_base64 = UUTID::from_base64(base64);
    EXPECT_EQ(test_id.to_string(), from_base64.to_string());
    
    // Hex/Base16
    std::string hex = test_id.to_string();
    EXPECT_EQ(hex.length(), 32);
    auto from_hex = UUTID::from_string(hex);
    EXPECT_EQ(test_id.to_string(), from_hex.to_string());
    
    // UUID format
    std::string uuid = test_id.to_uuid_string();
    EXPECT_EQ(uuid.length(), 36);
    auto from_uuid = UUTID::from_string(uuid);
    EXPECT_EQ(test_id.to_string(), from_uuid.to_string());
}

TEST_F(UUTIDTest, InvalidInputs) {
    // Invalid base64
    EXPECT_THROW(UUTID::from_base64("invalid"), std::runtime_error);
    EXPECT_THROW(UUTID::from_base64("@@@@@@@@@@@@@@@@@@@@@@"), std::runtime_error);
    
    // Invalid hex
    EXPECT_THROW(UUTID::from_string("invalid"), std::runtime_error);
    EXPECT_THROW(UUTID::from_string("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"), std::runtime_error);
    
    // Invalid UUID
    EXPECT_THROW(UUTID::from_string("invalid-uuid-format"), std::runtime_error);
}

TEST_F(UUTIDTest, TimeOrdering) {
    auto id1 = UUTID::new_id();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto id2 = UUTID::new_id();
    
    auto time1 = id1.time();
    auto time2 = id2.time();
    
    EXPECT_LT(time1, time2);
}

TEST_F(UUTIDTest, Uniqueness) {
    std::set<std::string> ids;
    const int num_ids = 1000;
    
    for (int i = 0; i < num_ids; ++i) {
        auto id = UUTID::new_id();
        std::string id_str = id.to_string();
        EXPECT_TRUE(ids.insert(id_str).second) << "Generated duplicate ID: " << id_str;
    }
    
    EXPECT_EQ(ids.size(), num_ids);
}

TEST_F(UUTIDTest, FromStringAllFormats) {
    auto original = UUTID::new_id();
    
    // Test binary format
    auto& bytes = original.bytes();
    std::string binary(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    EXPECT_EQ(binary.length(), 16);
    
    // Test base64
    std::string base64 = original.base64();
    EXPECT_EQ(base64.length(), 22);
    
    // Test hex/base16
    std::string hex = original.to_string();
    EXPECT_EQ(hex.length(), 32);
    
    // Test UUID
    std::string uuid = original.to_uuid_string();
    EXPECT_EQ(uuid.length(), 36);
    
    // Test conversions
    auto from_base64 = UUTID::from_base64(base64);
    auto from_hex = UUTID::from_string(hex);
    auto from_uuid = UUTID::from_string(uuid);
    
    EXPECT_EQ(original.to_string(), from_base64.to_string());
    EXPECT_EQ(original.to_string(), from_hex.to_string());
    EXPECT_EQ(original.to_string(), from_uuid.to_string());
}
