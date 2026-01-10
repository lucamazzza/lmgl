#include "lmgl/assets/texture_library.hpp"
#include <gtest/gtest.h>

namespace lmgl {

namespace assets {

class TextureLibraryTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Clear library before each test
        TextureLibrary::get_instance().clear();
    }

    void TearDown() override {
        // Clean up after tests
        TextureLibrary::get_instance().clear();
    }
};

TEST_F(TextureLibraryTest, Singleton) {
    auto &lib1 = TextureLibrary::get_instance();
    auto &lib2 = TextureLibrary::get_instance();

    EXPECT_EQ(&lib1, &lib2);
}

// === Basic Operations ===

TEST_F(TextureLibraryTest, InitiallyEmpty) {
    auto &lib = TextureLibrary::get_instance();
    EXPECT_EQ(lib.size(), 0);
}

TEST_F(TextureLibraryTest, ExistsReturnsFalseForNonExistent) {
    auto &lib = TextureLibrary::get_instance();
    EXPECT_FALSE(lib.exists("nonexistent.png"));
}

TEST_F(TextureLibraryTest, GetReturnsNullptrForNonExistent) {
    auto &lib = TextureLibrary::get_instance();
    EXPECT_EQ(lib.get("nonexistent.png"), nullptr);
}

TEST_F(TextureLibraryTest, Clear) {
    auto &lib = TextureLibrary::get_instance();
    lib.clear();
    EXPECT_EQ(lib.size(), 0);
}

} // namespace assets

} // namespace lmgl
