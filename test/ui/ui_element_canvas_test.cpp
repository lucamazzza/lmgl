#include "lmgl/ui/canvas.hpp"
#include "lmgl/ui/ui_element.hpp"

#include <gtest/gtest.h>

using namespace lmgl::ui;

// Mock UI Element for testing
class MockUIElement : public UIElement {
  public:
    MockUIElement(const std::string &name = "MockElement") : UIElement(name), render_called(false) {}

    void render(float canvas_width, float canvas_height, const glm::mat4 &projection) override {
        render_called = true;
        last_canvas_width = canvas_width;
        last_canvas_height = canvas_height;
    }

    bool render_called;
    float last_canvas_width = 0.0f;
    float last_canvas_height = 0.0f;
};

// UIElement Tests

TEST(UIElementTest, DefaultConstructor) {
    auto element = std::make_shared<MockUIElement>();
    EXPECT_EQ(element->get_name(), "MockElement");
    EXPECT_EQ(element->get_position(), glm::vec2(0.0f, 0.0f));
    EXPECT_EQ(element->get_size(), glm::vec2(100.0f, 100.0f));
    EXPECT_EQ(element->get_anchor(), Anchor::TopLeft);
    EXPECT_EQ(element->get_render_order(), 0);
    EXPECT_TRUE(element->is_visible());
    EXPECT_EQ(element->get_children().size(), 0);
    EXPECT_EQ(element->get_parent(), nullptr);
}

TEST(UIElementTest, NamedConstructor) {
    auto element = std::make_shared<MockUIElement>("TestElement");
    EXPECT_EQ(element->get_name(), "TestElement");
}

TEST(UIElementTest, SetPosition) {
    auto element = std::make_shared<MockUIElement>();
    glm::vec2 pos(100.0f, 200.0f);
    element->set_position(pos);
    EXPECT_EQ(element->get_position(), pos);
}

TEST(UIElementTest, SetSize) {
    auto element = std::make_shared<MockUIElement>();
    glm::vec2 size(300.0f, 400.0f);
    element->set_size(size);
    EXPECT_EQ(element->get_size(), size);
}

TEST(UIElementTest, SetAnchor) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::BottomRight);
    EXPECT_EQ(element->get_anchor(), Anchor::BottomRight);
}

TEST(UIElementTest, SetRenderOrder) {
    auto element = std::make_shared<MockUIElement>();
    element->set_render_order(10);
    EXPECT_EQ(element->get_render_order(), 10);
}

TEST(UIElementTest, SetVisible) {
    auto element = std::make_shared<MockUIElement>();
    element->set_visible(false);
    EXPECT_FALSE(element->is_visible());
    element->set_visible(true);
    EXPECT_TRUE(element->is_visible());
}

TEST(UIElementTest, SetName) {
    auto element = std::make_shared<MockUIElement>();
    element->set_name("NewName");
    EXPECT_EQ(element->get_name(), "NewName");
}

TEST(UIElementTest, AddChild) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    auto child = std::make_shared<MockUIElement>("Child");
    parent->add_child(child);
    EXPECT_EQ(parent->get_children().size(), 1);
    EXPECT_EQ(parent->get_children()[0], child);
    EXPECT_EQ(child->get_parent(), parent);
}

TEST(UIElementTest, AddMultipleChildren) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    auto child1 = std::make_shared<MockUIElement>("Child1");
    auto child2 = std::make_shared<MockUIElement>("Child2");
    parent->add_child(child1);
    parent->add_child(child2);
    EXPECT_EQ(parent->get_children().size(), 2);
    EXPECT_EQ(child1->get_parent(), parent);
    EXPECT_EQ(child2->get_parent(), parent);
}

TEST(UIElementTest, AddNullChild) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    parent->add_child(nullptr);
    EXPECT_EQ(parent->get_children().size(), 0);
}

TEST(UIElementTest, RemoveChild) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    auto child = std::make_shared<MockUIElement>("Child");
    parent->add_child(child);
    parent->remove_child(child);
    EXPECT_EQ(parent->get_children().size(), 0);
    EXPECT_EQ(child->get_parent(), nullptr);
}

