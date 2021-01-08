#pragma once
#include <iostream>
#include <fstream>
#include <string>

struct option{
	std::string p_text;//what the player says
	std::string r_text;//what the npc says back

	bool return_to_start;

	option* next_choices;
	unsigned int next_choices_num;

	//if it requires prereq
	bool requires_prereq;
	int* prereq_index;
};

struct conversation{
	std::string openeing_text;
	std::string title;
	int npc;

	option* options_list;//all possible conversation options 
	bool* options_unlocked;
	unsigned int total;//how many options there are

	bool* prereq;

};

class dialog{
public:
	dialog();
	~dialog();

	void update();
	void init();

private:

	//check to makesure that the conversation is inputted correctly
	bool check_conversations(conversation* conv);

	void inport_conversation(std::string loc);
	void print_option(option* op);

};

