/******************************************************************************
 * Project:  NextGIS Formbuilder
 * Purpose:  main app+gui class
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

#include "fb.h"
#include "ui_fb.h"

#include "elements.h"


FB::~FB()
{
    delete ui;
}

FB::FB(QWidget *parent): QWidget(parent), ui(new Ui::FB)
{
    isInited = false;
    project = NULL;

    this->setObjectName("FB"); // at least for style sheets

    ui->setupUi(this);
}

void FB::initGui ()
{
    if (isInited)
        return;
    isInited = true;

    //----------------------------------------------------------------------
    //                              Working area
    //----------------------------------------------------------------------

    wScreen = new FBScreen(this); // currently with no form
    wScreen->updateStyle();
    wScreen->setDevice(0);
    wScreen->setState(0); // default maximized grey area

    //----------------------------------------------------------------------
    //                              Top menu
    //----------------------------------------------------------------------

    tabMenuTop = new QTabWidget(this);
    tabMenuTop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    tabMenuTop->setMaximumHeight(120);
    tabMenuTop->setMinimumHeight(100);
    tabMenuTop->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));

    wProject = new QWidget();
    tabMenuTop->addTab(wProject, tr(" Project "));
    QHBoxLayout *layProject = new QHBoxLayout(wProject);
    layProject->setContentsMargins(0,4,0,4);
    layProject->setSpacing(2);
    layProject->addStretch();

    wView = new QWidget();
    tabMenuTop->addTab(wView, tr("  View  "));
    lhView = new QHBoxLayout(wView);
    lhView->setContentsMargins(4,4,4,4);
    lhView->addStretch();

    wTools = new QWidget();
    tabMenuTop->addTab(wTools, tr("  Tools  "));
    QHBoxLayout *layTools = new QHBoxLayout(wTools);
    layTools->setContentsMargins(4,4,4,4);
    layTools->setSpacing(2);
    layTools->addStretch();

    wSettings = new QWidget();
    tabMenuTop->addTab(wSettings, tr(" Settings "));
    QHBoxLayout *laySettings = new QHBoxLayout(wSettings);
    laySettings->setContentsMargins(4,4,4,4);
    laySettings->addStretch();

    wAbout = new QWidget();
    tabMenuTop->addTab(wAbout, tr(" About "));
    QHBoxLayout *layAbout = new QHBoxLayout(wAbout);
    layAbout->setContentsMargins(4,4,4,4);
    layAbout->addStretch();

    // Project buttons.
    toolbNewVoid = this->addTopMenuButton(wProject,":/img/new_void.png",
                        tr("New"),tr("New void project"));
    QObject::connect(toolbNewVoid, SIGNAL(clicked()),
                     this, SLOT(onNewVoidClick()));
    toolbNewShape = this->addTopMenuButton(wProject,":/img/new_shp.png",
            tr("New from Shapefile"),tr("New project from \nShapefile"));
    QObject::connect(toolbNewShape, SIGNAL(clicked()),
                     this, SLOT(onNewShapeClick()));
    toolbNewNgw = this->addTopMenuButton(wProject,":/img/new_ngw.png",
      tr("New from NextGIS Web"),tr("New project from \nNextGIS Web connection"));
    QObject::connect(toolbNewNgw, SIGNAL(clicked()),
                     this, SLOT(onNewNgwClick()));
    toolbOpen = this->addTopMenuButton(wProject,":/img/open.png",
                         tr("Open"),tr("Open .ngfp file"));
    QObject::connect(toolbOpen, SIGNAL(clicked()),
                     this, SLOT(onOpenClick()));
    toolbSave = this->addTopMenuButton(wProject,":/img/save.png",
                          tr("Save"),tr("Save to .ngfp file"));
    QObject::connect(toolbSave, SIGNAL(clicked()),
                     this, SLOT(onSaveClick()));
    toolbSaveAs = this->addTopMenuButton(wProject,":/img/save_as.png",
                    tr("Save as"),tr("Save to .ngfp file \nas ..."));
    QObject::connect(toolbSaveAs, SIGNAL(clicked()),
                     this, SLOT(onSaveAsClick()));

    // All view pickers and combos.
    toolbScreenAndroid = this->addTopMenuButton(wView,
               ":/img/android.png", tr("Android"), tr("Android screen"),false);
    QObject::connect(toolbScreenAndroid,SIGNAL(clicked()),
                         this,SLOT(onScreenAndroidPick()));
    toolbScreenIos = this->addTopMenuButton(wView,
               ":/img/ios.png", tr("iOS"), tr("iOS screen"),false);
    QObject::connect(toolbScreenIos,SIGNAL(clicked()),
                         this,SLOT(onScreenIosPick()));
    toolbScreenWeb = this->addTopMenuButton(wView,
               ":/img/web.png", tr("Web"), tr("Web screen"),false);
    QObject::connect(toolbScreenWeb,SIGNAL(clicked()),
                         this,SLOT(onScreenWebPick()));
    toolbScreenQgis = this->addTopMenuButton(wView,
               ":/img/qgis.png", tr("QGIS"), tr("QuantumGIS screen"),false);
    QObject::connect(toolbScreenQgis,SIGNAL(clicked()),
                         this,SLOT(onScreenQgisPick()));
    this->addTopMenuSplitter(wView);
    QStringList stubList;
    comboScreenDevice = this->addTopMenuCombo(wView, tr("Device\nsettings"),
                                              stubList);
    QObject::connect(comboScreenDevice, SIGNAL(activated(int)),
                     this, SLOT(onScreenDeviceSelect(int)));
    // TODO: add widget with device info here ...
    this->addTopMenuSplitter(wView);
    this->updateMenuView(); // just for first appearance

    // Tools.
    toolbUndo = this->addTopMenuButton(wTools,":/img/undo.png",
                tr("Undo"),tr("Cancel last form \noperation"),false,true);
    toolbRedo = this->addTopMenuButton(wTools,":/img/redo.png",
                tr("Redo"),tr("Return last canceld\nform operation"),false,true);
    this->addTopMenuSplitter(wTools);
    toolbClearScreen = this->addTopMenuButton(wTools,":/img/clear_screen.png",
                tr("Clear"),tr("Clear all screen\nelements"),false,true);
    QObject::connect(toolbClearScreen, SIGNAL(clicked()),
                     this, SLOT(onClearScreenClick()));
    toolbDeleteElem = this->addTopMenuButton(wTools,":/img/delete_elem.png",
                 tr("Delete"),tr("Delete selected\nelement"),false,true);
    QObject::connect(toolbDeleteElem, SIGNAL(clicked()),
                     this, SLOT(onDeleteElemClick()));
    this->addTopMenuSplitter(wTools);
    toolbFieldManager = this->addTopMenuButton(wTools,":/img/fields.png",
      tr("Fields"),tr("Modify fields\nof the project"),false,true);
    QObject::connect(toolbFieldManager, SIGNAL(clicked()),
                     this, SLOT(onFieldsManagerClick()));
    toolbImportControls = this->addTopMenuButton(wTools,":/img/import_controls.png",
      tr("Import"),tr("Import elements\nfrom another project"),false,true);
    QObject::connect(toolbImportControls, SIGNAL(clicked()),
                     this, SLOT(onImportControlsClick()));
    toolbUpdateData = this->addTopMenuButton(wTools,":/img/update_data.png",
      tr("Update"),tr("Update layer with data\nfrom other Shapefile"),false,true);
    QObject::connect(toolbUpdateData, SIGNAL(clicked()),
                     this, SLOT(onUpdateDataClick()));

    // Settings.

    // About.

    //----------------------------------------------------------------------
    //                              Left menu
    //----------------------------------------------------------------------

    wMenuLeft = new QWidget(this);
    wMenuLeft->setObjectName("wMenuLeft");
    wMenuLeft->setMaximumWidth(240);
    wMenuLeft->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);

    butArrowLeft = new QPushButton(wMenuLeft);
    butArrowLeft->setIcon(QIcon(":/img/arrow_left.png"));
    butArrowLeft->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    //butArrowLeft->setAlignment(Qt::AlignTop);
    butArrowLeft->setFlat(true);
    butArrowLeft->setCursor(Qt::PointingHandCursor);
    QObject::connect(butArrowLeft,SIGNAL(clicked()),
                     this,SLOT(onLeftArrowClick()));

    treeLeftFull = new QTreeWidget(wMenuLeft);
    treeLeftFull->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    treeLeftFull->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
    treeLeftFull->setColumnCount(1);
    treeLeftFull->setHeaderHidden(true);
    treeLeftFull->setCursor(Qt::PointingHandCursor);
    treeLeftFull->setFocusPolicy(Qt::NoFocus); // so not to show dotted frame
    treeLeftFull->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QObject::connect(treeLeftFull,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onAddElemPress(QTreeWidgetItem*,int)));

    treeLeftShort = new QTreeWidget(wMenuLeft);
    treeLeftShort->setMaximumWidth(35);
    treeLeftShort->setMinimumWidth(35);
    treeLeftShort->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    treeLeftShort->setIndentation(0);
    treeLeftShort->setItemsExpandable(false);
    treeLeftShort->setColumnCount(1);
    treeLeftShort->setHeaderHidden(true);
    treeLeftShort->setCursor(Qt::PointingHandCursor);
    treeLeftShort->setFocusPolicy(Qt::NoFocus);
    treeLeftShort->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QObject::connect(treeLeftShort,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,SLOT(onAddElemPress(QTreeWidgetItem*,int)));

    QHBoxLayout *lhMenuLeft = new QHBoxLayout(wMenuLeft);
    QVBoxLayout *lvMenuLeft1 = new QVBoxLayout();
    QVBoxLayout *lvMenuLeft2 = new QVBoxLayout();
    lvMenuLeft1->addWidget(treeLeftFull);
    lvMenuLeft1->addWidget(treeLeftShort);
    lvMenuLeft2->addWidget(butArrowLeft);
    lvMenuLeft2->addStretch();
    lhMenuLeft->addLayout(lvMenuLeft1);
    lhMenuLeft->addLayout(lvMenuLeft2);

    this->updateLeftTrees(); // fill trees with elements' representations
    this->flipLeftMenu(false);

    //----------------------------------------------------------------------
    //                              Right menu
    //----------------------------------------------------------------------
    // see left menu for comments

    wMenuRight = new QWidget(this);
    wMenuRight->setObjectName("wMenuRight");
    wMenuRight->setMaximumWidth(300);
    wMenuRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    //wMenuRight->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    butArrowRight = new QPushButton(wMenuRight);
    butArrowRight->setIcon(QIcon(":/img/arrow_right.png"));
    butArrowRight->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    butArrowRight->setFlat(true);
    butArrowRight->setCursor(Qt::PointingHandCursor);
    QObject::connect(butArrowRight,SIGNAL(clicked()),
            this,SLOT(onRightArrowClick()));

    labRight = new QLabel(wMenuRight);
    labRight->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    labRight->setAlignment(Qt::AlignCenter);
    labRight->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));

    QHBoxLayout *lhMenuRight = new QHBoxLayout(wMenuRight);
    QVBoxLayout *lvRight1 = new QVBoxLayout();
    lvRight = new QVBoxLayout();
    //lvRight->setContentsMargins(5,5,5,5);
    lvRight->setSpacing(12);
    lvRight1->addWidget(butArrowRight);
    lvRight1->addStretch();
    lhMenuRight->addLayout(lvRight1);
    lvRight->addWidget(labRight);
    lvRight->addStretch();
    lhMenuRight->addLayout(lvRight);

    this->updateRightMenu(); // initial menu settings
    this->flipRightMenu(false);

    //----------------------------------------------------------------------
    //                              Other GUI
    //----------------------------------------------------------------------

    labBottom = new QLabel(this);
    labBottom->setText(" ...");
    labBottom->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labBottom->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
    this->updateProjectString();

    dlgProgress = new FBDialogProgress(this);

    //----------------------------------------------------------------------
    //                            Layout all window
    //----------------------------------------------------------------------

    lhMiddle = new QHBoxLayout();
    lhMiddle->addWidget(wMenuLeft);
    lhMiddle->addWidget(wScreen);
    lhMiddle->addWidget(wMenuRight);

    lvAll = new QVBoxLayout(this);
    lvAll->setContentsMargins(0,5,0,5);
    lvAll->setSpacing(10);
    lvAll->addWidget(tabMenuTop);
    lvAll->addLayout(lhMiddle);
    lvAll->addWidget(labBottom);

    //----------------------------------------------------------------------
    //                         Common GUI settings
    //----------------------------------------------------------------------

    this->updateEnableness();
}


// Sets the general "white" style for all GUI. If not to call this - all fonts and
// colors will be system-default, except several buttons and menus.
void FB::setFbStyle ()
{
    this->setStyleSheet("QWidget#"+this->objectName()+" { background: white;"
                        " color: black; }"); // explicitly set dark font color, otherwise
                                             // users who have dark system background
                                             // color and light font color will have
                                             // troubles with displaying GUI
    tabMenuTop->setStyleSheet("QTabWidget {background: white;}"
                              "QTabWidget::pane {"
                              "border-top: 1px solid "
                              +QString(FB_COLOR_MEDIUMGREY)+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";}"
                              "QTabWidget::tab-bar {"
                              "left: 5px; "
                              "top: 1px; }"
                              "QTabBar::tab {"
                              "border: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-bottom-color: white; "
                              "border-top-left-radius: 4px;"
                              "border-top-right-radius: 4px;"
                              "min-width: 8ex;"
                              "padding: 2px;"
                              "color: black }"
                              "QTabBar::tab:!selected {"
                              "border: 1px solid white;"
                              "border-bottom-color: "+FB_COLOR_MEDIUMGREY+";"
                              "margin-top: 2px; }");

    wMenuLeft->setStyleSheet("QWidget#"+wMenuLeft->objectName()+"{"
                             "border-top: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-right: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                             "border-top-right-radius: 4px;"
                             "border-bottom-right-radius: 4px;}");

    butArrowLeft->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");

    treeLeftFull->setStyleSheet("QTreeWidget"
                              "{"
                                  "background: white;"
                                  "border: none;"
                                  "icon-size: 20px;"
                              "}"
                              "QTreeView::branch:has-children:opened"
                              "{"
                                   "image: url(:/img/arrow_open.png);"
                              "}"
                              "QTreeView::branch:has-children:closed"
                              "{"
                                   "image: url(:/img/arrow_close.png);"
                              "}"
                              "QTreeView::branch:!has-children:opened"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeView::branch:!has-children:closed"
                              "{"
                                   "image: none;"
                              "}"
                              "QTreeWidget::item"
                              "{"
                                  "border: none;"
                                  "padding-top: 5;"
                                  "padding-bottom: 5;"
                              "}"
                              "QTreeWidget::item:has-children"
                              "{"
                                  //"text-decoration: underline;"
                                  "color: "+QString(FB_COLOR_DARKGREY)+";"
                              "}"
                              "QTreeWidget::item:!has-children"
                              "{"
                                  "color: black;"
                              "}"
                              "QTreeWidget::item:has-children:hover"
                              "{"
                                  "background-color: white;"
                              "}"
                              "QTreeWidget::item:!has-children:hover"
                              "{"
                                  "background-color: "+FB_COLOR_DARKBLUE+";"
                              "}"
                              "QTreeWidget::item:has-children:selected"
                              "{"
                                  "background-color: white;"
                                  "color: "+FB_COLOR_DARKGREY+";"
                              "}"
                              "QTreeWidget::item:!has-children:selected"
                              "{"
                                  "background-color: "+FB_COLOR_LIGHTBLUE+";"
                                  "color: black;"
                              "}");

    treeLeftShort->setStyleSheet(treeLeftFull->styleSheet());

    wMenuRight->setStyleSheet("QWidget#"+wMenuRight->objectName()+"{"
                              "border-top: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-bottom: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-left: 1px solid "+FB_COLOR_MEDIUMGREY+";"
                              "border-top-left-radius: 4px;"
                              "border-bottom-left-radius: 4px; }");

    butArrowRight->setStyleSheet("QPushButton:default{border:none;}"
                             "QPushButton:flat{border:none;}"
                             "QPushButton:checked{border:none;}");

    labBottom->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"; "
                             "background: white;}");
}


/****************************************************************************/
/*                                                                          */
/*                         Main GUI private slots                           */
/*                                                                          */
/****************************************************************************/

