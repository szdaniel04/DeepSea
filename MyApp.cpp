#include "MyApp.h"
#include "includes/ObjParser.h"
#include "includes/SDL_GLDebugMessageCallback.h"
#include "imgui/imgui.h"

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// engedélyezzük és állítsuk be a debug callback függvényt ha debug context-ben vagyunk 
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	AttachShader(m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert");
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_ZH.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

MeshObject<Vertex> createQuad()
{
	MeshObject<Vertex> mesh;

	mesh.vertexArray = {
		{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	};

	mesh.indexArray = { 0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7 };

	return mesh;
}

void CMyApp::InitGeometry()
{
	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{0, offsetof(Vertex, position), 3, GL_FLOAT},
		{1, offsetof(Vertex, normal), 3, GL_FLOAT},
		{2, offsetof(Vertex, texcoord), 2, GL_FLOAT},
	};

	m_quadGPU = CreateGLObjectFromMesh(createQuad(), vertexAttribList);
	m_pufferFishGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/PufferFish.obj"), vertexAttribList);
	m_subGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/sub.obj"), vertexAttribList);
	m_armGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/Arm.obj"), vertexAttribList);
	m_clawGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/Claw.obj"), vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_quadGPU);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		ImageRGBA image = ImageFromFile("Assets/oceanbottom.png");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_OceanBottomTextureID);
		glTextureStorage2D(m_OceanBottomTextureID, NumberOfMIPLevels(image), GL_RGBA8, image.width, image.height);
		glTextureSubImage2D(m_OceanBottomTextureID, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glGenerateTextureMipmap(m_OceanBottomTextureID);
	}
	{
		ImageRGBA image = ImageFromFile("Assets/ocean.png");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_OceanTextureID);
		glTextureStorage2D(m_OceanTextureID, NumberOfMIPLevels(image), GL_RGBA8, image.width, image.height);
		glTextureSubImage2D(m_OceanTextureID, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glGenerateTextureMipmap(m_OceanTextureID);
	}
	{
		ImageRGBA image = ImageFromFile("Assets/sub.png");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_SubTextureID);
		glTextureStorage2D(m_SubTextureID, NumberOfMIPLevels(image), GL_RGBA8, image.width, image.height);
		glTextureSubImage2D(m_SubTextureID, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glGenerateTextureMipmap(m_SubTextureID);
	}
	{
		ImageRGBA image = ImageFromFile("Assets/Caustics.png");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_CausticsTextureID);
		glTextureStorage2D(m_CausticsTextureID, NumberOfMIPLevels(image), GL_RGBA8, image.width, image.height);
		glTextureSubImage2D(m_CausticsTextureID, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glGenerateTextureMipmap(m_CausticsTextureID);
	}
	{
		ImageRGBA image = ImageFromFile("Assets/PufferFish.png");
		glCreateTextures(GL_TEXTURE_2D, 1, &m_PufferFishTextureID);
		glTextureStorage2D(m_PufferFishTextureID, NumberOfMIPLevels(image), GL_RGBA8, image.width, image.height);
		glTextureSubImage2D(m_PufferFishTextureID, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glGenerateTextureMipmap(m_PufferFishTextureID);
	}
	
}

void CMyApp::CleanTextures()
{
	glDeleteSamplers(1, &m_SamplerID);
	glDeleteTextures(1, &m_OceanTextureID);
	glDeleteTextures(1, &m_OceanBottomTextureID);
	glDeleteTextures(1, &m_CausticsTextureID);
	glDeleteTextures(1, &m_SubTextureID);
	glDeleteTextures(1, &m_PufferFishTextureID);

}

bool CMyApp::Init()
{
	SetupDebugCallback();

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();



	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// kamera
	m_camera.SetView(
		glm::vec3(0.0, -55.0, 100.0),  // honnan nézzük a színteret	   - eye
		glm::vec3(0.0, 50.0, 105.0),  // a színtér melyik pontját nézzük - at
		glm::vec3(0.0, 1.0, 0.0)); // felfelé mutató irány a világban - up

	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

static bool HitPlane(const Ray& ray, const glm::vec3& planeQ, const glm::vec3& planeI, const glm::vec3& planeJ, Intersection& result)
{
	// sík parametrikus egyenlete: palneQ + u * planeI + v * planeJ
	glm::mat3 A(-ray.direction, planeI, planeJ);
	glm::vec3 B = ray.origin - planeQ;

	if (fabsf(glm::determinant(A)) < 1e-6) return false;
	glm::vec3 X = glm::inverse(A) * B;

	if (X.x < 0.0) {
		return false;
	}
	result.t = X.x;
	result.uv.x = X.y;
	result.uv.y = X.z;

	return true;
}


static bool HitSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius, float& t)
{
	glm::vec3 p_m_c = rayOrigin - sphereCenter;
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayDir, p_m_c);
	float c = glm::dot(p_m_c, p_m_c) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	float sqrtDiscriminant = sqrtf(discriminant);

	// Mivel 2*a, es sqrt(D) mindig pozitívak, ezért tudjuk, hogy t0 < t1
	float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
	float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

	if (t1 < 0.0f) // mivel t0 < t1, ha t1 negatív, akkor t0 is az
	{
		return false;
	}

	if (t0 < 0.0f)
	{
		t = t1;
	}
	else
	{
		t = t0;
	}

	return true;
}

Ray CMyApp::CalculatePixelRay(glm::vec2 pixel) const
{
	// NDC koordináták kiszámítása
	glm::vec3 pickedNDC = glm::vec3(
		2.0f * (pixel.x + 0.5f) / m_windowSize.x - 1.0f,
		1.0f - 2.0f * (pixel.y + 0.5f) / m_windowSize.y, 0.0f);

	// A világ koordináták kiszámítása az inverz ViewProj mátrix segítségével
	glm::vec4 pickedWorld = glm::inverse(m_camera.GetViewProj()) * glm::vec4(pickedNDC, 1.0f);
	pickedWorld /= pickedWorld.w; // homogén osztás
	Ray ray;

	// Raycasting kezdőpontja a kamera pozíciója
	ray.origin = m_camera.GetEye();
	// Iránya a kamera pozíciójából a kattintott pont világ koordinátái felé
	// FIGYELEM: NEM egység hosszúságú vektor!
	ray.direction = glm::vec3(pickedWorld) - ray.origin;
	return ray;
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	if (m_IsPicking) {
		// a felhasználó Ctrl + kattintott, itt kezeljük le
		// sugár indítása a kattintott pixelen át
		Ray ray = CalculatePixelRay(glm::vec2(m_PickedPixel.x, m_PickedPixel.y));
		
        m_IsPicking = false;
	}

	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	glm::vec3 cam = m_camera.GetEye();
	float y = cam.y;
	glm::vec3 clr = glm::exp(glm::vec3(0.014f, 0.01f, 0.004f) * glm::min(0.0f, y));
	glClearColor(clr.r, clr.g, clr.b, 1.0f);
}

void CMyApp::SetCommonUniforms()
{
	// - Uniform paraméterek

	// view és projekciós mátrix
	glProgramUniformMatrix4fv(m_programID, ul(m_programID, "viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glProgramUniform3fv(m_programID, ul(m_programID, "cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glProgramUniform4fv(m_programID, ul(m_programID, "lightPos"), 1, glm::value_ptr(m_lightPos));
	glProgramUniform4fv(m_programID, ul(m_programID, "lightPos2"), 1, glm::value_ptr(m_lightPos2));
	
	glProgramUniform3fv(m_programID, ul(m_programID, "La"), 1, glm::value_ptr(m_La));
	glProgramUniform3fv(m_programID, ul(m_programID, "Ld"), 1, glm::value_ptr(m_Ld));
	glProgramUniform3fv(m_programID, ul(m_programID, "Ls"), 1, glm::value_ptr(m_Ls));
	
	glProgramUniform1f(m_programID, ul(m_programID, "lightConstantAttenuation"), m_lightConstantAttenuation);
	glProgramUniform1f(m_programID, ul(m_programID, "lightLinearAttenuation"), m_lightLinearAttenuation);
	glProgramUniform1f(m_programID, ul(m_programID, "lightQuadraticAttenuation"), m_lightQuadraticAttenuation);
	glProgramUniform3fv(m_programID, ul(m_programID, "cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));

	glProgramUniform1f(m_programID, ul(m_programID, "m_ElapsedTimeInSec"), m_ElapsedTimeInSec);

}


void CMyApp::Draw(OGLObject gpu, GLuint textureID, glm::mat4 world){
	SetCommonUniforms();
	glUseProgram(m_programID);
	glProgramUniformMatrix4fv(m_programID, ul(m_programID, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glProgramUniformMatrix4fv(m_programID, ul(m_programID, "worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glProgramUniform1i(m_programID, ul(m_programID, "texImage"), 0);
	glBindVertexArray(gpu.vaoID);
	glBindTextureUnit(0, textureID);
	glBindSampler(0, m_SamplerID);
	glDrawElements(GL_TRIANGLES, gpu.count, GL_UNSIGNED_INT, nullptr);
	glBindTextureUnit(0, 0);
	glBindSampler(0, 0);
	glBindVertexArray(0);
}

void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//ocean
	glProgramUniform1i(m_programID, ul(m_programID, "state"), SHADER_STATE_OCEAN);
	glm::mat4 oceanBottom = glm::mat4(1.f);
	oceanBottom = glm::translate(glm::vec3(.0,-150.0,.0)) * glm::scale(glm::vec3(1000.)) * glm::rotate(oceanBottom,float(M_PI/2),glm::vec3(1.,0.,0.));
	Draw(m_quadGPU, m_OceanBottomTextureID, oceanBottom);

	glProgramUniform1i(m_programID, ul(m_programID, "state"), SHADER_STATE_OCEAN_SURFACE);
	glm::mat4 oceanSurface = glm::mat4(1.f);
	oceanSurface = glm::scale(glm::vec3(1000.)) * glm::rotate(oceanSurface,float(M_PI/2),glm::vec3(1.,0.,0.));
	Draw(m_quadGPU, m_OceanTextureID, oceanSurface);
	//pufferfishes
	glProgramUniform1i(m_programID, ul(m_programID, "state"), SHADER_STATE_DEFAULT);
	int N = 5;
	glm::mat4 pos;
	for(int i = 0; i < N; ++i){
		pos = glm::translate(glm::vec3(100*cos(2*M_PI*i/N), -140+130*i/N, 100*sin(2*M_PI*i/N)));
		Draw(m_pufferFishGPU, m_PufferFishTextureID, pos);
	}
	//sub
	glm::mat4 sub = glm::translate(glm::vec3(0,-140,0));
	m_lightPos2 = m_lightPos2 * sub * glm::translate(glm::vec3(-13,9,0)); 
	Draw(m_subGPU,m_SubTextureID, sub);
	glm::mat4 arm = sub * glm::translate(glm::vec3(18.75,-3.75,0.)) *glm::rotate(armRotation,glm::vec3(0,1,0));
	Draw(m_armGPU,m_SubTextureID,arm);
	glm::mat4 rclaw = arm * glm::translate(glm::vec3(9,0,1.75)) * glm::rotate(float(M_PI), glm::vec3(1,0,0))* glm::rotate(clawRotation, glm::vec3(0,1,0));
	glm::mat4 lclaw = arm * glm::translate(glm::vec3(9,0,-1.75)) * glm::rotate(clawRotation, glm::vec3(0,1,0));
	Draw(m_clawGPU,m_SubTextureID,rclaw);
	Draw(m_clawGPU,m_SubTextureID,lclaw);
	glBindVertexArray(0);
	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::RenderGUI()
{
	ImGui::SliderAngle("Arm rotation:", &armRotation, -90, 90);
	ImGui::SliderAngle("Claw rotation:", &clawRotation, 0, 90);

	
	ImGui::Checkbox("Red signal light", &enableLight);
	glProgramUniform1i(m_programID, ul(m_programID, "enableRedLight"), enableLight);
}


void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.repeat == 0) // Először lett megnyomva
	{
		if (key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL)
		{
			CleanShaders();
			InitShaders();
		}
		if (key.keysym.sym == SDLK_F1)
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat.
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Váltogassuk FILL és LINE között!
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Állítsuk be az újat!
		}

		if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
		{
			m_IsCtrlDown = true;
		}
	}
	m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
	if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
	{
		m_IsCtrlDown = false;
	}
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	if ( m_IsCtrlDown )
	{
		m_IsPicking = true;
	}
	m_PickedPixel = { mouse.x, mouse.y };
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_cameraManipulator.MouseWheel(wheel);
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_windowSize = glm::uvec2(_w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);
}


void CMyApp::OtherEvent(const SDL_Event& ev)
{

}