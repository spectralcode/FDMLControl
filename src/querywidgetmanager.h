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

#pragma once

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QtXml/qxml.h>
#include <QtXml/qdom.h>
#include "intvaluewidget.h"
#include "buttonquerywidget.h"
#include "comboboxquerywidget.h"

class QueryWidgetManager : public QWidget
{
	Q_OBJECT

public:
	QueryWidgetManager(QWidget* parent = nullptr);
	~QueryWidgetManager();

	void createWidgetsFromXmlFile(QString filePath);
	void queryCurrentValues();
	void setExpertWidgetsVisible(bool visible);
	QList<IntValueWidget*> getIntValueWidgets() { return this->intValueWidgets; }
	QList<ButtonQueryWidget*> getButtonWidgets() { return this->buttonWidgets; }
	QList<ComboBoxQueryWidget*> getComboBoxWidgets() { return this->comboBoxWidgets; }


private:
	void queryCurrentIntValues();
	void queryCurrentComboBoxValues();
	void optainIntValueWidgets(QDomElement root, QString tag);
	void optainButtonWidgets(QDomElement root, QString tag);
	void optainComboBoxWidgets(QDomElement root, QString tag);

	QList<IntValueWidget*> intValueWidgets;
	QList<ButtonQueryWidget*> buttonWidgets;
	QList<ComboBoxQueryWidget*> comboBoxWidgets;

public slots:

signals:
	void error(QString);
	void info(QString);

};
