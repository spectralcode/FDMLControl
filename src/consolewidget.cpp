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

#include "consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
	: QueryWidget(parent)
{
	ui.setupUi(this);
	connect(this->ui.lineEdit, &QLineEdit::returnPressed, this, &ConsoleWidget::slot_commandSent);
	connect(this->ui.pushButton_send, &QPushButton::clicked, this, &ConsoleWidget::slot_commandSent);
}

ConsoleWidget::~ConsoleWidget()
{
}

void ConsoleWidget::slot_commandSent() {
	QString command = this->ui.lineEdit->text();
	emit query(this, command);
}

void ConsoleWidget::slot_addText(QString text) {
	this->ui.plainTextEdit->appendPlainText(text);
}

void ConsoleWidget::handleResponse(QString initialQuery, QString response) {
	this->slot_addText(initialQuery.remove(QRegExp("[\\r\\n]")) + "> " + response.remove(QRegExp("[\\r\\n]")));
}
