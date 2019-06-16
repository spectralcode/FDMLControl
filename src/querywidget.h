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

//Abstract class that defines the necessary signals, slots and methods for communication with QueryManager


#pragma once

#include <QWidget>

class QueryWidget : public QWidget
{
	Q_OBJECT

public:
	QueryWidget(QWidget *parent = Q_NULLPTR);
	~QueryWidget();
	void setExpert(bool expertMode) { this->expert = expertMode; }
	bool isExpert() { return this->expert; }

protected:
	bool expert;

public slots:
	virtual void handleResponse(QString initialQuery, QString response) = 0;

signals:
	void query(QueryWidget*, QString);
	void error(QString);
	void info(QString);
};
