#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;

uniform sampler2D u_shadowTexture;
uniform vec4 u_color;
uniform vec2 u_resolution;
uniform float u_radius;

vec4 drawShadows()
{
    // distance of this pixel from the center
    vec2 center = vec2(u_radius);
    vec2 inputPixelCoord = v_texCoord * u_resolution;
    float dist = length(inputPixelCoord - center) / u_radius;
    float occluder = texture2D(CC_Texture0, v_texCoord).a;

    float shadowMapDistance = 1.0;

    if (occluder == 0.0)
    {
        float texelWidth = 1.0 / u_resolution.x;
        vec2 range = vec2(u_radius * 2.0);
        vec2 ratio = range / u_resolution;
        
        // Translate u and v into [0, 1] domain
        vec2 uv = v_texCoord / ratio;
        
        //translate u and v into [-1 , 1] domain
        float u0 = uv.x * 2.0 - 1.0;
        float v0 = uv.y * 2.0 - 1.0;
        
        //we use these to determine which quadrant we are in
        if(abs(v0) < abs(u0)) // left and right
        {
            //then, as u0 approaches 0 (the center), v should also approach 0
            v0 = v0 / abs(u0);
            //convert back from [-1,1] domain to [0,1] domain
            v0 = (v0 + 1.0) / 2.0;
            vec2 newCoordsH = vec2(ceil(uv.x - 0.5), (v0 * ratio) - texelWidth);
            
            //horizontal info was stored in the Red component
            shadowMapDistance = texture2D(u_shadowTexture, newCoordsH).r;
        }
        else // top and bottom
        {
            //then, as v0 approaches 0 (the center), u should also approach 0
            u0 = u0 / abs(v0);
            //convert back from [-1,1] domain to [0,1] domain
            u0 = (u0 + 1.0) / 2.0; //from [-1,1] domain to [0,1] domain
            vec2 newCoordsV = vec2(ceil(uv.y - 0.5), (u0 * ratio) - texelWidth);

            //vertical info was stored in the Green component
            shadowMapDistance = texture2D(u_shadowTexture, newCoordsV).g;
        }
    }
    
    //if distance to this pixel is lower than distance from shadowMap,
    //then we are not in shadow
    float light = dist < shadowMapDistance ? 1.0 - dist : 0.0;
//    light = pow(light, 2.0);
    return u_color * vec4(light, light, light, 1);
}

void main()
{
    gl_FragColor = drawShadows();
}
