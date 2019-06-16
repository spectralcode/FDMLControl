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

#include "intvaluewidget.h"

IntValueWidget::IntValueWidget(QWidget* parent, QString name) : QueryWidget(parent){
	this->value = 0;
	this->name = name;
	this->spinBox = new QSpinBox(this);
	this->label = new QLabel(this->name, this);
	this->layout = new QHBoxLayout(this);
    this->layout->setMargin(0);
	this->layout->addWidget(this->label);
	this->layout->addWidget(this->spinBox);
	connect(this->spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntValueWidget::changeValue);
}

IntValueWidget::IntValueWidget(int minValue, int maxValue, int defaultValue, int stepSize, bool expert, QString infoText, QWidget* parent, QString name) : IntValueWidget(parent, name) {
	this->setMin(minValue);
	this->setMax(maxValue);
	this->setValue(defaultValue);
	this->setStepSize(stepSize);
	this->setExpert(expert);
	this->setToolTip("<font>" + infoText + "</font>"); //the font-tags convert the infoText to rich text, which is automatically word wrapped in a tooltip. The tooltip will adjust the width and not display the whole text in one line
}

IntValueWidget::~IntValueWidget()
{
}

void IntValueWidget::setMin(int minValue)
{
	this->spinBox->setMinimum(minValue);
}

void IntValueWidget::setMax(int maxValue)
{
	this->spinBox->setMaximum(maxValue);
}

void IntValueWidget::setStepSize(int stepSize){
	this->spinBox->setSingleStep(stepSize);
}

void IntValueWidget::setValue(int value) {
	this->value = value;
	disconnect(this->spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntValueWidget::changeValue);
	this->spinBox->setValue(this->value);
	connect(this->spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntValueWidget::changeValue);
}

void IntValueWidget::changeValue(int value) {
	this->setValue(value);
	QString command = this->name + " " + QString::number(value);
	emit query(this, command);
}

void IntValueWidget::queryCurrentValue(){
	QString command;
	if (this->name.contains(' ')) {
		int pos = this->name.indexOf(' ');
		QString sub = this->name.left(pos);
        QString back = this->name.at(name.size()-1);
        command = sub + "? " + back;
	}
	else {
		command = this->name + "?";
	}
    emit query(this, command);
}

void IntValueWidget::handleResponse(QString initialQuery, QString response){
	//check if response is not empty
	if (!(response.size() > 2)) {
		emit error(tr("Could not change value, response too short. Initial query: ") + initialQuery + tr(" Response: ") + response);
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
	if (status == "200") {
		if (recoveredResponse.contains("DATA")) {
			int dataPos = recoveredResponse.indexOf(">") + 2;
			recoveredResponse.remove(0, dataPos);
			int value = recoveredResponse.toInt();
			this->setValue(value);
		}
	}
	else if (status == "401") {
		emit error(tr("Invalid command! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
	else {
		emit error(tr("Unexpected response! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
}
