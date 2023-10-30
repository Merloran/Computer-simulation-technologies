#version 460 core
out vec4 color;

in vec3 worldNormal;  
in vec3 worldPosition;  
uniform vec3 objectColor = vec3(0.0f, 0.0f, 1.0f);

void main()
{
	const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	const vec3 lightPosition = vec3(10.0f, 50.0f, 10.0f);
    // ambient
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 lightDirection = normalize(lightPosition - worldPosition);
    float diff = max(dot(normalize(worldNormal), lightDirection), 0.0f);
    vec3 diffuse = diff * lightColor;
        
    vec3 result = (ambient + diffuse) * objectColor;
    color = vec4(result, 1.0);
}
