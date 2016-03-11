/*
 * graph_parser.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: anhttran
 */

#include "graph_parser.h"

/*
 * get application graph from a text file
 */
AppGraph::AppGraph(const char *filename){
	ifstream app_file;

	cout << "Parsing File: " << filename << "........" <<  endl;

	app_file.open(filename);
	if  (app_file.is_open()){

//		app_graph_parser(app_file);
		// reset all bandwidth to zeros
		for (int i=0; i<MAX_N_TASKS; i++){
			for (int j=0; j<MAX_N_TASKS; j++){
				com_bandwidth[i][j] = 0;
			}
		}

		string line;
		int count = 0;	// number of lines containing numbers

		while (app_file.good()){
//		while (!app_file.fail()){
			getline (app_file, line);	// read a line
	//			cout << line << endl;

			// parse line
			char *cstr = new char[line.size()+1];
			strcpy(cstr, line.c_str());
			char *token = strtok(cstr, " ");	// first token

			if (token != NULL){
				string token_str (token);
				if (token_str[0] == '#'
						|| !token_str.compare("[ntasks]")
						|| !token_str.compare("[graph]")){ // comment lines
	//					cout << "COMMENT LINE" << endl;
				}
				else {
					if (count == 0){	// n_tasks info
						n_tasks = atoi(token);
						cout << "n_tasks = " << n_tasks << endl;
					}
					else {		// bandwidth requirement info
						int i=atoi(token);

						token = strtok(NULL, " "); // next token
						int j=atoi(token);

						token = strtok(NULL, " "); // next token
//						cout << "a[" << i << "," << j << "] = " << token << endl;
						com_bandwidth[i][j] = atof(token);
					}

					count += 1;
				}
			}

			delete[] cstr;
		}

		app_file.close();

		cout << "File Parsing Done----!" << endl;

	}
	else {
		cout << "Unable to open file";
		exit (0);
	}
}

void AppGraph::app_graph_parser(ifstream app_file){
/*
	// reset all bandwidth to zeros
	for (int i=0; i<MAX_N_TASKS; i++){
		for (int j=0; j<MAX_N_TASKS; j++){
			com_bandwidth[i][j] = 0;
		}
	}

	string line;
	int count = 0;	// number of lines containing numbers

	while (app_file.good()){
		getline (app_file, line);	// read a line
//			cout << line << endl;

		// parse line
		char *cstr = new char[line.size()+1];
		strcpy(cstr, line.c_str());
		char *token = strtok(cstr, " ");	// first token

		if (token != NULL){
			string token_str (token);
			if (!token_str.compare("#") || !token_str.compare("[ntasks]") || !token_str.compare("[graph]")){ // comment lines
//					cout << "COMMENT LINE" << endl;
			}
			else {
				if (count == 0){	// n_tasks info
					n_tasks = atoi(token);
					cout << "n_tasks = " << n_tasks << endl;
				}
				else {		// bandwidth requirement info
					int i=atoi(token);

					token = strtok(NULL, " \n"); // next token
					int j=atoi(token);

					token = strtok(NULL, " \n"); // next token
					cout << "a[" << i << "," << j << "] = " << token << endl;
					com_bandwidth[i][j] = atoi(token);
				}

				count += 1;
			}
		}
	}
*/
}
