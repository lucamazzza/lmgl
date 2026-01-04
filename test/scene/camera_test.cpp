#include <gtest/gtest.h>
#include "lmgl/scene/camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace lmgl {

namespace scene {

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code before each test
        camera = std::make_unique<Camera>();
    }

    void TearDown() override {
        // Cleanup code after each test
    }
    
    std::unique_ptr<Camera> camera;
    
    const float EPSILON = 0.0001f;
    
    bool vec3_equals(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f) {
        return std::abs(a.x - b.x) < epsilon &&
               std::abs(a.y - b.y) < epsilon &&
               std::abs(a.z - b.z) < epsilon;
    }
    
    bool mat4_equals(const glm::mat4& a, const glm::mat4& b, float epsilon = 0.0001f) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (std::abs(a[i][j] - b[i][j]) >= epsilon) {
                    return false;
                }
            }
        }
        return true;
    }
};

TEST_F(CameraTest, DefaultConstructor) {
    EXPECT_TRUE(vec3_equals(camera->get_position(), glm::vec3(0.0f, 0.0f, 3.0f)));
    EXPECT_TRUE(vec3_equals(camera->get_target(), glm::vec3(0.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(vec3_equals(camera->get_up(), glm::vec3(0.0f, 1.0f, 0.0f)));
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Perspective);
}

TEST_F(CameraTest, CustomConstructor) {
    Camera custom_camera(60.0f, 1.5f, 0.5f, 200.0f);
    EXPECT_EQ(custom_camera.get_projection_mode(), Camera::ProjectionMode::Perspective);
}

TEST_F(CameraTest, SetPosition) {
    glm::vec3 new_pos(5.0f, 10.0f, 15.0f);
    camera->set_position(new_pos);
    EXPECT_TRUE(vec3_equals(camera->get_position(), new_pos));
}

TEST_F(CameraTest, SetTarget) {
    glm::vec3 new_target(1.0f, 2.0f, 3.0f);
    camera->set_target(new_target);
    EXPECT_TRUE(vec3_equals(camera->get_target(), new_target));
}

TEST_F(CameraTest, SetUp) {
    glm::vec3 new_up(0.0f, 0.0f, 1.0f);
    camera->set_up(new_up);
    EXPECT_TRUE(vec3_equals(camera->get_up(), new_up));
}

TEST_F(CameraTest, SetPerspective) {
    camera->set_perspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Perspective);
    
    glm::mat4 proj = camera->get_projection_matrix();
    glm::mat4 identity(1.0f);
    EXPECT_FALSE(mat4_equals(proj, identity));
}

TEST_F(CameraTest, SetOrthographic) {
    camera->set_orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Orthographic);
    
    glm::mat4 proj = camera->get_projection_matrix();
    glm::mat4 identity(1.0f);
    EXPECT_FALSE(mat4_equals(proj, identity));
}

TEST_F(CameraTest, PerspectiveProjectionMatrixCorrectness) {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float near = 0.1f;
    float far = 100.0f;
    
    camera->set_perspective(fov, aspect, near, far);
    glm::mat4 camera_proj = camera->get_projection_matrix();
    glm::mat4 expected_proj = glm::perspective(glm::radians(fov), aspect, near, far);
    
    EXPECT_TRUE(mat4_equals(camera_proj, expected_proj));
}

TEST_F(CameraTest, OrthographicProjectionMatrixCorrectness) {
    float left = -5.0f, right = 5.0f;
    float bottom = -5.0f, top = 5.0f;
    float near = 0.1f, far = 100.0f;
    
    camera->set_orthographic(left, right, bottom, top, near, far);
    glm::mat4 camera_proj = camera->get_projection_matrix();
    glm::mat4 expected_proj = glm::ortho(left, right, bottom, top, near, far);
    
    EXPECT_TRUE(mat4_equals(camera_proj, expected_proj));
}

TEST_F(CameraTest, ViewMatrixCorrectness) {
    glm::vec3 pos(0.0f, 0.0f, 5.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    camera->set_position(pos);
    camera->set_target(target);
    camera->set_up(up);
    
    glm::mat4 view = camera->get_view_matrix();
    glm::mat4 expected = glm::lookAt(pos, target, up);
    
    EXPECT_TRUE(mat4_equals(view, expected));
}

TEST_F(CameraTest, ViewProjectionMatrix) {
    glm::mat4 view_proj = camera->get_view_projection_matrix();
    glm::mat4 expected = camera->get_projection_matrix() * camera->get_view_matrix();
    
    EXPECT_TRUE(mat4_equals(view_proj, expected));
}

TEST_F(CameraTest, UnprojectScreenCenter) {
    camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    
    float width = 800.0f;
    float height = 600.0f;
    
    glm::vec3 ray = camera->unproject(width / 2.0f, height / 2.0f, width, height);
    
    float length = glm::length(ray);
    EXPECT_NEAR(length, 1.0f, EPSILON);
    
    EXPECT_LT(ray.z, 0.0f);
}

TEST_F(CameraTest, UnprojectReturnsNormalizedVector) {
    float width = 1920.0f;
    float height = 1080.0f;
    
    glm::vec3 ray_tl = camera->unproject(0.0f, 0.0f, width, height);
    glm::vec3 ray_br = camera->unproject(width, height, width, height);
    glm::vec3 ray_center = camera->unproject(width / 2.0f, height / 2.0f, width, height);
    
    EXPECT_NEAR(glm::length(ray_tl), 1.0f, EPSILON);
    EXPECT_NEAR(glm::length(ray_br), 1.0f, EPSILON);
    EXPECT_NEAR(glm::length(ray_center), 1.0f, EPSILON);
}

TEST_F(CameraTest, UnprojectDifferentScreenPositions) {
    float width = 800.0f;
    float height = 600.0f;
    
    glm::vec3 ray_left = camera->unproject(0.0f, height / 2.0f, width, height);
    glm::vec3 ray_right = camera->unproject(width, height / 2.0f, width, height);
    glm::vec3 ray_top = camera->unproject(width / 2.0f, 0.0f, width, height);
    glm::vec3 ray_bottom = camera->unproject(width / 2.0f, height, width, height);
    
    EXPECT_FALSE(vec3_equals(ray_left, ray_right));
    EXPECT_FALSE(vec3_equals(ray_top, ray_bottom));
}

TEST_F(CameraTest, CameraLookingDown) {
    camera->set_position(glm::vec3(0.0f, 10.0f, 0.0f));
    camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->set_up(glm::vec3(0.0f, 0.0f, -1.0f));
    
    glm::mat4 view = camera->get_view_matrix();
    EXPECT_FALSE(mat4_equals(view, glm::mat4(1.0f)));
}

TEST_F(CameraTest, MultipleProjectionSwitches) {
    camera->set_perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Perspective);
    
    camera->set_orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Orthographic);
    
    camera->set_perspective(60.0f, 4.0f / 3.0f, 0.5f, 500.0f);
    EXPECT_EQ(camera->get_projection_mode(), Camera::ProjectionMode::Perspective);
}

} // namespace scene

} // namespace lmgl
