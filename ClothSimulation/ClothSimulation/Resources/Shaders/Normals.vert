#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 view;			//64
uniform mat4 projection;	//64
uniform mat4 model;

out vec3 worldNormal;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	worldNormal = normalize(vec3(projection * vec4(normalMatrix * normal, 0.0)));
}