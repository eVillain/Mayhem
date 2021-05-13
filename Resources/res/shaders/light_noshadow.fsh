#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;

uniform vec4 u_color;
uniform vec2 u_resolution;
uniform float u_radius;

vec4 drawLight()
{
    // distance of this pixel from the center
    vec2 center = vec2(u_radius);
    vec2 inputPixelCoord = v_texCoord * u_resolution;
    float dist = length(inputPixelCoord - center) / u_radius;

    float light = 1.0 - dist;
//    light = pow(light, 2.0);
    return u_color * vec4(light, light, light, 1);
}

void main()
{
    gl_FragColor = drawLight();
}
