#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace PhotoStage
{
PreferencesDialog::PreferencesDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    mPreferences = Preferences::instance();

    ui->cbIncludeSubdirs->setChecked(
        mPreferences->library_include_subfolders);
}

PreferencesDialog::~PreferencesDialog()
{
    mPreferences->save();

    delete ui;
}

void PreferencesDialog::onLibraryShowSubdirsClicked(bool state)
{
    mPreferences->library_include_subfolders = state;
}
}