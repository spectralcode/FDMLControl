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
#include <QtWidgets/QApplication>

void style(){
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette dark;
    dark.setColor(QPalette::Text, QColor(255, 255, 255));
    dark.setColor(QPalette::WindowText, QColor(255, 255, 255));
    dark.setColor(QPalette::Window, QColor(50, 50, 50));
    dark.setColor(QPalette::Button,QColor(50, 50, 50));
    dark.setColor(QPalette::Base, QColor(25, 25, 25));
    dark.setColor(QPalette::AlternateBase, QColor(50, 50, 50));
    dark.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    dark.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
    dark.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    dark.setColor(QPalette::BrightText, QColor(255, 0, 0));
    dark.setColor(QPalette::Link, QColor(40, 130, 220));

    dark.setColor(QPalette::Disabled,QPalette::Text,QColor(99,99,99));
    dark.setColor(QPalette::Disabled,QPalette::WindowText,QColor(99,99,99));;
    dark.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(99,99,99));
    dark.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    dark.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(99,99,99));

    qApp->setPalette(dark);
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FDMLControl w;
    style();
	w.show();
	return a.exec();
}
