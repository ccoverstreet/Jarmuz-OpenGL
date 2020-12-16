#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
varying vec4 pix;
uniform vec4 pos_offset;
uniform mat2 rotation;

void main()
{
	pix = vec4(rotation * vec2(position.x, position.y), 0, 1) + pos_offset;	
	gl_Position = pix;
}

#shader fragment
#version 330 core

varying vec4 pix;
layout(location = 0) out vec4 color;
in vec4 gl_FragCoord;

uniform vec4 my_color;

void main()
{
	color = my_color;
};


