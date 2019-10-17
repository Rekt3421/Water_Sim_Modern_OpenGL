#version 400
uniform sampler2D diffuse_tex;

out vec4 fragcolor;           
in vec2 tex_coord;
     
void main(void)
{   
   vec4 tex_color = texture(diffuse_tex, tex_coord);
   fragcolor = tex_color;
}