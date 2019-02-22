#include "importbackgroundtask.h"
#include "utils.h"
#include <QtConcurrent>

namespace PhotoStage {
ImportBackgroundTask::ImportBackgroundTask(const ImportInfo& info)
    : BackgroundTask("Import Task"), mInfo(info)

{
  setDescription(QString("Importing..."));

  setAutoDelete(false);
}

int ImportBackgroundTask::progressMinimum()
{
  return 0;
}

int ImportBackgroundTask::progressMaximum()
{
  return mInfo.sourceFiles.size() - 1;
}

void ImportBackgroundTask::run()
{
  qDebug() << "Importer threadId in run:" << this->thread()->currentThreadId();

  int i = 0;

  mLastPath.clear();
  mLastPathId = -1;

  // create the new Import Collection

  bool      complete = false;
  long long importcollectionid;
  QMetaObject::invokeMethod(qApp, [this, &complete, &importcollectionid]() {
    importcollectionid = DatabaseAccess::collectionDao()->addImportCollection(
        mInfo.sourceFiles.size());
    complete = true;
  });
  // wait for the code to have completed
  while (!complete)
    ;

  QListIterator<QFileInfo> it(mInfo.sourceFiles);

  while (it.hasNext() && running())
  {
    QFileInfo fileInfo = it.next();

    try
    {
      ExifInfo  exifInfo = preparePhotoImport(fileInfo, mInfo.options);
      long long hash     = computeImageFileHash(fileInfo);

      complete = false;
      QMetaObject::invokeMethod(qApp, [exifInfo, importcollectionid,
                                       this, hash, fileInfo, &complete] {
        qDebug() << "ThreadId in lambda: " << QThread::currentThreadId();
        DatabaseAccess::photoDao()->importPhoto(importcollectionid, mLastPathId,
                                                fileInfo.fileName(), hash,
                                                exifInfo);
        complete = true;
      });
      while (!complete)
        ;
    }
    catch (const std::runtime_error& error)
    {
      // TODO: put error somewhere
      std::cerr << "Can't import file" << std::endl;
    }
    i++;
    emit progressUpdated(i);
  }

  emit taskFinished(this);
}

ExifInfo ImportBackgroundTask::preparePhotoImport(const QFileInfo&     fileInfo,
                                                  const ImportOptions& options)
{
  // TODO :move this section to the importer
  qDebug() << "Importing file" << fileInfo.canonicalFilePath();
  QString fileName = fileInfo.fileName();
  QString srcpath  = fileInfo.canonicalFilePath();
  QString dstdir   = options.destinationDir.canonicalFilePath();
  QString dstpath  = dstdir + QDir::separator() + fileName;

  bool copySuccess = false;

  // take action on the file(in case of copy & move)
  switch (options.importMode)
  {
    case ImportOptions::ImportAdd:
      // do nothing. just import the filepaths into the DB
      dstpath     = srcpath;
      dstdir      = fileInfo.canonicalPath();
      copySuccess = true;
      break;

    case ImportOptions::ImportCopy:

      copySuccess = QFile::copy(srcpath, dstpath);

      if (!copySuccess)
        qDebug() << "File copy failed from" << srcpath << "to" << dstpath;
      break;

    case ImportOptions::ImportMove:
      // move each file
      copySuccess = QFile::rename(
          srcpath, dstpath); // rename moves if on different filesystems

      if (!copySuccess)
        qDebug() << "File move failed from" << srcpath << "to" << dstpath;
      break;
  }

  if (!copySuccess)
    throw new std::runtime_error("Copy failed");

  if (mLastPath != dstdir)
  {
    mLastPath = dstdir;
    // TODO: find way to prevent this call
    QStringList pathlist =
        dstdir.split(QDir::separator(), QString::KeepEmptyParts);
    bool completed = false;
    QMetaObject::invokeMethod(qApp,[this,&pathlist,&completed](){
      mLastPathId = DatabaseAccess::pathDao()->createPaths(pathlist);
      completed = true;
    });
    while (!completed)
      ;
  }

  // read all exif data.
  ExivFacade* ex = ExivFacade::createExivReader();

  ExifInfo ei;

  if (!ex->openFile(srcpath))
    qDebug() << "Skipping exif info";
  else
    ei = ex->data();
  delete (ex);

  if (ei.dateTimeOriginal == nullptr)
    ei.dateTimeOriginal = fileInfo.lastModified();

  if (ei.dateTimeDigitized == nullptr)
    ei.dateTimeDigitized = fileInfo.created();

  return ei;
}

} // namespace PhotoStage