// ADD ELEM FROM RIGHT MENU
void FB::onAddElemPress (QTreeWidgetItem* item, int column)
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL)
        return;

    if (item->childCount() != 0) // check if it is not a group header in tree
        return;

    QString keyName = item->data(0,Qt::UserRole).toString();
    FBFactory *fct = FBFactory::getFctByName(keyName);
    if (fct == NULL)
        return;

    FBElem *elem = fct->create();
    form->addElem(elem,NULL);

    this->onElemSelect();
}


// SELECT ELEM ON THE SCREEN
void FB::onElemSelect ()
{
    this->updateRightMenu();
}


// NEW VOID PROJECT BUTTON CLICK
void FB::onNewVoidClick ()
{
    toolbNewVoid->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
    {
        toolbNewVoid->setDown(false);
        return;
    }

    FBDialogProjectNew dialog(this);
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString geomType = dialog.getSelectedGeom();

        FBProjectVoid *projVoid = new FBProjectVoid(
                    FBProject::findGeomTypeByNgw(geomType));
        QObject::connect(projVoid, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        this->newProjectCommonActions(projVoid, ""); // just void string stub
    }
}


// NEW SHAPEFILE PROJECT BUTTON CLICK
void FB::onNewShapeClick ()
{
    toolbNewShape->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
    {
        toolbNewShape->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("shp");
    dialog.setNameFilter("*.shp");
    dialog.setWindowTitle(tr("Creating new project. Select Shapefile ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = this->getLastPathShapefile();
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString pathShapefile;
        QStringList sPaths = dialog.selectedFiles();
        pathShapefile = sPaths[0];

        FBProjectShapefile *projShp = new FBProjectShapefile();
        QObject::connect(projShp, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        this->newProjectCommonActions(projShp, pathShapefile);
    }
}


// NEW NEXTGISWEB PROJECT BUTTON CLICK
void FB::onNewNgwClick ()
{
    toolbNewNgw->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
    {
        toolbNewNgw->setDown(false);
        return;
    }

    QString lastUrl;
    QString lastLogin;
    this->getLastPathNgw(lastUrl,lastLogin);
    FBDialogProjectNgw dialog(this,lastUrl,lastLogin);
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString pathNgwUrl;
        QString strUrl, strLogin, strPass;
        int nId;
        Json::Value jsonLayerMeta;
        pathNgwUrl = dialog.getSelectedNgwResource(
                    strUrl, strLogin, strPass, nId, jsonLayerMeta);

        FBProjectNgw *projNgw = new FBProjectNgw(
                    strUrl, strLogin, strPass, nId, jsonLayerMeta);
        QObject::connect(projNgw, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        this->newProjectCommonActions(projNgw, pathNgwUrl);
    }
}


// OPEN PROJECT BUTTON CLICK
void FB::onOpenClick ()
{
    toolbOpen->setDown(true);
    if (this->onAskToLeaveUnsafeProject())
    {
        toolbOpen->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Open project. Select project file ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = this->getLastPathProjectfile();
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog, SIGNAL(finished(int)),
                     this, SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        // Check file extension anyway because user can type it manually.
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
        {
            this->onShowError(tr("Wrong file extension. Must be .")
                          + QString(FB_PROJECT_EXTENSION));
            return;
        }

        // Create new project, by opening it.
        FBProject *projOpen = new FBProject();
        QObject::connect(projOpen, SIGNAL(changeProgress(int)),
                dlgProgress, SLOT(onChangeProgress(int)));
        FBErr err = projOpen->open(strFullPath);
        if (err != FBErrNone)
        {
            delete projOpen;
            this->onShowErrorFull(tr("Unable to open project!"), err);
            return;
        }

        // Create and show new form, replacing old one. Fill it with elems from
        // project.
        FBForm *form = this->createForm();
        if (!form->fromJson(FBProject::readForm(strFullPath)))
        {
            delete projOpen;
            this->onShowErrorFull(tr("Unable to read form in a project!"),
                                  FBErrReadNgfpFail);
            return;
        }

        // If all was correct: replace old project and form with new ones.
        if (project != NULL)
            delete project;
        project = projOpen;
        wScreen->deleteForm();
        wScreen->setForm(form);

        // Update list of fields for all Input elements.
        FBElemInput::updateFields(project->getFields().keys());

        this->pickDefaultScreen(); // will be helpful if there is void screen now
        this->updateRightMenu();
        this->updateEnableness();
        this->updateProjectString();
        this->updateSettings();
    }
}


// SAVE PROJECT BUTTON CLICK
void FB::onSaveClick ()
{
    toolbSave->setDown(true);

    FBForm *form = wScreen->getFormPtr();
    if (project == NULL || form == NULL || !project->wasFirstSaved())
    {
        toolbSave->setDown(false);
        return;
    }

    this->saveProjectCommonActions(project->getCurrentNgfpPath());
}


// SAVE AS PROJECT BUTTON CLICK
void FB::onSaveAsClick ()
{
    toolbSaveAs->setDown(true);

    FBForm *form = wScreen->getFormPtr();
    if (project == NULL || form == NULL)
    {
        toolbSaveAs->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*." + QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Save project as ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = this->getLastPathProjectfile();
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QString ngfpFullPath = dialog.selectedFiles().first();
        if (!ngfpFullPath.endsWith("."+QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
            ngfpFullPath = ngfpFullPath+"."+QString(FB_PROJECT_EXTENSION);

        this->saveProjectCommonActions(ngfpFullPath);
    }
}


// SCREENS PICK
void FB::onScreenAndroidPick ()
{
    FBScreenAndroid *screen = new FBScreenAndroid(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenAndroid);
}
void FB::onScreenIosPick ()
{
    FBScreenIos *screen = new FBScreenIos(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenIos);
}
void FB::onScreenWebPick ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenWeb);
}
void FB::onScreenQgisPick ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,false);
    this->afterPickScreen(toolbScreenQgis);
}
// SCREEN DEVICE SELECT
void FB::onScreenDeviceSelect (int index)
{
    this->updateScreen();
}
// SCREEN STATE PICK
void FB::onScreenStatePick ()
{
    // We must always have the default screen type - so here we set
    // the first occur state in the array.
    QObject *obj = this->sender();
    if (obj == NULL)
    {
        return;
    }
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
    }
    static_cast<QToolButton*>(obj)->setDown(true);
    this->updateScreen();
}


