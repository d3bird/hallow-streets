#include "city.h"


city::city() {
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
	Time = NULL;

	city_info = NULL;
	layout = NULL;

	cube_matrices = NULL;//contains all the cubes mats
	cube_shader = NULL;

	wall = NULL;
	wall_d = NULL;
	wall_c = NULL;
	
	//path finding data
	terrian_map = NULL;
	x_width = 9;// ROW;//rows
	z_width = 10;//COL;//collums


	//debug information
	draw_wall_c = true;
	draw_wall = true;
	draw_path_cubes = true;
	draw_light_posts = true;
	draw_sidewalk = true;
}

city::~city(){

}

void city::draw() {

}

void city::update() {

}

void city::init(object_manger* OBJM) {
	std::cout << "creating city" << std::endl;
	std::cout << "creating city info" << std::endl;

	if (city_info == NULL) {
		city_info = new city_gen();
	}

	city_info->set_time(Time);
	city_info->set_projection(projection);
	city_info->init();

	layout = city_info->get_layout();
	int** layout_expanded = city_info->get_expanded_layout();

	//x_width = 9;// ROW;//rows
	//z_width = 10;//COL;//collums
	int key = city_info->get_expandion_key();
	x_width = city_info->get_height() * key;
	z_width = city_info->get_width() * key;

	//generate the mats from the layout
	std::vector<glm::mat4> generated_mats_debug_cubes;
	std::vector<glm::mat4> generated_mats_wall;
	std::vector<glm::mat4> generated_mats_wall_c;
	std::vector<glm::mat4> generated_mats_sidewalk;

	for (int i = 0; i < x_width; i++) {
		for (int h = 0; h < z_width; h++) {

			//if (layout[i][h] == small_road || layout[i][h] == road_top) {
			if (layout_expanded[i][h] == 1) {
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 0, i * 2));
				generated_mats_debug_cubes.push_back(temp);
			}
			else if (layout_expanded[i][h] == 3) {//for a wall
				//std::cout << "wall start found" << std::endl;
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 2, i * 2));
				if (i - 1 >= 0 && layout_expanded[i - 1][h] != 0) {//if the topspot is a rode
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
				}
				else if (i + 1 < x_width && layout_expanded[i + 1][h] != 0) {
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
				}
				else {
					temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));

				}
				generated_mats_wall.push_back(temp);
			}
			else if (layout_expanded[i][h] == 4) {//for a corner
				//std::cout << "wall_c start found" << std::endl;
				glm::mat4 temp = glm::mat4(1.0f);

				temp = glm::translate(temp, glm::vec3(h * 2, 2, i * 2));
				if (layout_expanded[i - 1][h] != 0 && layout_expanded[i][h - 1] != 0) {
					temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));//bottom left
				}
				else if (layout_expanded[i + 1][h] != 0 && layout_expanded[i][h - 1] != 0) {
					temp = glm::rotate(temp, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));//top left
				}
				else if (layout_expanded[i - 1][h] != 0 && layout_expanded[i][h + 1] != 0) {
					temp = glm::rotate(temp, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));//top right
				}
				else {//no need to rotate
					//temp = glm::rotate(temp, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));//bottom right

				}

				generated_mats_wall_c.push_back(temp);
			}
			else if (layout_expanded[i][h] >= 6 && layout_expanded[i][h] <= 10) {//placing sidewalks

				glm::mat4 trans = glm::mat4(1.0f);
				
				
				std::cout << layout_expanded[i][h]<< " ";;
				
				switch (layout_expanded[i][h]){
				case 6://sidewalk top
					trans = glm::translate(trans, glm::vec3((h * 2) + 14, 2, i * 2));
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 7://sidewalk bottom
					trans = glm::translate(trans, glm::vec3((h * 2), 2, (i * 2)+14 ));
					//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 8://sidewalk left
					trans = glm::translate(trans, glm::vec3((h * 2), 2, i * 2));
					trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				case 9://sidewalk right
					trans = glm::translate(trans, glm::vec3((h * 2) + 14, 2, (i * 2)+14));
					trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					break;
				default:
					break;
				}
				
				generated_mats_sidewalk.push_back(trans);
			}
		}
	}

	std::cout<< std::endl;

	std::cout << "done "<< generated_mats_sidewalk.size() << std::endl;
	std::cout << "creating pathfinding data" << std::endl;

	cube_amount = x_width * z_width;

	terrian_map = new map_tile * [x_width];
	for (int i = 0; i < x_width; i++) {
		terrian_map[i] = new map_tile[z_width];
	}

	float cube_offset = 2.0f;
	float x = 0;
	float y = 0;
	float z = 0;

	bool first = true;

	int xloc = 0;
	int zloc = 0;

	//generate the pathfinding data structure
	for (unsigned int i = 0; i < cube_amount; i++) {
		map_tile temp;
		temp.x = x;
		temp.y = y;
		temp.z = z;
		temp.g_cost = 1;
		temp.blocked = false;
		temp.buffer_loc = i;
		temp.type = 1;
		terrian_map[xloc][zloc] = temp;

		x += cube_offset;
		xloc++;
		if (x == (cube_offset * x_width)) {
			z += cube_offset;
			zloc++;
			x = 0;
			xloc = 0;
		}

	}

	//generate the complete path finding refference map
	for (int x = 0; x < x_width; x++) {
		for (int z = 0; z < z_width; z++) {
			switch (layout_expanded[x][z])
			{
			case 0:
				terrian_map[x][z].blocked = true;
				terrian_map[x][z].type = 0;
				break;
			case 1:
				terrian_map[x][z].blocked = false;
				break;
			default:
				break;
			}

		}
	}

	city_info->print_layout();
	//city_info->print_expanded_layout();
	//print_map();

	// Source is the left-most bottom-most corner 
	Pair src = make_pair(8, 0);

	// Destination is the left-most top-most corner 
	Pair dest = make_pair(0, 0);

	aStarSearch(src, dest);
	std::cout << "done" << std::endl;
	std::cout << "creating the model buffer" << std::endl;

	if (cube_shader == NULL) {
		cube_shader = new Shader("asteroids.vs", "asteroids.fs");
	}
	else {
		std::cout << "using premade shader for the cubes" << std::endl;
	}

	std::cout << "spawning objects" << std::endl;
	std::cout << "spawning wall" << std::endl;

	
	for (int i = 0; i < generated_mats_wall.size(); i++) {
		OBJM->spawn_item(WALL_T, -1, -1, generated_mats_wall[i]);
	}

	std::cout << "spawning wall_c" << std::endl;
	
	for (int i = 0; i < generated_mats_wall_c.size(); i++) {
		OBJM->spawn_item(WALL_C_T, -1, -1, generated_mats_wall_c[i]);
	}
	
	std::cout << "spawning cubes" << std::endl;

	for (int i = 0; i < generated_mats_debug_cubes.size(); i++) {
		OBJM->spawn_item(CUBE_T, -1, -1, generated_mats_debug_cubes[i]);
	}

	std::cout << "spawning sidewalks" << std::endl;

	for (int i = 0; i < generated_mats_sidewalk.size(); i++) {
		OBJM->spawn_item(SIDEWALK_T, -1, -1, generated_mats_sidewalk[i]);
	}

	std::cout << "spawning lightposts" << std::endl;


	unsigned int light_post_amount;
	unsigned int light_post_buffer;
	glm::mat4* light_post_mats;

	light_post_amount = 5;
	light_post_mats = new glm::mat4[light_post_amount];

	for (int i = 0; i < light_post_amount; i++) {
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3((i * 2) * 3, 4, 0));
		trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));//bottom left
		light_post_mats[i] = trans;
		OBJM->spawn_item(LIGHT_POST_T, -1, -1, light_post_mats[i]);

	}

	
	std::cout << "done" << std::endl;
	std::cout << "done creating city" << std::endl;

	check();
	//while (true);
}



