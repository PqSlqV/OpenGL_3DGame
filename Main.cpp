#include "main.h"

Window Main_Window(1280, 720, "Game");
Camera camera;
bool cntrl = false;
bool five = false;
std::set<C_Animation> Animations;
std::set<C_Skelet> Skelets;
std::set<C_Mesh> Meshes;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F11: Main_Window.Set_FullScreen(); break;
		case GLFW_KEY_F:camera.Set_Free_Camera(); break;
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
		case GLFW_KEY_LEFT_CONTROL:
		{
			if (cntrl)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				cntrl = false;
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				glfwSetCursorPos(window, Main_Window.Get_Width() / 2, Main_Window.Get_Height() / 2);
				cntrl = true;
			}
			break;
		}
		case GLFW_KEY_KP_5: five == true ? five = false : five = true; break;
		default:
			break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Main_Window.Set_Size(width, height);
	camera.Set_Projection_Matrix(45.0f, 1.0f, 5000.0f, (float)width / (float)height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (cntrl)
	{
		int X = Main_Window.Get_Width();
		int Y = Main_Window.Get_Height();
		float TurnSpeedX = float((xpos - X / 2) / 10);
		float TurnSpeedY = float((Y / 2 - ypos) / 10);
		camera.Set_Rotate_View(-TurnSpeedY, TurnSpeedX);
		glfwSetCursorPos(window, X / 2, Y / 2);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}


NPC Npc;
NPC Npc2;
void processInput(GLFWwindow* window)
{
	Vec3 move;
	float cam_speed = 0.05f;
	bool Move_act = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cam_speed = 2.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		move.z = cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		move.z = -cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		move.z = -cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		move.x = cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		move.x = -cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		move.y = -cam_speed;
		Move_act = true;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		move.y = cam_speed;
		Move_act = true;
	}
	if (Move_act)
	{
		camera.Set_Move(move);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.Set_Rotate_View(-5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.Set_Rotate_View(5.0f);
	}
	NPC* npc;
	NPC* npc2;
	if (five)
	{
		npc = &Npc;
		npc2 = &Npc2;
	}
	else
	{
		npc = &Npc2;
		npc2 = &Npc;
	}
		
	if (glfwGetKey(window, GLFW_KEY_KP_8))
	{
		Vec3 move(0.0f, 0.0f, cam_speed);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2))
	{
		Vec3 move(0.0f, 0.0f, -cam_speed);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4))
	{
		Vec3 move(cam_speed, 0.0f,  0.0f);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6))
	{
		Vec3 move(-cam_speed, 0.0f,  0.0f);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_3))
	{
		Vec3 move( 0.0f, cam_speed, 0.0f);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_1))
	{
		Vec3 move( 0.0f, -cam_speed, 0.0f);
		npc->Set_Position(move, *npc2);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_7))
	{
		npc->Set_RotateY(5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9))
	{
		npc->Set_RotateY(-5.0f);
	}
	camera.Set_View_Matrix();
}


int main()
{
	auto start = std::chrono::high_resolution_clock::now();

	Shader SunShader("Sun");
	Sunshine Sun;
	Sun.Set_Shader(SunShader);

	Shader LandShader("Land");
	World Land;
	Land.Set_Shader(LandShader);

	Shader NpcShader("Shader");
	
	Npc.Set_Shader(NpcShader);
	Npc2.Set_Shader(NpcShader);
	camera.Set_Projection_Matrix(45.0f, 1.0f, 5000.0f, (float)Main_Window.Get_Width() / (float)Main_Window.Get_Height());

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - start;
	std::cout << duration.count() << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLFWwindow* window = Main_Window.Get_Window();
	int i = 0;

	float Last_Time = 0.0f;
	Npc.Set_Animation(0);
	{
		Vec3 a(0.0f, 0.0f, 5.0f);
		Npc.Set_Position(a);
	}
	Npc2.Set_Animation(0);
	{
		Vec3 a(0.0f, 0.0f, -5.0f);
		Npc2.Set_Position(a);
	}
	while (!glfwWindowShouldClose(window))
	{
		float Curr_Time = float(glfwGetTime());
		float Delta_Time = Curr_Time - Last_Time;
		Last_Time = Curr_Time;
		glfwPollEvents();
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthFunc(GL_LEQUAL);
		SunShader.use();
		Sun.Draw();
		glDepthFunc(GL_LESS);


		LandShader.use();
		Land.Draw();

		NpcShader.use();

		//Npc.Play_Animation(Delta_Time);
		Npc.Draw();

	   // Npc2.Play_Animation(Delta_Time);
		Npc2.Draw();

		glfwSwapBuffers(window);
		i++;
		if (i > 360)
			i = 0;
	}
	return 0;
}