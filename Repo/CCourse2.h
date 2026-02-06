#pragma once

#include <string>
#include <vector>
#include "CStudent.h"

class CCourse
{
private:
	std::vector<CStudent> students; //List of students

public:
	void _add_student();
	void _edit_student();
	void _print_class_list();

	bool _save_file(std::string filename);
	bool _load_file(std::string filename);

};