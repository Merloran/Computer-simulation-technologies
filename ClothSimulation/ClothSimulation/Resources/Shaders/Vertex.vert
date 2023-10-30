#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
    worldPosition = vec3(model * vec4(position, 1.0f));
    worldNormal = mat3(transpose(inverse(model))) * normal;
	gl_Position = projection * view * vec4(worldPosition, 1.0f);
}