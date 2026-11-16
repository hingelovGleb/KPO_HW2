#include "domain/domain.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace pos;

template <typename F>
void expectError(F action) {
    bool failed = false;
    try { action(); }
    catch (const std::invalid_argument&) { failed = true; }
    assert(failed);
}

int main() {
    Course course;
    int student = course.addStudent("Ivan");
    int exam = course.addCriterion("Exam", 10, 3);
    int homework = course.addCriterion("Homework", 20, 1);

    expectError([&] { course.setEvaluation(student, exam, 12); });
    expectError([&] { course.setEvaluation(student, 999, 5); });
    expectError([&] { course.setEvaluation(999, exam, 5); });
    expectError([&] { course.addCriterion("Wrong", 0, 1); });

    course.setEvaluation(student, exam, 8);
    course.setEvaluation(student, homework, 10);
    WeightedStrategy weighted;
    EqualStrategy equal;
    assert(std::abs(FinalScoreService::calculate(course, student, weighted) - 72.5) < 1e-8);
    assert(std::abs(FinalScoreService::calculate(course, student, equal) - 65.0) < 1e-8);

    course.setEvaluation(student, exam, 10);
    assert(std::abs(FinalScoreService::calculate(course, student, weighted) - 87.5) < 1e-8);
    course.removeEvaluation(student, homework);
    assert(std::abs(FinalScoreService::calculate(course, student, weighted) - 75.0) < 1e-8);

    course.removeCriterion(exam);
    assert(course.evaluations().empty());
    std::cout << "All tests passed\n";
}