// UNDO TOOL CLICK
void FB::onUndoClick ()
{

}
// REDO TOOL CLICK
void FB::onRedoClick ()
{

}


// CLEAR SCREEN TOOL CLICK
void FB::onClearScreenClick ()
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    toolbClearScreen->setDown(true);
    if (this->onShowWarning(tr("Clear screen from all elements?"))
            != QMessageBox::Ok)
    {
        toolbClearScreen->setDown(false);
        return;
    }
    form->clear();
    this->updateRightMenu();
    toolbClearScreen->setDown(false);
}
// DELETE ELEM TOOL CLICK
void FB::onDeleteElemClick ()
{
    FBForm *form = wScreen->getFormPtr();
    if (form == NULL || form->isVoid())
        return;
    toolbDeleteElem->setDown(true);
    form->deleteSelected();
    this->updateRightMenu();
    toolbDeleteElem->setDown(false);
}


// FIELDS MANAGER
void FB::onFieldsManagerClick ()
{
    if (project == NULL)
        return;

    toolbFieldManager->setDown(true);

    FBDialogFieldsManager dialog(this);
    dialog.loadFields(project->getFields());
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        QMap<QString,FBField> newFields = dialog.getFields();
        QSet<QString> deletedFields = dialog.getDeletedFields();

        // Reset project fields with newly created list.
        project->resetFields(newFields);

        // Update the list of deleted fields after each session of the dialog.
        // We need this because we must remember which existing fields (of the
        // underlying layer) to delete during savig project time in case when the
        // new fields with such names have been created after deletion of these
        // fields. We can do this because the keynames of fields are unique.
        project->expandFieldsDeleted(deletedFields);

        // Update list of fields for all Input elements.
        FBElemInput::updateFields(project->getFields().keys());

        // We must reset the fields, which were added to the project after the
        // deletion of fields with the same keynames as deleted ones.
        FBForm* form = wScreen->getFormPtr();
        if (form != NULL)
        {
            QList<FBElem*> elems = form->getAllElems();
            for (int i=0; i<elems.size(); i++)
            {
                this->resetSelectedFieldsForElem(elems[i],deletedFields);
            }
        }

        // Update right menu so the comboboxes with lists of fields of currently
        // selected elem will be updated.
        this->updateRightMenu();
    }
}


