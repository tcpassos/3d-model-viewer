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
    float shininess;
    float opacity;
};

uniform vec3 lightPos;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform float ambientStrength = 0.3;
uniform float specularStrength = 0.5;
uniform float diffuseStrength = 1.0;

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

	FragColor = vec4((ambient + diffuse + specular), 1.0) * texture(texBuff, TexCoord) * material.opacity;
}