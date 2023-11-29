#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;  
    vec3 emissive;
    float shininess;
    float opacity;
};

uniform sampler2D texBuff;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main() {

	// ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // emissive
    // not fully implemented yet
    vec3 emissive = material.emissive;

	FragColor = vec4((ambient + diffuse + specular + emissive), 1.0) * texture(texBuff, TexCoord);
    FragColor.a *= material.opacity;
}