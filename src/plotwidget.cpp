/***************************************************************************
**                                                                        **
**  This file is part of FDMLControl.                                     **
**  FDMLControl enables the control of the Optores FDML laser via a       **
**  serial connection.                                                    **
**  Copyright (C) 2019 Miroslav Zabic                                     **
**                                                                        **
**  FDMLControl is free software: you can redistribute it and/or modify   **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**          Author:     Miroslav Zabic                                    **
**          Contact:    zabic@iqo.uni-hannover.de                         **
**          Date:       12 June 2019                                      **
**          Version:    1.0.0                                             **
****************************************************************************/

#include "plotwidget.h"

PlotWidget::PlotWidget(QWidget *parent)
	: QueryWidget(parent)
{
	this->plot = new QCustomPlot(this);



    this->plot->setBackground( QColor(50, 50, 50));
    this->plot->axisRect()->setBackground(QColor(55, 55, 55));
    this->plot->xAxis->setBasePen(QPen(Qt::white, 1));
    this->plot->yAxis->setBasePen(QPen(Qt::white, 1));
    this->plot->xAxis->setTickPen(QPen(Qt::white, 1));
    this->plot->yAxis->setTickPen(QPen(Qt::white, 1));
    this->plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    this->plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    this->plot->xAxis->setTickLabelColor(Qt::white);
    this->plot->yAxis->setTickLabelColor(Qt::white);

	this->plot->setMinimumWidth(270);
	this->plot->setMinimumHeight(200);
	this->plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->plot->addGraph();
    QPen mainOutPen = QPen(QColor(255, 150, 155));
    mainOutPen.setWidth(2);
    this->plot->graph(0)->setPen(mainOutPen);;
	this->plot->graph(0)->setName("Main Out");
	this->plot->addGraph();
    this->plot->graph(1)->setPen(QPen(Qt::white));
	this->plot->graph(1)->setName("Cavity");
	this->plot->xAxis->setLabel("Time");
	this->plot->yAxis->setLabel("Intensity [a.u.]");
    this->plot->xAxis->setLabelColor(Qt::white);
    this->plot->yAxis->setLabelColor(Qt::white);
	this->plot->yAxis->setRange(0, 2048);
	this->plot->legend->setVisible(true);
	QSize iconSize = this->plot->legend->iconSize();
	iconSize.setWidth(12);
	this->plot->legend->setIconSize(iconSize);
    this->plot->legend->setBrush(QColor(80, 80, 80, 200));
    this->plot->legend->setTextColor(Qt::white);
    this->plot->legend->setBorderPen(QColor(180, 180, 180, 200));
	this->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

	this->layout = new QVBoxLayout(this);
	this->layout->addWidget(this->plot);


	this->timer = new QTimer(this);
	this->autoUpdate = false;
	
	auto hlayout = new QHBoxLayout(this);
	this->cavitySoaCurrent = new QLabel(this);
	this->cavitySoaCurrent->setText(tr("SOA current: 0   mA"));
	this->boosterSoaCurrent = new QLabel(this);
	this->boosterSoaCurrent->setText(tr("BOA current: 0   mA"));

	this->checkBox = new QCheckBox("Auto update", this);
	
    hlayout->addSpacing(6);
	hlayout->addWidget(this->checkBox);
	hlayout->addWidget(this->cavitySoaCurrent);
	hlayout->addWidget(this->boosterSoaCurrent);

	this->layout->addLayout(hlayout);
    this->layout->setMargin(2);

	connect(this->checkBox, &QCheckBox::stateChanged, this, &PlotWidget::setAutoUpdate);
	connect(this->plot, &QCustomPlot::mouseDoubleClick, this, &PlotWidget::rescalePlot);

}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::queryCurrentValues(){
	emit query(this, INTENSITY_QUERY_MAIN);
	emit query(this, INTENSITY_QUERY_CAVITY);
	emit query(this, SOA_CURRENT);
	emit query(this, BOA_CURRENT);
}

