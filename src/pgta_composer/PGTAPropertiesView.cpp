
#include <QPainter>
#include <QToolTip>
#include <QFileDialog>
#include <limits>
#include <math.h>
#include <sstream>
#include "PGTAPropertiesView.h"
#include "ui_PGTAPropertiesView.h"

PGTAPropertiesView::PGTAPropertiesView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PGTAPropertiesView)
{
    ui->setupUi(this);
    // We want the slider to be centered on 0db with max value 6db and min -96db
    // range for slider is thus 17017 to max int. When the value is saved to the model it will
    // be converted using the following equation 20*log_10(slidervalue/(maxint/2)).
    static float minSliderValue = pow(10.0f, -96.0f/20.0f) * std::numeric_limits<int>::max()/2.0f;
	ui->EditGain->setMinimum(minSliderValue);
	ui->EditGain->setMaximum(std::numeric_limits<int>::max());
	ui->EditGain->setValue(std::numeric_limits<int>::max() / 2);
    ConnectSignals();
}

PGTAPropertiesView::~PGTAPropertiesView()
{
    delete ui;
}

void PGTAPropertiesView::ConnectSignals()
{
    // gain slider
    connect(ui->EditGain, SIGNAL(sliderMoved(int)), this, SLOT(slotShowSliderTooltip(int)));
    // file browser
    connect(ui->Browse, SIGNAL(clicked()),this, SLOT(slotShowFileBrowser()));
}


Ui::PGTAPropertiesView* PGTAPropertiesView::GetUi() const
{
    return ui;
}

void PGTAPropertiesView::SetSamplePropertiesShown(bool shown)
{
    if (shown)
    {
        ui->SampleProperties->show();
    }
    else
    {
        ui->SampleProperties->hide();
    }
}

void PGTAPropertiesView::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PGTAPropertiesView::ClearProperyValues()
{
    ui->EditDefaultFile->clear();
    ui->EditName->clear();
    ui->EditDefaultFile->clear();
    ui->EditStartTime->clear();
    ui->EditPeriod->clear();
    ui->EditDeviation->clear();
    ui->EditProbability->clear();
    ui->EditGain->setValue(std::numeric_limits<int>::max()/2);
}

void PGTAPropertiesView::slotShowSliderTooltip(int position)
{
    QString toolTip;
    // convert slider value into db
    float value = 20.0f * log10(position/(std::numeric_limits<int>::max()/2.0f));
    value = int(value * 10)/10.0f;

    // show max 1 decimal place
    std::stringstream ss;
    ss.precision(3);
    ss << value << "db";

    // display tooltip
    toolTip = QString::fromStdString(ss.str());
    QPoint slider = ui->EditGain->mapToGlobal(QPoint( 0, 0 ));
    QPoint cursor = QCursor::pos();
    QToolTip::showText(QPoint(cursor.x(), slider.y()), toolTip);
}

void PGTAPropertiesView::slotShowFileBrowser()
{
    QString directory = QFileDialog::getOpenFileName(this,
                                tr("Find Files"), QDir::currentPath());
    if (!directory.isEmpty())
    {
        ui->EditDefaultFile->setText(directory);
        ui->EditDefaultFile->setFocus();
    }
}
