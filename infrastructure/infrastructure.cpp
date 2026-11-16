#include "infrastructure/infrastructure.hpp"
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace pos {

FileCourseRepository::FileCourseRepository(std::string path) : path_(std::move(path)) {}

Course FileCourseRepository::load() const {
    Course course;
    std::ifstream file(path_);
    if (!file) return course;
    char type;
    while (file >> type) {
        if (type == 'S') {
            int id;
            std::string name;
            if (!(file >> id >> std::quoted(name))) break;
            course.addStudent(name, id);
        } else if (type == 'C') {
            int id;
            double maximum, weight;
            std::string name;
            if (!(file >> id >> std::quoted(name) >> maximum >> weight)) break;
            course.addCriterion(name, maximum, weight, id);
        } else if (type == 'E') {
            int studentId, criterionId;
            double score;
            if (!(file >> studentId >> criterionId >> score)) break;
            course.setEvaluation(studentId, criterionId, score);
        } else {
            throw std::runtime_error("Unknown record in data file");
        }
    }
    if (!file.eof()) throw std::runtime_error("Cannot read data file");
    return course;
}

void FileCourseRepository::save(const Course& course) const {
    std::ofstream file(path_);
    if (!file) throw std::runtime_error("Cannot save data file");
    file << std::setprecision(17);
    for (const auto& s : course.students())
        file << "S " << s.id << ' ' << std::quoted(s.name) << '\n';
    for (const auto& c : course.criteria())
        file << "C " << c.id << ' ' << std::quoted(c.name) << ' '
             << c.maxScore << ' ' << c.weight << '\n';
    for (const auto& e : course.evaluations())
        file << "E " << e.studentId << ' ' << e.criterionId << ' '
             << e.score.value() << '\n';
    if (!file) throw std::runtime_error("Cannot write data file");
}

std::string CsvExporter::escaped(const std::string& value) {
    std::string result = "\"";
    for (char c : value) {
        if (c == '"') result += "\"\"";
        else result += c;
    }
    return result + "\"";
}

void CsvExporter::write(const std::string& path, const std::vector<ReportRow>& rows) const {
    if (path.empty()) throw std::invalid_argument("File name cannot be empty");
    std::ofstream file(path);
    if (!file) throw std::runtime_error("Cannot create CSV file");
    file << "id,name,result_percent\n" << std::fixed << std::setprecision(2);
    for (const auto& row : rows)
        file << row.id << ',' << escaped(row.name) << ',' << row.result << '\n';
    if (!file) throw std::runtime_error("Cannot write CSV file");
}

AppContainer::AppContainer(const std::string& dataPath)
    : repository(dataPath), exporter(), app(repository, exporter) {}

} // namespace pos
