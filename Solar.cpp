


#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "InitShader.h"
#include "Camera.h"
#include "CubeObj.h"
#include "GroundObj.h"
#include "RotatingCube3App.h"


// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;
static GLuint g_window_w;
static GLuint g_window_h;


//////////////////////////////////////////////////////////////////////
// Camera 
//////////////////////////////////////////////////////////////////////
static Camera g_camera;

//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////
GLuint s_program_id;



//////////////////////////////////////////////////////////////////////
//// Animation Parameters
//////////////////////////////////////////////////////////////////////
float g_elaped_time_s = 0.f;	// 




/**
InitOpenGL: 프로그램 초기 값 설정을 위해 최초 한 번 호출되는 함수. (main 함수 참고)
OpenGL에 관련한 초기 값과 프로그램에 필요한 다른 초기 값을 설정한다.
예를들어, VAO와 VBO를 여기서 생성할 수 있다.
*/
void InitOpenGL()
{
	//////////////////////////////////////////////////////////////////////////////////////
	//// 3. Shader Programs 등록
	////    Ref: https://www.khronos.org/opengl/wiki/Shader_Compilation
	//////////////////////////////////////////////////////////////////////////////////////
	s_program_id = CreateFromFiles("../RotatingCube3/v_shader.glsl", "../RotatingCube3/f_shader.glsl");
	glUseProgram(s_program_id);



	////////////////////////////////////////////////////////////////////////////////////
	//// 4. OpenGL 설정
	//////////////////////////////////////////////////////////////////////////////////////
	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	// Initial State of Camera
	// 카메라 초기 위치 설정한다.
	g_camera.lookAt(glm::vec3(3.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));


	////////////////////////////////////////////////////////////////////////////////////
	//// 5. VAO, VBO 생성
	////    Ref: https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
	///////////////////////////////////////////////////////////////////////////////////

	// Cube VAO 생성
	InitCube();

	// 바닥 격자 VAO 생성
	InitGround();

}







/**
ClearOpenGLResource: 프로그램이 끝나기 메모리 해제를 위해 한 번 호출되는 함수. (main 함수 참고)
프로그램에서 사용한 메모리를 여기에서 해제할 수 있다.
예를들어, VAO와 VBO를 여기서 지울 수 있다.
*/
void ClearOpenGLResource()
{
	// Delete (VAO, VBO)
	DeleteCube();
}





