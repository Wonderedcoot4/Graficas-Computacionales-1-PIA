////////////////////////////////////////////////////////////////////////////////
// Filename: Modelo.vs
////////////////////////////////////////////////////////////////////////////////
#version 400


/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 inputPosition;
in vec3 inputNormal;
in vec2 inputTexCoord;
in vec3 inputTangent;
in vec3 inputBinormal;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out vec2 texCoord;
out vec3 normal;
out vec3 tangente;
out vec3 binormal;
out vec3 posicion;
out vec3 dirLuz;

///////////////////////
// UNIFORM VARIABLES //
///////////////////////
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightDirection;


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	posicion = vec3(gl_Position);

	dirLuz = normalize(lightDirection - posicion);

	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;
		
	normal = normalize(vec3(worldMatrix * vec4(inputNormal, 0)));
	tangente = normalize(vec3(worldMatrix * vec4(inputTangent, 0)));
	tangente = normalize(tangente - normal * dot(normal,tangente));
	binormal = normalize(cross(normal, tangente));
	// Store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;
	
}