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

#include <QtWidgets/QMainWindow>
#include "ui_fdmlcontrol.h"
#include "querywidgetmanager.h"
#include "consolewidget.h"
#include "comsettingswidget.h"
#include "querymanager.h"
#include <QSerialPort>
#include <QMessageBox>
#include <QThread>
#include "plotwidget.h"
#include <QStringLiteral>
#include "messageconsole.h"
#include "authenticationwidget.h"

#define COMMAND_FILE_PATH QCoreApplication::applicationDirPath() + "/fdml_commands.xml"
#define KEY_FILE_PATH QCoreApplication::applicationDirPath() + "/key.dat"

class FDMLControl : public QMainWindow
{
	Q_OBJECT
	QThread comThread;

public:
	FDMLControl(QWidget *parent = Q_NULLPTR);
	~FDMLControl();

private:
	Ui::FDMLControlClass ui;
	ConsoleWidget* console;
	ComSettingsWidget* comSettings;
	QueryManager* queryManager;
	QSerialPort* serial;
	QueryWidgetManager* widgetManager;
	PlotWidget* plot;
	MessageConsole* logConsole;
	AuthenticationWidget* authenticator; 

	QAction* expertViewAction;
	QAction* logViewAction;
	QAction* readOutAction;

    QWidget* acqFrame;
    QWidget* galvoFrame;
    QWidget* rgpolFrame;
    QWidget* filterFrame;
    QWidget* boosterFrame;
    QWidget* bncFrame;
    QWidget* otherFrame;

    void initQueryWidgets();
    void sortQueryWidgetIntoGui(QueryWidget* widget, QVBoxLayout* acquisitionLayout, QVBoxLayout* galvoLayout, QVBoxLayout* rgpolLayout, QVBoxLayout* filterLayout, QVBoxLayout* boosterLayout, QVBoxLayout* bncLayout, QVBoxLayout* otherLayout);
    void initGui();
    void initMenu();
    void enableGroupBoxes(bool enable);
    QVBoxLayout* generateLayout(QString name);


public slots:
	void slot_openSerialPort();
	void slot_closeSerialPort();
	void enableGui(bool enable);
	void showAbout();
	void toggleExpertView();
	void toggleLogView();
	void readOutDevice();

signals:
	void serialResponse(QString);
	void openSerialPort(ComSettings);
	void closeSerialPort();
};