void PlotWidget::setAutoUpdate(bool autoUpdate){
	if (this->autoUpdate == autoUpdate) {
		return;
	}
	if (autoUpdate && this->checkBox->isChecked()) {
		connect(this->timer, &QTimer::timeout, this, &PlotWidget::queryCurrentValues);
		this->timer->start(AUTO_UPDATE_TIME_MS);
		this->autoUpdate = true;
	}
	else {
		disconnect(this->timer, &QTimer::timeout, this, &PlotWidget::queryCurrentValues);
		this->timer->stop();
		this->autoUpdate = false;
		this->checkBox->setChecked(false);
	}
}

void PlotWidget::rescalePlot(){
	this->plot->graph(0)->rescaleKeyAxis();
	this->plot->replot();
}


void PlotWidget::handleResponse(QString initialQuery, QString response) {
	//check if response is not empty
	if (!(response.size() > 2)) {
		emit error(tr("Unexpected response. Response too short. Initial query: ") + initialQuery + tr(" Response: ") + response);
		return;
	}
	//extract initial query and actual response part from response
	QString recoveredQuery = response.left((response.indexOf("\n")) - 1);
	QString recoveredResponse = response.right((response.size() - response.indexOf("\n")) - 1);
	recoveredQuery.remove(QRegExp("[\\r\\n]"));
	recoveredResponse.remove(QRegExp("[\\r\\n]"));

	//check if response belongs to initial query as expected
	if (recoveredQuery != initialQuery) {
		emit error(tr("Fatal Error: incorrect order of elements in command queue. Supposed query: ") + initialQuery + tr(" Actual query: ") + recoveredQuery + tr(" Response to actual query: ") + response);
		return;
	}

	//get three-digit status code and extract value from response if status ok
	QString status = recoveredResponse.left(3);
	if (status == "200" && recoveredResponse.contains("DATA")) {
		if (initialQuery == SOA_CURRENT) {
			int dataPos = recoveredResponse.indexOf(">") + 2;
			recoveredResponse.remove(0, dataPos);
			int value = recoveredResponse.toInt();
			this->cavitySoaCurrent->setText(tr("SOA current: ") + QString::number(value) + tr(" mA"));
			return;
		}
		if (initialQuery == BOA_CURRENT) {
			int dataPos = recoveredResponse.indexOf(">") + 2;
			recoveredResponse.remove(0, dataPos);
			int value = recoveredResponse.toInt();
			this->boosterSoaCurrent->setText(tr("BOA current: ") + QString::number(value) + tr(" mA"));
			return;
		}
		bool rescaleKeyAxis = false;
		//check if response is for "Main" plot or for "Cavity" plot
		QVector<qreal>* plotVector = nullptr;
		QCPGraph* graph = nullptr;
		if (initialQuery == INTENSITY_QUERY_MAIN) {
			plotVector = &(this->plotValuesMain);
			graph = this->plot->graph(0);
		}
		else if (initialQuery == INTENSITY_QUERY_CAVITY) {
			plotVector = &(this->plotValuesCavity);
			graph = this->plot->graph(1);
		}
		else {
			emit error(tr("Unexpected error occured during read out of intensity values."));
			return;
		}

		//extract values from response
		int dataPos = recoveredResponse.indexOf(">") + 2;
		recoveredResponse.remove(0, dataPos);
		QStringList values = recoveredResponse.split(" ", QString::SkipEmptyParts);

		//resize plot vectors if necessary
		int samplesInRespons = values.size();
		if (plotVector->size() != samplesInRespons) {
			plotVector->resize(samplesInRespons);
			this->sampleNumbers.resize(samplesInRespons);
			for (int i = 0; i<samplesInRespons; i++) {
				this->sampleNumbers[i] = i;
			}
			rescaleKeyAxis = true;
		}

		//convert samples from string to numbers
		for (int i = 0; i < samplesInRespons; i++) {
			bool check = false;
			int val = values.at(i).toInt(&check, 10);
			if (check) {
				(*plotVector)[i] = val;
			}
			else {
				//Conversion to int failed
			}
		}

		//update plot
		graph->setData(this->sampleNumbers, *plotVector, true);
		if (rescaleKeyAxis) { graph->rescaleKeyAxis(); }
		this->plot->replot();
		QCoreApplication::processEvents();
	}
	else if (status == "401") {
		emit error(tr("Invalid command! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
	else {
		emit error(tr("Could not display plot values! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
}
