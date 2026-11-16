#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace pos {

void require(bool ok, const std::string& message);

struct Student { int id; std::string name; };
struct Criterion { int id; std::string name; double maxScore; double weight; };

class Score {
    double value_;
public:
    Score(double value, double maximum);
    double value() const;
};

struct Evaluation { int studentId; int criterionId; Score score; };
class Course;

class EvaluationFactory {
public:
    static Evaluation create(const Course& course, int studentId, int criterionId, double score);
};

class Course {
    std::vector<Student> students_;
    std::vector<Criterion> criteria_;
    std::vector<Evaluation> evaluations_;
    int nextStudentId_ = 1;
    int nextCriterionId_ = 1;
public:
    const std::vector<Student>& students() const;
    const std::vector<Criterion>& criteria() const;
    const std::vector<Evaluation>& evaluations() const;
    const Student* findStudent(int id) const;
    const Criterion* findCriterion(int id) const;
    int addStudent(const std::string& name, int savedId = 0);
    int addCriterion(const std::string& name, double maximum, double weight, int savedId = 0);
    void setEvaluation(int studentId, int criterionId, double value);
    void removeEvaluation(int studentId, int criterionId);
    void removeStudent(int id);
    void removeCriterion(int id);
};

class IScoreStrategy {
public:
    virtual ~IScoreStrategy() = default;
    virtual double weight(const Criterion& criterion) const = 0;
};

class WeightedStrategy : public IScoreStrategy {
public:
    double weight(const Criterion& criterion) const override;
};

class EqualStrategy : public IScoreStrategy {
public:
    double weight(const Criterion& criterion) const override;
};

class FinalScoreService {
public:
    static double calculate(const Course& course, int studentId, const IScoreStrategy& strategy);
};

} // namespace pos
