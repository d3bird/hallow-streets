#include "keyboard_manger.h"

keyboard_manger::keyboard_manger()
{
}

keyboard_manger::~keyboard_manger()
{
}

char* keyboard_manger::key_board_input(int key, int action) {
	char* output = NULL;
	bool true_action = action == GLFW_PRESS;

	if (true_action) {
		//	output = new char(' ');
		char temp = ' ';
		switch (key)
		{
		case GLFW_KEY_Q:
			temp = 'q';
			break;
		case GLFW_KEY_W:
			temp = 'w';
			break;
		case GLFW_KEY_E:
			temp = 'e';
			break;
		case GLFW_KEY_R:
			temp = 'r';
			break;
		case GLFW_KEY_T:
			temp = 't';
			break;
		case GLFW_KEY_Y:
			temp = 'y';
			break;
		case GLFW_KEY_U:
			temp = 'u';
			break;
		case GLFW_KEY_I:
			temp = 'i';
			break;
		case GLFW_KEY_O:
			temp = 'o';
			break;
		case GLFW_KEY_P:
			temp = 'p';
			break;
		case GLFW_KEY_A:
			temp = 'a';
			break;
		case GLFW_KEY_S:
			temp = 's';
			break;
		case GLFW_KEY_D:
			temp = 'd';
			break;
		case GLFW_KEY_F:
			temp = 'f';
			break;
		case GLFW_KEY_G:
			temp = 'g';
			break;
		case GLFW_KEY_H:
			temp = 'h';
			break;
		case GLFW_KEY_J:
			temp = 'j';
			break;
		case GLFW_KEY_K:
			temp = 'k';
			break;
		case GLFW_KEY_L:
			temp = 'l';
			break;
		case GLFW_KEY_Z:
			temp = 'z';
			break;
		case GLFW_KEY_X:
			temp = 'x';
			break;
		case GLFW_KEY_C:
			temp = 'c';
			break;
		case GLFW_KEY_V:
			temp = 'v';
			break;
		case GLFW_KEY_B:
			temp = 'b';
			break;
		case GLFW_KEY_N:
			temp = 'n';
			break;
		case GLFW_KEY_M:
			temp = 'm';
			break;

		case GLFW_KEY_1:
			temp = '1';
			break;
		case GLFW_KEY_2:
			temp = '2';
			break;
		case GLFW_KEY_3:
			temp = '3';
			break;
		case GLFW_KEY_4:
			temp = '4';
			break;
		case GLFW_KEY_5:
			temp = '5';
			break;
		case GLFW_KEY_6:
			temp = '6';
			break;
		case GLFW_KEY_7:
			temp = '7';
			break;
		case GLFW_KEY_8:
			temp = '8';
			break;
		case GLFW_KEY_9:
			temp = '9';
			break;
		case GLFW_KEY_0:
			temp = '0';
			break;

		default:

			break;
		}
		output = new char(temp);
	}
	return output;
}
