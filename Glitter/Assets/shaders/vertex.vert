#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTex;
layout (location = 3) in vec3 inTang;
layout (location = 4) in vec3 inBitang;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec2 tex;
out vec3 normal;
out vec3 fragPos;
out vec3 tangLightPos;
out vec3 tangFragPos;
out vec3 tangViewPos;

uniform Light light;
uniform vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(inPos, 1.0);

    tex = inTex;
    normal = mat3(transpose(inverse(model))) * inNormal; 

    fragPos = vec3(model * vec4(inPos, 1.0));

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 T = normalize(normalMatrix * inTang);
    vec3 N = normalize(normalMatrix * inNormal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    tangLightPos = TBN * light.position;
    tangFragPos = TBN * fragPos;
    tangViewPos = TBN * viewPos;
}

