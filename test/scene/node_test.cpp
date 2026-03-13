#include <gtest/gtest.h>

#include "lmgl/scene/node.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

namespace lmgl {

namespace scene {

class NodeTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
    }
    void TearDown() override {
        // Cleanup code after each test
    }
};

TEST_F(NodeTest, DefaultConstruction) {
    auto node = std::make_shared<Node>();
    EXPECT_EQ(node->get_name(), "Node");
    EXPECT_EQ(node->get_children().size(), 0);
    EXPECT_EQ(node->get_parent(), nullptr);
    EXPECT_EQ(node->get_position(), glm::vec3(0.0f));
    EXPECT_EQ(node->get_euler_angles(), glm::vec3(0.0f));
    EXPECT_EQ(node->get_scale(), glm::vec3(1.0f));
    EXPECT_EQ(node->get_local_transform(), glm::mat4(1.0f));
    EXPECT_EQ(node->get_world_transform(), glm::mat4(1.0f));
}

TEST_F(NodeTest, SetPosition) {
    auto node = std::make_shared<Node>();
    glm::vec3 new_position(1.0f, 2.0f, 3.0f);
    node->set_position(new_position);
    EXPECT_EQ(node->get_position(), new_position);
}

TEST_F(NodeTest, PositionAffectsLocalTransform) {
    auto node = std::make_shared<Node>();
    node->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 transformed = node->get_local_transform() * origin;
    EXPECT_NEAR(transformed.x, 5.0f, 0.001f);
    EXPECT_NEAR(transformed.y, 0.0f, 0.001f);
    EXPECT_NEAR(transformed.z, 0.0f, 0.001f);
}

TEST_F(NodeTest, SetRotationEuler) {
    auto node = std::make_shared<Node>();
    node->set_rotation(glm::vec3(0.0f, 90.0f, 0.0f));
    EXPECT_NE(node->get_rotation(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(node->get_euler_angles().y, 90.0f, 0.1f);
}

TEST_F(NodeTest, SetRotationQuaternion) {
    auto node = std::make_shared<Node>();
    glm::quat quat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    node->set_rotation(quat);
    EXPECT_EQ(node->get_rotation(), quat);
    EXPECT_NEAR(node->get_euler_angles().y, 45.0f, 0.1f);
}

TEST_F(NodeTest, SetScaleVector) {
    auto node = std::make_shared<Node>();
    glm::vec3 new_scale(2.0f, 3.0f, 4.0f);
    node->set_scale(new_scale);
    EXPECT_EQ(node->get_scale(), new_scale);
}

TEST_F(NodeTest, SetScaleUniform) {
    auto node = std::make_shared<Node>();
    node->set_scale(2.0f);
    EXPECT_EQ(node->get_scale(), glm::vec3(2.0f));
}

TEST_F(NodeTest, AddChild) {
    auto parent = std::make_shared<Node>("Parent");
    auto child = std::make_shared<Node>("Child");
    parent->add_child(child);
    EXPECT_EQ(parent->get_children().size(), 1);
    EXPECT_EQ(parent->get_children()[0], child);
    EXPECT_EQ(child->get_parent(), parent);
}

TEST_F(NodeTest, AddMultipleChildren) {
    auto parent = std::make_shared<Node>("Parent");
    auto child1 = std::make_shared<Node>("Child1");
    auto child2 = std::make_shared<Node>("Child2");
    auto child3 = std::make_shared<Node>("Child3");
    parent->add_child(child1);
    parent->add_child(child2);
    parent->add_child(child3);
    EXPECT_EQ(parent->get_children().size(), 3);
    EXPECT_EQ(parent->get_children()[0], child1);
    EXPECT_EQ(parent->get_children()[1], child2);
    EXPECT_EQ(parent->get_children()[2], child3);
    EXPECT_EQ(child1->get_parent(), parent);
    EXPECT_EQ(child2->get_parent(), parent);
    EXPECT_EQ(child3->get_parent(), parent);
}

TEST_F(NodeTest, RemoveChild) {
    auto parent = std::make_shared<Node>("Parent");
    auto child = std::make_shared<Node>("Child");
    parent->add_child(child);
    EXPECT_EQ(parent->get_children().size(), 1);
    parent->remove_child(child);
    EXPECT_EQ(parent->get_children().size(), 0);
    EXPECT_EQ(child->get_parent(), nullptr);
}

TEST_F(NodeTest, ChildDetachFromParent) {
    auto parent = std::make_shared<Node>("Parent");
    auto child = std::make_shared<Node>("Child");
    parent->add_child(child);
    EXPECT_EQ(parent->get_children().size(), 1);
    child->detach_from_parent();
    EXPECT_EQ(parent->get_children().size(), 0);
    EXPECT_EQ(child->get_parent(), nullptr);
}

TEST_F(NodeTest, ReparentChild) {
    auto parent1 = std::make_shared<Node>("Parent1");
    auto parent2 = std::make_shared<Node>("Parent2");
    auto child = std::make_shared<Node>("Child");
    parent1->add_child(child);
    EXPECT_EQ(child->get_parent(), parent1);
    parent2->add_child(child);
    EXPECT_EQ(child->get_parent(), parent2);
    EXPECT_EQ(parent1->get_children().size(), 0);
    EXPECT_EQ(parent2->get_children().size(), 1);
}

TEST_F(NodeTest, UpdateTransformWithIdentityParent) {
    auto node = std::make_shared<Node>();
    node->set_position(glm::vec3(1.0f, 2.0f, 3.0f));
    node->update_transform(glm::mat4(1.0f));
    EXPECT_EQ(node->get_world_transform(), node->get_local_transform());
}

TEST_F(NodeTest, TransformPropagationWithScale) {
    auto parent = std::make_shared<Node>("Parent");
    auto child = std::make_shared<Node>("Child");
    parent->set_scale(2.0f);
    child->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    parent->add_child(child);
    parent->update_transform(glm::mat4(1.0f));
    glm::vec4 child_global_pos = child->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(child_global_pos.x, 2.0f, 0.001f);
}

TEST_F(NodeTest, RecursiveTransformUpdate) {
    auto root = std::make_shared<Node>("Root");
    auto child = std::make_shared<Node>("Child");
    auto grandchild = std::make_shared<Node>("Grandchild");
    root->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    child->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    grandchild->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    root->add_child(child);
    child->add_child(grandchild);
    root->update_transform(glm::mat4(1.0f));
    glm::vec4 grandchild_global_pos = grandchild->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(grandchild_global_pos.x, 3.0f, 0.001f);
}

TEST_F(NodeTest, RenameNode) {
    auto node = std::make_shared<Node>("OldName");
    EXPECT_EQ(node->get_name(), "OldName");
    node->set_name("NewName");
    EXPECT_EQ(node->get_name(), "NewName");
}

} // namespace scene

} // namespace lmgl
