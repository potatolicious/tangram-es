#pragma once

#include "style.h"
#include "gl/typedMesh.h"

namespace Tangram {

class DebugStyle : public Style {

protected:

    struct PosColVertex {
        // Position Data
        glm::vec3 pos;
        // Color Data
        GLuint abgr;
    };

    virtual void constructVertexLayout() override;
    virtual void constructShaderProgram() override;
    virtual void onBeginBuildTile(Tile& _tile) const override;

    typedef TypedMesh<PosColVertex> Mesh;

    virtual VboMesh* newMesh() const override {
        return nullptr;
    };

public:

    DebugStyle(GLenum _drawMode = GL_LINE_LOOP);
    DebugStyle(std::string _name, GLenum _drawMode = GL_LINE_LOOP);

    virtual ~DebugStyle() {
    }

    virtual bool isOpaque() const override { return false; };

};

}
