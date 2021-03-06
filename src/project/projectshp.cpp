/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  Shapefile project implementations
 * Author:   Mikhail Gusev, gusevmihs@gmail.com
 ******************************************************************************
*   Copyright (C) 2014-2016 NextGIS
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

#include "projects.h"
 

FBProjectShapefile::FBProjectShapefile ():
    FBProjectGdal ()
{
}

FBProjectShapefile::~FBProjectShapefile ()
{
}

FBErr FBProjectShapefile::readFirst (QString anyPath)
{
    if (isInited)
        return FBErrAlreadyInited;

    FBErr err = this->readFromDataset(anyPath);
    if (err != FBErrNone)
        return err;

    version = FBProject::getProgVersionStr();

    strDatasetPath = anyPath; // store the path to dataset for the first save
    strNgfpPath = ""; // need to be saved first time

    isInited = true;
    return FBErrNone;
}


