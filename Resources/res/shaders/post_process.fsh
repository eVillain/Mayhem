#ifdef GL_ES
precision lowp float;
#endif

uniform sampler2D u_lightTexture;
uniform vec2 u_resolution;
uniform vec2 u_cursor;
uniform float u_radius;

varying vec2 v_texCoord;

float magnification = 40.0;
float fringe = 0.75;     //chromatic aberration/fringing

vec4 combineTextures(vec2 texCoord)
{
    vec4 image = texture2D(CC_Texture0, texCoord);
    vec4 light = texture2D(u_lightTexture, texCoord);
    return image * light;
}

void main()
{
    vec4 outputColor = vec4(0);
    
    vec2 pixelPos = v_texCoord * u_resolution;
    if (u_radius > 1.0)
    {
        vec2 diff = u_cursor - pixelPos;
        float distFromCursor = length(diff);

        if (distFromCursor < u_radius)
        {
            // Lens effect
            vec2 diffRatio = diff / u_radius; // ranges [-1, 1]
            float dist = distFromCursor / u_radius; // range [0, 1]
            float inverseDist = 1.0 - (dist - 0.75); // range [0, 1]

            inverseDist = clamp(inverseDist, 0.0, 1.0);
            pixelPos = pixelPos + (diffRatio * inverseDist * magnification);
            vec2 texCoord = pixelPos / u_resolution;

            vec2 texel = vec2(1.0) / u_resolution.y;
            vec2 aberration = texel * fringe * dist;
            outputColor.r = combineTextures(texCoord + vec2(0.0, 1.0) * aberration).r;
            outputColor.g = combineTextures(texCoord + vec2(-0.866, -0.5) * aberration).g;
            outputColor.b = combineTextures(texCoord + vec2(0.866, -0.5) * aberration).b;
        }
        else
        {
            vec2 texCoord = pixelPos / u_resolution;
            outputColor = combineTextures(texCoord);
        }
        
        if (distFromCursor < u_radius + 1.0 &&
            distFromCursor >= u_radius)
        {
            outputColor.rgb *= 0.1;
        }
    }
    else
    {
        outputColor = combineTextures(v_texCoord);
    }

    gl_FragColor = outputColor;
}
