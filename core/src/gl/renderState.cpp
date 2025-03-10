#include "renderState.h"

namespace Tangram {

namespace RenderState {

    Blending blending;
    DepthTest depthTest;
    StencilTest stencilTest;
    Culling culling;
    DepthWrite depthWrite;
    BlendingFunc blendingFunc;
    StencilWrite stencilWrite;
    StencilFunc stencilFunc;
    StencilOp stencilOp;
    ColorWrite colorWrite;
    FrontFace frontFace;
    CullFace cullFace;

    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;

    TextureUnit textureUnit;
    Texture texture;

    GLuint getTextureUnit(GLuint _unit) {
        if (_unit >= TANGRAM_MAX_TEXTURE_UNIT) {
            logMsg("Warning: trying to access unavailable texture unit");
        }

        return GL_TEXTURE0 + _unit;
    }

    void bindVertexBuffer(GLuint _id) { glBindBuffer(GL_ARRAY_BUFFER, _id); }
    void bindIndexBuffer(GLuint _id) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id); }
    void activeTextureUnit(GLuint _unit) { glActiveTexture(getTextureUnit(_unit)); }
    void bindTexture(GLenum _target, GLuint _textureId) { glBindTexture(_target, _textureId); }

    void configure() {
        blending.init(GL_FALSE);
        blendingFunc.init(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        culling.init(GL_TRUE);
        cullFace.init(GL_BACK);
        frontFace.init(GL_CCW);
        depthTest.init(GL_TRUE);
        depthWrite.init(GL_TRUE);

        glDisable(GL_STENCIL_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearDepthf(1.0);
        glDepthRangef(0.0, 1.0);
        glClearColor(0.3, 0.3, 0.3, 1.0);

        vertexBuffer.init(std::numeric_limits<unsigned int>::max(), false);
        indexBuffer.init(std::numeric_limits<unsigned int>::max(), false);
        texture.init(GL_TEXTURE_2D, std::numeric_limits<unsigned int>::max(), false);
        texture.init(GL_TEXTURE_CUBE_MAP, std::numeric_limits<unsigned int>::max(), false);
        textureUnit.init(std::numeric_limits<unsigned int>::max(), false);
    }

}

}
