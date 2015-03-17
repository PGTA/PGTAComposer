
#include<QPainter>
#include "PGTAPropertiesView.h"
#include "ui_PGTAPropertiesView.h"

PGTAPropertiesView::PGTAPropertiesView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PGTAPropertiesView)
{
    ui->setupUi(this);
}

PGTAPropertiesView::~PGTAPropertiesView()
{
    delete ui;
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