// IMPORT CONTROLS
void FB::onImportControlsClick()
{
    FBForm* form = wScreen->getFormPtr();
    if (project == NULL || form == NULL)
        return;

    toolbImportControls->setDown(true);

    if (!form->isVoid() && this->onShowWarning(tr("If you import elements from"
        " another project all current elements will be removed. Continue?"))
            != QMessageBox::Ok)
    {
        toolbImportControls->setDown(false);
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix(FB_PROJECT_EXTENSION);
    dialog.setNameFilter("*."+QString(FB_PROJECT_EXTENSION));
    dialog.setWindowTitle(tr("Import form elements. Select project file ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = this->getLastPathProjectfile();
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {
        // Check file extension anyway because user can type it manually.
        QString strFullPath = dialog.selectedFiles().first();
        if (!strFullPath.endsWith("." + QString(FB_PROJECT_EXTENSION),
                                  Qt::CaseInsensitive))
        {
            this->onShowError(tr("Wrong file extension. Must be .")
                          + QString(FB_PROJECT_EXTENSION));
            return;
        }

        // Process selected project.
        FBProject *projOther = new FBProject();
        FBErr err = projOther->open(strFullPath);
        if (err != FBErrNone)
        {
            this->onShowErrorFull(tr("Unable to process selected project!"),err);
            return;
        }

        // Load and show its form elements, clearing old ones.
        if (!form->fromJson(FBProject::readForm(strFullPath)))
        {
            delete projOther;
            this->onShowErrorFull(tr("Unable to read form in a project!"),
                                  FBErrReadNgfpFail);
            return;
        }

        // We need to set to undefined selected fields to those elements, which
        // have the fields selected not from current project.
        // For that we firstly create a set of fields which does not occur both
        // in selected and current projects and than call the common method to
        // reset fields. So those fields which exist in both projects will not be
        // reseted.
        QMap<QString,FBField> fieldsOther = projOther->getFields();
        delete projOther;
        QMap<QString,FBField> fieldsThis = project->getFields();
        QSet<QString> notSameFields;
        QMap<QString,FBField>::const_iterator it = fieldsThis.constBegin();
        while (it != fieldsThis.constEnd())
        {
            // We check full fields equality.
            QString keyname = it.key();
            FBField fieldThis = it.value();
            FBField fieldOther = fieldsOther.value(keyname);
            if (!fieldThis.isEqual(fieldOther)) // will be compared with default-
            {                                   // constructed field if there is no
                notSameFields.insert(keyname);   // such field in other dataset
            }
            ++it;
        }
        QList<FBElem*> elems = form->getAllElems();
        for (int i=0; i<elems.size(); i++)
        {
            this->resetSelectedFieldsForElem(elems[i],notSameFields);
        }

        // Update right menu so the comboboxes with lists of fields of currently
        // selected elem will be updated.
        this->updateRightMenu();
    }
}


// UPDATE DATA
void FB::onUpdateDataClick ()
{
    if (project == NULL)
        return;

    toolbUpdateData->setDown(true);

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDefaultSuffix("shp");
    dialog.setNameFilter("*.shp");
    dialog.setWindowTitle(tr("Update data in project. Select a Shapefile ..."));
    dialog.setDirectory(QDir()); // current directory
    QString lastPath = this->getLastPathShapefile();
    if (lastPath != "")
    {
        QFileInfo fileInfo(lastPath);
        dialog.setDirectory(fileInfo.absoluteDir());
    }
    QObject::connect(&dialog,SIGNAL(finished(int)),
                  this,SLOT(onProjDialogFinished(int)));

    if (dialog.exec())
    {

    }
}


// LANGUAGE SELECT
void FB::onSettingLanguageSelect ()
{

}


// ABOUT CLICK
void FB::onAboutGraphicsClick ()
{

}


// ARROWS CLICK
void FB::onLeftArrowClick ()
{
    this->flipLeftMenu(!treeLeftFull->isVisible());
}
void FB::onRightArrowClick ()
{
    this->flipRightMenu(!labRight->isVisible());
}


/*****************************************************************************/
/*                                                                           */
/*                          Other private slots                              */
/*                                                                           */
/*****************************************************************************/

void FB::onShowInfo(QString msg)
{
    onShowBox(msg,tr("Information"));
}

int FB::onShowWarning (QString msg)
{
    return onShowBox(msg,tr("Warning"));
}

void FB::onShowError (QString msg)
{
    onShowBox(msg,tr("Error"));
}

int FB::onShowBox (QString msg, QString caption)
{
    QMessageBox msgBox(this);
    msgBox.setStyleSheet(""); // TODO: why does not work?
    msgBox.setText(msg);
    msgBox.setWindowTitle(caption);
    QMessageBox::Icon icon;
    if (caption == "Warning")
    {
        icon = QMessageBox::Warning;
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    }
    else
    {
        if (caption == "Error")
        {
            icon = QMessageBox::Critical;
        }
        else
        {
            icon = QMessageBox::Information;
        }
        msgBox.setStandardButtons(QMessageBox::Ok);
    }
    msgBox.setIcon(icon);
    return msgBox.exec();
}

int FB::onShowErrorFull (QString msgMain, FBErr err)
{
    QMessageBox msgBox(this);
    msgBox.setStyleSheet("");
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(msgMain);
    QString str = this->getErrStr(err);
    if (str != "")
    {
        msgBox.setInformativeText(str);
        msgBox.setDetailedText(FBProject::CUR_ERR_INFO);
    }
    FBProject::CUR_ERR_INFO = "";
    return msgBox.exec();
}


// Some common actions.
// Call only in according methods.
bool FB::onAskToLeaveUnsafeProject ()
{
    return (project != NULL
            && project->isSaveRequired()
            && this->onShowWarning(tr("Project hasn't been saved. Continue?"))
                != QMessageBox::Ok);
}


// Some common GUI actions after closing project dialogs.
// Connect only to finished() signal of according dialogues.
void FB::onProjDialogFinished (int code)
{
    toolbNewVoid->setDown(false);
    toolbNewShape->setDown(false);
    toolbNewNgw->setDown(false);
    toolbOpen->setDown(false);
    toolbSave->setDown(false);
    toolbSaveAs->setDown(false);
    toolbFieldManager->setDown(false);
    toolbImportControls->setDown(false);
    toolbUpdateData->setDown(false);
}


// This slot is called after the long action of saving project is ended in the
// separate thread.
// Connect only to according methods.
void FB::onSaveAnyEnded (FBErr err)
{
    // Close progress dialog and do according actions.
    dlgProgress->accept();
    if (err != FBErrNone)
    {
        this->onShowErrorFull(tr("Error saving project"),err);
    }
    else
    {
        this->onShowInfo(tr("Project saved successfully"));
        this->updateEnableness();
        this->updateProjectString();
        this->updateSettings();
    }
    toolbSaveAs->setDown(false);
    toolbSave->setDown(false);
}


/****************************************************************************/
/*                                                                          */
/*                         Private methods                                  */
/*                                                                          */
/****************************************************************************/

// For elem in the form with "Field attributes" set to undefined required fields,
// if are any.
void FB::resetSelectedFieldsForElem (FBElem *elem, QSet<QString> allFieldsToReset)
{
    // Select only "Input elements", because only them contain "Field
    // attributes".
    FBElemInput *e = qobject_cast<FBElemInput*>(elem);
    if (e != NULL)
    {
        // Some elements may have several Field attributes, so we try to reset
        // them all, if needed.
        QStringList selectedFields = e->getSelectedFields();
        for (int k=0; k<selectedFields.size(); k++)
        {
            if (allFieldsToReset.contains(selectedFields[k]))
            {
                e->resetSelectedField(selectedFields[k]);
            }
        }
    }
}


void FB::updateSettings ()
{

}
QString FB::getLastPathProjectfile ()
{
    return "";
}
QString FB::getLastPathShapefile ()
{
    return "";
}
void FB::getLastPathNgw (QString &url, QString &login)
{

}


// Convert main error code to string.
QString FB::getErrStr (FBErr err)
{
    QString ret;
    switch (err)
    {
        case FBErrWrongVersion: ret = tr("Wrong file version"); break;
        case FBErrIncorrectJson: ret = tr("Incorrect JSON file structure"); break;
        case FBErrIncorrectFileStructure: ret = tr("Incorrect file structure"); break;
        case FBErrIncorrectGdalDataset: ret = tr("Incorrect GDAL dataset"); break;
        case FBErrWrongSavePath: ret = tr("Wrong save path"); break;
        case FBErrTempFileFail: ret = tr("Temporary file error"); break;
        case FBErrGDALFail: ret = tr("GDAL error"); break;
        case FBErrCPLFail: ret = tr("GDAL CPL error"); break;
        case FBErrReadNgfpFail: ret = tr("Reading project file error"); break;
        default: ret = ""; break; // some errors will be explicitly not processed
    }
    return ret;
}


// Convert group type of element to string.
QString FB::getGroupStr (FBElemtype type)
{
    QString str = tr("Unknown");
    switch (type)
    {
        case FBDecoration: str = tr("Decoration"); break;
        case FBInput: str = tr("Input"); break;
        case FBGrouping: str = tr("Grouping"); break;
        case FBLayout: str = tr("Layout"); break;
    }
    return str;
}


// Common actions for form creation.
FBForm *FB::createForm ()
{
    FBForm *form = new FBForm();
    QObject::connect(form, SIGNAL(elemPressed()),
                     this, SLOT(onElemSelect()));
    return form;
}


// Create new button for any tab of the top menu.
QToolButton *FB::addTopMenuButton (QWidget *parentTab, QString imgPath, QString name,
         QString description, bool isSmall, bool withCaption)
{
    QToolButton *but = new QToolButton(parentTab);
    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    but->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    but->setAutoRaise(true);
    but->setIcon(QIcon(imgPath));
    but->setText(name); // necessarily do this because it will store correspondence
                        // to screen arrays for screen menu buttons
    but->setFont(QFont(FB_GUI_FONTTYPE, FB_GUI_FONTSIZE_SMALL));
    but->setToolTip(description);
    but->setCursor(Qt::PointingHandCursor);
    // TODO: Make real semitransparent style (~20%).
    but->setStyleSheet("QToolButton"
      "{border: none; color: "+QString(FB_COLOR_DARKGREY)+";}"
      "QToolButton:hover"
      "{background-color: "+FB_COLOR_DARKBLUE+"; color: white;}"
      "QToolButton:pressed{"
      "background-color: "+FB_COLOR_LIGHTBLUE+"; color: white;}"
      "QToolButton:disabled{}");
    if (isSmall)
    {
        but->setIconSize(QSize(60,60));
        //but->setMaximumWidth(45);
        but->setFixedSize(35,35);
    }
    else
    {
        but->setIconSize(QSize(60,60));
        //but->setMaximumWidth(90);
        but->setFixedSize(65,65);
    }

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(but);

    if (withCaption)
    {
        /*
        QLabel *lab = new QLabel(parentTab);
        lab->setText(name);
        lab->setAlignment(Qt::AlignCenter);
        lab->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
        lab->setStyleSheet("QLabel {color: "+QString(FB_COLOR_MEDIUMGREY)+"}");
        lab->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
        lay->addWidget(lab);
        */
        but->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        but->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
        but->setIconSize(QSize(43,43));
    }

    hlParent->insertLayout(hlParent->count()-1,lay); // last is stretch item.

    return but;
}


// Create new combobox with caption for any tab of the top menu.
QComboBox *FB::addTopMenuCombo (QWidget *parentTab, QString caption,
                                QStringList values)
{
    QComboBox *combo = new QComboBox(parentTab);
    combo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    combo->setMaximumWidth(125);
    combo->setFont(QFont("Segoe UI",FB_GUI_FONTSIZE_NORMAL));
    for (int i=0; i<values.size(); i++)
    {
        combo->addItem(values[i]);
    }

    QLabel *lab = new QLabel(parentTab);
    lab->setText(caption);
    lab->setAlignment(Qt::AlignCenter);
    lab->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_SMALL));
    lab->setStyleSheet("QLabel {color: "+QString(FB_COLOR_DARKGREY)+"}");
    lab->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(10);
    vl->addWidget(combo);
    vl->addWidget(lab);
    hlParent->insertLayout(hlParent->count()-1,vl);

    return combo;
}


// Create splitter vertical line for any tab in the top menu.
void FB::addTopMenuSplitter (QWidget *parentTab)
{
    QWidget *wid = new QWidget(wView);
    wid->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    wid->setFixedWidth(1);
    wid->setStyleSheet("QWidget {background-color: "
                             +QString(FB_COLOR_LIGHTMEDIUMGREY)+"}");
    QHBoxLayout *hlParent = (QHBoxLayout*)parentTab->layout();
    hlParent->insertWidget(hlParent->count()-1,wid); // last is stretch item.
}


// Minimize or maximize menus.
void FB::flipLeftMenu (bool isFull)
{
    treeLeftFull->setVisible(isFull);
    treeLeftShort->setVisible(!isFull);
    if (!isFull)
        butArrowLeft->setIcon(QIcon(":/img/arrow_right.png"));
    else
        butArrowLeft->setIcon(QIcon(":/img/arrow_left.png"));

    // TODO: open and select elems in the one tree, that was selected in another,
    // i.e. synchronize trees.
}
void FB::flipRightMenu (bool isFull)
{
    labRight->setVisible(isFull);
    for (int i=0; i<tablesRight.size(); i++)
    {
        tablesRight[i]->setVisible(isFull);
    }
    if (!isFull)
        butArrowRight->setIcon(QIcon(":/img/arrow_left.png"));
    else
        butArrowRight->setIcon(QIcon(":/img/arrow_right.png"));
}


// Create one table in right menu.
QTableWidget* FB::addRightMenuTable ()
{
    QTableWidget *table = new QTableWidget(wMenuRight);
    table->setMinimumHeight(0);
    //table->setMaximumHeight(50);
    table->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFont(QFont(FB_GUI_FONTTYPE,FB_GUI_FONTSIZE_NORMAL));
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->setRowCount(0);
    table->setColumnCount(2);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setStyleSheet("QTableWidget"
                         "{border: none;"
                         "selection-background-color: "
                         +QString(FB_COLOR_LIGHTBLUE)+";"
                         "gridline-color: white;"
                         "color: "+FB_COLOR_DARKGREY+";}"
                         "QTableWidget::item"
                         "{border: 1px solid "+FB_COLOR_LIGHTMEDIUMGREY+";}"
                         "QTableWidget::item:selected"
                         "{border: 1px solid white;"
                         "background-color: "+FB_COLOR_LIGHTBLUE+";}"
                         "QTableWidget::item:focus"
                         "{"
                            //"border: 2px solid red;"
                         "}");
    lvRight->insertWidget(lvRight->count()-1,table); // before stretch item
    tablesRight.append(table);
    return table;
}


// Update enable states for all GUI elements in program.
void FB::updateEnableness ()
{
    if (project == NULL)
    {
        wMenuLeft->setEnabled(false);
        wMenuRight->setEnabled(false);
        toolbSave->setEnabled(false);
        toolbSaveAs->setEnabled(false);
        toolbScreenAndroid->setEnabled(false);
        toolbScreenIos->setEnabled(false);
        toolbScreenWeb->setEnabled(false);
        toolbScreenQgis->setEnabled(false);
        comboScreenDevice->setEnabled(false);
        for (int i=0; i<toolbsScreenState.size(); i++)
        {
            toolbsScreenState[i]->setEnabled(false);
        }
        toolbUndo->setEnabled(false);
        toolbRedo->setEnabled(false);
        toolbClearScreen->setEnabled(false);
        toolbDeleteElem->setEnabled(false);
        toolbImportControls->setEnabled(false);
        toolbUpdateData->setEnabled(false);
        toolbFieldManager->setEnabled(false);
    }
    else
    {
        wMenuLeft->setEnabled(true);
        wMenuRight->setEnabled(true);
        if (project->wasFirstSaved())
        {
            toolbSave->setEnabled(true);
        }
        else
        {
            toolbSave->setEnabled(false);
        }
        toolbSaveAs->setEnabled(true);
        toolbUndo->setEnabled(true);
        toolbRedo->setEnabled(true);
        toolbClearScreen->setEnabled(true);
        toolbDeleteElem->setEnabled(true);
        toolbScreenAndroid->setEnabled(true);
        toolbScreenIos->setEnabled(true);
        toolbScreenWeb->setEnabled(true);
        toolbScreenQgis->setEnabled(true);
        comboScreenDevice->setEnabled(true);
        for (int i=0; i<toolbsScreenState.size(); i++)
        {
            toolbsScreenState[i]->setEnabled(true);
        }
        toolbImportControls->setEnabled(true);
        toolbUpdateData->setEnabled(true);
        toolbFieldManager->setEnabled(true);
    }

    /*
    // TEMPORARY BLOCKINGS:
    toolbScreenWeb->setEnabled(false);
    toolbScreenQgis->setEnabled(false);
    toolbUndo->setEnabled(false);
    toolbRedo->setEnabled(false);
    */
}


// Clear and than fill two left trees with element factories.
void FB::updateLeftTrees ()
{
    treeLeftFull->clear();
    treeLeftShort->clear();

    // Create elements' representations for all trees.
    QMap<FBElemtype,QTreeWidgetItem*> groupItems;
    QList<FBFactory*> fcts = FBFactory::getAllFcts();
    for (int i=0; i<fcts.size(); i++)
    {
        if (!groupItems.contains(fcts[i]->getType()))
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,this->getGroupStr(fcts[i]->getType()));
            groupItems.insert(fcts[i]->getType(),item);
            treeLeftFull->addTopLevelItem(item);
        }

        QTreeWidgetItem *item1 = new QTreeWidgetItem();
        item1->setText(0,fcts[i]->getDisplayName());
        item1->setData(0,Qt::UserRole,fcts[i]->getKeyName());
        item1->setIcon(0,QIcon(fcts[i]->getImgPath()));
        groupItems[fcts[i]->getType()]->addChild(item1);

        QTreeWidgetItem *item2 = new QTreeWidgetItem();
        item2->setData(0,Qt::UserRole,fcts[i]->getKeyName());
        item2->setIcon(0,QIcon(fcts[i]->getImgPath()));
        item2->setToolTip(0,fcts[i]->getDisplayName());
        treeLeftShort->addTopLevelItem(item2);
    }

    treeLeftFull->sortItems(0,Qt::AscendingOrder);
    //treeLeftFull->expandAll();
}


