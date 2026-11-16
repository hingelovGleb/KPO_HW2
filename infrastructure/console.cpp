#include "infrastructure/console.hpp"
#include "application/application.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace pos;

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Input closed");
    return line;
}

template <typename T>
T readNumber(const std::string& prompt) {
    std::string line = readLine(prompt);
    std::istringstream in(line);
    T value;
    char extra;
    if (!(in >> value) || (in >> extra))
        throw std::invalid_argument("Enter a valid number");
    return value;
}

void showStudents(const Course& course) {
    std::cout << "\nStudents:\n";
    for (const auto& s : course.students())
        std::cout << s.id << " | " << s.name << '\n';
}

void showCriteria(const Course& course) {
    std::cout << "\nCriteria:\n";
    for (const auto& c : course.criteria())
        std::cout << c.id << " | " << c.name << " | max "
                  << c.maxScore << " | weight " << c.weight << '\n';
}

void showEvaluations(const Course& course) {
    std::cout << "\nEvaluations:\n";
    for (const auto& e : course.evaluations()) {
        const Student* student = course.findStudent(e.studentId);
        const Criterion* criterion = course.findCriterion(e.criterionId);
        std::cout << student->name << " | " << criterion->name
                  << " | " << e.score.value() << '/' << criterion->maxScore << '\n';
    }
}

void showResults(const PosFacade& app, bool equal) {
    std::cout << "\nResults (%):\n" << std::fixed << std::setprecision(2);
    for (const auto& row : app.results(equal))
        std::cout << row.id << " | " << row.name << " | " << row.result << "%\n";
}

void menu() {
    std::cout << "\n=== POS ===\n"
              << "1  Add student\n2  List students\n"
              << "3  Add criterion\n4  List criteria\n"
              << "5  Set or change evaluation\n6  List evaluations\n"
              << "7  Remove evaluation\n8  Show results\n"
              << "9  Export CSV\n10 Remove student\n"
              << "11 Remove criterion\n0  Exit\n";
}

void process(PosFacade& app, int command) {
    switch (command) {
        case 1: {
            int id = app.addStudent(readLine("Student name: "));
            std::cout << "Added student ID " << id << '\n';
            break;
        }
        case 2: showStudents(app.course()); break;
        case 3: {
            std::string name = readLine("Criterion name: ");
            double max = readNumber<double>("Maximum score: ");
            double weight = readNumber<double>("Weight: ");
            std::cout << "Added criterion ID " << app.addCriterion(name, max, weight) << '\n';
            break;
        }
        case 4: showCriteria(app.course()); break;
        case 5: {
            int student = readNumber<int>("Student ID: ");
            int criterion = readNumber<int>("Criterion ID: ");
            double score = readNumber<double>("Score: ");
            app.setEvaluation(student, criterion, score);
            std::cout << "Evaluation saved\n";
            break;
        }
        case 6: showEvaluations(app.course()); break;
        case 7: {
            int student = readNumber<int>("Student ID: ");
            int criterion = readNumber<int>("Criterion ID: ");
            app.removeEvaluation(student, criterion);
            std::cout << "Evaluation removed\n";
            break;
        }
        case 8: {
            bool equal = readNumber<int>("Mode (0 = weighted, 1 = equal): ") == 1;
            showResults(app, equal);
            break;
        }
        case 9: {
            bool equal = readNumber<int>("Mode (0 = weighted, 1 = equal): ") == 1;
            std::string path = readLine("CSV file name (example: results.csv): ");
            app.exportResults(path, equal);
            std::cout << "CSV exported\n";
            break;
        }
        case 10: {
            app.removeStudent(readNumber<int>("Student ID: "));
            std::cout << "Student removed\n";
            break;
        }
        case 11: {
            app.removeCriterion(readNumber<int>("Criterion ID: "));
            std::cout << "Criterion removed\n";
            break;
        }
        default: std::cout << "Unknown command\n";
    }
}

void runConsole(pos::PosFacade& app) {
    while (true) {
        menu();
        try {
            int command = readNumber<int>("Choose: ");
            if (command == 0) break;
            process(app, command);
        } catch (const std::exception& e) {
            if (!std::cin) break;
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}
