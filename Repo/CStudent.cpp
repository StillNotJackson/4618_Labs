#include "stdafx.h"
#include "CStudent.h"
#include <regex>

bool CStudent::set_student_number(const std::string& studentNumber)
{
	std::regex pattern("^[Aa]0[01][0-9]{6}$");

	if (!std::regex_match(studentNumber, pattern))
		return false;

	_student_number = studentNumber;
	_student_number[0] = 'A';
	return true;
};
std::string CStudent::get_student_number() const
{
	return _student_number;
}

void CStudent::set_grade_lab(float grade)
{
	_grade_lab = grade;
};
float CStudent::get_grade_lab() const
{
	return _grade_lab;
}

void CStudent::set_grade_quiz(float grade)
{
	_grade_quiz = grade;
}
float CStudent::get_grade_quiz() const
{
	return _grade_quiz;
}

void CStudent::set_grade_midterm(float grade)
{
	_grade_midterm = grade;
}
float CStudent::get_grade_midterm() const
{
	return _grade_midterm;
}

void CStudent::set_grade_endterm(float grade)
{
	_grade_endterm = grade;
}
float CStudent::get_grade_endterm() const
{
	return _grade_endterm;
}

void CStudent::set_grade_final(float grade)
{
	_grade_final = grade;
}
float CStudent::get_grade_final() const
{
	return _grade_final;
}

nlohmann::json CStudent::to_json() const
{
	return nlohmann::json{
		{"student_number", _student_number},
		{"grade_lab", _grade_lab},
		{"grade_quiz", _grade_quiz},
		{"grade_midterm", _grade_midterm},
		{"grade_endterm", _grade_endterm},
		{"grade_final",_grade_final}
	};
}

CStudent CStudent::from_json(const nlohmann::json json_stud)
{
	CStudent student_loaded;

	student_loaded._student_number = json_stud.at("student_number").get<std::string>();
	student_loaded._grade_lab = json_stud.at("grade_lab").get<float>();
	student_loaded._grade_quiz = json_stud.at("grade_quiz").get<float>();
	student_loaded._grade_midterm = json_stud.at("grade_midterm").get<float>();
	student_loaded._grade_endterm = json_stud.at("grade_endterm").get<float>();
	student_loaded._grade_final = json_stud.at("grade_final").get<float>();

	return student_loaded;
} 