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

#include "comboboxquerywidget.h"

ComboBoxQueryWidget::ComboBoxQueryWidget(QWidget *parent) : QueryWidget(parent) {
	this->comboBox = new QComboBox(this);
	this->label = new QLabel(this);
	this->layout = new QHBoxLayout(this);
    this->layout->setMargin(0);
	this->layout->addWidget(this->label);
	this->layout->addWidget(this->comboBox);
	this->enableDisableComboBox = false;
	
	connect(this->comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &ComboBoxQueryWidget::changeValue);
}

ComboBoxQueryWidget::ComboBoxQueryWidget(QString name, QStringList options, bool expert, QString infoText, QWidget* parent) : ComboBoxQueryWidget(parent) {
	this->name = name;
	this->options = options;
	this->expert = expert;
	this->setToolTip("<font>" + infoText + "</font>");
	this->label->setText(name + ": ");
	this->comboBox->addItems(options);

	if (this->comboBox->count() == 2 && this->comboBox->itemText(0).contains("enable", Qt::CaseInsensitive)) {
		this->enableDisableComboBox = true;
	}else{
		this->enableDisableComboBox = false;
	}
}

ComboBoxQueryWidget::~ComboBoxQueryWidget()
{
}

void ComboBoxQueryWidget::handleResponse(QString initialQuery, QString response){
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
			//check if this combobox is a enable/disable comboBox and convert response (1 or -1) to coresponding comboBox index (0 or 1) 
			if (this->enableDisableComboBox) {
				value = value == 1 ? 0 : 1; 
			} 
            if(this->comboBox->count() > value){
                this->comboBox->setCurrentIndex(value);
            }else{
                emit error(tr("Invalid response! Probably fdml_commands.xml needs to be updated. Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
            }
		}
	}
	else if (status == "401") {
		emit error(tr("Invalid command! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
	else {
		emit error(tr("Unexpected response! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
}

void ComboBoxQueryWidget::changeValue(int value) {
	QString command = this->name + " " + QString::number(value);
	emit query(this, command);
}

void ComboBoxQueryWidget::queryCurrentValue() {
	QString command;
	if (this->name.contains(' ')) {
		int pos = this->name.indexOf(' ');
		QString sub = this->name.left(pos);
		//QString back = this->name.back();
		QString back = this->name.at(name.size() - 1);
		//command = this->name.left(pos) + "? " + this->name.back();
		command = sub + "? " + back;
	}
	else {
		command = this->name + "?";
	}
	emit query(this, command);
}