// Delete old and than create new tables for right menu and update its label.
void FB::updateRightMenu ()
{
    // Destroy all previous tables.
    for (int i=0; i<tablesRight.size(); i++)
    {
        // TODO: do we need to use QTableWidget::removeCellWidget(i,1) or
        // clearContents() or setRowCount(0) here to correctly delete tables?
        lvRight->removeWidget(tablesRight[i]);
        delete tablesRight[i];
    }
    tablesRight.clear();

    // Get form elements.
    QList<FBElem*> elemsSelected;
    FBForm *form = wScreen->getFormPtr();
    if (form != NULL)
        elemsSelected = form->getSelectedElems();

    if (elemsSelected.size() == 0)
    {
        // Show default string.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Select an element ..."));
    }

    else if (elemsSelected.size() == 1)
    { 
        // Create a table for one selected elem's attributes.
        // TODO: create different tables for different attribute roles.
        QTableWidget* table = this->addRightMenuTable();

        // Get attrs of selected elem.
        FBElem *elem = elemsSelected.first();
        QSet<FBAttr*> attrs = elem->getAttrs();
        QSet<FBAttr*>::const_iterator it = attrs.constBegin();
        while (it != attrs.constEnd())
        {
            QString alias = (*it)->getDisplayName();
            QTableWidgetItem *itemAlias = new QTableWidgetItem(alias);
            itemAlias->setFlags(Qt::ItemIsEnabled);
            itemAlias->setToolTip(alias);
            if ((*it)->getRole() == FBImportant)
            {
                QFont font;
                font.setBold(true);
                itemAlias->setFont(font);
            }
            QWidget *widget = (*it)->getWidget();
            table->setRowCount(table->rowCount()+1);
            table->setItem(table->rowCount()-1,0,itemAlias);
            table->setCellWidget(table->rowCount()-1,1,widget);
            ++it;
        }

        table->sortItems(0);

        // We must hide table if the right menu is in minimized state
        // for now.
        if (!labRight->isVisible())
        {
            table->hide();
        }

        // Show caption of elem.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_DARKBLUE)+";}");
        labRight->setText(elem->getDisplayName());
    }

    else // size > 1
    {
        // Show string that many elems selected.
        labRight->setStyleSheet("QLabel{border: none;color: "
                                +QString(FB_COLOR_LIGHTBLUE)+";}");
        labRight->setText(tr("Elements selected: ")
                          + QString::number(elemsSelected.size()));
    }
}


