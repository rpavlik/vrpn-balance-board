/** @file
	@brief Implementation

	@date 2012

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
	
*/

//           Copyright Iowa State University 2012.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)

// Internal Includes
#include "mainwindow.h"
#include "ui_main.h"

// Library/third-party includes
#include <QtGui/QApplication>
#include <QInputDialog>
#include <vrpn_QAnalogRemote.h>

// Standard includes
// - none

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}

// Balance board data: (requires WiiUse 0.13 or newer, preferably 0.14 or newer)
//    channel[64] = Balance board: top-left sensor, kg
//    channel[65] = Balance board: top-right sensor, kg
//    channel[66] = Balance board: bottom-left sensor, kg
//    channel[67] = Balance board: bottom-right sensor, kg
//    channel[68] = Balance board: total mass, kg
//    channel[69] = Balance board: center of gravity x, in [-1, 1]
//    channel[70] = Balance board: center of gravity y, in [-1, 1]
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow) {
	ui->setupUi(this);
	_fields.push_back(ui->frontLeft);
	_fields.push_back(ui->frontRight);
	_fields.push_back(ui->rearLeft);
	_fields.push_back(ui->rearRight);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_actionConnect_triggered() {

	QString server = QInputDialog::getText(this, QLatin1String("Connect to VRPN Server"), QLatin1String("Enter VRPN device and server to use"));
	if (server.isNull()) {
		return;
	}

	_container.stop();
	_container.clear();
	vrpn_QAnalogRemote * remote = _container.add(new vrpn_QAnalogRemote(server));
	connect(remote, SIGNAL(analogReport(QList<double>)), this, SLOT(analogReport(QList<double>)));
	_container.start();
}

void MainWindow::_setKg(QLineEdit * field, double kg) {
	static const QString formatString("%1 kg");
	field->setText(formatString.arg(kg, 4));
}

void MainWindow::analogReport(QList<double> channels) {
	for (int i = 0; i < 4; ++i) {
		_setKg(_fields[i], channels[64 + i]);
	}
}