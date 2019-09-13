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
    connect(this, &FDMLControl::info, this->logConsole, &MessageConsole::slot_displayInfo);
    connect(this, &FDMLControl::error, this->logConsole, &MessageConsole::slot_displayError);
	connect(this->queryManager, &QueryManager::serialOpen, this, &FDMLControl::enableGui);
	connect(this->queryManager, &QueryManager::info, this->logConsole, &MessageConsole::slot_displayInfo);
	connect(this->queryManager, &QueryManager::error, this->logConsole, &MessageConsole::slot_displayError);
    connect(this->widgetManager, &QueryWidgetManager::info, this->logConsole, &MessageConsole::slot_displayInfo);
    connect(this->widgetManager, &QueryWidgetManager::error, this->logConsole, &MessageConsole::slot_displayError);
	connect(&comThread, &QThread::finished, queryManager, &QueryManager::deleteLater);
	connect(&comThread, &QThread::finished, &comThread, &QThread::deleteLater);
	comThread.start();

	this->initGui();
}

FDMLControl::~FDMLControl() {
    this->queryManager->abort = true;
	emit this->closeSerialPort();
	QApplication::processEvents();
	comThread.quit();
	comThread.wait();
	delete this->widgetManager;
	delete this->authenticator;
}

void FDMLControl::initQueryWidgets(){
    //load gui elements from xml file
    QDir filePath(COMMAND_FILE_PATH);
	this->widgetManager->createWidgetsFromXmlFile(filePath.absolutePath());
	QList<IntValueWidget*> intValueWidgets = this->widgetManager->getIntValueWidgets();
	QList<ButtonQueryWidget*> buttonWidgets = this->widgetManager->getButtonWidgets();
    QList<ComboBoxQueryWidget*> comboBoxWidgets = this->widgetManager->getComboBoxWidgets();

    //init button area
	auto buttonLayout = new QHBoxLayout(this);
    buttonLayout->setContentsMargins(1, 3, 1, 3);
    buttonLayout->setSpacing(1);
	this->ui.groupBox_commands->setLayout(buttonLayout);
    this->ui.groupBox_commands->setFixedHeight(55);
	for (int i = 0; i < buttonWidgets.size(); i++) {
		buttonLayout->addWidget(buttonWidgets.at(i));
		connect(buttonWidgets.at(i), &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery, Qt::QueuedConnection);
	}
    buttonLayout->addStretch();

    //init scroll area
    auto scrollAreaLayout = new QVBoxLayout(this->ui.scrollAreaWidgetContents);
    scrollAreaLayout->setSpacing(6);
    scrollAreaLayout->setContentsMargins(6, 0, 6, 0);

    auto acquisitionLayout = this->generateLayout(tr("Acquisition"));
    auto galvoLayout = this->generateLayout(tr("Galvo Waveforms"));
    auto rgpolLayout = this->generateLayout(tr("Rg Pol"));
    auto filterLayout = this->generateLayout(tr("Filter Control"));
    auto boosterLayout = this->generateLayout(tr("Booster SOA"));
    auto bncLayout = this->generateLayout(tr("BNC Configuration"));
    auto otherLayout = this->generateLayout(tr("Other"));

    for (int i = 0; i < intValueWidgets.size(); i++) {
     this->sortQueryWidgetIntoGui(intValueWidgets.at(i), acquisitionLayout, galvoLayout, rgpolLayout, filterLayout, boosterLayout, bncLayout,otherLayout);
    }

    for (int i = 0; i < comboBoxWidgets.size(); i++) {
        this->sortQueryWidgetIntoGui(comboBoxWidgets.at(i), acquisitionLayout, galvoLayout, rgpolLayout, filterLayout, boosterLayout, bncLayout,otherLayout);
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

    //init plot, log and authenticator
	connect(this->plot, &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
	connect(this->plot, &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
    connect(this->plot, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
    connect(this->console, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
    connect(this->authenticator, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery);
    connect(this->authenticator, &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
    connect(this->authenticator, &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
}

void FDMLControl::sortQueryWidgetIntoGui(QueryWidget* widget, QVBoxLayout* acquisitionLayout, QVBoxLayout* galvoLayout, QVBoxLayout* rgpolLayout, QVBoxLayout* filterLayout, QVBoxLayout* boosterLayout, QVBoxLayout* bncLayout, QVBoxLayout* otherLayout){
    QString currName = widget->getName();
    if(currName.contains("acq.") || currName.contains("scpll.")){
        acquisitionLayout->addWidget(widget);
    }
    else if (currName.contains("xyout.")) {
        galvoLayout->addWidget(widget);
    }
    else if (currName.contains("rg.")) {
        rgpolLayout->addWidget(widget);
    }
    else if (currName.contains("flt.")) {
        filterLayout->addWidget(widget);
    }
    else if (currName.contains("ldc.")) {
        boosterLayout->addWidget(widget);
    }
    else if (currName.contains("bnc.")) {
        bncLayout->addWidget(widget);
    }
    else {
        otherLayout->addWidget(widget);
    }
    connect(widget, &QueryWidget::query, this->queryManager, &QueryManager::receiveQuery, Qt::QueuedConnection);
    connect(widget, &QueryWidget::info, this->logConsole, &MessageConsole::slot_displayInfo);
    connect(widget, &QueryWidget::error, this->logConsole, &MessageConsole::slot_displayError);
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
    QAction* saveToFileAction = fileMenu->addAction(tr("&Save Parameters"), this->widgetManager, &QueryWidgetManager::saveWidgetValuesToFile);
    saveToFileAction->setStatusTip(tr("Save parameter values that are visible in right area of FDMLControl"));
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
    this->authenticateAction = deviceMenu->addAction(tr("&Authenticate As Admin"), this, &FDMLControl::authenticateAsAdmin);
    this->authenticateAction->setStatusTip(tr("Log in as administrator to edit expert parameters."));


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
    this->queryManager->abort = false;
	emit this->openSerialPort(this->comSettings->settings());
}

void FDMLControl::slot_closeSerialPort(){
    this->queryManager->abort = true;
	this->plot->setAutoUpdate(false);
	emit closeSerialPort();
}

void FDMLControl::enableGui(bool enable){
	this->enableGroupBoxes(enable);
	this->plot->setAutoUpdate(enable);
	this->readOutAction->setEnabled(enable);
    this->authenticateAction->setEnabled(enable);

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
           "Contact:    zabic"
           "@"
           "iqo.uni-hannover.de<br>"
           "Date:       ")+ VERSION_DATE + tr("<br>"
           "Version:    ") + VERSION);
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

void FDMLControl::authenticateAsAdmin(){
    if(this->authenticator->isKeyAvailable()){
        this->authenticator->authenticate();
    }else{
        emit error(tr("Administrator authentication not possible, key file is missing."));
    }
}
