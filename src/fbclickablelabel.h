/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2017 NextGIS
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#ifndef FBCLICKABLELABEL_H
#define FBCLICKABLELABEL_H

#include <QLabel>
#include <QWidget>


class FbClickableLabel: public QLabel 
{ 
    Q_OBJECT 

    public:
        explicit FbClickableLabel (QWidget* wParent = nullptr);
        ~FbClickableLabel ();

    signals:
        void clicked ();

    protected:
        void mousePressEvent (QMouseEvent* event);

};

#endif //FBCLICKABLELABEL_H
