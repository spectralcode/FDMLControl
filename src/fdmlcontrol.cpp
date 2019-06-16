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

#include "fdmlcontrol.h"
#include <QDir>

FDMLControl::FDMLControl(QWidget *parent) : QMainWindow(parent){
	//qRegisterMetaType is needed to enabel Qt::QueuedConnection for signal slot communication with "ComSettings"
	qRegisterMetaType<ComSettings >("ComSettings");

	ui.setupUi(this);

    this->acqFrame = nullptr;
    this->galvoFrame = nullptr;
    this->rgpolFrame = nullptr;
    this->filterFrame = nullptr;
    this->boosterFrame = nullptr;
    this->bncFrame = nullptr;
    this->otherFrame = nullptr;

	this->console = new ConsoleWidget(this);
	this->comSettings = new ComSettingsWidget(this);
	this->widgetManager = new QueryWidgetManager();
	this->plot = ui.widget;
	this->logConsole = new MessageConsole(this);
	this->authenticator = new AuthenticationWidget(KEY_FILE_PATH);
	
	this->queryManager = new QueryManager();
	this->queryManager->moveToThread(&comThread);
	connect(this, &FDMLControl::openSerialPort, this->queryManager, &QueryManager::openSerialPort);
	connect(this, &FDMLControl::closeSerialPort, this->queryManager, &QueryManager::closeSerialPort);
	connect(this->queryManager, &QueryManager::serialOpen, this, &FDMLControl::enableGui);
	connect(this->console, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
	connect(this->plot, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
	connect(this->queryManager, &QueryManager::info, this->logConsole, &MessageConsole::slot_displayInfo);
	connect(this->queryManager, &QueryManager::error, this->logConsole, &MessageConsole::slot_displayError);
	connect(this->authenticator, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
	connect(this->authenticator, &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
	connect(this->authenticator, &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
	connect(&comThread, &QThread::finished, queryManager, &QueryManager::deleteLater);
	connect(&comThread, &QThread::finished, &comThread, &QThread::deleteLater);
	comThread.start();

	this->initGui();
}

FDMLControl::~FDMLControl() {
	emit this->closeSerialPort();
	QApplication::processEvents();
	comThread.quit();
	comThread.wait();
	delete this->widgetManager;
	delete this->authenticator;
}

void FDMLControl::initQueryWidgets(){
	QDir filePath(COMMAND_FILE_PATH);
	this->widgetManager->createWidgetsFromXmlFile(filePath.absolutePath());
	QList<IntValueWidget*> intValueWidgets = this->widgetManager->getIntValueWidgets();
	QList<ButtonQueryWidget*> buttonWidgets = this->widgetManager->getButtonWidgets();
	auto comboBoxWidgets = this->widgetManager->getComboBoxWidgets();

	auto buttonLayout = new QHBoxLayout(this);
	buttonLayout->setContentsMargins(1, 3, 1, 3);
	buttonLayout->setSpacing(0);
	this->ui.groupBox_commands->setLayout(buttonLayout);
    this->ui.groupBox_commands->setFixedHeight(55);
	buttonLayout->addSpacing(10);
	for (int i = 0; i < buttonWidgets.size(); i++) {
		buttonLayout->addWidget(buttonWidgets.at(i));
		connect(buttonWidgets.at(i), &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery, Qt::QueuedConnection);
	}

	//Init IntVlaueWidgets and scroll area
    auto scrollAreaLayout = new QVBoxLayout(this->ui.scrollAreaWidgetContents);
    scrollAreaLayout->setSpacing(6);
    scrollAreaLayout->setContentsMargins(6, 0, 6, 0);

	auto acquisitionLayout = this->generateLayout("Acquisition");
	auto galvoLayout = this->generateLayout("Glavo Waveforms");
	auto rgpolLayout = this->generateLayout("Rg Pol");
	auto filterLayout = this->generateLayout("Filter Control");
	auto boosterLayout = this->generateLayout("BoosterSOA");
	auto bncLayout = this->generateLayout("BNC Configuration");
	auto otherLayout = this->generateLayout("Other");

	for (int i = 0; i < intValueWidgets.size(); i++) {
		QString currName = intValueWidgets.at(i)->getName();
		if(currName.contains("acq.") || currName.contains("scpll.")){
			acquisitionLayout->addWidget(intValueWidgets.at(i));
		}
		else if (currName.contains("xyout.")) {
			galvoLayout->addWidget(intValueWidgets.at(i));
		}
		else if (currName.contains("rg.")) {
			rgpolLayout->addWidget(intValueWidgets.at(i));
		}
		else if (currName.contains("flt.")) {
			filterLayout->addWidget(intValueWidgets.at(i));
		}
		else if (currName.contains("ldc.")) {
			boosterLayout->addWidget(intValueWidgets.at(i));
		}
		else if (currName.contains("bnc.")) {
			bncLayout->addWidget(intValueWidgets.at(i));
		}
		else {
			otherLayout->addWidget(intValueWidgets.at(i));
		}
		connect(intValueWidgets.at(i), &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery, Qt::QueuedConnection);
		connect(intValueWidgets.at(i), &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
		connect(intValueWidgets.at(i), &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
	}

	//Init ComboBoxQueryWidgets
	for (int i = 0; i < comboBoxWidgets.size(); i++) {
		QString currName = comboBoxWidgets.at(i)->getName();
		if (currName.contains("acq.") || currName.contains("scpll.")) {
			acquisitionLayout->addWidget(comboBoxWidgets.at(i));
		}
		else if (currName.contains("xyout.")) {
			galvoLayout->addWidget(comboBoxWidgets.at(i));
		}
		else if (currName.contains("rg.")) {
			rgpolLayout->addWidget(comboBoxWidgets.at(i));
		}
		else if (currName.contains("flt.")) {
			rgpolLayout->addWidget(comboBoxWidgets.at(i));
		}
		else if (currName.contains("ldc.")) {
			boosterLayout->addWidget(comboBoxWidgets.at(i));
		}
		else if (currName.contains("bnc.")) {
			bncLayout->addWidget(comboBoxWidgets.at(i));
		}
		else {
			otherLayout->addWidget(comboBoxWidgets.at(i));
		}
		connect(comboBoxWidgets.at(i), &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery, Qt::QueuedConnection);
		connect(comboBoxWidgets.at(i), &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
		connect(comboBoxWidgets.at(i), &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
	}

    this->acqFrame = acquisitionLayout->parentWidget();
    this->galvoFrame = galvoLayout->parentWidget();
    this->rgpolFrame = rgpolLayout->parentWidget();
    this->filterFrame = filterLayout->parentWidget();
    this->boosterFrame = boosterLayout->parentWidget();
    this->bncFrame = bncLayout->parentWidget();
    this->otherFrame = otherLayout->parentWidget();

	scrollAreaLayout->addWidget(acqFrame);
	scrollAreaLayout->addWidget(galvoFrame);
	scrollAreaLayout->addWidget(rgpolFrame);
	scrollAreaLayout->addWidget(filterFrame);
	scrollAreaLayout->addWidget(boosterFrame);
	scrollAreaLayout->addWidget(bncFrame);
	scrollAreaLayout->addWidget(otherFrame);

	connect(this->plot, &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
	connect(this->plot, &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);

}

void FDMLControl::initGui(){
	this->initQueryWidgets();
	this->initMenu();

	this->ui.groupBox_log->layout()->addWidget(this->logConsole);
	this->ui.groupBox_customCommand->layout()->addWidget(this->console);
	this->ui.groupBox_com->layout()->addWidget(this->comSettings);

	connect(this->comSettings, &ComSettingsWidget::connectCom, this, &FDMLControl::slot_openSerialPort);
	connect(this->comSettings, &ComSettingsWidget::disconnectCom, this, &FDMLControl::slot_closeSerialPort);

	this->enableGui(false);
}

void FDMLControl::initMenu(){
    QMenu *fileMenu = this->menuBar()->addMenu(tr("&File"));
	QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Close FDML Control"));

	QMenu* viewMenu = this->menuBar()->addMenu(tr("&View"));
	this->expertViewAction = viewMenu->addAction(tr("&Expert"), this, &FDMLControl::toggleExpertView);
	this->expertViewAction->setCheckable(true);
    this->expertViewAction->setChecked(false);
	this->toggleExpertView();
	this->expertViewAction->setStatusTip(tr("Make expert settings visible."));

	this->logViewAction = viewMenu->addAction(tr("&Log Console"), this, &FDMLControl::toggleLogView);
	this->logViewAction->setCheckable(true);
	this->logViewAction->setChecked(true);
	this->toggleLogView();
	this->logViewAction->setStatusTip(tr("Enable/Disable status and serial communication log console."));

	QMenu* deviceMenu = this->menuBar()->addMenu(tr("&Device"));
	this->readOutAction = deviceMenu->addAction(tr("&Read From Device"), this, &FDMLControl::readOutDevice);
	this->readOutAction->setStatusTip(tr("Read out current FDML Laser parameters and update graphical user interface."));

	QMenu* helpMenu = this->menuBar()->addMenu(tr("&Help"));
	QAction* aboutAction = helpMenu->addAction(tr("&About"), this, &FDMLControl::showAbout);
    aboutAction->setStatusTip(tr("Information about this application."));
}

void FDMLControl::enableGroupBoxes(bool enable){
	this->ui.groupBox_customCommand->setEnabled(enable);
	this->ui.groupBox_parameter->setEnabled(enable);
	this->ui.groupBox_laserIntensity->setEnabled(enable);
	this->ui.groupBox_commands->setEnabled(enable);
}

QVBoxLayout * FDMLControl::generateLayout(QString name){
    auto frame = new QFrame(this->ui.scrollAreaWidgetContents);

    QPalette pal;
    pal.setColor(QPalette::Background, QColor(32,32,32));
    pal.setColor(QPalette::Disabled, QPalette::Background, QColor(82,82,82));
    frame->setAutoFillBackground(true);
    frame->setPalette(pal);
    frame->show();

    frame->setFrameShape(QFrame::StyledPanel);

	auto layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
	layout->setContentsMargins(6, 0, 6, 0);
	
	QFont font;
	font.setBold(true);
    auto label = new QLabel(frame);

	label->setFont(font);
	label->setText(name + ": ");

	layout->addWidget(label);
	return layout;
}

void FDMLControl::slot_openSerialPort(){
	emit this->openSerialPort(this->comSettings->settings());
}

void FDMLControl::slot_closeSerialPort(){
	this->plot->setAutoUpdate(false);
	emit closeSerialPort();
}

void FDMLControl::enableGui(bool enable){
	this->enableGroupBoxes(enable);
	this->plot->setAutoUpdate(enable);
	this->readOutAction->setEnabled(enable);

	if (enable) {
		this->authenticator->authenticate();
		this->widgetManager->queryCurrentValues();	
	}
}

void FDMLControl::showAbout() {
	QMessageBox::about(this, tr("About FDML Control"),
        tr("<b>FDMLControl</b> enables the control of the Optores FDML laser via a <br>"
          "serial connection.<br><br>"
          "<small>FDMLControl is free software: you can redistribute it and/or modify<br>"
          "it under the terms of the GNU General Public License as published by<br>"
          "the Free Software Foundation, either version 3 of the License, or<br>"
          "(at your option) any later version.<br>"
          "This program is distributed in the hope that it will be useful,<br>"
          "but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br>"
          "GNU General Public License for more details.<br>"
          "You should have received a copy of the GNU General Public License<br>"
          "along with this program. If not, see http://www.gnu.org/licenses/.</small><br><br>"
           "Author:     Miroslav Zabic<br>"
           "Contact:    zabic@iqo.uni-hannover.de<br>"
           "Date:       12 June 2019<br>"
           "Version:    1.0.0"));
}

void FDMLControl::toggleExpertView() {
    bool visibleChecked = this->expertViewAction->isChecked();
    this->widgetManager->setExpertWidgetsVisible(visibleChecked);
    this->ui.groupBox_customCommand->setVisible(visibleChecked);

    if(this->rgpolFrame != nullptr){this->rgpolFrame->setVisible(visibleChecked);} // todo: Check all frames to see if the contained elements are expert elements before changing the visibility of the frame
    if(this->filterFrame != nullptr){this->filterFrame->setVisible(visibleChecked);} // todo: Check all frames to see if the contained elements are expert elements before changing the visibility of the frame

}

void FDMLControl::toggleLogView() {
	this->ui.groupBox_log->setVisible(this->logViewAction->isChecked());
}

void FDMLControl::readOutDevice(){
	this->widgetManager->queryCurrentValues();
}
