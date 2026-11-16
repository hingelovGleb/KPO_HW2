#pragma once
#include "domain/domain.hpp"
#include <string>
#include <vector>

namespace pos {

class ICourseRepository {
public:
    virtual ~ICourseRepository() = default;
    virtual Course load() const = 0;
    virtual void save(const Course& course) const = 0;
};

struct ReportRow { int id; std::string name; double result; };

class IReportExporter {
public:
    virtual ~IReportExporter() = default;
    virtual void write(const std::string& path, const std::vector<ReportRow>& rows) const = 0;
};

// Facade. Все действия консоли идут через этот класс.
class PosFacade {
    ICourseRepository& repository_;
    IReportExporter& exporter_;
    Course course_;
    WeightedStrategy weighted_;
    EqualStrategy equal_;
    const IScoreStrategy& strategy(bool equal) const;
public:
    PosFacade(ICourseRepository& repository, IReportExporter& exporter);
    const Course& course() const;
    int addStudent(const std::string& name);
    int addCriterion(const std::string& name, double maximum, double weight);
    void setEvaluation(int studentId, int criterionId, double score);
    void removeEvaluation(int studentId, int criterionId);
    void removeStudent(int id);
    void removeCriterion(int id);
    std::vector<ReportRow> results(bool equal = false) const;
    void exportResults(const std::string& path, bool equal = false) const;
};

} // namespace pos
