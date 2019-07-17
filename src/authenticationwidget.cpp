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

#include "authenticationwidget.h"

AuthenticationWidget::AuthenticationWidget(QString keyFilePath, QWidget* parent, QString name) : QueryWidget(parent){
	this->name = name;
	this->keyAvailable = false;
	this->key = "";
	this->loadKeyFromFile(keyFilePath);
}

AuthenticationWidget::~AuthenticationWidget()
{
}

bool AuthenticationWidget::isKeyAvailable(){
    return this->keyAvailable;
}

void AuthenticationWidget::loadKeyFromFile(QString filePath){
	QFile inputFile(filePath);
	if (inputFile.open(QIODevice::ReadOnly)){
		QTextStream in(&inputFile);
		this->key = in.readLine(); //read first line of key file. key should be stored in first line of key file
		inputFile.close();
	}
	this->keyAvailable = this->key.length() > 0 ? true : false;
}

void AuthenticationWidget::authenticate() {
	if(keyAvailable){
		emit query(this, "auth.challenge?");
    }
}

void AuthenticationWidget::handleResponse(QString initialQuery, QString response){
	//check if key was loaded (redundant check, handleResponse will be called after query(..) was emitted. query(..) can just be emitted if key is available)
	if (!keyAvailable) {
		return;
	}
	//check if response is not empty
	if (!(response.size() > 2)) {
		emit error(tr("Authentication error, response too short. Initial query: ") + initialQuery + tr(" Response: ") + response);
		return;
	}
	//extract initial query and actual response part from response
	QString recoveredQuery = response.left((response.indexOf("\n")) - 1);
	QString recoveredResponse = response.right((response.size() - response.indexOf("\n")) - 1);
	recoveredQuery.remove(QRegExp("[\\r\\n]"));
	recoveredResponse.remove(QRegExp("[\\r\\n]"));
	//check if response belongs to initial query as expected
	if (recoveredQuery != initialQuery) { 
		emit error(tr("Fatal Error: incorrect order of elements in command queue. Supposed query: ") + initialQuery + tr(" Actual query: ") + recoveredQuery + tr(" Response to actual query: ") + response);
		return;
	}
	//get three-digit status code and extract value from response if status ok
	QString status = recoveredResponse.left(3); 
	if (status == "200") {
		if (recoveredResponse.contains("DATA")) {
			if (recoveredResponse.contains("challenge")) {
				int dataPos = recoveredResponse.indexOf(">") + 13;
				recoveredResponse.remove(0, dataPos);
				QString challenge = recoveredResponse;
				QByteArray answer = QMessageAuthenticationCode::hash(challenge.toLocal8Bit(), this->key.toLocal8Bit(), QCryptographicHash::Sha256).toHex();
				emit query(this, "auth admin =" + QString(answer));
			}
			else {
				return;
			}
		}
	}
    else if (status == "401"){
		emit error(tr("Invalid command! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
	else {
		emit error(tr("Unexpected response! Initial query: ") + initialQuery + tr(" Response: ") + recoveredResponse);
	}
}
