#include <QDebug>

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"

namespace PhotoStage {

bool is_block_device(const std::string& path)
{
  const char* pathname = path.data();
  struct stat statbuf;

  qDebug() << "Testing path: " << path.data();
  int result = lstat(pathname, &statbuf);

  if (result != 0)
  {
    std::cerr << "Error determining if path \"" << path << "\" is a block device." << std::endl;
    return false;
  }
  qDebug() << "Path is " << statbuf.st_mode << "," << S_ISBLK(statbuf.st_mode);
  return S_ISBLK(statbuf.st_mode);
}

}
