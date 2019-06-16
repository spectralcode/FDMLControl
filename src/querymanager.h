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

#include <QObject>
#include <QSerialPort>
#include <QElapsedTimer>
#include <QThread>
#include "querywidget.h"
#include "comsettingswidget.h"

class QueryManager : public QObject
{
	Q_OBJECT

public:
	QueryManager();
	~QueryManager();

private:
	QSerialPort* serial;
	QTimer* timeoutTimer;
	bool sendLog;


public slots:
	void receiveQuery(QueryWidget* widget, QString query);
	void openSerialPort(ComSettings settings);
	void closeSerialPort();

signals:
	void serialOpen(bool);
	void log(QString, QString);
	void error(QString);
	void info(QString);
};
