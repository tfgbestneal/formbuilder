/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  various elements
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

#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <QWidget>

#include "attributes.h"

// Abstract element which can write values to the layer fields.
class FBElemInput: public FBElem
{
    Q_OBJECT
    public:
     FBElemInput ();
     virtual ~FBElemInput () { }
     static void updateFields (QStringList fieldsKeyNames);
     virtual void resetSelectedField (QString keyname);
     virtual QStringList getSelectedFields ();
    protected:
     FBAttrField *attrFieldPtr;
     FBAttrString *attrCaptionPtr;
};

// Abstract element which can write values to the layer fields and these values
// can vary in different sessions of working with form in Mobile application.
class FBElemInputVariate: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemInputVariate ();
     virtual ~FBElemInputVariate () { }
    protected:
     FBAttrBoolean *attrKeepLastPtr;
};

/*
// Abstract element may contain other elements.
class FBElemContainer: protected FBElem
{
    Q_OBJECT
    public:
     FBElemContainer ();
     virtual ~FBElemContainer ();
     virtual void addElem (FBElem* afterElem) = 0; // afterElem can be NULL
     virtual void removeElem (FBElem* elem) = 0;
     virtual void removeAllElems () = 0;
     virtual Json::Value toJson () = 0; // also calls toJson() of inner elems
     virtual bool fromJson (Json::Value jsonValue) = 0; // also calls fromJson() of inner elems
    protected:
     QWidget *wContainer; // can hold any layout in derived classes
     virtual void clearContents(); // override to avoid deleteion of lvContainer
};
*/

class FBElemText: public FBElem
{
    Q_OBJECT
    public:
     FBElemText ();
     virtual ~FBElemText () { }
     virtual QString getKeyName () { return FB_ELEMNAME_TEXT_LABEL; }
     virtual QString getDisplayName () { return tr("Text label"); }
     virtual QString getDescription () { return tr("Label which displays static text"); }
     virtual FBElemtype getType () { return FBDecoration; }
};

class FBElemSpace: public FBElem
{
    Q_OBJECT
    public:
     FBElemSpace ();
     virtual ~FBElemSpace () { }
     virtual QString getKeyName () { return FB_ELEMNAME_SPACE; }
     virtual QString getDisplayName () { return tr("Space"); }
     virtual QString getDescription () { return tr("Void space for making indents"); }
     virtual FBElemtype getType () { return FBDecoration; }
};

class FBElemTextedit: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemTextedit ();
     virtual ~FBElemTextedit () { }
     virtual QString getKeyName () { return FB_ELEMNAME_TEXT_EDIT; }
     virtual QString getDisplayName () { return tr("Text edit"); }
     virtual QString getDescription () { return tr("Element for editing simple text"); }
     virtual FBElemtype getType () { return FBInput; }
    protected slots:
     void onChangeAttrValue (FBAttr *attr); // see FBElemDatetime
    protected:
     FBAttrString *attrTextPtr;
     FBAttrBoolean *attrNgwLogin;
};

class FBElemCombobox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemCombobox (QWidget *appWidget);
     virtual ~FBElemCombobox () { }
     virtual void modifyJsonOut (Json::Value &jsonValue);
     virtual void modifyElemIn (Json::Value jsonValue);
     virtual QString getKeyName () { return FB_ELEMNAME_COMBOBOX; }
     virtual QString getDisplayName () { return tr("Combobox"); }
     virtual QString getDescription () { return tr("Drop-down list with predefined values"); }
     virtual FBElemtype getType () { return FBInput; }
    protected:
     FBAttrListvalues *attrListvalsPtr;
};

class FBElemDoublecombobox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemDoublecombobox (QWidget *appWidget);
     virtual ~FBElemDoublecombobox () { }
     virtual QString getKeyName () { return FB_ELEMNAME_DOUBLE_COMBOBOX; }
     virtual QString getDisplayName () { return tr("Dependent combos"); }
     virtual QString getDescription () { return tr("A pair of comboboxes. The values list"
                                                   " of the slave combobox depends on the values"
                                                   " of the master combobox"); }
     virtual FBElemtype getType () { return FBInput; }
    protected:
     virtual void resetSelectedField (QString keyname);
     virtual QStringList getSelectedFields ();
    protected:
     FBAttrField *attrField1Ptr;
     FBAttrField *attrField2Ptr;
};

class FBElemCheckbox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemCheckbox ();
     virtual ~FBElemCheckbox () { }
     virtual QString getKeyName () { return FB_ELEMNAME_CHECKBOX; }
     virtual QString getDisplayName () { return tr("Checkbox"); }
     virtual QString getDescription () { return tr("Element which allows two values:"
                                                   " true and false"); }
     virtual FBElemtype getType () { return FBInput; }
};

