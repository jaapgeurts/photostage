#ifndef PHOTOSTAGE_CONSTANTS
#define PHOTOSTAGE_CONSTANTS

#include <QString>

const int     PREVIEW_IMG_WIDTH  = 1024;
const int     PREVIEW_IMG_HEIGHT = 1024;

const int     HASH_INPUT_LEN = 1L << 20;

const QString MIMETYPE_TILEVIEW_SELECTION = "application/x-photostage-tileview-selection";

// CARTOGRAPHY SETTINGS
const QString SETTINGS_GROUP_CARTOGRAPHY = "cartography";
const QString SETTINGS_LATITUDE          = "latitude";
const QString SETTINGS_LONGITUDE         = "longitude";
const QString SETTINGS_ZOOMLEVEL         = "zoomlevel";

// IMPORT DIALOG SETTINGS
const QString SETTINGS_GROUP_IMPORTDIALOG = "importdialog";
const QString SETTINGS_IMPORTMODE         = "importmode";
const QString SETTINGS_IMPORT_SOURCEPATH  = "sourcepath";

// MAIN WINDOW SETTINGS
const QString SETTINGS_GROUP_MAINWINDOW         = "mainwindow";
const QString SETTINGS_WINDOW_LOCATION          = "location";
const QString SETTINGS_SPLITTER_FILMSTRIP_SIZES = "splitter_filmstrip";

// LIBRARY SETTINGS
const QString SETTINGS_GROUP_LIBRARY          = "library";
const QString SETTINGS_SPLITTER_LIBRARY_SIZES = "splitter_main";
const QString SETTINGS_LIBRARY_FILES_PATHITEM = "files/pathitem";

// DEVELOP SETTINGS
const QString SETTINGS_GROUP_DEVELOP          = "develop";
const QString SETTINGS_SPLITTER_DEVELOP_SIZES = "splitter_main";

// REGISTERED MODULE  NUMBERS
const int DEVELOP_SETTINGS_RAW = 1;

#endif // CONSTANTS
