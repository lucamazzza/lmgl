#include <gtest/gtest.h>
#include "lmgl/scene/light.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace lmgl::scene;

class LightTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
        dir_light = std::make_shared<Light>(LightType::Directional);
        point_light = std::make_shared<Light>(LightType::Point);
        spot_light = std::make_shared<Light>(LightType::Spot);
    }

    void TearDown() override {
        // Cleanup code after each test
    }

    std::shared_ptr<Light> dir_light;
    std::shared_ptr<Light> point_light;
    std::shared_ptr<Light> spot_light;
};

TEST_F(LightTest, DefaultConstructor) {
    Light light;
    EXPECT_EQ(light.get_type(), LightType::Point);
}

TEST_F(LightTest, TypedConstructor) {
    EXPECT_EQ(dir_light->get_type(), LightType::Directional);
    EXPECT_EQ(point_light->get_type(), LightType::Point);
    EXPECT_EQ(spot_light->get_type(), LightType::Spot);
}

TEST_F(LightTest, DefaultColor) {
    EXPECT_EQ(dir_light->get_color(), glm::vec3(1.0f, 1.0f, 1.0f));
}

TEST_F(LightTest, SetColor) {
    glm::vec3 color(1.0f, 0.5f, 0.0f);
    dir_light->set_color(color);
    EXPECT_EQ(dir_light->get_color(), color);
}

TEST_F(LightTest, DefaultIntensity) {
    EXPECT_FLOAT_EQ(dir_light->get_intensity(), 1.0f);
}

TEST_F(LightTest, SetIntensity) {
    dir_light->set_intensity(2.5f);
    EXPECT_FLOAT_EQ(dir_light->get_intensity(), 2.5f);
}

TEST_F(LightTest, DefaultDirection) {
    EXPECT_EQ(dir_light->get_direction(), glm::vec3(0.0f, -1.0f, 0.0f));
}

TEST_F(LightTest, SetDirection) {
    glm::vec3 dir(1.0f, -1.0f, 0.0f);
    dir_light->set_direction(dir);
    
    // Should be normalized
    glm::vec3 expected = glm::normalize(dir);
    EXPECT_NEAR(dir_light->get_direction().x, expected.x, 0.0001f);
    EXPECT_NEAR(dir_light->get_direction().y, expected.y, 0.0001f);
    EXPECT_NEAR(dir_light->get_direction().z, expected.z, 0.0001f);
}

TEST_F(LightTest, DirectionIsNormalized) {
    glm::vec3 dir(5.0f, 3.0f, 2.0f);
    dir_light->set_direction(dir);
    
    float length = glm::length(dir_light->get_direction());
    EXPECT_NEAR(length, 1.0f, 0.0001f);
}

TEST_F(LightTest, DefaultPosition) {
    EXPECT_EQ(point_light->get_position(), glm::vec3(0.0f, 0.0f, 0.0f));
}

TEST_F(LightTest, SetPosition) {
    glm::vec3 pos(5.0f, 10.0f, -3.0f);
    point_light->set_position(pos);
    EXPECT_EQ(point_light->get_position(), pos);
}

TEST_F(LightTest, DefaultRange) {
    EXPECT_FLOAT_EQ(point_light->get_range(), 10.0f);
}

TEST_F(LightTest, SetRange) {
    point_light->set_range(25.0f);
    EXPECT_FLOAT_EQ(point_light->get_range(), 25.0f);
}

TEST_F(LightTest, DefaultInnerConeAngle) {
    float expected = glm::radians(30.0f);
    EXPECT_NEAR(spot_light->get_inner_cone(), expected, 0.0001f);
}

TEST_F(LightTest, SetInnerConeAngle) {
    float angle = glm::radians(20.0f);
    spot_light->set_inner_cone(angle);
    EXPECT_NEAR(spot_light->get_inner_cone(), angle, 0.0001f);
}

