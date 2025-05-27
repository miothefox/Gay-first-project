#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <Windows.h>

#include <glad/gl.h>
#include "wglext.h"

bool windowShouldClose = false;

LRESULT WINAPI window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg) {
	case WM_DESTROY:
		windowShouldClose = true;
		return 0;
	default:
		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}
}


void poll_events() {
	MSG message;
	while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

HMODULE glModule;


void(*get_proc_address(char const* procname))(void) {
	void(*proc)(void) = (void(*)(void))wglGetProcAddress(procname);
	if (!proc) proc = (void(*)(void))GetProcAddress(glModule, procname);
	return proc;
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){

#ifdef _DEBUG
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stderr);
#endif

	char const* kClassName = "GayFoxClassName";
	WNDCLASSA wc = {};
	wc.lpfnWndProc = &window_proc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kClassName;
	RegisterClassA(&wc);

	HWND window = CreateWindowExA(
		CS_OWNDC,
		kClassName,
		"Gay Fox Game",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
		
		);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	HDC hdc = GetDC(window);
	int format = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, format, &pfd);

	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);


	int attribs[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	WGL_CONTEXT_MINOR_VERSION_ARB, 5,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 
	NULL,
	};

	hglrc = wglCreateContextAttribsARB(hdc, NULL, attribs);
	wglMakeCurrent(hdc, hglrc);

	glModule = LoadLibraryA("opengl32");
	gladLoadGL(&get_proc_address);

	printf("%s\n", glGetString(GL_VENDOR));
	printf("%s\n", glGetString(GL_RENDERER));
	printf("%s\n", glGetString(GL_VERSION));

	ShowWindow(window, nShowCmd);

	glClearColor(1.0f, 0.5f, 0.0f, 1.0f);

	float data[] = {
		-0.5f, -0.5f, +0.0f,
		+0.5f, -0.5f, +0.0f,
		+0.0f, +0.5f, +0.0f
	};

	GLuint vao, vbo;
	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);

	glNamedBufferStorage(vbo, sizeof(data), data, GL_NONE);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, 12);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);

	char const* vertSource = R"[[(
#version 450 core
layout (location = 0) in vec3 iPosition;
out vec3 vPosition;

void main(void) {
gl_Position = vec4(iPosition, 1.0);
vPosition = iPosition + vec3(0.5);
}

)[[";

	char const* fragSource = R"[[(
#version 450 core

in vec3 vPosition;
layout (location = 0) out vec4 oColor;

void main(void) {
oColor = vec4(vPosition, 1.0);
}
)[[";

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertShader, 1, &vertSource, nullptr);
	glShaderSource(fragShader, 1, &fragSource, nullptr);

	glCompileShader(vertShader);
	glCompileShader(fragShader);

	GLint param;
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &param);

	if (param > 0) {
		char* infoLog = (char*)malloc(param);
		glGetShaderInfoLog(vertShader, param, nullptr, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}

	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &param);
	
	if (param > 0) {
		char* infoLog = (char*)malloc(param);
		glGetShaderInfoLog(fragShader, param, nullptr, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &param);

	if (param > 0) {
		char* infoLog = (char*)malloc(param);
		glGetProgramInfoLog(program, param, nullptr, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}

	while (!windowShouldClose) {
		poll_events();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
	}

	ShowWindow(window, SW_HIDE);

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	PostQuitMessage(0);
	poll_events();
	
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	DestroyWindow(window);
	UnregisterClassA(kClassName, NULL);
	}