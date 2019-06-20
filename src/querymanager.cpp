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

#include "querymanager.h"

QueryManager::QueryManager(){
	this->serial = new QSerialPort(this);
	this->sendLog = true;
    this->abort = false;
}

QueryManager::~QueryManager()
{
}

void QueryManager::receiveQuery(QueryWidget* widget, QString query) {
    if(abort){return;}
	//Convert query string to QByteArray and send query to serial device
	QByteArray carriageReturn = "\r";
	QByteArray command = query.toLocal8Bit() + carriageReturn;
	int bytesWritten = this->serial->write(command);
	if (bytesWritten == -1) {
		emit error(tr("Serial write error! Query: ") + query);
		return;
	}
	if (this->serial->waitForBytesWritten(1000)) {
        //Read serial response until end of response was received ("\n" or "\r") or until timeout (1000 ms) occured
		QByteArray response = this->serial->readAll();
		QElapsedTimer timer;
		timer.start();
        while (!timer.hasExpired(2000) && (!response.contains("\r\r\n"))) {
            serial->waitForReadyRead(20);
			response += this->serial->readAll();
		}
		//Convert serial response to string and remove all carriage returns and new lines
		QString responseString = QString::fromLocal8Bit(response);
        //responseString.remove(QRegExp("[\\r\\n]"));

		//Forward serial response to inquiring object/widget
		QMetaObject::invokeMethod(widget, "handleResponse", Qt::QueuedConnection, Q_ARG(QString, query), Q_ARG(QString, responseString));
        if (sendLog) { 
			QString logString = query + "> " + responseString;
			logString.replace(QRegExp("[\\r\\n]"), " ");
			emit this->info(logString);
		}
	}
	else {
		emit error(tr("Serial write timeout! Query: ") + query);
	}
 }

void QueryManager::openSerialPort(ComSettings settings){
	const ComSettings p = settings;
	this->serial->setPortName(p.name);
	this->serial->setBaudRate(p.baudRate);
	this->serial->setDataBits(p.dataBits);
	this->serial->setParity(p.parity);
	this->serial->setStopBits(p.stopBits);
	this->serial->setFlowControl(p.flowControl);
	if (this->serial->open(QIODevice::ReadWrite)) {
		QThread::sleep(2); //some serial devices require additional time (2 seconds) to fully establish a connection
		emit this->serialOpen(true);
	}
	else {
		if (this->serial->isOpen()) {
            emit error(tr("Serial port already opened!"));
		}
		else {
			emit error(tr("Could not open serial port!"));
		}
	}
}

void QueryManager::closeSerialPort(){
	if (this->serial->isOpen()) {
		this->serial->close();
		emit this->serialOpen(false);
	}
}
