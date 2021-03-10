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
    debug_info_draw = true;
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
    draw_debug_info = false;
    draw_server_windows = false;
    draw_model_windows = false;
    show_item_stats = false;
    edit_cell = false;
    spawn_item = false;
    show_animation_stats = false;
    edit_routine = false;
    OBJM = NULL;
    AM = NULL;
    item_data = NULL;
    actors = NULL;
    routines = NULL;
    routines_edit_index = -1;
    show_actors_that_follow_routine = -1;
    follow = DEFF_ERROR_ROUTINE;

    clear_on_spawn = true;

    spawn_x = 0;
    spawn_y = 0;
    spawn_z = 0;

    spawn_rot_x = 0;
    spawn_rot_y = 1;
    spawn_rot_z = 0;

    angle = 0;
    is_actor = false;
    type = CUBE_T;
    select_routine = false;
    routine_ = DEFF_ERROR_ROUTINE;
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

void GUI::draw_server_window() {

    static float w = 200.0f;
    static float h = 300.0f;

    ImGui::Begin("server settings", &draw_server_windows);

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

    ImGui::End();
}

void GUI::draw_model_window() {

    static float width = 200.0f;
    static float height = 300.0f;

    ImGui::Begin("server settings", &draw_model_windows);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::BeginChild("option_selection", ImVec2(width, height), true);
    if (ImGui::Button("spawn item")) {
        show_item_stats = false;
        edit_cell = false;
        spawn_item = true;
        show_animation_stats = false;
        edit_routine = false;
    }
    if (ImGui::Button("edit cell")) {
        spawn_item = false;
        show_item_stats = false;
        edit_cell = true;
        show_animation_stats = false;
        edit_routine = false;
    }
    if (ImGui::Button("edit animation routine")) {
        spawn_item = false;
        edit_cell = false;
        show_item_stats = false;
        show_animation_stats = false;
        edit_routine = true;
    }
    if (ImGui::Button("show item stats")) {
        spawn_item = false;
        edit_cell = false;
        show_item_stats = true;
        show_animation_stats = false;
        edit_routine = false;
    }
    if (ImGui::Button("show animation stats")) {
        spawn_item = false;
        edit_cell = false;
        show_item_stats = false;
        show_animation_stats = true;
        edit_routine = false;
    }
   
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, height));
    if (ImGui::IsItemActive())
        width += ImGui::GetIO().MouseDelta.x;
    ImGui::SameLine();
    ImGui::BeginChild("option options", ImVec2(0, height), true);
    if (spawn_item) {
        std::string temp = "spawn item: ";
        temp += OBJM->item_type_to_string(type);
        ImGui::Text(temp.c_str());
        ImGui::Text("item info: ");
        ImGui::InputFloat("x loc", &spawn_x);
        ImGui::InputFloat("y loc", &spawn_y);
        ImGui::InputFloat("z loc", &spawn_z);
        if (ImGui::Button("set loc to cammera possition")) {
            glm::vec3 temp_loc = cam->get_pos();
            spawn_x = temp_loc.x;
            spawn_y = temp_loc.y;
            spawn_z = temp_loc.z;
        }
        ImGui::Text("(each cube is 2.0f appart. To snap to grid make cords divisible by 2");
        ImGui::NewLine();
        ImGui::SliderFloat("x rot", &spawn_rot_x, 0.0f, 1.0f);
        ImGui::SliderFloat("y rot", &spawn_rot_y, 0.0f, 1.0f);
        ImGui::SliderFloat("z rot", &spawn_rot_z, 0.0f, 1.0f);
        ImGui::Text("how much to rotate around each object");
        ImGui::NewLine();
        ImGui::InputFloat("angle", &angle);
        ImGui::NewLine();
        if (!is_actor) {
            if (ImGui::Button("turn into actor:")) {
                is_actor = true;
            }
        }
        else {
            if (ImGui::Button("turn into object:")) {
                is_actor = false;
            }
        }
        if (is_actor) {
            temp = "routine: ";
            temp += AM->routine_designation_tostring(routine_);
            ImGui::Text(temp.c_str());
            if (ImGui::Button("select routine")) {
                select_routine = true;
            }
        }
        if (ImGui::Button("spawn object")) {
            spawn_object();
        }
    }
    else if (edit_cell) {
        ImGui::Text("edit cell:");
    }
    else if (edit_routine) {
        ImGui::Text("edit routine:");
        if (AM == NULL) {
            ImGui::Text("AM is null");
        }
        else {
            if (routines == NULL) {
                ImGui::Text("routines is null");
                routines = AM->get_routines_list();
            }
            else {
                if (routines_edit_index >= 0 && routines_edit_index < routines[0].size()) {
                    int n = routines_edit_index;
                    if (routines[0][n]->defined) {
                        std::string temp;
                        temp = "designation: ";
                        temp += AM->routine_designation_tostring(routines[0][n]->designation);
                        ImGui::Text(temp.c_str());
                      //  temp = "x_min: ";
                        //temp += std::to_string(routines[0][n]->x_min);
                        //ImGui::Text(temp.c_str());
                        //ImGui::SameLine();
                        ImGui::InputInt("x_min", &routines[0][n]->x_min);
                       // temp = "z_min: ";
                       // temp += std::to_string(routines[0][n]->z_min);
                       // ImGui::Text(temp.c_str());
                       // ImGui::SameLine();
                        ImGui::InputInt("z_min", &routines[0][n]->z_min);
                       // temp = "x_max: ";
                       // temp += std::to_string(routines[0][n]->x_max);
                       // ImGui::Text(temp.c_str());
                      //  ImGui::SameLine();
                        ImGui::InputInt("x_max", &routines[0][n]->x_max);
                       // temp = "z_max: ";
                      //  temp += std::to_string(routines[0][n]->z_max);
                       // ImGui::Text(temp.c_str());
                       // ImGui::SameLine();
                        ImGui::InputInt("z_max", &routines[0][n]->z_max);
                        ImGui::Text("behavior: ");
                        temp += std::to_string(routines[0][n]->behavior);
                        ImGui::Text(temp.c_str());
                        temp = "flee_player: ";
                        if (routines[0][n]->flee_player) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        temp = "min_flee_distance: ";
                        temp += std::to_string(routines[0][n]->min_flee_distance);
                        ImGui::Text(temp.c_str());
                        temp = "return_area: ";
                        if (routines[0][n]->return_area) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        temp = "rail_network: ";
                        if (routines[0][n]->rail_network) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        ImGui::NewLine();
                    }
                }
            }
        }
    }
    else if (show_item_stats) {
        ImGui::Text("show item stats:");
        if (OBJM == NULL) {
            ImGui::Text("can not show info, OBJM is NULL");
        }
        else {
            if (item_data == NULL) {
                item_data = OBJM->get_all_item_info();
                ImGui::Text("item_info was null");
            }
            else {
                ImGui::BeginChild("Scrolling 1");
                for (int n = 0; n < item_data[0].size(); n++) {
                    std::string temp = "item_name ";
                    if (item_data[0][n]->item_name != NULL) {
                        temp += *item_data[0][n]->item_name;
                    }
                    ImGui::Text(temp.c_str());

                    temp = "max amount: ";
                    temp += std::to_string(item_data[0][n]->buffer_size);
                    ImGui::Text(temp.c_str());

                    temp = "currently spawned: ";
                    temp += std::to_string(item_data[0][n]->amount);

                    ImGui::Text(temp.c_str());
                    if (item_data[0][n]->draw) {
                        ImGui::Text("drawling: true");
                    }
                    else {
                        ImGui::Text("drawling: false");
                    }
                    ImGui::NewLine();
                }
                ImGui::EndChild();
            }
        }
    }
    else if (show_animation_stats) {
        if (AM == NULL) {
            ImGui::Text("can not show animation info, AM is NULL");
        }
        else {
            ImGui::Text("show animation stats:");
            if (actors == NULL) {
                ImGui::Text("actors pointer was null");
                actors = AM->get_actor_list();
            }
            else {
                std::string temp;
                ImGui::BeginChild("Scrolling 3");
                if (show_actors_that_follow_routine != -1) {
                    if (ImGui::Button("clear search")) {
                        show_actors_that_follow_routine = -1;
                   }
                }
                for (int n = 0; n < actors[0].size(); n++) {
                    if (show_actors_that_follow_routine == -1 || 
                        actors[0][n]->routine == routines[0][show_actors_that_follow_routine]->designation) {

                        temp = "id: ";
                        temp += std::to_string(actors[0][n]->id);
                        ImGui::Text(temp.c_str());
                        temp = "model: ";
                        temp += *(actors[0][n]->object->item_name);
                        ImGui::Text(temp.c_str());
                        temp = "has_sound: ";
                        if (actors[0][n]->has_sound) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        temp = "routine: ";
                        temp += AM->routine_designation_tostring(actors[0][n]->routine);
                        ImGui::Text(temp.c_str());
                        temp = "move_speed: ";
                        temp += std::to_string(actors[0][n]->move_speed);
                        ImGui::Text(temp.c_str());
                        temp = "being_held: ";
                        if (actors[0][n]->being_held) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        temp = "holding_somethig: ";
                        if (actors[0][n]->holding_somethig) {
                            temp += "true";
                        }
                        else {
                            temp += "false";
                        }
                        ImGui::Text(temp.c_str());
                        temp = "cooldown: ";
                        temp += std::to_string(actors[0][n]->cooldown);
                        ImGui::Text(temp.c_str());
                        temp = "cooldown_max: ";
                        temp += std::to_string(actors[0][n]->cooldown_max);
                        ImGui::Text(temp.c_str());
                        ImGui::NewLine();
                    }
                }
                ImGui::EndChild();
            }

        }
    }
    ImGui::EndChild();
    ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
    if (ImGui::IsItemActive())
        height += ImGui::GetIO().MouseDelta.y;
    ImGui::BeginChild("child3", ImVec2(0, 0), true);
    if ((show_animation_stats || edit_routine)&&AM != NULL) {
        if (show_animation_stats) {
            ImGui::Text("showing routine stats:");
            if (routines == NULL) {
                ImGui::Text("routines was NULL");
                routines = AM->get_routines_list();
            }
            else {
                ImGui::BeginChild("Scrolling routines");
                std::string temp;
                for (int n = 0; n < routines[0].size(); n++) {
                    //ImGui::Text(Some text");
                    if (routines[0][n]->defined) {
                        // temp = "designation: ";
                        temp = AM->routine_designation_tostring(routines[0][n]->designation);
                        //ImGui::Text(temp.c_str());
                        if (ImGui::CollapsingHeader(temp.c_str())) {

                            temp = "num_of_actors_using_this: ";
                            temp += std::to_string(routines[0][n]->num_of_actors_using_this);
                            ImGui::Text(temp.c_str());
                            temp = "x_min: ";
                            temp += std::to_string(routines[0][n]->x_min);
                            ImGui::Text(temp.c_str());
                            temp = "z_min: ";
                            temp += std::to_string(routines[0][n]->z_min);
                            ImGui::Text(temp.c_str());
                            temp = "x_max: ";
                            temp += std::to_string(routines[0][n]->x_max);
                            ImGui::Text(temp.c_str());
                            temp = "z_max: ";
                            temp += std::to_string(routines[0][n]->z_max);
                            ImGui::Text(temp.c_str());
                            ImGui::Text("behavior: ");
                            temp += std::to_string(routines[0][n]->behavior);
                            ImGui::Text(temp.c_str());
                            temp = "flee_player: ";
                            if (routines[0][n]->flee_player) {
                                temp += "true";
                            }
                            else {
                                temp += "false";
                            }
                            ImGui::Text(temp.c_str());
                            temp = "min_flee_distance: ";
                            temp += std::to_string(routines[0][n]->min_flee_distance);
                            ImGui::Text(temp.c_str());
                            temp = "return_area: ";
                            if (routines[0][n]->return_area) {
                                temp += "true";
                            }
                            else {
                                temp += "false";
                            }
                            ImGui::Text(temp.c_str());
                            temp = "rail_network: ";
                            if (routines[0][n]->rail_network) {
                                temp += "true";
                            }
                            else {
                                temp += "false";
                            }
                            ImGui::Text(temp.c_str());
                            if (ImGui::Button("show only actors that follow this routine")) {
                               // if (show_actors_that_follow_routine == n) {
                                 //   show_actors_that_follow_routine = -1;
                               // }
                              //  else {
                                if (show_actors_that_follow_routine == -1) {
                                    show_actors_that_follow_routine = n;
                                }
                                    //follow = routines[0][n]->designation;
                               // }
                            }
                            ImGui::NewLine();
                      
                        }
                    }
             
                }
                ImGui::EndChild();
            }
        }
        else if (edit_routine) {
            ImGui::Text("pick a routine to edit");
            if (AM == NULL) {
                ImGui::Text("AM is null");
            }
            else {
                if (routines == NULL) {
                    ImGui::Text("routines is null");
                    routines = AM->get_routines_list();
                }
                else {
                    for (int n = 0; n < routines[0].size(); n++) {
                        //ImGui::Text(Some text");
                        if (routines[0][n]->defined) {
                            if (ImGui::Button((AM->routine_designation_tostring(routines[0][n]->designation)).c_str())) {
                                routines_edit_index = n;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (spawn_item) {
        if (item_data == NULL) {
            item_data = OBJM->get_all_item_info();
        }
        else {
            std::string temp = "unkown";
            if (is_actor && select_routine) {
                if (AM != NULL) {
                    if (routines == NULL) {
                        routines = AM->get_routines_list();
                    }
                    else {
                        ImGui::Text("select an routine to attach to object");
                        ImGui::BeginChild("routines selection list");
                        for (int n = 0; n < routines[0].size(); n++) {
                            //ImGui::Text(Some text");
                            if (routines[0][n]->defined) {
                                // temp = "designation: ";
                                temp = AM->routine_designation_tostring(routines[0][n]->designation);
                                //ImGui::Text(temp.c_str());
                                if (ImGui::CollapsingHeader(temp.c_str())) {

                                    temp = "num_of_actors_using_this: ";
                                    temp += std::to_string(routines[0][n]->num_of_actors_using_this);
                                    ImGui::Text(temp.c_str());
                                    temp = "x_min: ";
                                    temp += std::to_string(routines[0][n]->x_min);
                                    ImGui::Text(temp.c_str());
                                    temp = "z_min: ";
                                    temp += std::to_string(routines[0][n]->z_min);
                                    ImGui::Text(temp.c_str());
                                    temp = "x_max: ";
                                    temp += std::to_string(routines[0][n]->x_max);
                                    ImGui::Text(temp.c_str());
                                    temp = "z_max: ";
                                    temp += std::to_string(routines[0][n]->z_max);
                                    ImGui::Text(temp.c_str());
                                    ImGui::Text("behavior: ");
                                    temp += std::to_string(routines[0][n]->behavior);
                                    ImGui::Text(temp.c_str());
                                    temp = "flee_player: ";
                                    if (routines[0][n]->flee_player) {
                                        temp += "true";
                                    }
                                    else {
                                        temp += "false";
                                    }
                                    ImGui::Text(temp.c_str());
                                    temp = "min_flee_distance: ";
                                    temp += std::to_string(routines[0][n]->min_flee_distance);
                                    ImGui::Text(temp.c_str());
                                    temp = "return_area: ";
                                    if (routines[0][n]->return_area) {
                                        temp += "true";
                                    }
                                    else {
                                        temp += "false";
                                    }
                                    ImGui::Text(temp.c_str());
                                    temp = "rail_network: ";
                                    if (routines[0][n]->rail_network) {
                                        temp += "true";
                                    }
                                    else {
                                        temp += "false";
                                    }
                                    ImGui::Text(temp.c_str());
                                    if (ImGui::Button("select this routine")) {
                                        routine_ = routines[0][n]->designation;
                                        select_routine = false;
                                    }
                                    ImGui::NewLine();

                                }
                            }

                        }
                        ImGui::EndChild();
                    }
                }
                else {
                    ImGui::Text("AM was NULL");

                }
               
            }else{
                ImGui::Text("select an item to spawn");
                ImGui::BeginChild("item_selection_list");
                for (int n = 0; n < item_data[0].size(); n++) {
                    std::string temp = "item_name: ";
                    if (item_data[0][n]->item_name != NULL) {
                        temp += *item_data[0][n]->item_name;
                    }
                    // ImGui::Text(temp.c_str());

                    if (ImGui::CollapsingHeader(temp.c_str())) {
                        temp = "max amount: ";
                        temp += std::to_string(item_data[0][n]->buffer_size);
                        ImGui::Text(temp.c_str());

                        temp = "currently spawned: ";
                        temp += std::to_string(item_data[0][n]->amount);

                        ImGui::Text(temp.c_str());
                        if (item_data[0][n]->draw) {
                            ImGui::Text("drawling: true");
                        }
                        else {
                            ImGui::Text("drawling: false");
                        }

                        if (ImGui::Button("select this item to spawn")) {
                            type = item_data[0][n]->type;
                        }
                    }
                }
                ImGui::EndChild();
            }
        }


    }
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::End();
}

void GUI::debug_info() {
    ImGui::Begin("Debug Information", &debug_info_draw, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("settings/information"))
        {
            if (ImGui::MenuItem("frame rate", "")) { tab = 0; }
            if (ImGui::MenuItem("audio settings", "")) { tab = 1; }
            if (ImGui::MenuItem("model information", "")) { tab = 2; }
            if (ImGui::MenuItem("server information", "")) { tab = 3; }
            if (ImGui::MenuItem("close", "")) { debug_info_draw = false; }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }


    switch (tab) {
    case 1:
        ImGui::Text("over all");
        ImGui::SliderFloat("", overal, 0.0f, 1.0f);
        ImGui::Text("sound effects:");
        ImGui::SliderFloat("", effect, 0.0f, 1.0f);
        ImGui::Text("3D sound effects");
        ImGui::SliderFloat("", effects_3D, 0.0f, 1.0f);
        ImGui::Text("3D sound min range:");
        ImGui::Text("3D sound max range:");

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "current sounds in engine");

        ImGui::BeginChild("Scrolling 1");
        for (int n = 0; n < 20; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        break;
    case 2:
        if (draw_model_windows) {
            draw_model_windows = false;
        }
        else {
            draw_model_windows = true;
        }
        tab = 0;
        break;
    case 3:
        if (draw_server_windows) {
            draw_server_windows = false;
        }
        else {
            draw_server_windows = true;
        }
        tab = 0;
        break;

    case 0:
    default:
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        my_values = &last_frames[0];
        ImGui::PlotLines("Frame rate", my_values, number_of_values);
        ImGui::InputFloat("cammera speed", &(cam->MovementSpeed));
        break;
    }
    
    //if (ImGui::Button("send keyboard inputs to gui")) {
    //    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //}

    if (draw_server_windows) {
        draw_server_window();
    }
    if (draw_model_windows) {
        draw_model_window();
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

void GUI::spawn_object() {

    glm::vec3 loc(spawn_x, spawn_y, spawn_z);
    glm::vec3 rot(spawn_rot_x, spawn_rot_y, spawn_rot_z);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, loc);
    trans = glm::rotate(trans, glm::radians(angle), rot);

    item_info* temp_data = OBJM->spawn_item(type, -1, -1, -1, trans);

    temp_data->x_rot = rot.x;
    temp_data->y_rot = rot.y;
    temp_data->z_rot = rot.z;
    temp_data->angle = angle;
    temp_data->x = loc.x;
    temp_data->y = loc.y;
    temp_data->z = loc.z;

    if (is_actor) {
        AM->turn_object_into_actor(temp_data, routine_);
    }

    //clearn the options
    if (clear_on_spawn) {
        spawn_x = 0;
        spawn_y = 0;
        spawn_z = 0;

        spawn_rot_x = 0;
        spawn_rot_y = 1;
        spawn_rot_z = 0;

        angle = 0;
        is_actor = false;
        type = CUBE_T;
        select_routine = false;
        routine_ = DEFF_ERROR_ROUTINE;
    }
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

