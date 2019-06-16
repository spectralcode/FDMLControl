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

#ifndef MESSAGECONSOLE_H
#define MESSAGECONSOLE_H

#include <QTextEdit>
#include <QGridLayout>
#include <QDateTime>
#include <QDockWidget>
#include <QWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include <QCoreApplication>

#define MAX_MESSAGES 512


class MessageConsole : public QWidget
{
    Q_OBJECT
public:
    explicit MessageConsole(QWidget *parent = nullptr);
    ~MessageConsole();


private:
    QVector<QString>    messages;
    size_t              messages_index;
    QTextEdit*          textEdit;
    QGridLayout*        gridLayout;
    QPoint              mousePos;


    QString addStringToMessageBuffer(QString message);
    void contextMenuEvent(QContextMenuEvent* event);


signals:
    void error(QString);
    void info(QString);

public slots:
    void slot_displayInfo(QString info);
    void slot_displayError(QString error);
};

#endif // MESSAGECONSOLE_H
