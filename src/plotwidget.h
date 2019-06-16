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
**          Contact:    zabic                                             **
**                      at                                                **
**                      iqo.uni-hannover.de                               **
**                                                                        **
**          Date:       12 June 2019                                      **
**          Version:    1.0.0                                             **
****************************************************************************/

#pragma once

#define INTENSITY_QUERY_MAIN "fadc.dump? 0 256"
#define INTENSITY_QUERY_CAVITY "fadc.dump? 1 256"
#define SOA_CURRENT "reg.sadc.dump.ch? 16"
#define BOA_CURRENT "reg.sadc.dump.ch? 17"
#define AUTO_UPDATE_TIME_MS 3000

#include <QTimer>
#include <QCheckBox>
#include "querywidget.h"
#include "QCustomPlot/qcustomplot.h"

class PlotWidget : public QueryWidget
{
	Q_OBJECT
public:
	PlotWidget(QWidget *parent);
	~PlotWidget();

private:
	QTimer* timer;
	bool autoUpdate;
	QVBoxLayout* layout;
	QCustomPlot* plot;
	QCheckBox* checkBox;
	QLabel* cavitySoaCurrent;
	QLabel* boosterSoaCurrent;

	QVector<qreal> plotValuesMain;
	QVector<qreal> plotValuesCavity;
	QVector<qreal> sampleNumbers;

	void queryCurrentValues();

public slots:
	void handleResponse(QString initialQuery, QString response) override;
	void setAutoUpdate(bool autoUpdate);
	void rescalePlot();

signals:
};
