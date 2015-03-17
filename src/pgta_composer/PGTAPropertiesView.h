
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
    void slotShowSliderTooltip(int position);

private:
    void paintEvent(QPaintEvent *) override;
    void ConnectSignals();

private:
    Ui::PGTAPropertiesView *ui;
};

