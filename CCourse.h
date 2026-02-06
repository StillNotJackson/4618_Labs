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
	void _edit_student(int target_stud);
	//void CCourse::_edit_student();
	void _print_class_list();
	//bool _save_file(const std::string& filename);
	//bool _load_file(const std::string& filename);
	bool _save_file();
	bool _load_file();
	void _delete_student();
	bool CCourse::_has_students();

	int _get_student_count(); //const;

};

