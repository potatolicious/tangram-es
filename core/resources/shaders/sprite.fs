#ifdef GL_ES
precision mediump float;
#define LOWP lowp
#else
#define LOWP
#endif

uniform sampler2D u_tex;

varying vec2 v_uv;
varying float v_alpha;
varying vec4 v_color;

void main(void) {
    vec4 color = texture2D(u_tex, v_uv);
    gl_FragColor = vec4(color.rgb * v_color.rgb, v_alpha * color.a * v_color.a);
}
