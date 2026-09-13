#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 baseColor;

void main(){
    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDir);
    float diff = max(dot(N,L), 0.0);
    vec3 diffuse = diff * baseColor;
    vec3 ambient = 0.2 * baseColor;
    vec3 color = ambient + diffuse;
    FragColor = vec4(color, 1.0);
}
