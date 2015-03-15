
#pragma once

#include <QString>
#include <vector>

class PGTALibraryModel;
class TrackTreeModel;

class PGTAProject
{
public:
    PGTAProject();

private:
    PGTALibraryModel *m_projectLibrary;
    //qan::Graph* m_projectGraph;
    std::vector<TrackTreeModel*> *m_projectTrackModels;
    QString m_projectFilePath;
    QString m_projectWorkingDir;
};
