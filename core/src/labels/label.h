#pragma once

#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include <climits> // needed in aabb.h
#include "isect2d.h"
#include "fadeEffect.h"
#include "util/types.h"

#include <string>


namespace Tangram {

class LabelMesh;

class Label {

public:

    enum class Type {
        point,
        line,
        debug
    };

    enum State {
        fading_in       = 1,
        fading_out      = 1 << 1,
        visible         = 1 << 2,
        sleep           = 1 << 3,
        out_of_screen   = 1 << 4,
        wait_occ        = 1 << 5, // state waiting for first occlusion result
    };

    struct Vertex {
        Vertex(glm::vec2 pos, glm::vec2 uv)
            : pos(pos), uv(uv){}

        glm::vec2 pos;
        glm::vec2 uv;
        struct State {
            glm::vec2 screenPos;
            float alpha = 0.f;
            float rotation = 0.f;
        } state;
    };

    struct Transform {
        Transform(glm::vec2 _pos)
            : modelPosition1(_pos),
              modelPosition2(_pos),
              offset(glm::vec2(0)){}

        Transform(glm::vec2 _pos1, glm::vec2 _pos2, glm::vec2 _offset)
            : modelPosition1(_pos1),
              modelPosition2(_pos2),
              offset(_offset){}

        glm::vec2 modelPosition1;
        glm::vec2 modelPosition2;
        glm::vec2 offset;

        Vertex::State state;
    };

    Label(Transform _transform, glm::vec2 _size, Type _type, LabelMesh& _mesh, Range _vertexRange);

    virtual ~Label();

    Transform getTransform() const { return m_transform; }

    /* Update the transform of the label in world space, and project it to screen space */
    void updateTransform(const Transform& _transform, const glm::mat4& _mvp, const glm::vec2& _screenSize);

    /* Gets the oriented bounding box of the label */
    const isect2d::OBB& getOBB() const { return m_obb; }

    /* Gets the extent of the oriented bounding box of the label */
    const isect2d::AABB& getAABB() const { return m_aabb; }

    bool update(const glm::mat4& _mvp, const glm::vec2& _screenSize, float _dt);

    /* Push the pending transforms to the vbo by updating the vertices */
    void pushTransform();

    /* Update the screen position of the label */
    bool updateScreenTransform(const glm::mat4& _mvp, const glm::vec2& _screenSize);

    /* Sets the occlusion */
    void setOcclusion(bool _occlusion);

    /* Checks whether the label is in a state where it can occlusion */
    bool canOcclude();

    /* Mark the label as resolved */
    void occlusionSolved();

    bool occludedLastFrame() { return m_occludedLastFrame; }

    State getState() const { return m_currentState; }

    /* Checks whether the label is in a visible state */
    bool visibleState() const;

private:

    bool offViewport(const glm::vec2& _screenSize);

    void enterState(State _state, float _alpha = 1.0f);

    bool updateState(const glm::mat4& _mvp, const glm::vec2& _screenSize, float _dt);

    void setAlpha(float _alpha);

    void setScreenPosition(const glm::vec2& _screenPosition);

    void setRotation(float _rotation);

    State m_currentState;
    Type m_type;
    bool m_occludedLastFrame;
    bool m_occlusionSolved;
    FadeEffect m_fade;
    bool m_updateMeshVisibility;

protected:

    virtual void updateBBoxes() = 0;

    isect2d::OBB m_obb;
    isect2d::AABB m_aabb;
    bool m_dirty;
    Transform m_transform;
    glm::vec2 m_dim;

    // Back-pointer to owning container
    LabelMesh& m_mesh;

    // first vertex and count in m_mesh vertices
    Range m_vertexRange;
};

}
