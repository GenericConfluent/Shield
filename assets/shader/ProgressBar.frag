uniform vec2 windowPos;
uniform float width;
uniform float radius;
uniform float borderWidth;
uniform float progress;

void main() {
    const vec4 BLACK = vec4(17.0, 17.0, 17.0, 255.0) / 255.0;
    vec2 pos = gl_FragCoord.xy - windowPos.xy;
    float halfExtent = width * 0.5 - radius;
    if (abs(pos.x) <= halfExtent && radius - abs(pos.y) <= borderWidth) {
        gl_FragColor = BLACK;
        return;
    }

    // Now colour the edges of the two external circles
    float m = pos.x > 0 ? 1.0 : -1.0;
    vec2 radialCenter = vec2(m * halfExtent, 0.0);
    vec2 delta = pos.xy - radialCenter.xy;
    float distFromCenter = sqrt(delta.x * delta.x + delta.y * delta.y);
    if (m * delta.x > 0 && radius - distFromCenter <= borderWidth) {
        gl_FragColor = BLACK;
        return;
    }

    float x = pos + width * 0.5;
    x /= width;
    // x is now in range 0.0..=1.0
    if (progress > 1.0) {
        gl_FragColor = vec4(0.9, 0.0, 0.0, 1.0);
        return;
    }
    
    if (x < progress) {
        vec3 cyan = vec3(29.0, 226.0, 211.0);
        vec3 pink = vec3(221.0, 5.0, 241.0);
        vec3 colour = mix(cyan / 255.0, pink / 255.0, x);
        gl_FragColor = vec4(colour, 1.0);
    } else {
        const vec4 GREY = vec4(25.0, 25.0, 25.0, 255.0) / 255.0;
        gl_FragColor = GREY;
    }
}
