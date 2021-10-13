#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

	ui->pushButton->setEnabled(true);

	ui->pushButton_2->setEnabled(false);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    ui->widget->CameraCheck();

    ui->widget->CameraOpen();

    ui->widget->CameraStart();

	// 设置曝光
//	ui->widget->SetExposeTime(10000);

	// 设置增益
//	ui->widget->SetAdjustPlus(5);

	// 设置连续拉流。
	ui->widget->CameraChangeTrig(CammerWidget::trigContinous);

	//// 设置软触发拉流
	//ui->widget->CameraChangeTrig(CammerWidget::trigSoftware);
	//// 软触发100次
	//for (int i = 0; i<100; i++)
	//{
	//	ui->widget->ExecuteSoftTrig();
	//}

	ui->pushButton->setEnabled(false);

	ui->pushButton_2->setEnabled(true);
}

void Form::on_pushButton_2_clicked()
{
    ui->widget->CameraStop();

    ui->widget->CameraClose();

	ui->pushButton->setEnabled(true);

	ui->pushButton_2->setEnabled(false);
}

void Form::closeEvent(QCloseEvent * event)
{
	ui->widget->CameraStop();

	ui->widget->CameraClose();
}