#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 selection;

uniform mat4 projection;
uniform mat4 view;

uniform bool u_color1;
uniform bool u_color2;
uniform bool u_color3;

uniform bool first;

void main()
{
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);

	float colors_amount = 255;

	int id = gl_InstanceID%255;
	float color1 = 0.0;
	float color2 = 0.0;
	float color3 = 0.0;
	
	if(u_color1){
		color1 = float(0.003) + (float(id)/colors_amount);
	}
	if(u_color2){
		color2 = float(0.003) + (float(id)/colors_amount);
	}
	if(u_color3){
		color2 = float(0.003) + (float(id)/colors_amount);
	}



    selection = vec3(color1, color2, color3);

}