#include "dialog.h"

dialog::dialog(){


}

dialog::~dialog(){

}

void dialog::update() {

}

void dialog::init() {
	std::cout << "creating the dialog engine" << std::endl;

	inport_conversation("test1.con");

	std::cout << "finished the dialog engine" << std::endl;
}


//check to makesure that the conversation is inputted correctly
bool dialog::check_conversations(conversation* conv) {
	if (conv == NULL) {
		std::cout << "the inputted conv to check was null" << std::endl;
		return false;
	}


	//print_option()
}

void dialog::print_option(option* op) {


}

void dialog::inport_conversation(std::string loc) {

	std::string line;
	std::ifstream myfile(loc);

	int state =0;

	if (myfile.is_open()){
		while (getline(myfile, line)){
			std::cout << line << '\n';
			if (state == 1) {//get the base data for the conversation

			}
			else if (state == 2) {//get the conversation structure 

			}
			else {//get the conversation options

			}
		}
		myfile.close();
	}

	else std::cout << "Unable to open file: "<<loc<<std::endl;

}