TEST_F(LightTest, DefaultOuterConeAngle) {
    float expected = glm::radians(45.0f);
    EXPECT_NEAR(spot_light->get_outer_cone(), expected, 0.0001f);
}

TEST_F(LightTest, SetOuterConeAngle) {
    float angle = glm::radians(60.0f);
    spot_light->set_outer_cone(angle);
    EXPECT_NEAR(spot_light->get_outer_cone(), angle, 0.0001f);
}

TEST_F(LightTest, DefaultShadowCasting) {
    EXPECT_FALSE(dir_light->casts_shadows());
}

TEST_F(LightTest, SetCastsShadows) {
    dir_light->set_casts_shadows(true);
    EXPECT_TRUE(dir_light->casts_shadows());
    
    dir_light->set_casts_shadows(false);
    EXPECT_FALSE(dir_light->casts_shadows());
}

TEST_F(LightTest, CreateDirectional) {
    glm::vec3 direction(0.0f, -1.0f, -1.0f);
    glm::vec3 color(1.0f, 0.9f, 0.8f);
    
    auto light = Light::create_directional(direction, color);
    
    EXPECT_EQ(light->get_type(), LightType::Directional);
    EXPECT_EQ(light->get_color(), color);
    
    // Direction should be normalized
    float length = glm::length(light->get_direction());
    EXPECT_NEAR(length, 1.0f, 0.0001f);
}

TEST_F(LightTest, CreatePoint) {
    glm::vec3 position(5.0f, 3.0f, -2.0f);
    float range = 15.0f;
    glm::vec3 color(0.8f, 0.6f, 0.4f);
    
    auto light = Light::create_point(position, range, color);
    
    EXPECT_EQ(light->get_type(), LightType::Point);
    EXPECT_EQ(light->get_position(), position);
    EXPECT_FLOAT_EQ(light->get_range(), range);
    EXPECT_EQ(light->get_color(), color);
}

TEST_F(LightTest, CreateSpot) {
    glm::vec3 position(2.0f, 5.0f, 0.0f);
    glm::vec3 direction(0.0f, -1.0f, 0.0f);
    float angle = 30.0f;
    glm::vec3 color(1.0f, 1.0f, 0.5f);
    
    auto light = Light::create_spot(position, direction, angle, color);
    
    EXPECT_EQ(light->get_type(), LightType::Spot);
    EXPECT_EQ(light->get_position(), position);
    EXPECT_EQ(light->get_color(), color);
    
    // Check cone angles are in radians
    EXPECT_NEAR(light->get_outer_cone(), glm::radians(angle), 0.0001f);
    EXPECT_NEAR(light->get_inner_cone(), glm::radians(angle * 0.8f), 0.0001f);
}

// === Light Scenarios ===

TEST_F(LightTest, SunLight) {
    auto sun = Light::create_directional(
        glm::vec3(0.3f, -1.0f, -0.2f),
        glm::vec3(1.0f, 0.95f, 0.9f)
    );
    sun->set_intensity(3.0f);
    
    EXPECT_EQ(sun->get_type(), LightType::Directional);
    EXPECT_FLOAT_EQ(sun->get_intensity(), 3.0f);
}

TEST_F(LightTest, LampLight) {
    auto lamp = Light::create_point(
        glm::vec3(0.0f, 2.0f, 0.0f),
        5.0f,
        glm::vec3(1.0f, 0.8f, 0.6f)
    );
    lamp->set_intensity(10.0f);
    
    EXPECT_EQ(lamp->get_type(), LightType::Point);
    EXPECT_FLOAT_EQ(lamp->get_range(), 5.0f);
    EXPECT_FLOAT_EQ(lamp->get_intensity(), 10.0f);
}

TEST_F(LightTest, FlashlightLight) {
    auto flashlight = Light::create_spot(
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        25.0f
    );
    flashlight->set_intensity(50.0f);
    
    EXPECT_EQ(flashlight->get_type(), LightType::Spot);
    EXPECT_FLOAT_EQ(flashlight->get_intensity(), 50.0f);
}
