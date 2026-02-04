#include "lmgl/scene/material.hpp"
#include <glm/glm.hpp>
#include <gtest/gtest.h>

namespace lmgl {

namespace scene {

class MaterialTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
        material = std::make_shared<Material>("TestMaterial");
    }

    void TearDown() override {
        // Cleanup code after each test
    }

    std::shared_ptr<Material> material;
};

TEST_F(MaterialTest, DefaultConstructor) {
    Material mat;
    EXPECT_EQ(mat.get_name(), "Default Material");
}

TEST_F(MaterialTest, NamedConstructor) { EXPECT_EQ(material->get_name(), "TestMaterial"); }

TEST_F(MaterialTest, SetName) {
    material->set_name("NewName");
    EXPECT_EQ(material->get_name(), "NewName");
}

TEST_F(MaterialTest, DefaultAlbedo) { EXPECT_EQ(material->get_albedo(), glm::vec3(1.0f, 1.0f, 1.0f)); }

TEST_F(MaterialTest, SetAlbedo) {
    glm::vec3 color(0.5f, 0.7f, 0.3f);
    material->set_albedo(color);
    EXPECT_EQ(material->get_albedo(), color);
}

TEST_F(MaterialTest, DefaultMetallic) { EXPECT_FLOAT_EQ(material->get_metallic(), 0.0f); }

TEST_F(MaterialTest, SetMetallic) {
    material->set_metallic(0.8f);
    EXPECT_FLOAT_EQ(material->get_metallic(), 0.8f);
}

TEST_F(MaterialTest, DefaultRoughness) { EXPECT_FLOAT_EQ(material->get_roughness(), 0.5f); }

TEST_F(MaterialTest, SetRoughness) {
    material->set_roughness(0.3f);
    EXPECT_FLOAT_EQ(material->get_roughness(), 0.3f);
}

TEST_F(MaterialTest, DefaultAO) { EXPECT_FLOAT_EQ(material->get_ao(), 1.0f); }

TEST_F(MaterialTest, SetAO) {
    material->set_ao(0.6f);
    EXPECT_FLOAT_EQ(material->get_ao(), 0.6f);
}

TEST_F(MaterialTest, DefaultEmissive) { EXPECT_EQ(material->get_emissive(), glm::vec3(0.0f, 0.0f, 0.0f)); }

TEST_F(MaterialTest, SetEmissive) {
    glm::vec3 emissive(1.0f, 0.5f, 0.0f);
    material->set_emissive(emissive);
    EXPECT_EQ(material->get_emissive(), emissive);
}

TEST_F(MaterialTest, DefaultTextureMapsAreNull) {
    EXPECT_EQ(material->get_albedo_map(), nullptr);
    EXPECT_EQ(material->get_normal_map(), nullptr);
    EXPECT_EQ(material->get_metallic_map(), nullptr);
    EXPECT_EQ(material->get_roughness_map(), nullptr);
    EXPECT_EQ(material->get_ao_map(), nullptr);
    EXPECT_EQ(material->get_emissive_map(), nullptr);
}

TEST_F(MaterialTest, SetTextureMap) {
    auto dummy_texture = std::shared_ptr<lmgl::renderer::Texture>(nullptr);
    material->set_albedo_map(dummy_texture);
    EXPECT_EQ(material->get_albedo_map(), dummy_texture);
}

TEST_F(MaterialTest, MetallicWorkflow) {
    material->set_albedo(glm::vec3(0.8f, 0.8f, 0.8f));
    material->set_metallic(1.0f);
    material->set_roughness(0.2f);

    EXPECT_EQ(material->get_albedo(), glm::vec3(0.8f, 0.8f, 0.8f));
    EXPECT_FLOAT_EQ(material->get_metallic(), 1.0f);
    EXPECT_FLOAT_EQ(material->get_roughness(), 0.2f);
}

TEST_F(MaterialTest, DielectricWorkflow) {
    material->set_albedo(glm::vec3(0.3f, 0.5f, 0.8f));
    material->set_metallic(0.0f);
    material->set_roughness(0.7f);

    EXPECT_EQ(material->get_albedo(), glm::vec3(0.3f, 0.5f, 0.8f));
    EXPECT_FLOAT_EQ(material->get_metallic(), 0.0f);
    EXPECT_FLOAT_EQ(material->get_roughness(), 0.7f);
}

} // namespace scene

} // namespace lmgl
