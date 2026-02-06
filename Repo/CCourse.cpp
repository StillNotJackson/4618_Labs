#include "stdafx.h"
#include "CCourse.h"
#include <regex> //filtering inputs
#include <sstream>
#include <iomanip>
#include <fstream>
#include "json.hpp"

#include <windows.h> //help keep track of file path for json
#include <iostream>

bool json_name_check(const std::string& user_input, std::string& good_output)
{
	//const std::regex valid_name("^[A-Za-z0-9_-+$");
	const std::regex valid_name(R"(^[A-Za-z0-9_-]+$)");


	if (!std::regex_match(user_input, valid_name))
	{
		return false;
	}

	if (user_input.size() >= 5 && user_input.substr(user_input.size() - 5) == ".json")
	{
		good_output = user_input;
	}
	else 
	{
		good_output = user_input + ".json";
	}
	//good_output = user_input + ".json";
	return true;
}

float get_grade(std::string want_grade)
{
	std::regex grade_reg(R"(^(100(\.0+)?)|(\d{1,2}(\.\d+)?)$)");
	std::string grade2check;
	std::cout << want_grade;
	std::cin >> grade2check;
	while (true)
	{
		if (std::regex_match(grade2check, grade_reg))
		{
			float checked_grade = std::stof(grade2check);
			if (checked_grade >= 0.0 && checked_grade <= 100.0)
				return checked_grade;
		}
		std::cout << "\nInvalid grade. Enter a value between 0 and 100.\n";
		std::cin >> grade2check;
	}
}



int CCourse::_get_student_count() //const
{
	return CCourse::students.size();
}

void CCourse::_add_student()
{
	std::cout << "\nAdding Student\n";

	students.emplace_back(); //creates a student

	_edit_student(students.size() - 1);
}

void CCourse::_edit_student(int target_stud)
{
	if (!_has_students())
	{
		return;
	}

	std::string studnum;
	float checked_studgrade;
	CStudent& editStud = students[target_stud];
	bool check_student_number = false;

	do//Student Number loop
	{
		std::cout << "\nStudent Number:";
		std::cin >> studnum;
		check_student_number = editStud.set_student_number(studnum);

		if (check_student_number)
			break;

		std::cout << "\nInvalid student number, must start with A00 or A01 and be exactly 9 characters\n";
	} while (!check_student_number);


	editStud.set_grade_lab(get_grade("\nLab Grade:"));
	editStud.set_grade_quiz(get_grade("\nQuiz Grade:"));
	editStud.set_grade_midterm(get_grade("\nMidterm Grade:"));
	editStud.set_grade_endterm(get_grade("\nFinal Exam Grade:"));
	
	editStud.set_grade_final((editStud.get_grade_lab() * 0.4 + editStud.get_grade_quiz() * 0.1 + editStud.get_grade_midterm() * 0.2 + editStud.get_grade_endterm() * 0.3));
	//std::cout << "\nCongrats, you entered: " << checked_studgrade <<"\n";

}
void CCourse::_print_class_list()
{
	if (students.empty())
	{
		std::cout << "\nNo students in class.\n";
		return;
	};

	std::stringstream strstr_classlist;

	strstr_classlist << std::left
		<< std::setw(5) << "#"
		<< std::setw(15) << "Student"
		<< std::setw(7) << "Lab"
		<< std::setw(7) << "Quiz"
		<< std::setw(15) << "Midterm"
		<< std::setw(15) << "Final Exam"
		<< std::setw(15) << "Final Grade" << "\n";

	for (size_t iter_students = 0; iter_students < students.size(); ++iter_students)
	{
		const CStudent& list_student = students[iter_students];

		strstr_classlist << std::left
			<< std::setw(5) << iter_students+1
			<< std::setw(15) << list_student.get_student_number()
			<< std::fixed << std::setprecision(1)
			<< std::setw(7) << list_student.get_grade_lab()
			<< std::setw(7) << list_student.get_grade_quiz()
			<< std::setw(15) << list_student.get_grade_midterm()
			<< std::setw(15) << list_student.get_grade_endterm()
			<< std::setw(15) << list_student.get_grade_final() << "\n";
	}
	
	std::cout << strstr_classlist.str();
}

bool CCourse::_save_file()
{
	std::string save_user_input;
	std::string save_filename;

	std::cout << "\nName of save file: ";
	std::cin >> save_user_input;

	if (!json_name_check(save_user_input, save_filename))
	{
		std::cout << "\nInvalid filename\n";
		return false;
	}
	nlohmann::json j_file;
	j_file["students"] = nlohmann::json::array();

	for (int iter_stud2json = 0; iter_stud2json < students.size(); iter_stud2json++)
	{
		j_file["students"].push_back(students[iter_stud2json].to_json());
	}
	std::ofstream file(save_filename.c_str());

	if (!file.is_open())
	{
		std::cout << "\nCould not open file: " << save_filename << "\n";
		return false;
	}

	file << j_file.dump(4);
	std::cout << "\nSaved to " << save_filename << "\n";
	return true;
}

bool CCourse::_load_file()
{
	std::string load_user_input;
	std::string load_filename;

	std::cout << "\nName of file to load: ";
	std::cin >> load_user_input;

	if (!json_name_check(load_user_input, load_filename))
	{
		std::cout << "\nInvalid filename.\n";
		return false;
	}

	std::ifstream in(load_filename);

	if (!in.is_open())
	{
		std::cout << "\nSave file does not exist. Nice try Craig.\n";
		return false;
	}
	nlohmann::json json2stud;
	in >> json2stud;

	students.clear();
	for (int iter_json2stud = 0; iter_json2stud < json2stud["students"].size(); ++iter_json2stud)
	{
		students.push_back(CStudent::from_json(json2stud["students"][iter_json2stud]));
	}
	return true;
}

void CCourse::_delete_student()
{
	if (!_has_students())
	{
		return;
	}
	int killed_student;
	std::cout << "\nStudent to be deleted: ";
	std::cin >> killed_student;

	if (killed_student > students.size())
	{
		std::cout << "\nInvalid student index. Returning to menu.";
		return;
	}
	std::cout << "\nStudent " << students[killed_student-1].get_student_number() << " has been deleted.\n";

	students.erase(students.begin() + killed_student-1);

}

bool CCourse::_has_students()
{
	if (students.empty())
	{
		std::cout << "\nNo students present\n";
		return false;
	}
	return true;
}