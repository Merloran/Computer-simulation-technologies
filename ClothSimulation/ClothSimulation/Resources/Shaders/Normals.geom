#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 worldNormal[];


void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(worldNormal[index], 0.0) * 0.4f;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal
}  