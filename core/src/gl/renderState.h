#pragma once

#include "gl.h"
#include "platform.h"

#include <tuple>
#include <limits>

// Max texture units used at the same time by a shader
#define TANGRAM_MAX_TEXTURE_UNIT 6

namespace Tangram {

namespace RenderState {

    /* Configure the render states */
    void configure();
    /* Get the texture slot from a texture unit from 0 to TANGRAM_MAX_TEXTURE_UNIT-1 */
    GLuint getTextureUnit(GLuint _unit);
    /* Bind a vertex buffer */
    void bindVertexBuffer(GLuint _id);
    /* Bind an index buffer */
    void bindIndexBuffer(GLuint _id);
    /* Sets the currently active texture unit */
    void activeTextureUnit(GLuint _unit);
    /* Bind a texture for the specified target */
    void bindTexture(GLenum _target, GLuint _textureId);

    template <typename T>
    class State {
    public:
        void init(const typename T::Type& _default) {
            T::set(_default);
            m_current = _default;
        }

        inline void operator()(const typename T::Type& _value) {
            if (m_current != _value) {
                m_current = _value;
                T::set(m_current);
            }
        }
    private:
        typename T::Type m_current;
    };

    template <GLenum N>
    struct BoolSwitch {
        using Type = GLboolean;
        inline static void set(const Type& _type) {
            if (_type) {
                glEnable(N);
            } else {
                glDisable(N);
            }
        }
    };

    // http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
    // Generate integer sequence for getting values from 'params' tuple.
    template<int ...> struct seq {};
    template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
    template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

    template <typename F, F fn, typename ...Args>
    struct StateWrap {

        using Type = std::tuple<Args...>;
        Type params;

        void init(Args... _param, bool _force = true) {
            params = std::make_tuple(_param...);
            if (_force) {
                call(typename gens<sizeof...(Args)>::type());
            }
        }

        inline void operator()(Args... _args) {
            auto _params = std::make_tuple(_args...);

            if (_params != params) {
                params = _params;
                call(typename gens<sizeof...(Args)>::type());
            }
        }

        inline bool compare(Args... _args) {
            auto _params = std::make_tuple(_args...);
            return _params == params;
        }

        template<int ...S>
        inline void call(seq<S...>) {
            fn(std::get<S>(params) ...);
        }
    };


    using DepthTest = State<BoolSwitch<GL_DEPTH_TEST>>;
    using StencilTest = State<BoolSwitch<GL_STENCIL_TEST>>;
    using Blending = State<BoolSwitch<GL_BLEND>>;
    using Culling = State<BoolSwitch<GL_CULL_FACE>>;

#define FUN(X) decltype((X)), X

    using DepthWrite = StateWrap<FUN(glDepthMask),
                                 GLboolean>; // enabled

    using BlendingFunc = StateWrap<FUN(glBlendFunc),
                                   GLenum,  // sfactor
                                   GLenum>; // dfactor

    using StencilWrite = StateWrap<FUN(glStencilMask),
                                   GLuint>; // mask

    using StencilFunc = StateWrap<FUN(glStencilFunc),
                                  GLenum,  // func
                                  GLint,   // ref
                                  GLuint>; // mask

    using StencilOp = StateWrap<FUN(glStencilOp),
                                GLenum,  // stencil:fail
                                GLenum,  // stencil:pass, depth:fail
                                GLenum>; // both pass

    using ColorWrite = StateWrap<FUN(glColorMask),
                                 GLboolean,  // red
                                 GLboolean,  // green
                                 GLboolean,  // blue
                                 GLboolean>; // alpha

    using FrontFace = StateWrap<FUN(glFrontFace),
                                GLenum>;

    using CullFace = StateWrap<FUN(glCullFace),
                               GLenum>;

    using VertexBuffer = StateWrap<FUN(bindVertexBuffer), GLuint>;
    using IndexBuffer = StateWrap<FUN(bindIndexBuffer), GLuint>;

    using TextureUnit = StateWrap<FUN(activeTextureUnit), GLuint>;
    using Texture = StateWrap<FUN(bindTexture), GLenum, GLuint>;

#undef FUN

    extern DepthTest depthTest;
    extern DepthWrite depthWrite;
    extern Blending blending;
    extern BlendingFunc blendingFunc;
    extern StencilTest stencilTest;
    extern StencilWrite stencilWrite;
    extern StencilFunc stencilFunc;
    extern StencilOp stencilOp;
    extern ColorWrite colorWrite;
    extern FrontFace frontFace;
    extern CullFace cullFace;
    extern Culling culling;

    extern VertexBuffer vertexBuffer;
    extern IndexBuffer indexBuffer;

    extern TextureUnit textureUnit;
    extern Texture texture;
}

}
