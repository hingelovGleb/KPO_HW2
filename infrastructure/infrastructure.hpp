#pragma once
#include "application/application.hpp"
#include <string>

namespace pos {

class FileCourseRepository : public ICourseRepository {
    std::string path_;
public:
    explicit FileCourseRepository(std::string path);
    Course load() const override;
    void save(const Course& course) const override;
};

class CsvExporter : public IReportExporter {
    static std::string escaped(const std::string& value);
public:
    void write(const std::string& path, const std::vector<ReportRow>& rows) const override;
};

// Здесь вручную создаются зависимости.
struct AppContainer {
    FileCourseRepository repository;
    CsvExporter exporter;
    PosFacade app;
    explicit AppContainer(const std::string& dataPath);
};

} // namespace pos
