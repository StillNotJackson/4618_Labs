#pragma once

#include <string>
#include "json.hpp"

class CStudent
{
private:
	std::string _student_number;
	float _grade_lab = 0.0f;
	float _grade_quiz = 0.0f;
	float _grade_midterm = 0.0f;
	float _grade_endterm = 0.0f;
	float _grade_final = 0.0f;

public:
	bool set_student_number(const std::string& studentNumber);
	std::string get_student_number() const;

	void set_grade_lab(float grade);
	float get_grade_lab() const;

	void set_grade_quiz(float grade);
	float get_grade_quiz() const;

	void set_grade_midterm(float grade);
	float get_grade_midterm() const;

	void set_grade_endterm(float grade);
	float get_grade_endterm() const;

	void set_grade_final(float grade);
	float get_grade_final() const;

	nlohmann::json to_json() const;
	static CStudent from_json(const nlohmann::json json_stud); //Compiler insisted that this be static


};

