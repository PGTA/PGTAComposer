#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItem>
#include <QFileDialog>
#include <QStandardItemModel>
#include <sstream>      // std::ostringstream
#include <string>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    ui->setupUi(this);
    selectedSampleIndex = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getFileName(const std::string fileName)
{
   const std::size_t slashIndex = fileName.find_last_of("\\/");
   return QString::fromStdString(fileName.substr(slashIndex+1));
}

void MainWindow::saveTrackFile()
{
    if (fileName.length() == 0)
    {
        QString file = QFileDialog::getSaveFileName(this,tr("Save Track File"),"",tr("Track files (*.track)"));
        if (file.isEmpty()) {
            ui->statusbar->showMessage("Error saving track file!");
            return;
        }
        fileName = file.toStdString();
    }

    std::fstream output(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!newTrack.SerializePartialToOstream(&output))
    {
        ui->statusbar->showMessage("Error saving track file!");
        return;
    }

    //delete all global object allocated by libprotobuf
    google::protobuf::ShutdownProtobufLibrary();
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),getFileName(fileName));
    ui->statusbar->showMessage("Track file saved!");
}

void MainWindow::on_pushButton_clicked()
{
   saveTrackFile();
}

void MainWindow::on_actionNew_triggered()
{
}

void MainWindow::on_actionSave_triggered()
{
    saveTrackFile();
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Track files (*.track)"));
    if (fileList.size() <= 0) {
        return;
    }

    newTrack.Clear();
    oldTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();

    ui->sample_path_edit->setText("");
    ui->sample_frequency_edit->setText("");
    ui->sample_prob_edit->setText("");
    ui->sample_vol_edit->setText("");

    fileName = fileList.first().toStdString().c_str();

    std::fstream input(fileName, std::ios::in | std::ios::binary);
    if (!input)
    {
        ui->statusbar->showMessage("Invalid track file!");
        return;
    }

    bool parsed = oldTrack.ParseFromIstream(&input);
    input.close();

    if (!parsed)
    {
        ui->statusbar->showMessage("Failed to parse track!");
        return;
    }

    for (int i = 0; i< oldTrack.samples().size(); i++)
    {
        const PGTA::Track_Sample oldSample = oldTrack.samples().Get(i);
        trackSamples.push_back(newTrack.add_samples());
        trackSamples[i]->set_filepath(oldSample.filepath());
        trackSamples[i]->set_frequency(oldSample.frequency());
        trackSamples[i]->set_probability(oldSample.probability());
        trackSamples[i]->set_volumemultiplier(oldSample.volumemultiplier());
    }

    for (int i = 0; i< oldTrack.groups().size(); i++)
    {
        const PGTA::Track_Group oldGroup = oldTrack.groups().Get(i);
        trackGroups.push_back(newTrack.add_groups());

        for (int j = 0; j < oldGroup.sampleindex().size(); j++) {
            trackGroups[i]->add_sampleindex(oldGroup.sampleindex().Get(j));
        }
    }

    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),getFileName(fileName));
    refreshListView();
}

void MainWindow::refreshListView(){
    QStandardItemModel* listModel = new QStandardItemModel();
    for (auto *sample : trackSamples)
    {
        QStandardItem *items = new QStandardItem(getFileName(sample->filepath()));
        items->setEditable(false);
        listModel->appendRow(items);
    }
    ui->listView->setModel(listModel);
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    listViewClickHandler(index);
}

void MainWindow::listViewClickHandler(const QModelIndex &index) {
    selectedSampleIndex = index.row();
    PGTA::Track_Sample* selectedSample = trackSamples[selectedSampleIndex];
    QString qs(selectedSample->filepath().c_str());
    ui->sample_path_edit->setText(qs);
    qs = QString::number(selectedSample->frequency());
    ui->sample_frequency_edit->setText(qs);
    qs = QString::number(selectedSample->probability());
    ui->sample_prob_edit->setText(qs);
    qs = QString::number(selectedSample->volumemultiplier());
    ui->sample_vol_edit->setText(qs);
}

void MainWindow::on_save_sample_button_clicked()
{
    if (selectedSampleIndex == -1)
    {
        return;
    }

    PGTA::Track_Sample* selectedSample = trackSamples[selectedSampleIndex];

    int new_frequency, new_prob;
    float new_vol;
    QString new_path;

    try
    {
        new_path = ui->sample_path_edit->text();
        new_frequency = ui->sample_frequency_edit->text().toInt();
        new_prob = ui->sample_prob_edit->text().toUInt();
        new_vol = ui->sample_vol_edit->text().toFloat();
    } catch (int e)
    {
        ui->statusbar->showMessage("Invalid Sample Parameters!");
        return;
    }

    selectedSample->set_filepath(new_path.toStdString());
    selectedSample->set_frequency(new_frequency);
    selectedSample->set_probability(new_prob);
    selectedSample->set_volumemultiplier(new_vol);

    ui->statusbar->showMessage("Sample saved!");

}

void MainWindow::on_add_sample_button_clicked()
{
    trackSamples.push_back(newTrack.add_samples());
    refreshListView();
    QModelIndex modelIndex = ui->listView->model()->index(trackSamples.size()-1,0);
    ui->listView->setCurrentIndex(modelIndex);
    listViewClickHandler(modelIndex);
}

void MainWindow::on_remove_sample_button_clicked()
{
    QModelIndex currentIndex = ui->listView->currentIndex();

    oldTrack = newTrack;
    newTrack.Clear();
    trackSamples.clear();
    trackGroups.clear();

    for (int i = 0; i< oldTrack.samples().size(); i++)
    {
        if (i == currentIndex.row()){
            continue;
        }
        const PGTA::Track_Sample oldSample = oldTrack.samples().Get(i);
        trackSamples.push_back(newTrack.add_samples());
        trackSamples[i]->set_filepath(oldSample.filepath());
        trackSamples[i]->set_frequency(oldSample.frequency());
        trackSamples[i]->set_probability(oldSample.probability());
        trackSamples[i]->set_volumemultiplier(oldSample.volumemultiplier());
    }

    for (int i = 0; i< oldTrack.groups().size(); i++)
    {
        const PGTA::Track_Group oldGroup = oldTrack.groups().Get(i);
        trackGroups.push_back(newTrack.add_groups());

        for (int j = 0; j < oldGroup.sampleindex().size(); j++) {
            trackGroups[i]->add_sampleindex(oldGroup.sampleindex().Get(j));
        }
    }

    refreshListView();

}