class FBElemRadiogroup: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemRadiogroup (QWidget *appWidget);
     virtual ~FBElemRadiogroup () { }
     virtual QString getKeyName () { return FB_ELEMNAME_RADIOGROUP; }
     virtual QString getDisplayName () { return tr("Radiogroup"); }
     virtual QString getDescription () { return tr("List of predefined values which allows"
                                                   "\nthe selection of only one value"); }
     virtual FBElemtype getType () { return FBInput; }
};

class FBElemDatetime: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemDatetime (QWidget *appWidget);
     virtual ~FBElemDatetime () { }
     virtual QString getKeyName () { return FB_ELEMNAME_DATE_TIME; }
     virtual QString getDisplayName () { return tr("Date & time"); }
     virtual QString getDescription () { return tr("Date, time or date+time picker"); }
     virtual FBElemtype getType () { return FBInput; }
    protected slots:
     void onChangeAttrValue (FBAttr *attr);
    protected:
     // TODO: unite two attributes into one: the value of the date and its type (format),
     // because for now some strange dependancies between these two attributes are used.
     FBAttrSelect *attrTypePtr;
     FBAttrDatetime *attrDatetimePtr;
};

class FBElemButton: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemButton ();
     virtual ~FBElemButton () { }
     virtual QString getKeyName () { return FB_ELEMNAME_BUTTON; }
     virtual QString getDisplayName () { return tr("Button"); }
     virtual QString getDescription () { return tr("Element which writes only one value"
                                                   "\nwhen pressing on it"); }
     virtual FBElemtype getType () { return FBInput; }
};

class FBElemPhoto: public FBElem
{
    Q_OBJECT
    public:
     FBElemPhoto ();
     virtual ~FBElemPhoto () { }
     virtual QString getKeyName () { return FB_ELEMNAME_PHOTO; }
     virtual QString getDisplayName () { return tr("Photo"); }
     virtual QString getDescription () { return tr("Element which allows to make and"
                                                   "\nadd photos with embedded camera"); }
     virtual FBElemtype getType () { return FBSpecific; }
};

class FBElemSignature: public FBElem
{
    Q_OBJECT
    public:
     FBElemSignature ();
     virtual ~FBElemSignature () { }
     virtual QString getKeyName () { return FB_ELEMNAME_SIGNATURE; }
     virtual QString getDisplayName () { return tr("Signature"); }
     virtual QString getDescription () { return tr("Element which allows to put an \"official"
                                                   "\nsignature\""); }
     virtual FBElemtype getType () { return FBSpecific; }
};

class FBElemCounter: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemCounter ();
     virtual ~FBElemCounter () { }
     virtual QString getKeyName () { return FB_ELEMNAME_COUNTER; }
     virtual QString getDisplayName () { return tr("Counter"); }
     virtual QString getDescription () { return tr("Element which incrementally adds values"
                                                   "\nbased on predefined format"); }
     virtual FBElemtype getType () { return FBMetric; }
     void updateAllListValues (QStringList newListNames);
    protected slots:

    protected:
     void updateListValue (FBAttrGlobalselect *attr, QStringList newListNames);
    protected:
     FBAttrString *attrSuffixPtr;
     FBAttrString *attrPrefixPtr;
     FBAttrNumber *attrInitValuePtr;
     FBAttrGlobalselect *attrSuffixFromListPtr;
     FBAttrGlobalselect *attrPrefixFromListPtr;
};

class FBElemCoordinates: public FBElemInput
{
    Q_OBJECT
    public:
     FBElemCoordinates ();
     virtual ~FBElemCoordinates () { }
     virtual QString getKeyName () { return FB_ELEMNAME_COORDINATES; }
     virtual QString getDisplayName () { return tr("Coordinates"); }
     virtual QString getDescription () { return tr("Element which saves coordinates generated"
                                                   "\nby the target device to the selected text"
                                                   "\nfield"); }
     virtual FBElemtype getType () { return FBMetric; }
    protected:
     virtual void resetSelectedField (QString keyname);
     virtual QStringList getSelectedFields ();
    protected:
     FBAttrField *attrFieldLatPtr;
     FBAttrField *attrFieldLongPtr;
};

class FBElemSplitcombobox: public FBElemInputVariate
{
    Q_OBJECT
    public:
     FBElemSplitcombobox (QWidget *appWidget);
     virtual ~FBElemSplitcombobox () { }
     virtual QString getKeyName () { return FB_ELEMNAME_SPLIT_COMBOBOX; }
     virtual QString getDisplayName (){ return tr("Splitted combo"); }
     virtual QString getDescription () { return tr("A pair of linked comboboxes. Each item has an \n"
                                                   "inner value (which will be saved during the data \n"
                                                   "collection) and a pair of displayed values. The \n"
                                                   "collector will be able to see the same item but \n"
                                                   "in different languages"); }
     virtual FBElemtype getType () { return FBInput; }
    protected:
     FBAttrListvalues2 *attrListvals2Ptr;
     FBAttrString *attrLabel1Ptr;
     FBAttrString *attrLabel2Ptr;
};


#endif //ELEMENTS_H



