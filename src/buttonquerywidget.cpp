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
****************************************************************************/

#include "buttonquerywidget.h"

ButtonQueryWidget::ButtonQueryWidget(QWidget *parent) : QueryWidget(parent) {
	this->button = new QPushButton(this);
    //this->button->setFixedWidth(75);
	this->button->setFixedHeight(23);
    this->layout = new QHBoxLayout(this);
    this->layout->setMargin(1);
    this->layout->addWidget(this->button);;
    this->layout->setMargin(1);
	connect(this->button, &QPushButton::clicked, this, &ButtonQueryWidget::sendQuery);
}

ButtonQueryWidget::ButtonQueryWidget(QString name, QString command, bool expert, QString infoText, QWidget* parent) : ButtonQueryWidget(parent) {
    this->name = name;
    this->command = command;
    this->expert = expert;
    this->setToolTip("<font>" + infoText + "</font>");
    this->button->setText(name);
}

ButtonQueryWidget::~ButtonQueryWidget() {
}

void ButtonQueryWidget::handleResponse(QString initialQuery, QString response) {
}

void ButtonQueryWidget::sendQuery() {
	emit query(this, this->command);
}
