#/***************************************************************************
#**                                                                        **
#**  This file is part of FDMLControl.                                     **
#**  FDMLControl enables the control of the Optores FDML laser via a       **
#**  serial connection.                                                    **
#**  Copyright (C) 2019 Miroslav Zabic                                     **
#**                                                                        **
#**  FDMLControl is free software: you can redistribute it and/or modify   **
#**  it under the terms of the GNU General Public License as published by  **
#**  the Free Software Foundation, either version 3 of the License, or     **
#**  (at your option) any later version.                                   **
#**                                                                        **
#**  This program is distributed in the hope that it will be useful,       **
#**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
#**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
#**  GNU General Public License for more details.                          **
#**                                                                        **
#**  You should have received a copy of the GNU General Public License     **
#**  along with this program. If not, see http://www.gnu.org/licenses/.    **
#**                                                                        **
#****************************************************************************
#**          Author:     Miroslav Zabic                                    **
#**          Contact:    zabic                                             **
#**                      at                                                **
#**                      iqo.uni-hannover.de                               **
#**                                                                        **
#**          Date:       12 June 2019                                      **
#**          Version:    1.0.0                                             **
#****************************************************************************/

TEMPLATE = app
TARGET = FDMLControl
QT += core gui widgets serialport xml printsupport

CONFIG(debug, debug|release) {
    # Debug mode
    DESTDIR = ../x64/Debug
}
else {
    # Release mode
   DESTDIR = ../x64/Release
}

HEADERS += ./fdmlcontrol.h \
    ./QCustomPlot/qcustomplot.h \
    authenticationwidget.h \
    buttonquerywidget.h \
    comboboxquerywidget.h \
    comsettingswidget.h \
    consolewidget.h \
    intvaluewidget.h \
    messageconsole.h \
    plotwidget.h \
    querymanager.h \
    querywidget.h \
    querywidgetmanager.h

SOURCES += ./fdmlcontrol.cpp \
    ./QCustomPlot/qcustomplot.cpp \
    authenticationwidget.cpp \
    buttonquerywidget.cpp \
    comboboxquerywidget.cpp \
    comsettingswidget.cpp \
    consolewidget.cpp \
    intvaluewidget.cpp \
    main.cpp \
    messageconsole.cpp \
    plotwidget.cpp \
    querymanager.cpp \
    querywidget.cpp \
    querywidgetmanager.cpp

FORMS += ./ComSettingsWidget.ui \
    ./ConsoleWidget.ui \
    ./fdmlcontrol.ui

RESOURCES += fdmlcontrol.qrc

RC_ICONS = FDMLControl_icon.ico