void city::check() {
	std::cout << "checking to maksure that all vars were inited" << std::endl;

	bool error = false;
	if (view == glm::mat4(1.0f)) {
		std::cout << "view was never inited" << std::endl;
	}
	if (projection == glm::mat4(1.0f)) {
		std::cout << "projection was never inited" << std::endl;
	}
	if (Time == NULL) {
		std::cout << "Time was never inited" << std::endl;
	}
	if (city_info == NULL) {
		std::cout << "city_info was never inited" << std::endl;
	}
	if (layout == NULL) {
		std::cout << "layout was never inited" << std::endl;
	}
	if (cube_amount == 0) {
		std::cout << "cube_amount was never inited" << std::endl;
	}
	if (cube_matrices == NULL) {
		std::cout << "cube_matrices was never inited" << std::endl;
	}
	if (cube_shader == NULL) {
		std::cout << "cube_shader was never inited" << std::endl;
	}

	//path finding data
	if (terrian_map == NULL) {
		std::cout << "terrian_map was never inited" << std::endl;
	}


	//x_width = 9;// ROW;//rows
	//z_width = 10;//COL;//collums

	if (error) {
		std::cout << "there was a problem initalising vars in the city object" << std::endl;
		while (true);
	}
	else {
		std::cout << "passed all tests" << std::endl;
	}

}

