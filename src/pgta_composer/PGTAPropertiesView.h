
#pragma once

#include <QWidget>

namespace Ui {
class PGTAPropertiesView;
}

class PGTAPropertiesView : public QWidget
{
    Q_OBJECT

public:
    explicit PGTAPropertiesView(QWidget *parent = 0);
    ~PGTAPropertiesView();
    Ui::PGTAPropertiesView *GetUi() const;
    void SetSamplePropertiesShown(bool shown);
private slots:

private:
    void paintEvent(QPaintEvent *) override;

private:
    Ui::PGTAPropertiesView *ui;
};

