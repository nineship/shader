#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec3 aText;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   FragPos = vec3(model * vec4(aPos, 1.0));
   ourNor = mat3(transpose(inverse(model))) * aNor;
   TexCoord = vec2(aText.x, aText.y);
};