//path finding functions

bool city::isValid(int row, int col)
{
	// Returns true if row number and column number 
	// is in range 
	return (row >= 0) && (row < x_width) &&
		(col >= 0) && (col < z_width);
}

// A Utility Function to check whether the given cell is 
// blocked or not 

bool city::isUnBlocked(int row, int col)
{
	return !terrian_map[row][col].blocked;
}

// A Utility Function to check whether destination cell has 
// been reached or not 
bool city::isDestination(int row, int col, Pair dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	else
		return (false);
}

// A Utility Function to calculate the 'h' heuristics. 
double city::calculateHValue(int row, int col, Pair dest)
{
	// Return using the distance formula 
	return ((double)sqrt((row - dest.first) * (row - dest.first)
		+ (col - dest.second) * (col - dest.second)));
}

// A Utility Function to trace the path from the source 
// to destination 
void city::tracePath(cell** cellDetails, Pair dest)
{
	printf("\nThe Path is ");
	int row = dest.first;
	int col = dest.second;

	stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
		&& cellDetails[row][col].parent_j == col))
	{
		Path.push(make_pair(row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	Path.push(make_pair(row, col));
	while (!Path.empty())
	{
		pair<int, int> p = Path.top();
		Path.pop();
		printf("-> (%d,%d) ", p.first, p.second);
	}

	return;
}

// A Function to find the shortest path between 
// a given source cell to a destination cell according 
// to A* Search Algorithm 
void city::aStarSearch(Pair src, Pair dest){
	// If the source is out of range 
	if (isValid(src.first, src.second) == false)
	{
		printf("Source is invalid\n");
		return;
	}

	// If the destination is out of range 
	if (isValid(dest.first, dest.second) == false)
	{
		printf("Destination is invalid\n");
		return;
	}

	// Either the source or the destination is blocked 
	if (isUnBlocked(src.first, src.second) == false ||
		isUnBlocked(dest.first, dest.second) == false)
	{
		printf("Source or the destination is blocked\n");
		return;
	}

	// If the destination cell is the same as source cell 
	if (isDestination(src.first, src.second, dest) == true)
	{
		printf("We are already at the destination\n");
		return;
	}



	// Create a closed list and initialise it to false which means 
	// that no cell has been included yet 
	// This closed list is implemented as a boolean 2D array 

	bool** closedList = new bool* [x_width];
	for (int i = 0; i < x_width; i++) {
		closedList[i] = new bool[z_width];
	}

	for (int x = 0; x < x_width; x++) {
		for (int z = 0; z < z_width; z++) {
			closedList[x][z] = false;
		}
	}

	// Declare a 2D array of structure to hold the details 
	//of that cell 
	cell** cellDetails = new cell * [x_width];
	for (int i = 0; i < x_width; i++) {
		cellDetails[i] = new cell[z_width];
	}


	int i, j;

	for (i = 0; i < x_width; i++)
	{
		for (j = 0; j < z_width; j++)
		{
			cellDetails[i][j].f = FLT_MAX;
			cellDetails[i][j].g = FLT_MAX;
			cellDetails[i][j].h = FLT_MAX;
			cellDetails[i][j].parent_i = -1;
			cellDetails[i][j].parent_j = -1;
		}
	}

	// Initialising the parameters of the starting node 
	i = src.first, j = src.second;
	cellDetails[i][j].f = 0.0;
	cellDetails[i][j].g = 0.0;
	cellDetails[i][j].h = 0.0;
	cellDetails[i][j].parent_i = i;
	cellDetails[i][j].parent_j = j;

	/*
	Create an open list having information as-
	<f, <i, j>>
	where f = g + h,
	and i, j are the row and column index of that cell
	Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	This open list is implenented as a set of pair of pair.*/
	set<pPair> openList;

	// Put the starting cell on the open list and set its 
	// 'f' as 0 
	openList.insert(make_pair(0.0, make_pair(i, j)));

	// We set this boolean value as false as initially 
	// the destination is not reached. 
	bool foundDest = false;

	while (!openList.empty())
	{
		pPair p = *openList.begin();

		// Remove this vertex from the open list 
		openList.erase(openList.begin());

		// Add this vertex to the closed list 
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		/*
			Generating all the 8 successor of this cell

				N.W N N.E
				\ | /
				\ | /
				W----Cell----E
					/ | \
				/ | \
				S.W S S.E

			Cell-->Popped Cell (i, j)
			N --> North	 (i-1, j)
			S --> South	 (i+1, j)
			E --> East	 (i, j+1)
			W --> West		 (i, j-1)
			N.E--> North-East (i-1, j+1)
			N.W--> North-West (i-1, j-1)
			S.E--> South-East (i+1, j+1)
			S.W--> South-West (i+1, j-1)*/

			// To store the 'g', 'h' and 'f' of the 8 successors 
		double gNew, hNew, fNew;

		//----------- 1st Successor (North) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j].parent_i = i;
				cellDetails[i - 1][j].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i - 1][j] == false &&
				isUnBlocked(i - 1, j) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i - 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j].f == FLT_MAX ||
					cellDetails[i - 1][j].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i - 1, j)));

					// Update the details of this cell 
					cellDetails[i - 1][j].f = fNew;
					cellDetails[i - 1][j].g = gNew;
					cellDetails[i - 1][j].h = hNew;
					cellDetails[i - 1][j].parent_i = i;
					cellDetails[i - 1][j].parent_j = j;
				}
			}
		}

		//----------- 2nd Successor (South) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j].parent_i = i;
				cellDetails[i + 1][j].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i + 1][j] == false &&
				isUnBlocked(i + 1, j) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i + 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j].f == FLT_MAX ||
					cellDetails[i + 1][j].f > fNew)
				{
					openList.insert(make_pair(fNew, make_pair(i + 1, j)));
					// Update the details of this cell 
					cellDetails[i + 1][j].f = fNew;
					cellDetails[i + 1][j].g = gNew;
					cellDetails[i + 1][j].h = hNew;
					cellDetails[i + 1][j].parent_i = i;
					cellDetails[i + 1][j].parent_j = j;
				}
			}
		}

		//----------- 3rd Successor (East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j + 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j + 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j + 1].parent_i = i;
				cellDetails[i][j + 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i][j + 1] == false &&
				isUnBlocked(i, j + 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j + 1].f == FLT_MAX ||
					cellDetails[i][j + 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i, j + 1)));

					// Update the details of this cell 
					cellDetails[i][j + 1].f = fNew;
					cellDetails[i][j + 1].g = gNew;
					cellDetails[i][j + 1].h = hNew;
					cellDetails[i][j + 1].parent_i = i;
					cellDetails[i][j + 1].parent_j = j;
				}
			}
		}

		//----------- 4th Successor (West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j - 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j - 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j - 1].parent_i = i;
				cellDetails[i][j - 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i][j - 1] == false &&
				isUnBlocked(i, j - 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j - 1].f == FLT_MAX ||
					cellDetails[i][j - 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i, j - 1)));

					// Update the details of this cell 
					cellDetails[i][j - 1].f = fNew;
					cellDetails[i][j - 1].g = gNew;
					cellDetails[i][j - 1].h = hNew;
					cellDetails[i][j - 1].parent_i = i;
					cellDetails[i][j - 1].parent_j = j;
				}
			}
		}

		//----------- 5th Successor (North-East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j + 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j + 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j + 1].parent_i = i;
				cellDetails[i - 1][j + 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i - 1][j + 1] == false &&
				isUnBlocked(i - 1, j + 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j + 1].f == FLT_MAX ||
					cellDetails[i - 1][j + 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i - 1, j + 1)));

					// Update the details of this cell 
					cellDetails[i - 1][j + 1].f = fNew;
					cellDetails[i - 1][j + 1].g = gNew;
					cellDetails[i - 1][j + 1].h = hNew;
					cellDetails[i - 1][j + 1].parent_i = i;
					cellDetails[i - 1][j + 1].parent_j = j;
				}
			}
		}

		//----------- 6th Successor (North-West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j - 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j - 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j - 1].parent_i = i;
				cellDetails[i - 1][j - 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i - 1][j - 1] == false &&
				isUnBlocked(i - 1, j - 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j - 1].f == FLT_MAX ||
					cellDetails[i - 1][j - 1].f > fNew)
				{
					openList.insert(make_pair(fNew, make_pair(i - 1, j - 1)));
					// Update the details of this cell 
					cellDetails[i - 1][j - 1].f = fNew;
					cellDetails[i - 1][j - 1].g = gNew;
					cellDetails[i - 1][j - 1].h = hNew;
					cellDetails[i - 1][j - 1].parent_i = i;
					cellDetails[i - 1][j - 1].parent_j = j;
				}
			}
		}

		//----------- 7th Successor (South-East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j + 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j + 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j + 1].parent_i = i;
				cellDetails[i + 1][j + 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i + 1][j + 1] == false &&
				isUnBlocked(i + 1, j + 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j + 1].f == FLT_MAX ||
					cellDetails[i + 1][j + 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i + 1, j + 1)));

					// Update the details of this cell 
					cellDetails[i + 1][j + 1].f = fNew;
					cellDetails[i + 1][j + 1].g = gNew;
					cellDetails[i + 1][j + 1].h = hNew;
					cellDetails[i + 1][j + 1].parent_i = i;
					cellDetails[i + 1][j + 1].parent_j = j;
				}
			}
		}

		//----------- 8th Successor (South-West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j - 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j - 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j - 1].parent_i = i;
				cellDetails[i + 1][j - 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i + 1][j - 1] == false &&
				isUnBlocked(i + 1, j - 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//			 OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j - 1].f == FLT_MAX ||
					cellDetails[i + 1][j - 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i + 1, j - 1)));

					// Update the details of this cell 
					cellDetails[i + 1][j - 1].f = fNew;
					cellDetails[i + 1][j - 1].g = gNew;
					cellDetails[i + 1][j - 1].h = hNew;
					cellDetails[i + 1][j - 1].parent_i = i;
					cellDetails[i + 1][j - 1].parent_j = j;
				}
			}
		}
	}

	// When the destination cell is not found and the open 
	// list is empty, then we conclude that we failed to 
	// reach the destiantion cell. This may happen when the 
	// there is no way to destination cell (due to blockages) 
	if (foundDest == false)
		printf("Failed to find the Destination Cell\n");

	return;
}


//prints the map in respect to the how the path finder sees it
void city::print_map() {
	if (terrian_map != NULL) {
		std::cout << "printing out internal map representation" << std::endl;
		for (int x = 0; x < x_width; x++) {
			for (int z = 0; z < z_width; z++) {
				if (!terrian_map[x][z].blocked) {
					std::cout << "1 ";
				}
				else {
					std::cout << "0 ";
				}
			}
			std::cout << std::endl;
		}
	}
	else {
		std::cout << "the map data structure was never created" << std::endl;
	}
}
