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

#include "querywidgetmanager.h"

QueryWidgetManager::QueryWidgetManager(QWidget *parent)
	: QWidget(parent)
{

}

QueryWidgetManager::~QueryWidgetManager()
{
}

void QueryWidgetManager::createWidgetsFromXmlFile(QString filePath) {
	QDomDocument document;
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
		if (!document.setContent(&file)){
            emit error(tr("XML file parse error. Could not load elements from XML file."));
		}
		file.close();
	}

	QDomElement root = document.firstChildElement();
	this->optainIntValueWidgets(root, "IntValueElement");
	this->optainButtonWidgets(root, "Button");
	this->optainComboBoxWidgets(root, "ComboBox");
}

void QueryWidgetManager::queryCurrentValues(){
	this->queryCurrentIntValues();
	this->queryCurrentComboBoxValues();
}

void QueryWidgetManager::queryCurrentIntValues() {
    foreach(auto widget, this->intValueWidgets) {
		widget->queryCurrentValue();
	}
	QCoreApplication::processEvents();
}

void QueryWidgetManager::queryCurrentComboBoxValues() {
	foreach(auto widget, this->comboBoxWidgets) {
		widget->queryCurrentValue();
	}
	QCoreApplication::processEvents();
}

void QueryWidgetManager::setExpertWidgetsVisible(bool visible) {
	if (this->intValueWidgets.size() > 0) {
        foreach (auto widget, this->intValueWidgets){
			if (widget->isExpert()) {
				widget->setVisible(visible);
			}
		}
	}

	if (this->buttonWidgets.size() > 0) {
		foreach(auto widget, this->buttonWidgets) {
			if (widget->isExpert()) {
				widget->setVisible(visible);
			}
		}
	}

	if (this->comboBoxWidgets.size() > 0) {
		foreach(auto widget, this->comboBoxWidgets) {
			if (widget->isExpert()) {
				widget->setVisible(visible);
			}
		}
    }
}

void QueryWidgetManager::saveWidgetValuesToFile() {
    QString filters("Text files (*.txt);;All files (*.*)");
    QString defaultFilter("Text files (*.txt)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save FDML Laser Parameter Values"), QDir::currentPath(), filters, &defaultFilter);
    if(fileName == ""){
        emit error(tr("Save parameters to disk canceled."));
        return;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hh-mm-ss-zzz");

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(tr("Unable to write file! ") + file.errorString());
        return;
    }
    QTextStream out(&file);
    out << tr("FDMLControl Parameters") + "\n" + tr("Time stamp: ") + timestamp + "\n\n";
    out << tr("Integer Parameters: \n");
    foreach (auto widget, this->intValueWidgets){
        out << widget->content() + "\n";
    }
    out << tr("\nSelection Parameters: \n");
    foreach (auto widget, this->comboBoxWidgets){
        out << widget->content() + "\n";
    }
    file.close();
    emit info(tr("File saved to: ") + fileName);
}

void QueryWidgetManager::optainIntValueWidgets(QDomElement root, QString tag) {
	QDomNodeList nodes = root.elementsByTagName(tag);
	for (int i = 0; i < nodes.count(); i++) {
		QDomNode node = nodes.at(i);
		if (node.isElement()) {
			QDomElement element = node.toElement();
			int minVal = (element.attribute("minValue")).toInt();
			int maxVal = (element.attribute("maxValue")).toInt();
			int defaultVal = (element.attribute("defaultValue")).toInt();
			int stepSize = (element.attribute("stepSize")).toInt();
			bool expert = (element.attribute("expertMode")).toInt();
			QString infoText = element.attribute("infoText");
			QString name = element.attribute("command");

			this->intValueWidgets.append(new IntValueWidget(minVal, maxVal, defaultVal, stepSize, expert, infoText, this, name));
		}
	}
}

void QueryWidgetManager::optainButtonWidgets(QDomElement root, QString tag) {
	QDomNodeList nodes = root.elementsByTagName(tag);
	for (int i = 0; i < nodes.count(); i++) {
		QDomNode node = nodes.at(i);
		if (node.isElement()) {
			QDomElement element = node.toElement();
			QString name = (element.attribute("name"));
			QString command = (element.attribute("command"));
			bool expert = (element.attribute("expertMode")).toInt();
			QString infoText = element.attribute("infoText");

			this->buttonWidgets.append(new ButtonQueryWidget(name, command, expert, infoText, this));
		}
	}
}

void QueryWidgetManager::optainComboBoxWidgets(QDomElement root, QString tag) {
	QDomNodeList nodes = root.elementsByTagName(tag);
	for (int i = 0; i < nodes.count(); i++) {
		QDomNode node = nodes.at(i);
		if (node.isElement()) {
			QDomElement element = node.toElement();
			QString name = (element.attribute("name"));
			QStringList options = (element.attribute("options")).split(",");;
			bool expert = (element.attribute("expertMode")).toInt();
			QString infoText = element.attribute("infoText");

			this->comboBoxWidgets.append(new ComboBoxQueryWidget(name, options, expert, infoText));
		}
    }
}
