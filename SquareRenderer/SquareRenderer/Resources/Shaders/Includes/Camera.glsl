//? #version 450 core

layout(std140) uniform CameraUBO
{
    mat4 _P;    // projection mat
    mat4 _invP; // inverse projection mat

    mat4 _V;    // view mat
    mat4 _invV; // inverse view mat

    mat4 _VP;   // view-projection mat

    vec4 _dim;  // resolution (w,h,1/w,1/h)
    vec4 _clip;  // clipping planes (n,f,1/n,1/f)
};

vec3 _cameraPosition()
{
    return _invV[3].xyz;
}