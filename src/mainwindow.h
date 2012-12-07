/** @file
	@brief Header

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

#pragma once
#ifndef INCLUDED_mainwindow_h_GUID_49C47E36_F14E_4A98_6059_2A4E40DF79C3
#define INCLUDED_mainwindow_h_GUID_49C47E36_F14E_4A98_6059_2A4E40DF79C3


// Internal Includes
// - none

// Library/third-party includes
#include <QMainWindow>
#include <vrpn_QMainloopContainer.h>

// Standard includes
#include <vector>
#include <valarray>

namespace Ui {
	class MainWindow;
}
class QLineEdit;

class MainWindow : public QMainWindow {
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void on_actionConnect_triggered();
		void analogReport(QList<double> channels);
		void rezeroButton(int);

	private:
		typedef std::valarray<double> DoubleArray;
		void _setKg(QLineEdit * field, double kg);
		

		Ui::MainWindow *ui;
		DoubleArray _zero;
		DoubleArray _last;
		vrpn_QMainloopContainer _container;
		std::vector<QLineEdit*> _fields;
};


#endif // INCLUDED_mainwindow_h_GUID_49C47E36_F14E_4A98_6059_2A4E40DF79C3

