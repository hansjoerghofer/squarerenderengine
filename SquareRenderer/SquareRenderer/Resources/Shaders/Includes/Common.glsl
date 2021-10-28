//? #version 450 core

//-----------------------------------------------
//            COMMON FUNCTIONS
//-----------------------------------------------

mat3 _constructTBN(
    in mat4 model,
    in vec3 normal,
    in vec3 tangent)
{
    // create tangent space transform
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B 
    // with the cross product of T and N
    vec3 B = cross(N, T);

    return mat3(T, B, N);
}

float _linearizeDepth(float depth, float n, float f)
{
    float c0 = ( 1 - f / n ) / 2;
    float c1 = ( 1 + f / n ) / 2;

    return 1 / ( c0 * depth + c1 );
}

// this is supposed to get the world position from the depth buffer
vec3 _worldPosFromDepth(float depth, vec2 uv, mat4 invP, mat4 invV) 
{
    float z = depth * 2.0 - 1.0;

    vec4 posCS = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 posVS = invP * posCS;

    // Perspective division
    posVS /= posVS.w;

    vec4 posWS = invV * posVS;

    return posWS.xyz;
}