#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "includes/Camera.h"
#include "includes/CameraManipulator.h"
#include "includes/GLUtils.hpp"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő
	float DeltaTimeInSec = 0.0f;   // Előző Update óta eltelt idő
};

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Intersection
{
	glm::vec2 uv;
	float t;
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update(const SUpdateInfo&);
	void Render();
	void RenderGUI();

	void Draw(OGLObject, GLuint, glm::mat4);

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);

	void OtherEvent(const SDL_Event&);

protected:
	void SetupDebugCallback();

	//
	// Adat változók
	//

	float m_ElapsedTimeInSec = 0.0f;

	// Picking

	glm::ivec2 m_PickedPixel = glm::ivec2( 0, 0 );
	bool m_IsPicking = false;
	bool m_IsCtrlDown = false;

	glm::uvec2 m_windowSize = glm::uvec2(0, 0);

	Ray CalculatePixelRay(glm::vec2 pickerPos) const;


	// Kamera
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	//
	// OpenGL-es dolgok
	//

	// shaderekhez szükséges változók
	GLuint m_programID = 0; // shaderek programja
	glm::vec4 m_lightPos = glm::vec4(0,1,0,0);
	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0 );
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0 );
	glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0 );
	float m_lightConstantAttenuation = 1;
	float m_lightLinearAttenuation = 0;
	float m_lightQuadraticAttenuation = 0;

	glm::vec4 m_lightPos2 = glm::vec4(0,1,0,1);
	
	// Shaderek inicializálása, és törlése
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók

	void SetCommonUniforms();

	OGLObject m_quadGPU = {};
	OGLObject m_pufferFishGPU = {};
	OGLObject m_subGPU = {};
	OGLObject m_clawGPU = {};
	OGLObject m_armGPU = {};

	// Geometria inicializálása, és törlése
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói
	GLuint m_SamplerID = 0;

	GLuint m_OceanBottomTextureID = 0;
	GLuint m_OceanTextureID = 0;
	GLuint m_PufferFishTextureID = 0;
	GLuint m_SubTextureID = 0;
	GLuint m_CausticsTextureID = 0;
	GLuint m_ClawTextureID = 0;

	void InitTextures();
	void CleanTextures();

	float armRotation = 0.;
	float clawRotation = 45.;

	bool enableLight = true;

	const int SHADER_STATE_OCEAN = 0;
	const int SHADER_STATE_DEFAULT = 1;
	const int SHADER_STATE_OCEAN_SURFACE = 2;
};
