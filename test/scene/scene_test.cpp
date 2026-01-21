#include <gtest/gtest.h>

#include "lmgl/scene/scene.hpp"

namespace lmgl {

namespace scene {

class SceneTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup code before each test
    }
    void TearDown() override {
        // Cleanup code after each test
    }
};

TEST_F(SceneTest, DefaultConstruction) {
    auto scene = std::make_shared<Scene>();
    EXPECT_NE(scene, nullptr);
    EXPECT_EQ(scene->get_root()->get_name(), "Root");
}

TEST_F(SceneTest, NamedConstruction) {
    auto scene = std::make_shared<Scene>("TestScene");
    EXPECT_EQ(scene->get_name(), "TestScene");
    scene->set_name("UpdatedScene");
    EXPECT_EQ(scene->get_name(), "UpdatedScene");
}

TEST_F(SceneTest, RootNodeExists) {
    auto scene = std::make_shared<Scene>();
    auto root = scene->get_root();
    EXPECT_NE(root, nullptr);
    EXPECT_EQ(root->get_position(), glm::vec3(0.0f));
}

TEST_F(SceneTest, AddNodesToRoot) {
    auto scene = std::make_shared<Scene>();
    auto child1 = std::make_shared<Node>("Child1");
    auto child2 = std::make_shared<Node>("Child2");
    scene->get_root()->add_child(child1);
    scene->get_root()->add_child(child2);
    EXPECT_EQ(scene->get_root()->get_children().size(), 2);
}

TEST_F(SceneTest, UpdateTransformEntireHierarchy) {
    auto scene = std::make_shared<Scene>();
    auto par = std::make_shared<Node>("Parent");
    auto child = std::make_shared<Node>("Child");
    par->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    child->set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    scene->get_root()->add_child(par);
    par->add_child(child);
    scene->update();
    glm::vec4 child_pos = child->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(child_pos.x, 8.0f, 0.001f);
}

TEST_F(SceneTest, ComplexHierarchy) {
    auto scene = std::make_shared<Scene>();
    // Hierarchy:
    // Root
    //  ├── A (position (1,0,0))
    //  │   ├── B (position (1,0,0))
    //  │   └── C (position (0,1,0))
    //  └── D (position (-1,0,0))
    auto node_a = std::make_shared<Node>("A");
    auto node_b = std::make_shared<Node>("B");
    auto node_c = std::make_shared<Node>("C");
    auto node_d = std::make_shared<Node>("D");
    node_a->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    node_b->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    node_c->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
    node_d->set_position(glm::vec3(-1.0f, 0.0f, 0.0f));
    scene->get_root()->add_child(node_a);
    node_a->add_child(node_b);
    node_a->add_child(node_c);
    scene->get_root()->add_child(node_d);
    scene->update();
    glm::vec4 pos_a = node_a->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 pos_b = node_b->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 pos_c = node_c->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 pos_d = node_d->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(pos_a.x, 1.0f, 0.001f);
    EXPECT_NEAR(pos_b.x, 2.0f, 0.001f);
    EXPECT_NEAR(pos_c.y, 1.0f, 0.001f);
    EXPECT_NEAR(pos_d.x, -1.0f, 0.001f);
}

TEST_F(SceneTest, AddLight) {
    auto scene = std::make_shared<Scene>();
    auto light = std::make_shared<Light>(LightType::Directional);
    scene->add_light(light);
    EXPECT_EQ(scene->get_lights().size(), 1);
}

TEST_F(SceneTest, AddMultipleLights) {
    auto scene = std::make_shared<Scene>();
    auto dir_light = std::make_shared<Light>(LightType::Directional);
    auto point_light = std::make_shared<Light>(LightType::Point);
    auto spot_light = std::make_shared<Light>(LightType::Spot);
    
    scene->add_light(dir_light);
    scene->add_light(point_light);
    scene->add_light(spot_light);
    
    EXPECT_EQ(scene->get_lights().size(), 3);
}

TEST_F(SceneTest, RemoveLight) {
    auto scene = std::make_shared<Scene>();
    auto light = std::make_shared<Light>(LightType::Directional);
    scene->add_light(light);
    EXPECT_EQ(scene->get_lights().size(), 1);
    scene->remove_light(light);
    EXPECT_EQ(scene->get_lights().size(), 0);
}

TEST_F(SceneTest, GetLights) {
    auto scene = std::make_shared<Scene>();
    auto light1 = std::make_shared<Light>(LightType::Directional);
    auto light2 = std::make_shared<Light>(LightType::Point);
    
    scene->add_light(light1);
    scene->add_light(light2);
    
    const auto& lights = scene->get_lights();
    EXPECT_EQ(lights.size(), 2);
}

TEST_F(SceneTest, UpdateMultipleTimes) {
    auto scene = std::make_shared<Scene>();
    auto node = std::make_shared<Node>("TestNode");
    node->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    scene->get_root()->add_child(node);
    
    scene->update();
    scene->update();
    scene->update();
    
    glm::vec4 pos = node->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(pos.x, 1.0f, 0.001f);
}

TEST_F(SceneTest, DeepHierarchy) {
    auto scene = std::make_shared<Scene>();
    auto current = scene->get_root();
    
    for (int i = 0; i < 10; ++i) {
        auto node = std::make_shared<Node>("Node" + std::to_string(i));
        node->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
        current->add_child(node);
        current = node;
    }
    
    scene->update();
    
    glm::vec4 deepest_pos = current->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(deepest_pos.x, 10.0f, 0.001f);
}

TEST_F(SceneTest, RootNodeCannotBeRemoved) {
    auto scene = std::make_shared<Scene>();
    auto root = scene->get_root();
    EXPECT_NE(root, nullptr);
    EXPECT_EQ(root->get_parent(), nullptr);
}

TEST_F(SceneTest, SceneNameOperations) {
    auto scene = std::make_shared<Scene>("InitialName");
    EXPECT_EQ(scene->get_name(), "InitialName");
    
    scene->set_name("NewName");
    EXPECT_EQ(scene->get_name(), "NewName");
    
    scene->set_name("");
    EXPECT_EQ(scene->get_name(), "");
}

TEST_F(SceneTest, EmptySceneUpdate) {
    auto scene = std::make_shared<Scene>();
    EXPECT_NO_THROW(scene->update());
}

TEST_F(SceneTest, MultipleChildrenUpdate) {
    auto scene = std::make_shared<Scene>();
    
    for (int i = 0; i < 5; ++i) {
        auto child = std::make_shared<Node>("Child" + std::to_string(i));
        child->set_position(glm::vec3(float(i), 0.0f, 0.0f));
        scene->get_root()->add_child(child);
    }
    
    scene->update();
    
    const auto& children = scene->get_root()->get_children();
    EXPECT_EQ(children.size(), 5);
    
    for (size_t i = 0; i < children.size(); ++i) {
        glm::vec4 pos = children[i]->get_world_transform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        EXPECT_NEAR(pos.x, float(i), 0.001f);
    }
}

} // namespace scene

} // namespace lmgl