// Set bottom status label.
void FB::setBottomString (QString strToShorten, QString strToPrepend)
{
    if (strToShorten.size() > FB_BOTTOMSTRING_LEN_MAX)
    {
        strToShorten.remove(0, strToShorten.size() - FB_BOTTOMSTRING_LEN_MAX);
        strToShorten.prepend(" ... ");
    }
    labBottom->setText(" " + strToPrepend + strToShorten);
}


// Set bottom status label for project info.
void FB::updateProjectString ()
{
    QString strToShorten = "";
    QString strToPrepend = "";

    if (project == NULL)
    {
        strToShorten = tr("Create new or open existing project ...");
    }
    else
    {
        strToShorten = project->getProjectfilePath();
        if (strToShorten == "")
        {
            strToPrepend = tr("Need to save data to ngfp file. Current dataset: ");
            strToShorten = project->getDatasetPath();
        }
        else
        {
            strToPrepend = tr("Current project: ");
        }
    }

    this->setBottomString(strToShorten,strToPrepend);
}


// Recreate some buttons and selectors in the View tab of top menu according to the
// selected screen.
void FB::updateMenuView ()
{
    // Devices combobox.
    comboScreenDevice->clear();
    QList<FBDevice> devices = wScreen->getDevices();
    for (int i=0; i<devices.size(); i++)
    {
        comboScreenDevice->addItem(QIcon(devices[i].imgPath),
                                   devices[i].getDisplayString());
    }

    // States buttons.
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        lhView->removeWidget(toolbsScreenState[i]);
        delete toolbsScreenState[i];
    }
    toolbsScreenState.clear();
    QList<FBState> states = wScreen->getStates();
    for (int i=0; i<states.size(); i++)
    {
        toolbsScreenState.append(this->addTopMenuButton(wView,states[i].imgPath,
                                 states[i].name,states[i].descr));
        QObject::connect(toolbsScreenState.last(),SIGNAL(clicked()),
                         this,SLOT(onScreenStatePick()));
    }

    // Select defaults in GUI.
    // Do not call slots connecting to buttons and combobox.
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        toolbsScreenState[i]->setDown(false);
    }
    toolbsScreenState[0]->setDown(true);
    comboScreenDevice->setCurrentIndex(0); // will not trigger slot
}


