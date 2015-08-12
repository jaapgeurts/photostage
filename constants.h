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

const QString SETTINGS_WINDOW_LOCATION          = "location";
const QString SETTINGS_SPLITTER_FILMSTRIP_SIZES = "splitter_filmstrip";
const QString SETTINGS_SPLITTER_LIBRARY_SIZES   = "splitter_main";
const QString SETTINGS_LIBRARY_FILES_PATHITEM   = "files/pathitem";

#endif // CONSTANTS
