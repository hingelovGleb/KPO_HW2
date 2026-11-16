#include "domain/domain.hpp"
#include <algorithm>
#include <cmath>

namespace pos {

void require(bool ok, const std::string& message) {
    if (!ok) throw std::invalid_argument(message);
}

Score::Score(double value, double maximum) : value_(value) {
    require(std::isfinite(value) && std::isfinite(maximum) && maximum > 0 &&
            value >= 0 && value <= maximum, "Score must be between 0 and criterion maximum");
}

double Score::value() const { return value_; }

const std::vector<Student>& Course::students() const { return students_; }
const std::vector<Criterion>& Course::criteria() const { return criteria_; }
const std::vector<Evaluation>& Course::evaluations() const { return evaluations_; }

const Student* Course::findStudent(int id) const {
    for (const auto& student : students_)
        if (student.id == id) return &student;
    return nullptr;
}

const Criterion* Course::findCriterion(int id) const {
    for (const auto& criterion : criteria_)
        if (criterion.id == id) return &criterion;
    return nullptr;
}

int Course::addStudent(const std::string& name, int savedId) {
    require(!name.empty(), "Student name cannot be empty");
    int id = savedId ? savedId : nextStudentId_;
    require(id > 0 && !findStudent(id), "Invalid or duplicate student ID");
    students_.push_back({id, name});
    nextStudentId_ = std::max(nextStudentId_, id + 1);
    return id;
}

int Course::addCriterion(const std::string& name, double maximum, double weight, int savedId) {
    require(!name.empty(), "Criterion name cannot be empty");
    require(std::isfinite(maximum) && maximum > 0, "Maximum must be positive");
    require(std::isfinite(weight) && weight > 0, "Weight must be positive");
    int id = savedId ? savedId : nextCriterionId_;
    require(id > 0 && !findCriterion(id), "Invalid or duplicate criterion ID");
    criteria_.push_back({id, name, maximum, weight});
    nextCriterionId_ = std::max(nextCriterionId_, id + 1);
    return id;
}

void Course::setEvaluation(int studentId, int criterionId, double value) {
    Evaluation evaluation = EvaluationFactory::create(*this, studentId, criterionId, value);
    for (auto& old : evaluations_) {
        if (old.studentId == studentId && old.criterionId == criterionId) {
            old = evaluation;
            return;
        }
    }
    evaluations_.push_back(evaluation);
}

void Course::removeEvaluation(int studentId, int criterionId) {
    auto it = std::remove_if(evaluations_.begin(), evaluations_.end(),
        [=](const Evaluation& e) {
            return e.studentId == studentId && e.criterionId == criterionId;
        });
    require(it != evaluations_.end(), "Evaluation not found");
    evaluations_.erase(it, evaluations_.end());
}

void Course::removeStudent(int id) {
    require(findStudent(id), "Student not found");
    students_.erase(std::remove_if(students_.begin(), students_.end(),
        [=](const Student& s) { return s.id == id; }), students_.end());
    evaluations_.erase(std::remove_if(evaluations_.begin(), evaluations_.end(),
        [=](const Evaluation& e) { return e.studentId == id; }), evaluations_.end());
}

void Course::removeCriterion(int id) {
    require(findCriterion(id), "Criterion not found");
    criteria_.erase(std::remove_if(criteria_.begin(), criteria_.end(),
        [=](const Criterion& c) { return c.id == id; }), criteria_.end());
    evaluations_.erase(std::remove_if(evaluations_.begin(), evaluations_.end(),
        [=](const Evaluation& e) { return e.criterionId == id; }), evaluations_.end());
}

Evaluation EvaluationFactory::create(const Course& course, int studentId, int criterionId, double value) {
    require(course.findStudent(studentId), "Student not found");
    const Criterion* criterion = course.findCriterion(criterionId);
    require(criterion, "Criterion is not in rubric");
    return {studentId, criterionId, Score(value, criterion->maxScore)};
}

double WeightedStrategy::weight(const Criterion& criterion) const { return criterion.weight; }
double EqualStrategy::weight(const Criterion&) const { return 1.0; }

double FinalScoreService::calculate(const Course& course, int studentId, const IScoreStrategy& strategy) {
    require(course.findStudent(studentId), "Student not found");
    double total = 0;
    double weights = 0;
    for (const auto& criterion : course.criteria()) {
        double factor = strategy.weight(criterion);
        weights += factor;
        for (const auto& evaluation : course.evaluations()) {
            if (evaluation.studentId == studentId && evaluation.criterionId == criterion.id) {
                total += evaluation.score.value() / criterion.maxScore * factor;
                break;
            }
        }
    }
    return weights == 0 ? 0 : total / weights * 100;
}

} // namespace pos
