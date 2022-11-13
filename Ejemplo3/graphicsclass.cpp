////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_OpenGL = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_LightShaderSky = 0;
	m_BillShader = 0;
	m_Light = 0;
	angx = angy = angz = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(OpenGLClass* OpenGL, HWND hwnd)
{
	bool result;



	// Store a pointer to the OpenGL class object.
	m_OpenGL = OpenGL;

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 25.0f, 110.0f);
	//Son 2 texturas
	//Cambiar textura 2, 
	//Ver clase de multitextura
	terreno = new Terreno (hwnd, m_OpenGL, L"Terreno.jpg", L"Piedras.jpg", L"Piedras_normal.jpg", L"Terreno2.jpg", L"Terreno2Normal.jpg", L"MapaColores.png",
		(float)600, (float)600, 0, 1, 4, 5, 6);

	// Create the light shader object.
	m_LightShader = new LightShaderClass((char*)"light.vs", (char*)"light.ps");
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_OpenGL, hwnd);
	if(!result)
	{
		MessageBox(hwnd, (LPCSTR)"Could not initialize the light shader object.", (LPCSTR)"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-1.0f, 0.5f, -1.0f);

	sky = new SkyDome(hwnd, m_OpenGL, 32, 32, 256, L"sky.jpg");
	// Create the light shader object.
	m_LightShaderSky = new LightShaderClass((char*)"Sky.vs", (char*)"Sky.ps");
	if (!m_LightShaderSky)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShaderSky->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)"Could not initialize the light shader object.", (LPCSTR)"Error", MB_OK);
		return false;
	}

	
	// Create the light shader object.
	m_BillShader = new LightShaderClass((char*)"Sky.vs", (char*)"Sky.ps");
	if (!m_BillShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_BillShader->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)"Could not initialize the light shader object.", (LPCSTR)"Error", MB_OK);
		return false;
	}

	m_VeggieShader = new LightShaderClass((char*)"Bill.vs", (char*)"Bill.ps");
	if (!m_VeggieShader)
	{
		return false;
	}

	result = m_VeggieShader->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)"Could not initialize the light shader object.", (LPCSTR)"Error", MB_OK);
		return false;
	}

	float ancho = 0;


#pragma region Billboards
	                                                                    
	palmera = new Billboard(hwnd, m_OpenGL, L"palmera2.png", L"palmera2Norm.png", 50,    51,   50.0f, 100.f,  100.0f ,  -50,  -70);

#pragma endregion
#pragma region Carga de Modelos
												//los primeros 2 son jpg y luego png por lo que veo
	//LOS PRIMEROS 2 SON JPG Y LUEGO UN PNG
	//CABAÑA = new Modelos(hwnd, m_OpenGL, "cabaña.obj", L"superficie.jpg", L"SpecularMap.jpg", L"esfingeSpecular.png" , 2.0, 30.0, -0.200, -30.0, 2);
	
	m_ModeloShader = new LightShaderClass((char*)"Modelo.vs", (char*)"Modelo.ps");
	if (!m_ModeloShader)
	{
		return false;
	}
#pragma endregion
	// Initialize the light shader object.
	result = m_ModeloShader->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)"Could not initialize the light shader object.", (LPCSTR)"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown(m_OpenGL);
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light shader object.
	if (m_LightShaderSky)
	{
		m_LightShaderSky->Shutdown(m_OpenGL);
		delete m_LightShaderSky;
		m_LightShaderSky = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the terreno object.
	if (terreno)
	{
		delete terreno;
		m_Camera = 0;
	}

	// Release the pointer to the OpenGL class object.
	m_OpenGL = 0;

	return;
}


bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	rotation += 0.0174532925f * 2.0f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	float worldMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];
	float lightDirection[3];
	float diffuseLightColor[4];
	float worldMatrixBill[16];

	float factAtAmb = 0.2f;
	float factAtDif = 1.0f;

	// Clear the buffers to begin the scene.
	m_OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->SetRotation(angx, angy, angz);
	m_Camera->SetYPos(terreno->Superficie(m_Camera->GetXPos(), m_Camera->GetZPos()) + 3);
	m_Camera->Render(vel);

	// Get the world, view, and projection matrices from the opengl and camera objects.
	m_OpenGL->GetWorldMatrix(worldMatrix);
	m_OpenGL->GetWorldMatrix(worldMatrixBill);
	m_Camera->GetViewMatrix(viewMatrix);
	m_OpenGL->GetProjectionMatrix(projectionMatrix);

	// Get the light properties.
	m_Light->GetDirection(lightDirection);
	m_Light->GetDiffuseColor(diffuseLightColor);

	glDisable(GL_DEPTH_TEST);
	m_LightShaderSky->SetShader(m_OpenGL);
	m_LightShaderSky->PonMatriz4x4(m_OpenGL, (char*)"worldMatrix", worldMatrix);
	m_LightShaderSky->PonMatriz4x4(m_OpenGL, (char*)"viewMatrix", viewMatrix);
	m_LightShaderSky->PonMatriz4x4(m_OpenGL, (char*)"projectionMatrix", projectionMatrix);
	m_LightShaderSky->Pon1Entero(m_OpenGL, (char*)"cielo", 2);
	m_LightShaderSky->PonVec3(m_OpenGL, (char*)"lightDirection", lightDirection);
	m_LightShaderSky->PonVec4(m_OpenGL, (char*)"diffuseLightColor", diffuseLightColor);
	sky->Render(m_OpenGL);
	glEnable(GL_DEPTH_TEST);
	// Rotate the world matrix by the rotation value so that the triangle will spin.
	//m_OpenGL->MatrixRotationY(worldMatrix, rotation);

	// Set the light shader as the current shader program and set the matrices that it will use for rendering.
#pragma region Terreno
	m_LightShader->SetShader(m_OpenGL);
	m_LightShader->PonMatriz4x4(m_OpenGL, (char*)"worldMatrix", worldMatrix);
	m_LightShader->PonMatriz4x4(m_OpenGL, (char*)"viewMatrix", viewMatrix);
	m_LightShader->PonMatriz4x4(m_OpenGL, (char*)"projectionMatrix", projectionMatrix);
	m_LightShader->Pon1Entero(m_OpenGL, (char*)"shaderTexture", 0);
	m_LightShader->Pon1Entero(m_OpenGL, (char*)"shaderTexture2", 1);
	m_LightShader->Pon1Entero(m_OpenGL, (char*)"shaderTexture3", 4);
	m_LightShader->Pon1Entero(m_OpenGL, (char*)"shaderTexture4", 5);
	m_LightShader->Pon1Entero(m_OpenGL, (char*)"BlendMap", 6);
	m_LightShader->PonVec3(m_OpenGL, (char*)"lightDirection", lightDirection);
	m_LightShader->PonVec4(m_OpenGL, (char*)"diffuseLightColor", diffuseLightColor);
	// Render the model using the light shader.
	terreno->Render(m_OpenGL);

	
#pragma endregion

#pragma region Modelos
	//CargaModel(viewMatrix, projectionMatrix, lightDirection, diffuseLightColor, 2, CABAÑA, 8,7,6);
	//// Set the light shader as the current shader program and set the matrices that it will use for rendering.
	//m_ModeloShader->SetShader(m_OpenGL);
	//float modmatrix[16];
	//float rotacionYmod[16];
	//m_OpenGL->GetWorldMatrix(modmatrix);
	//m_OpenGL->MatrixTranslation(modmatrix, esfingeTest->x-200, esfingeTest->y + 2, esfingeTest->z);
	//m_OpenGL->MatrixRotationY(rotacionYmod, 0.0);
	//m_OpenGL->MatrixMultiply(modmatrix, rotacionYmod, modmatrix);
	//m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"worldMatrix", modmatrix);
	//m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"viewMatrix", viewMatrix);
	//m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"projectionMatrix", projectionMatrix);
	//m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"colorText", 6);
	//m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"normText", 7);
	//m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"specText", 8);
	//m_ModeloShader->PonVec3(m_OpenGL, (char*)"lightDirection", lightDirection);
	//float campos[3] = { m_Camera->GetXPos(), m_Camera->GetYPos(), m_Camera->GetZPos() };
	//m_ModeloShader->PonVec3(m_OpenGL, (char*)"camaraDir", campos);
	////enviar ka, kd y ks al shader
	//m_ModeloShader->PonVec4(m_OpenGL, (char*)"diffuseLightColor", diffuseLightColor);

	//esfingeTest->Render(m_OpenGL);
