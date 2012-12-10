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
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include <vrpn_QAnalogRemote.h>
#include <vrpn_QButtonRemote.h>

// Standard includes
#include <cmath>

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
	, ui(new Ui::MainWindow)
	, _scene(new QGraphicsScene) {
	ui->setupUi(this);

	// Set up chart
	ui->graphicsView->setScene(_scene.data());
	updateViewFit();
	_scene->addLine(QLineF(-1, 0, 1, 0));
	_scene->addLine(QLineF(0, -1, 0, 1));
	_point = _scene->addEllipse(-.01, -.01, .02, .02);
	_point->hide();

	// Stash pointers to the fields in the same order as the data coming in
	_fields.push_back(ui->frontLeft);
	_fields.push_back(ui->frontRight);
	_fields.push_back(ui->rearLeft);
	_fields.push_back(ui->rearRight);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_actionConnect_triggered() {

	QString server = QInputDialog::getText(this,
		QLatin1String("Connect to VRPN Server"),
		QLatin1String("Enter VRPN device and server to use"),
		QLineEdit::Normal,
		QString("BalanceBoard@metal-devices.vrac.iastate.edu:3800"));
	if (server.isNull()) {
		return;
	}

	_container.stop();
	_container.clear();

	_last.resize(0);
	_lastAdjusted.resize(0);
	_zero.resize(0);

	// Analog for values
	vrpn_QAnalogRemote * analog = _container.add(new vrpn_QAnalogRemote(server));
	connect(analog, SIGNAL(analogReport(QList<double>)), this, SLOT(analogReport(QList<double>)));

	// Button to detect front panel button AKA A button for re-zeroing.
	vrpn_QButtonRemote * button = _container.add(new vrpn_QButtonRemote(server));
	connect(button, SIGNAL(buttonReleased(int)), this, SLOT(rezeroButton(int)));

	_container.start();
}

namespace {
	double ensureNonNegative(double in) {
		return (std::max)(0.0, in);
	}
} // end of namespace

void MainWindow::analogReport(QList<double> channels) {
	_last.resize(4);
	for (int i = 0; i < 4; ++i) {
		_last[i] = channels[64 + i];
	}

	if (_zero.size() == 0) {
		// Assume our initial reading is 0.
		_rezero();
	}

	_lastAdjusted = (_last - _zero).apply(&ensureNonNegative);

	for (int i = 0; i < 4; ++i) {
		_setKg(_fields[i], _lastAdjusted[i]);
	}
}

void MainWindow::_updateShowingCenterOfGravity() {
	if (_lastAdjusted.min() > 0) {
		_point->show();
		_point->setPos(_getCenterOfGravity());
	} else {
		_point->hide();
	}
}


void MainWindow::rezeroButton(int) {
	_rezero();
}

/// @brief Always fit [-1, 1] on both axes.
void MainWindow::updateViewFit() {
	static const QRectF sceneRect(-1, -1, 2, 2);
	
	/*
	QTransform newXform;
	QTransform::quadToQuad(QPolygonF(sceneRect), QPolygonF(QRectF(ui->graphicsView->geometry())), newXform);
	*/
	_scene->setSceneRect(sceneRect);
	ui->graphicsView->fitInView(sceneRect);
	//ui->graphicsView->setTransform(newXform);
	/*
	const QSize s = ui->graphicsView->size();
	const float xScale = s.width() / 2.0;
	const float yScale = s.height() / 2.0;
	ui->graphicsView->setTransform(QTransform::fromScale(xScale, yScale).translate(-1, -1));
	*/
	//_scene->setSceneRect(-1, -1, 2, 2);
	
}

void MainWindow::_rezero() {
	if (_last.size() == 0) {
		return;
	}
	_zero = _last;
}

void MainWindow::_setKg(QLineEdit * field, double kg) {
	static const QString formatString("%1 kg");
	field->setText(formatString.arg(kg, 6, 'f', 3));
}


QPointF MainWindow::_getCenterOfGravity() {
	static const double xcoords_data[4] = {-1, 1, -1, 1};
	static const DoubleArray xcoords(xcoords_data, 4);
	static const double ycoords_data[4] = {1, 1, -1, -1};
	static const DoubleArray ycoords(ycoords_data, 4);

	double total = _lastAdjusted.sum();
	DoubleArray normalized = _lastAdjusted / total;

	// Do a weighted average of X sensor coordinates (weighted by normalized weight)
	// and the same for Y
	return QPointF((normalized * xcoords).sum(), (normalized * xcoords).sum());
}