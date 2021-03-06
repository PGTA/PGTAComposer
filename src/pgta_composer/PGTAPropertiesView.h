
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
    void ClearProperyValues();

private slots:
    void slotShowSliderTooltip(int position);
    void slotShowFileBrowser();

private:
    void paintEvent(QPaintEvent *) override;
    void ConnectSignals();

private:
    Ui::PGTAPropertiesView *ui;
};