/**
Display: 윈도우 화면이 업데이트 될 필요가 있을 때 호출되는 callback 함수.

윈도우 상에 최종 결과를 렌더링 하는 코드는 이 함수 내에 구현해야한다.
원도우가 처음 열릴 때, 윈도우 크기가 바뀔 때, 다른 윈도우에 의해 화면의 일부
또는 전체가 가려졌다가 다시 나타날 때 등 시스템이 해당 윈도우 내의 그림에 대한
업데이트가 필요하다고 판단하는 경우 자동으로 호출된다.
강제 호출이 필요한 경우에는 glutPostRedisplay() 함수를 호출하면된다.

이 함수는 불시에 빈번하게 호출된다는 것을 명심하고, 윈도우 상태 변화와 무관한
1회성 코드는 가능한한 이 함수 밖에 구현해야한다. 특히 메모리 할당, VAO, VBO 생성
등의 하드웨어 점유를 시도하는 코드는 특별한 이유가 없다면 절대 이 함수에 포함시키면
안된다. 예를 들어, 메시 모델을 정의하고 VAO, VBO를 설정하는 부분은 최초 1회만
실행하면되므로 main() 함수 등 외부에 구현해야한다. 정의된 메시 모델을 프레임 버퍼에
그리도록 지시하는 코드만 이 함수에 구현하면 된다.

만일, 이 함수 내에서 동적 메모리 할당을 해야하는 경우가 있다면 해당 메모리는 반드시
이 함수가 끝나기 전에 해제 해야한다.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// 전체 화면을 지운다.
	// glClear는 Display 함수 가장 윗 부분에서 한 번만 호출되어야한다.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Vertex shader 의 matrix 변수들의 location을 받아온다.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");


	// Projection Transform Matrix 설정.
	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w/g_window_h, 0.01f, 10000.f);
	glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(g_camera.GetGLProjectionMatrix()));
	glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	// Camera Transform Matrix 설정.
	glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.GetGLViewMatrix()));
	//glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


	// 바닥 격자
	glm::mat4 T0(1.f); // 단위 행렬
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T0));
	
	DrawGround();


	//// 큐브 1. (태양)
	
	glm::mat4 scaleconti = glm::scale(glm::vec3(1.f+abs(0.3f*sin(g_elaped_time_s)), 1.f+abs(0.3f*sin(g_elaped_time_s)), 1.f+abs(0.3f*sin(g_elaped_time_s))));
	
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(scaleconti));
	
	DrawCube();


	//// 큐브 2. (지구)
	glm::mat4 rotate2sec = glm::rotate(glm::radians(360.f*(g_elaped_time_s / 2)), glm::vec3(0.f, 1.f, 0.f));		// y축 중심으로 2초에 한바퀴(360도) 회전
	glm::mat4 rotate20sec = glm::rotate(glm::radians(360.f*(g_elaped_time_s / 20)), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 trans_x = glm::translate(glm::vec3(2.0f, 0.5f, 0.f));
	glm::mat4 scale_half = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)); //scale은 음수가 들어가면 안댐
	glm::mat4 T2 = rotate20sec *trans_x * rotate2sec *scale_half;
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T2));

	DrawCube();


	//// 큐브 3. //제자리 회전
	glm::mat4 rotatemoon = glm::rotate(glm::radians(360.f*(g_elaped_time_s /2)),glm::vec3(0.f, 1.f* (g_elaped_time_s / 20), 0.f));
	glm::mat4 T3 =  rotatemoon *trans_x*scale_half*scale_half;
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T3));
	
	DrawCube();


	////// 큐브 4.
	//glm::mat4 trans_y = glm::translate(glm::vec3(-1.0f, 0.5f*sin(g_elaped_time_s), 0.f)); //sin은 증/감하는 함수, g_elaped은 증가만 하는 수(시간) 따라서 위아래로 왔다갔다함 0.5곱했으니 0.5~-0.5 왔다갔다
	//glm::mat4 T4 = trans_y * T1 * scale_half;
	//glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T4));

	//DrawCube();
	glm::mat4 trans = glm::translate(glm::vec3(3.0f, 0.f, 0.f));
	glm::mat4 T4 = rotate20sec*trans*scale_half;
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T4));

	DrawCube()

	// flipping the float buffers
	// glutSwapBuffers는 항상 Display 함수 가장 아래 부분에서 한 번만 호출되어야한다.
	glutSwapBuffers();
}



/**
Timer: 지정된 시간 후에 자동으로 호출되는 callback 함수.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{
	// Timer 호출 시간 간격을 누적하여, 최초 Timer가 호출된 후부터 현재까지 흘러간 계산한다.
	g_elaped_time_s += value/1000.f;

	// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
	// 결과적으로 Display() 함수가 호출 된다.
	glutPostRedisplay();

	// 1/60 초 후에 Timer 함수가 다시 호출되로록 한다.
	// Timer 함수 가 동일한 시간 간격으로 반복 호출되게하여,
	// 애니메이션 효과를 표현할 수 있다
	glutTimerFunc((unsigned int)(1000/60), Timer, (1000/60));
}



/**
Reshape: 윈도우의 크기가 조정될 때마다 자동으로 호출되는 callback 함수.

@param w, h는 각각 조정된 윈도우의 가로 크기와 세로 크기 (픽셀 단위).
ref: https://www.opengl.org/resources/libraries/glut/spec3/node48.html#SECTION00083000000000000000
*/
void Reshape(int w, int h)
{
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;

	//  w : window width   h : window height
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

/**
Keyboard: 키보드 입력이 있을 때마다 자동으로 호출되는 함수.
@param key는 눌려진 키보드의 문자값.
@param x,y는 현재 마우스 포인터의 좌표값.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void Keyboard(unsigned char key, int x, int y)
{
	// keyboard '1' 이 눌려졌을 때.
	if (key == '1')
	{
		// Fragment shader에 정의 되어있는 'shading_mode' 변수의 location을 받아온다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");

		// 'shading_mode' 값으로 1을 설정.
		glUniform1i(shading_mode_loc, 1);


		// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
		// 결과적으로 Display() 함수를 호출하게 된다.
		glutPostRedisplay();
	}

	// keyboard '2' 가 눌려졌을 때.
	else if (key == '2')
	{
		// Fragment shader에 정의 되어있는 'shading_mode' 변수의 location을 받아온다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");

		// 'shading_mode' 값으로 2를 설정.
		glUniform1i(shading_mode_loc, 2);


		// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
		// 결과적으로 Display() 함수를 호출하게 된다.
		glutPostRedisplay();
	}
}





/**
Mouse: 마우스 버튼이 입력될 때마다 자동으로 호출되는 함수.
파라메터의 의미는 다음과 같다.
@param button: 사용된 버튼의 종류
  GLUT_LEFT_BUTTON - 왼쪽 버튼
  GLUT_RIGHT_BUTTON - 오른쪽 버튼
  GLUT_MIDDLE_BUTTON - 가운데 버튼 (휠이 눌러졌을 때)
  3 - 마우스 휠 (휠이 위로 돌아 갔음).
  4 - 마우스 휠 (휠이 아래로 돌아 갔음).
@param state: 조작 상태
  GLUT_DOWN - 눌러 졌음
  GLUT_UP - 놓여졌음
@param x,y: 조작이 일어났을 때, 마우스 포인터의 좌표값.
*/
void Mouse(int button, int state, int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if (button == 3)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, -1, 0.01f);
		glutPostRedisplay();
	}
	else if (button == 4)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, 1, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}





/**
MouseMotion: 마우스 포인터가 움직일 때마다 자동으로 호출되는 함수.
@prarm x,y는 현재 마우스 포인터의 좌표값을 나타낸다.
*/
void MouseMotion(int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;

	if (g_left_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if (g_right_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}