// Some common GUI actions: after change screen settings.
// Call only in according methods.
void FB::afterPickScreen (QToolButton *toolbDown)
{
    this->updateMenuView();
    this->updateScreen();
    toolbScreenAndroid->setDown(false);
    toolbScreenIos->setDown(false);
    toolbScreenWeb->setDown(false);
    toolbScreenQgis->setDown(false);
    if (toolbDown != NULL)
        toolbDown->setDown(true);
}


// Recreate screen to default one.
void FB::pickDefaultScreen ()
{
    // TODO: think about what other screen to set in this situation - may be
    // read it from the current project structure. Currently we set Android
    // screen.
    FBScreenAndroid *screen = new FBScreenAndroid(this);
    this->recreateScreen(screen,false); // with form copy, if was some
    this->afterPickScreen(toolbScreenAndroid);
}
// Recreate screen to void. Just "reset to grey screen".
void FB::pickVoidScreen ()
{
    FBScreen *screen = new FBScreen(this);
    this->recreateScreen(screen,true); // do not copy form
    this->afterPickScreen(NULL);
}


// Create new screen with copying old form (or not).
void FB::recreateScreen (FBScreen *newScreen, bool destroyForm)
{
    if (newScreen == NULL)
        return;
    if (!destroyForm)
    {
        FBForm *curForm = wScreen->takeForm();
        newScreen->setForm(curForm);
    }
    lhMiddle->removeWidget(wScreen);
    delete wScreen;
    lhMiddle->insertWidget(1,newScreen);
    newScreen->updateStyle(); // this will also update the style of form elems
    wScreen = newScreen;
}


