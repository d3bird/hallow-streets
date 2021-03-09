#include "GUI.h"

GUI::GUI() {
    Time = NULL;
    deltatime = NULL;
    gui_window = NULL;

    online = false;
    server = false;

    show_demo_window = true;
    show_another_window = false;
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    tab = 0;
    my_tool_active = true;
    //my_color = ImVec4(1, 1, 0, 1);

    my_color = new float[4];
    my_color[0] = 0;
    my_color[1] = 0;
    my_color[2] = 0;
    my_color[3] = 0;
    

    overal = new float(0.5f);
    effect = new float(0.5f);
    effects_3D = new float(0.5f);

    number_of_values = 10;
    my_values = new float[number_of_values];
    for (int i = 0; i < number_of_values; i++) {
        //my_values[i] = 0;
        last_frames.push_back(0);
    }
    passed_time = 0;
    record_check_point = 1;
}

GUI::~GUI(){
    delete[] my_color;
    delete overal;
    delete effect;
    delete effects_3D;
}


void GUI::draw() {
    update_values();

    debug_info();
    //draw_demo_window();
}


void GUI::update_values() {
    if (passed_time > record_check_point) {
        last_frames.insert(last_frames.begin(), ImGui::GetIO().Framerate);
        passed_time = 0;
        for (; last_frames.size() > number_of_values + 1;) {
            last_frames.pop_back();
        }
    }
    else {
        passed_time += (*deltatime);
    }

}

    // Edit a color (stored as ~4 floats)
  //  ImGui::ColorEdit4("Color", my_color);


void GUI::debug_info() {
    
    ImGui::Begin("Debug Information", &my_tool_active, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("settings/information"))
        {
            if (ImGui::MenuItem("frame rate", "")) { tab = 0; }
            if (ImGui::MenuItem("audio settings", "")) { tab = 1; }
            if (ImGui::MenuItem("model information", "")) { tab = 2; }
            if (ImGui::MenuItem("server information", "")) { tab = 3; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("open window"))
        {
            if (ImGui::MenuItem("spawn_object", "")) { tab = 0; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    static float w = 200.0f;
    static float h = 300.0f;

    switch (tab){
    case 1:
        ImGui::SliderFloat("over all", overal, 0.0f, 1.0f);
        ImGui::SliderFloat("sound effects", effect, 0.0f, 1.0f);
        ImGui::SliderFloat("3D sound effects", effects_3D, 0.0f, 1.0f);
        ImGui::Text("3D sound min range:");
        ImGui::Text("3D sound max range:");

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "current sounds in engine");
      
        ImGui::BeginChild("Scrolling 1");
        for (int n = 0; n < 20; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("Scrolling 2");
        for (int n = 0; n < 10; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        break;
    case 2:
       // ImGui::Begin("Splitter test");

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::BeginChild("child1", ImVec2(w, h), true);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, h));
        if (ImGui::IsItemActive())
            w += ImGui::GetIO().MouseDelta.x;
        ImGui::SameLine();
        ImGui::BeginChild("child2", ImVec2(0, h), true);
        ImGui::EndChild();
        ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
        if (ImGui::IsItemActive())
            h += ImGui::GetIO().MouseDelta.y;
        ImGui::BeginChild("child3", ImVec2(0, 0), true);
        ImGui::EndChild();
        ImGui::PopStyleVar();

       // ImGui::End();
        break;
    case 3:

        break;
    case 0:
    default:
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        my_values = &last_frames[0];
        ImGui::PlotLines("Frame rate", my_values, number_of_values);
        break;
    }
    


    // Display contents in a scrolling region
   /* ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
        ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();*/
    ImGui::End();

}


static float f = 0.0f;

void GUI::draw_demo_window() {
    
    static int counter = 0;
    ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

void GUI::draw_example_from_class() {
   /* ImGui::Text("This is some useful text.");//non-editable text box
   // ImGui::InputText();
    ImGui::Checkbox("Demo Window", &show_demo_window);//takes a bool it modifies
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);//takes a float and bounds
    ImGui::ColorEdit3("clear color", (float*)&clear_color);//float array
    ImGui::Button("Select a directory!");//returns true when clicked
    //I don’t 100 % know how it knows, but we pass the mouse coordinates to ImGui, so a simple X, Y check works
        //ImGui::GetID(””);//file selector (use fileDialog_show_SelectDirectory)
    ImGui::BeginTooltip(); 
    
    ImGui::EndTooltip();//creates a hoverover window
    ImGui::BeginMenu("Menu"); 
    ImGui::EndMenu();
    ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);//arbitrary no.
    ImGui::CollapsingHeader("")
        ImGui::BulletText("");
    ImGui::Separator(); */
}


void GUI::update() {


}

void GUI::init() {
	
	//gui_window = new Shader("GUI.vs", "GUI.fs", "GUI.gs");

 //   float points[] = {
 //      -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
 //       0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
 //       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
 //      -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
 //   };
 //   glGenBuffers(1, &VBO);
 //   glGenVertexArrays(1, &VAO);
 //   glBindVertexArray(VAO);
 //   glBindBuffer(GL_ARRAY_BUFFER, VBO);
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
 //   glEnableVertexAttribArray(0);
 //   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
 //   glEnableVertexAttribArray(1);
 //   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
 //   glBindVertexArray(0);

	if (Time != NULL) {
		deltatime = Time->get_time_change();
	}
	else {
		std::cout << "there was a problem getting time in the GUI" << std::endl;
	}
}

