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

#include "messageconsole.h"

MessageConsole::MessageConsole(QWidget *parent) : QWidget(parent){
    this->gridLayout = new QGridLayout(this);
    this->textEdit = new QTextEdit(this);
    this->textEdit->setReadOnly(true);
    this->gridLayout->setMargin(0);
    this->gridLayout->addWidget(this->textEdit, 0, 0, 1, 1);
    this->setMinimumWidth(320);
    this->setMinimumHeight(50);
    this->messages = QVector<QString>(MAX_MESSAGES);

    for(auto string : this->messages){
        string = "";
    }
    this->messages_index = 0;
}

MessageConsole::~MessageConsole(){

}

QString MessageConsole::addStringToMessageBuffer(QString message){
    this->messages[this->messages_index] = message+"<br>";
    QString messagesString;
    //append all messages to messagesString and make first one bold
    for(int i = messages_index+MAX_MESSAGES; i > messages_index; i--){
        messagesString = messagesString + (i == messages_index+MAX_MESSAGES ? "<b>"+this->messages.at(i%MAX_MESSAGES)+"</b>" : this->messages.at(i%MAX_MESSAGES));
    }
    this->messages_index = (this->messages_index+1) % MAX_MESSAGES;
    return messagesString;
}

void MessageConsole::contextMenuEvent(QContextMenuEvent* event){
    QMenu* menu = this->textEdit->createStandardContextMenu();
    menu->addAction("test");
    menu->exec(event->globalPos());
    delete menu;

}

void MessageConsole::slot_displayInfo(QString info){
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss") + " ";
    QString htmlStart = "<font color=\"#4863A0\">";
    QString htmlEnd = "</font>";
    this->textEdit->setText(addStringToMessageBuffer(currentTime + htmlStart + info + htmlEnd));
}

void MessageConsole::slot_displayError(QString error){
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss") + " ";
    QString htmlStart = "<font color=\"#E41B17\">";
    QString htmlEnd = "</font>";
    this->textEdit->setText(addStringToMessageBuffer(currentTime + htmlStart + "<b>ERROR: </b>" + error + htmlEnd));
}
