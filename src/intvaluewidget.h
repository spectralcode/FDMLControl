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
#include <qwidget.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QCoreApplication>
#include "querywidget.h"

class IntValueWidget : public QueryWidget
{
	Q_OBJECT
public:
	IntValueWidget(QWidget *parent = 0, QString name = "");
	IntValueWidget(int minValue, int maxValue, int defaultValue, int stepSize, bool expert, QString infoText, QWidget *parent = 0, QString name = "");
	~IntValueWidget();
	void setMin(int minValue);
	void setMax(int maxValue);
	void setStepSize(int stepSize);
	QString getName() { return this->name; }

private:
	int value;
	QString name;
	QSpinBox* spinBox;
	QLabel* label;
	QHBoxLayout* layout;


public slots:
	void setValue(int value); //Sets current value to value and does not emit valueChanged signal
	void changeValue(int value); //Sets current value to value and emits valueChanged signal
	void queryCurrentValue();
	void handleResponse(QString initialQuery, QString response) override;

signals:
	void valueChanged(QString name, int value);

};

