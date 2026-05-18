#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdrEnabled;

void main()
{             
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdrColor * 1.0);

    if (hdrEnabled)
        FragColor = vec4(mapped, 1.0);
    else
        FragColor = vec4(hdrColor, 1.0);
}  

