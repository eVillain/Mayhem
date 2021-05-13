#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;

uniform vec2 u_resolution;
uniform float u_radius;

vec4 horizontalReduction()
{
    // Because our texture width is 2 this is either 0.0 or 1.0
    float u = floor(v_texCoord.x * u_resolution.x);
    vec2 texCoord = vec2(u * u_radius / u_resolution.x, v_texCoord.y);
    
//    return vec4(texCoord, 0, 1);
    
    vec2 result = vec2(1,1);
    float texelWidth = 1.0 / u_resolution.x;
    for (float x = 0.0; x < u_radius; x += 1.0)
    {
        float texCoordX = texCoord.x + (x * texelWidth);
        vec2 color = texture2D(CC_Texture0, vec2(texCoordX, v_texCoord.y)).rg;
        result = min(result, color);
    }

    return vec4(result, 0, 1);
}

//vec4 verticalReduction()
//{
//    // Because our texture width is 2 this is either 0.0 or 1.0
//    float v = floor(v_texCoord.x * u_resolution.x);
//    float ratio = u_resolution.y / u_resolution.x;
//
//    float localStartY = mix(u_center.y - u_radius, u_center.y, v) / u_resolution.y;
//    float localEndY = mix(u_center.y, u_center.y + u_radius, v) / u_resolution.y;
////    localStartY = max(localStartY, 0.0);
////    localEndY = min(localEndY, 1.0);
////    return vec4(localStartY, localEndY, 0, 1);
//    vec2 result = vec2(1,1);
//    float texelHeight = 1.0 / u_resolution.y;
//    for (float y = localStartY; y < localEndY + texelHeight; y += texelHeight)
//    {
//        vec2 texCoord = vec2(v_texCoord.y * ratio, y);
//        vec2 color = texture2D(CC_Texture0, texCoord).rg;
//        result = min(result, color);
//    }
//
//    return vec4(result, 0, 1);
//}

void main()
{
    gl_FragColor = horizontalReduction();
//    gl_FragColor = verticalReduction();
}
