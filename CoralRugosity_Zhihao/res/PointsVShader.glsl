#version 120
uniform mat4 mat_projection;
uniform mat4 mat_view;
uniform mat4 mat_model;

attribute vec3 a_position;
varying vec3 v_position;


void main()
{
    gl_Position = mat_projection* mat_view * mat_model * vec4(a_position,1.0);

    v_position = vec3(mat_model*vec4(a_position,1.0));
}
