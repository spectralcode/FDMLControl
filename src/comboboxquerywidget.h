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

#include <QComboBox>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include "querywidget.h"
#include "eventguard.h"

class ComboBoxQueryWidget : public QueryWidget
{
	Q_OBJECT

public:
	ComboBoxQueryWidget(QWidget *parent = nullptr);
	ComboBoxQueryWidget(QString name, QStringList options, bool expert, QString infoText, QWidget *parent = nullptr);
	~ComboBoxQueryWidget();	
    QString content() override;


private:
	QComboBox* comboBox;
	QLabel* label;
	QHBoxLayout* layout;
	QStringList options;
	bool enableDisableComboBox;


public slots:
	void handleResponse(QString initialQuery, QString response) override;
	void changeValue(int value);
	void queryCurrentValue();


signals:

};
