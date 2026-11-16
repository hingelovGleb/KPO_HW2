#include "application/application.hpp"

namespace pos {

PosFacade::PosFacade(ICourseRepository& repository, IReportExporter& exporter)
    : repository_(repository), exporter_(exporter), course_(repository.load()) {}

const IScoreStrategy& PosFacade::strategy(bool equal) const {
    return equal ? static_cast<const IScoreStrategy&>(equal_)
                 : static_cast<const IScoreStrategy&>(weighted_);
}

const Course& PosFacade::course() const { return course_; }

int PosFacade::addStudent(const std::string& name) {
    int id = course_.addStudent(name);
    repository_.save(course_);
    return id;
}

int PosFacade::addCriterion(const std::string& name, double maximum, double weight) {
    int id = course_.addCriterion(name, maximum, weight);
    repository_.save(course_);
    return id;
}

void PosFacade::setEvaluation(int studentId, int criterionId, double score) {
    course_.setEvaluation(studentId, criterionId, score);
    repository_.save(course_);
}

void PosFacade::removeEvaluation(int studentId, int criterionId) {
    course_.removeEvaluation(studentId, criterionId);
    repository_.save(course_);
}

void PosFacade::removeStudent(int id) {
    course_.removeStudent(id);
    repository_.save(course_);
}

void PosFacade::removeCriterion(int id) {
    course_.removeCriterion(id);
    repository_.save(course_);
}

std::vector<ReportRow> PosFacade::results(bool equal) const {
    std::vector<ReportRow> rows;
    for (const auto& student : course_.students()) {
        rows.push_back({student.id, student.name,
            FinalScoreService::calculate(course_, student.id, strategy(equal))});
    }
    return rows;
}

void PosFacade::exportResults(const std::string& path, bool equal) const {
    exporter_.write(path, results(equal));
}

} // namespace pos
