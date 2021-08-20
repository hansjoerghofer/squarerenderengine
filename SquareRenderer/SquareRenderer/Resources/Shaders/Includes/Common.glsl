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