TEST(UIElementTest, RemoveNonExistentChild) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    auto child1 = std::make_shared<MockUIElement>("Child1");
    auto child2 = std::make_shared<MockUIElement>("Child2");
    parent->add_child(child1);
    parent->remove_child(child2);
    EXPECT_EQ(parent->get_children().size(), 1);
}

TEST(UIElementTest, RemoveNullChild) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    parent->remove_child(nullptr);
    EXPECT_EQ(parent->get_children().size(), 0);
}

// Anchor Tests

TEST(UIElementTest, AbsolutePosition_TopLeft) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::TopLeft);
    element->set_position(glm::vec2(10.0f, 20.0f));
    glm::vec2 abs_pos = element->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 10.0f);
    EXPECT_FLOAT_EQ(abs_pos.y, 20.0f);
}

TEST(UIElementTest, AbsolutePosition_TopCenter) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::TopCenter);
    element->set_position(glm::vec2(10.0f, 20.0f));
    glm::vec2 abs_pos = element->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 410.0f); // 800 * 0.5 + 10
    EXPECT_FLOAT_EQ(abs_pos.y, 20.0f);
}

TEST(UIElementTest, AbsolutePosition_TopRight) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::TopRight);
    element->set_position(glm::vec2(-10.0f, 20.0f));
    glm::vec2 abs_pos = element->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 790.0f); // 800 - 10
    EXPECT_FLOAT_EQ(abs_pos.y, 20.0f);
}

TEST(UIElementTest, AbsolutePosition_MiddleCenter) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::Center);
    element->set_position(glm::vec2(0.0f, 0.0f));
    glm::vec2 abs_pos = element->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 400.0f); // 800 * 0.5
    EXPECT_FLOAT_EQ(abs_pos.y, 300.0f); // 600 * 0.5
}

TEST(UIElementTest, AbsolutePosition_BottomRight) {
    auto element = std::make_shared<MockUIElement>();
    element->set_anchor(Anchor::BottomRight);
    element->set_position(glm::vec2(-10.0f, -20.0f));
    glm::vec2 abs_pos = element->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 790.0f); // 800 - 10
    EXPECT_FLOAT_EQ(abs_pos.y, 580.0f); // 600 - 20
}

TEST(UIElementTest, AbsolutePosition_WithParent) {
    auto parent = std::make_shared<MockUIElement>("Parent");
    auto child = std::make_shared<MockUIElement>("Child");
    parent->set_anchor(Anchor::TopLeft);
    parent->set_position(glm::vec2(100.0f, 50.0f));
    child->set_anchor(Anchor::TopLeft);
    child->set_position(glm::vec2(20.0f, 10.0f));
    parent->add_child(child);
    glm::vec2 abs_pos = child->get_absolute_position(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(abs_pos.x, 120.0f); // 100 + 20
    EXPECT_FLOAT_EQ(abs_pos.y, 60.0f);  // 50 + 10
}

// Canvas Tests

TEST(CanvasTest, DefaultConstructor) {
    Canvas canvas;
    EXPECT_EQ(canvas.get_width(), 1920);
    EXPECT_EQ(canvas.get_height(), 1080);
    EXPECT_TRUE(canvas.is_visible());
    EXPECT_EQ(canvas.get_elements().size(), 0);
}

TEST(CanvasTest, Resize) {
    Canvas canvas(800, 600);
    canvas.resize(1024, 768);
    EXPECT_EQ(canvas.get_width(), 1024);
    EXPECT_EQ(canvas.get_height(), 768);
}

TEST(CanvasTest, AddElement) {
    Canvas canvas;
    auto element = std::make_shared<MockUIElement>();
    canvas.add_element(element);
    EXPECT_EQ(canvas.get_elements().size(), 1);
}

TEST(CanvasTest, RemoveElement) {
    Canvas canvas;
    auto element = std::make_shared<MockUIElement>();
    canvas.add_element(element);
    canvas.remove_element(element);
    EXPECT_EQ(canvas.get_elements().size(), 0);
}

TEST(CanvasTest, ProjectionMatrixUpdatesOnResize) {
    Canvas canvas(800, 600);
    glm::mat4 old_projection = canvas.get_projection_matrix();
    canvas.resize(1024, 768);
    glm::mat4 new_projection = canvas.get_projection_matrix();
    EXPECT_NE(old_projection, new_projection);
}