#pragma endregion




#pragma region carga Billboards
	//PALMERITAAAAAS
	m_OpenGL->MatrixTranslation(worldMatrixBill, palmera->x + 0.2, terreno->Superficie(palmera->x, palmera->z) , palmera->z + 0.2);
	float rotaypalmera1[16];
	m_OpenGL->MatrixRotationY(rotaypalmera1, -palmera->angBill(m_Camera->GetXPos(), m_Camera->GetZPos()));
	m_OpenGL->MatrixMultiply(worldMatrixBill, rotaypalmera1, worldMatrixBill);
	m_VeggieShader->SetShader(m_OpenGL);
	m_VeggieShader->PonMatriz4x4(m_OpenGL, (char*)"worldMatrix", worldMatrixBill);
	m_VeggieShader->PonMatriz4x4(m_OpenGL, (char*)"viewMatrix", viewMatrix);
	m_VeggieShader->PonMatriz4x4(m_OpenGL, (char*)"projectionMatrix", projectionMatrix);
	m_VeggieShader->Pon1Entero(m_OpenGL, (char*)"billtext", 50);
	m_VeggieShader->Pon1Entero(m_OpenGL, (char*)"billnorm", 51);
	m_VeggieShader->PonVec3(m_OpenGL, (char*)"lightDirection", lightDirection);
	m_VeggieShader->PonVec4(m_OpenGL, (char*)"diffuseLightColor", diffuseLightColor);
	palmera->Render(m_OpenGL);

	

#pragma endregion 
	///
	

	// Present the rendered scene to the screen.
	m_OpenGL->EndScene();


	return true;
}

void GraphicsClass::CargaModel(float* viewMatrix, float* projectionMatrix, float* lightDirection,
	float* diffuseLightColor, int NumeroModelo, Modelos* modelo,
	int normalId, int specIdtext, int colorText)

{
	//extern HWND m_hwnd;
	bool entra;
	entra = true;
	if (entra == true)
	{
		//Ya jala aaa
		float modmatrix[16];
		/*	float rotacionYmod[16];
			float viewMatrix[16];
			float projectionMatrix[16];
			float lightDirection[3];
			float diffuseLightColor[4];*/
		m_ModeloShader->SetShader(m_OpenGL);
		m_OpenGL->GetWorldMatrix(modmatrix);
		m_OpenGL->MatrixTranslation(modmatrix, modelo->x, terreno->Superficie(modelo->x, modelo->z), modelo->z);
		//m_OpenGL->MatrixRotationY(rotacionYmod, 0.0); en teoria esto ahra la rotacion
		//m_OpenGL->MatrixMultiply(modmatrix, rotacionYmod, modmatrix);
		m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"worldMatrix", modmatrix);
		m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"viewMatrix", viewMatrix);
		m_ModeloShader->PonMatriz4x4(m_OpenGL, (char*)"projectionMatrix", projectionMatrix);
		m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"colorText", colorText);
		//m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"colorText", colorText);
		/*m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"normText", normalId);
		m_ModeloShader->Pon1Entero(m_OpenGL, (char*)"specText", specIdtext);*/
		m_ModeloShader->PonVec3(m_OpenGL, (char*)"lightDirection", lightDirection);

		//m_ModeloShader->PonVec3(m_OpenGL, (char*)"camaraDir", campos);
		m_ModeloShader->PonVec4(m_OpenGL, (char*)"diffuseLightColor", diffuseLightColor);
		modelo->Render(m_OpenGL);
	}
};