#include "lmgl/scene/frustum.hpp"
#include "lmgl/scene/camera.hpp"
#include <gtest/gtest.h>
#include <glm/glm.hpp>

// Avoid name collision with glm::frustum
using lmgl::scene::Frustum;
using lmgl::scene::AABB;
using lmgl::scene::BoundingSphere;
using lmgl::scene::Plane;

class FrustumTest : public ::testing::Test {
protected:
    void SetUp() override {
        camera = std::make_shared<lmgl::scene::Camera>(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        camera->set_position(glm::vec3(0.0f, 0.0f, 5.0f));
        camera->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    std::shared_ptr<lmgl::scene::Camera> camera;
};

TEST_F(FrustumTest, AABBConstruction) {
    AABB aabb(glm::vec3(-1.0f), glm::vec3(1.0f));
    EXPECT_EQ(aabb.min, glm::vec3(-1.0f));
    EXPECT_EQ(aabb.max, glm::vec3(1.0f));
}

TEST_F(FrustumTest, AABBCenter) {
    AABB aabb(glm::vec3(-2.0f, -2.0f, -2.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    glm::vec3 center = aabb.get_center();
    EXPECT_EQ(center, glm::vec3(0.0f));
}

TEST_F(FrustumTest, AABBExtents) {
    AABB aabb(glm::vec3(-2.0f), glm::vec3(2.0f));
    glm::vec3 extents = aabb.get_extents();
    EXPECT_EQ(extents, glm::vec3(2.0f));
}

TEST_F(FrustumTest, AABBExpand) {
    AABB aabb;
    aabb.expand(glm::vec3(-1.0f, -1.0f, -1.0f));
    aabb.expand(glm::vec3(1.0f, 1.0f, 1.0f));
    EXPECT_EQ(aabb.min, glm::vec3(-1.0f));
    EXPECT_EQ(aabb.max, glm::vec3(1.0f));
}

TEST_F(FrustumTest, AABBMerge) {
    AABB aabb1(glm::vec3(-1.0f), glm::vec3(1.0f));
    AABB aabb2(glm::vec3(0.0f), glm::vec3(2.0f));
    aabb1.merge(aabb2);
    EXPECT_EQ(aabb1.min, glm::vec3(-1.0f));
    EXPECT_EQ(aabb1.max, glm::vec3(2.0f));
}

TEST_F(FrustumTest, BoundingSphereFromAABB) {
    AABB aabb(glm::vec3(-1.0f), glm::vec3(1.0f));
    BoundingSphere sphere = BoundingSphere::from_aabb(aabb);
    EXPECT_EQ(sphere.center, glm::vec3(0.0f));
    EXPECT_GT(sphere.radius, 0.0f);
}

TEST_F(FrustumTest, PlaneDistanceToPoint) {
    Plane plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float dist = plane.distance_to_point(glm::vec3(0.0f, 5.0f, 0.0f));
    EXPECT_GT(dist, 0.0f);
}

TEST_F(FrustumTest, FrustumUpdate) {
    Frustum frustum;
    glm::mat4 vp = camera->get_view_projection_matrix();
    
    EXPECT_NO_THROW(frustum.update(vp));
}

TEST_F(FrustumTest, FrustumContainsPoint) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    // Point in front of camera should be visible
    EXPECT_TRUE(frustum.contains_point(glm::vec3(0.0f, 0.0f, 0.0f)));
    
    // Point far behind camera should not be visible
    EXPECT_FALSE(frustum.contains_point(glm::vec3(0.0f, 0.0f, 200.0f)));
}

TEST_F(FrustumTest, FrustumContainsSphere) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    BoundingSphere sphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
    EXPECT_TRUE(frustum.contains_sphere(sphere));
    
    BoundingSphere far_sphere(glm::vec3(0.0f, 0.0f, 200.0f), 1.0f);
    EXPECT_FALSE(frustum.contains_sphere(far_sphere));
}

TEST_F(FrustumTest, FrustumContainsAABB) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    AABB aabb(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    EXPECT_TRUE(frustum.contains_aabb(aabb));
    
    AABB far_aabb(glm::vec3(199.0f), glm::vec3(201.0f));
    EXPECT_FALSE(frustum.contains_aabb(far_aabb));
}

TEST_F(FrustumTest, AABBTransform) {
    AABB aabb(glm::vec3(-1.0f), glm::vec3(1.0f));
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
    
    AABB transformed = aabb.transform(transform);
    
    EXPECT_EQ(transformed.get_center(), glm::vec3(5.0f, 0.0f, 0.0f));
}

TEST_F(FrustumTest, BoundingSphereTransform) {
    BoundingSphere sphere(glm::vec3(0.0f), 1.0f);
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
    
    BoundingSphere transformed = sphere.transform(transform);
    
    EXPECT_EQ(transformed.center, glm::vec3(3.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(transformed.radius, 1.0f);
}

TEST_F(FrustumTest, FrustumContainsOrigin) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    EXPECT_TRUE(frustum.contains_point(glm::vec3(0.0f, 0.0f, 0.0f)));
}

TEST_F(FrustumTest, FrustumDoesNotContainPointsBehindCamera) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    EXPECT_FALSE(frustum.contains_point(glm::vec3(0.0f, 0.0f, 10.0f)));
}

TEST_F(FrustumTest, FrustumDoesNotContainPointsBeyondFarPlane) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    EXPECT_FALSE(frustum.contains_point(glm::vec3(0.0f, 0.0f, -150.0f)));
}

TEST_F(FrustumTest, FrustumContainsPointsOnSides) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    EXPECT_TRUE(frustum.contains_point(glm::vec3(1.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(frustum.contains_point(glm::vec3(-1.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(frustum.contains_point(glm::vec3(0.0f, 1.0f, 0.0f)));
    EXPECT_TRUE(frustum.contains_point(glm::vec3(0.0f, -1.0f, 0.0f)));
}

TEST_F(FrustumTest, FrustumDoesNotContainPointsOutsideSides) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    EXPECT_FALSE(frustum.contains_point(glm::vec3(100.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(frustum.contains_point(glm::vec3(-100.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(frustum.contains_point(glm::vec3(0.0f, 100.0f, 0.0f)));
    EXPECT_FALSE(frustum.contains_point(glm::vec3(0.0f, -100.0f, 0.0f)));
}

TEST_F(FrustumTest, FrustumContainsSpherePartiallyInside) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    // Camera at z=5, far plane at 100 units, so max z is -95
    BoundingSphere sphere(glm::vec3(0.0f, 0.0f, -93.0f), 2.0f);
    EXPECT_TRUE(frustum.contains_sphere(sphere));
}

TEST_F(FrustumTest, FrustumDoesNotContainSphereTotallyOutside) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    BoundingSphere sphere(glm::vec3(0.0f, 0.0f, -105.0f), 2.0f);
    EXPECT_FALSE(frustum.contains_sphere(sphere));
}

TEST_F(FrustumTest, FrustumContainsAABBPartiallyInside) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    // Camera at z=5, far plane at 100 units, so max z is -95
    AABB aabb(glm::vec3(-1.0f, -1.0f, -92.0f), glm::vec3(1.0f, 1.0f, -90.0f));
    EXPECT_TRUE(frustum.contains_aabb(aabb));
}

TEST_F(FrustumTest, FrustumDoesNotContainAABBTotallyOutside) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    AABB aabb(glm::vec3(-1.0f, -1.0f, -105.0f), glm::vec3(1.0f, 1.0f, -103.0f));
    EXPECT_FALSE(frustum.contains_aabb(aabb));
}

TEST_F(FrustumTest, PlaneNormalization) {
    Plane plane;
    plane.normal = glm::vec3(2.0f, 0.0f, 0.0f);
    plane.distance = 4.0f;
    plane.normalize();
    
    EXPECT_FLOAT_EQ(glm::length(plane.normal), 1.0f);
    EXPECT_FLOAT_EQ(plane.distance, 2.0f);
}

TEST_F(FrustumTest, PlaneDistanceToPointOnPlane) {
    Plane plane(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float dist = plane.distance_to_point(glm::vec3(0.0f, 5.0f, 0.0f));
    EXPECT_NEAR(dist, 0.0f, 0.001f);
}

TEST_F(FrustumTest, PlaneDistanceToPointAbovePlane) {
    Plane plane(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float dist = plane.distance_to_point(glm::vec3(0.0f, 10.0f, 0.0f));
    EXPECT_GT(dist, 0.0f);
}

TEST_F(FrustumTest, PlaneDistanceToPointBelowPlane) {
    Plane plane(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float dist = plane.distance_to_point(glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_LT(dist, 0.0f);
}

TEST_F(FrustumTest, AABBTransformWithScale) {
    AABB aabb(glm::vec3(-1.0f), glm::vec3(1.0f));
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    
    AABB transformed = aabb.transform(transform);
    
    EXPECT_EQ(transformed.min, glm::vec3(-2.0f));
    EXPECT_EQ(transformed.max, glm::vec3(2.0f));
}

TEST_F(FrustumTest, AABBTransformWithRotation) {
    AABB aabb(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    AABB transformed = aabb.transform(transform);
    
    EXPECT_TRUE(transformed.min.x < transformed.max.x);
    EXPECT_TRUE(transformed.min.y < transformed.max.y);
    EXPECT_TRUE(transformed.min.z < transformed.max.z);
}

TEST_F(FrustumTest, BoundingSphereTransformWithScale) {
    BoundingSphere sphere(glm::vec3(0.0f), 1.0f);
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
    
    BoundingSphere transformed = sphere.transform(transform);
    
    EXPECT_FLOAT_EQ(transformed.radius, 3.0f);
}

TEST_F(FrustumTest, BoundingSphereTransformWithNonUniformScale) {
    BoundingSphere sphere(glm::vec3(0.0f), 1.0f);
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 1.0f));
    
    BoundingSphere transformed = sphere.transform(transform);
    
    EXPECT_FLOAT_EQ(transformed.radius, 3.0f);
}

TEST_F(FrustumTest, AABBDefaultConstructor) {
    AABB aabb;
    EXPECT_EQ(aabb.min, glm::vec3(0.0f));
    EXPECT_EQ(aabb.max, glm::vec3(0.0f));
}

TEST_F(FrustumTest, BoundingSphereDefaultConstructor) {
    BoundingSphere sphere;
    EXPECT_EQ(sphere.center, glm::vec3(0.0f));
    EXPECT_FLOAT_EQ(sphere.radius, 0.0f);
}

TEST_F(FrustumTest, PlaneDefaultConstructor) {
    Plane plane;
    EXPECT_EQ(plane.normal, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_FLOAT_EQ(plane.distance, 0.0f);
}

TEST_F(FrustumTest, AABBExpandMultiplePoints) {
    AABB aabb;
    aabb.expand(glm::vec3(-1.0f, 2.0f, -3.0f));
    aabb.expand(glm::vec3(4.0f, -5.0f, 6.0f));
    aabb.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    
    EXPECT_EQ(aabb.min, glm::vec3(-1.0f, -5.0f, -3.0f));
    EXPECT_EQ(aabb.max, glm::vec3(4.0f, 2.0f, 6.0f));
}

TEST_F(FrustumTest, AABBMergeMultiple) {
    AABB aabb1(glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(-2.0f, -2.0f, -2.0f));
    AABB aabb2(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    AABB aabb3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    
    aabb1.merge(aabb2);
    aabb1.merge(aabb3);
    
    EXPECT_EQ(aabb1.min, glm::vec3(-5.0f));
    EXPECT_EQ(aabb1.max, glm::vec3(5.0f));
}

TEST_F(FrustumTest, BoundingSphereFromAABBNonCentered) {
    AABB aabb(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    BoundingSphere sphere = BoundingSphere::from_aabb(aabb);
    
    EXPECT_EQ(sphere.center, glm::vec3(7.5f, 7.5f, 7.5f));
    EXPECT_GT(sphere.radius, 0.0f);
}

TEST_F(FrustumTest, FrustumGetPlane) {
    Frustum frustum;
    frustum.update(camera->get_view_projection_matrix());
    
    const Plane& left = frustum.get_plane(Frustum::Left);
    const Plane& right = frustum.get_plane(Frustum::Right);
    const Plane& bottom = frustum.get_plane(Frustum::Bottom);
    const Plane& top = frustum.get_plane(Frustum::Top);
    const Plane& near = frustum.get_plane(Frustum::Near);
    const Plane& far = frustum.get_plane(Frustum::Far);
    
    EXPECT_FLOAT_EQ(glm::length(left.normal), 1.0f);
    EXPECT_FLOAT_EQ(glm::length(right.normal), 1.0f);
    EXPECT_FLOAT_EQ(glm::length(bottom.normal), 1.0f);
    EXPECT_FLOAT_EQ(glm::length(top.normal), 1.0f);
    EXPECT_FLOAT_EQ(glm::length(near.normal), 1.0f);
    EXPECT_FLOAT_EQ(glm::length(far.normal), 1.0f);
}