// Update device and state settings of the screen.
void FB::updateScreen ()
{
    int index;
    index = comboScreenDevice->currentIndex();
    if (index == -1)
    {
        index = 0;
    }
    wScreen->setDevice(index);
    index = 0;
    for (int i=0; i<toolbsScreenState.size(); i++)
    {
        if (toolbsScreenState[i]->isDown())
        {
            index = i;
            break;
        }
    }
    wScreen->setState(index);
}


// Common steps for all new projects creation methods.
// Call only in according methods.
void FB::newProjectCommonActions (FBProject *proj, QString path)
{
    // Replacing old project if new one was correctly created.
    FBErr err = proj->create(path);
    if (err != FBErrNone)
    {
        delete proj;
        this->onShowErrorFull(tr("Unable to create new project! "), err);
        return;
    }
    if (project != NULL)
        delete project;
    project = proj;

    // Create and show new void form replacing old one.
    wScreen->deleteForm();
    FBForm *form = this->createForm();
    wScreen->setForm(form);

    // Reset list of fields for all future Input elements.
    FBElemInput::updateFields(project->getFields().keys());

    this->pickDefaultScreen(); // will be helpful if there is void screen now
    this->updateRightMenu();
    this->updateEnableness();
    this->updateProjectString();
    this->updateSettings();
}

// Common steps for all saving projects methods.
// Call only in according methods.
void FB::saveProjectCommonActions (QString ngfpFullPath)
{
    // Start new thread for the long saving action.
    // At the same time we show a modal dialog in order to block all GUI events. If
    // do not do this - all occasional clicks and actions by user during the work of
    // the thread can cause an "undefined behaviour" because widgets will receive
    // mouse events right after the thread ends its work. What did not help to solve
    // this problem:
    // 1. From Qt docs: QProgressDialog pr(...); pr->show();
    // pr.setWindowModality(Qt::WindowModal);
    // 2. QApplication::processEvents(QEventLoop::ExcludeUserInputEvents) as addition
    // to 1.
    // 3. Blocking the window or all widgets via setEnabled(false);

    dlgProgress->setWindowTitle(tr("Saving ..."));
    FBThreadSaveAs *thread = new FBThreadSaveAs(this, ngfpFullPath, project,
                      wScreen->getFormPtr()->toJson()); // NULL-check was made outside
    QObject::connect(thread,SIGNAL(resultReady(FBErr)),
                     this, SLOT(onSaveAnyEnded(FBErr)));
    QObject::connect(thread,SIGNAL(finished()),
                     thread, SLOT(deleteLater()));
    thread->start();
    dlgProgress->exec();

    // The further work is made in the separate thread which ends with calling
    // onSaveAsEnded() method.
}


/*****************************************************************************/
/*                                                                           */
/*                           FBDialogProgress                                */
/*                                                                           */
/*****************************************************************************/

FBDialogProgress::FBDialogProgress (QWidget *parent):
    QDialog(parent)
{
    this->setStyleSheet("");
    this->setStyleSheet("QWidget { color: black }");

    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint
                         | Qt::CustomizeWindowHint); // hide all window buttons
    this->setWindowTitle("...");
    this->setModal(true);

    bar = new QProgressBar(this);
    bar->setMaximum(99);
    bar->setValue(0);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(bar);
}


/*****************************************************************************/
/*                                                                           */
/*                           FBThreadSaveAs                                  */
/*                                                                           */
/*****************************************************************************/

FBThreadSaveAs::FBThreadSaveAs (QObject *parent, QString strFullPath,
                                FBProject *project, Json::Value jsonForm):
    QThread (parent)
{
    this->strFullPath = strFullPath;
    this->project = project;
    this->jsonForm = jsonForm;
}

void FBThreadSaveAs::run ()
{
    FBErr err = project->saveAs(strFullPath,jsonForm);
    emit resultReady(err);
}


