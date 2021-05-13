#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;

uniform vec2 u_resolution;
uniform float u_radius;

float computeDistance(vec2 texCoord)
{
    vec2 center = vec2(u_radius);
    vec2 inputPixelCoord = texCoord * u_resolution;
    vec4 color = texture2D(CC_Texture0, texCoord);
    float distanceToCenter = color.a > 0.3 ? length(inputPixelCoord - center) / u_radius : 1.0;
    return distanceToCenter;
}

vec4 distortQuadrantsH()
{
    vec2 center = vec2(u_radius);
    vec2 range = center * 2.0;
    vec2 ratio = range / u_resolution;
    // Translate u and v into [0, 1] domain
    vec2 uv = v_texCoord / ratio;

//    return vec4(uv, 0, 1);
    //translate u and v into [-1 , 1] domain
    float u0 = (uv.x * 2.0) - 1.0;
    float v0 = (uv.y * 2.0) - 1.0;

    //then, as u0 approaches 0 (the center), v should also approach 0
    v0 = v0 * abs(u0);
//    return vec4(v0, u0, 0, 1);
//    convert back from [-1,1] domain to [0,1] domain
    v0 = (v0 + 1.0) / 2.0;
//    return vec4(v0, 0, 0, 1);

    //we now have the coordinates for reading from the initial image
    vec2 newCoordsH = vec2(v_texCoord.x, v0 * ratio.y);
    vec2 newCoordsV = vec2(v0 * ratio.y, v_texCoord.x);
    
    //read for both horizontal and vertical direction and store them in separate channels
    float horizontal = computeDistance(newCoordsH);
    float vertical = computeDistance(newCoordsV);
    return vec4(horizontal, vertical, 0, 1);
}

//vec4 distortQuadrantsV()
//{
//    vec2 bottomLeft = (u_center - vec2(u_radius))/ u_resolution;
//    vec2 range = vec2(u_radius / u_resolution) * 2.0;
//    
//    // Translate u and v into [0, 1] domain
//    vec2 uv = vec2((v_texCoord.x - bottomLeft.x) / range.x,
//                   (v_texCoord.y - bottomLeft.y) / range.y);
//    
//    //translate u and v into [-1 , 1] domain
//    float u0 = uv.x * 2.0 - 1.0;
//    float v0 = uv.y * 2.0 - 1.0;
//    //then, as u0 approaches 0 (the center), v should also approach 0
//    u0 = u0 * abs(v0);
//    //convert back from [-1,1] domain to [0,1] domain
//    u0 = (u0 + 1.0) / 2.0;
//    
//    //we now have the coordinates for reading from the initial image
//    vec2 newCoordsH = vec2((u0 * range.x) + bottomLeft.x,
//                           (uv.y * range.y) + bottomLeft.y);
//    vec2 newCoordsV = vec2((uv.y * range.x) + bottomLeft.x,
//                           (u0 * range.y) + bottomLeft.y);
//    
//    //read for both horizontal and vertical direction and store them in separate channels
//    float horizontal = computeDistance(newCoordsH);
//    float vertical = computeDistance(newCoordsV);
//    return vec4(horizontal, vertical, 0, 1);
//}

void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord);

    gl_FragColor = distortQuadrantsH();
}
