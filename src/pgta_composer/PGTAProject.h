
#pragma once

#include <QString>
#include <memory>
#include <vector>
#include "PGTATrackTreeModel.h"
#include "PGTALibraryModel.h"

class PGTAProject
{
public:
    PGTAProject();
    void ImportTrack(const QString &filePath);
    //void RemoveTrack();
    void ImportSample(const QString &filePath);
    //void RemoveSample();
    PGTATrackTreeModel *GetCurrentTracK() const;
public:
    QString WorkingDir() const;
    void SetWorkingDir(const QString &filePath);
    QString FilePath() const;
    void SetFilePath(const QString &filePath);

private:
    //qan::Graph* m_projectGraph;

    std::vector<PGTATrackTreeModel> m_projectTrackModels;
    PGTALibraryModel m_projectLibrary;
    QString m_projectFilePath;
    QString m_projectWorkingDir;
};
