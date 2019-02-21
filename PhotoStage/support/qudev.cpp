#include <QDebug>
#include <iostream>

#include "qudev.h"

static int t = 0;

QUDevDevice::QUDevDevice(udev_device* device) : mDevice(device)
{
  t++;
  qDebug() << "QUDevDevice created: " << t;
}

QUDevDevice::QUDevDevice(const QUDevDevice& other)
{
  t++;
  qDebug() << "copied: " << t;
  mDevice = other.mDevice;

  udev_device_ref(mDevice);
}
QUDevDevice::~QUDevDevice()
{
  t--;
  qDebug() << "QUDevDevice destroyed " << t;
  udev_device_unref(mDevice);
}

QUDevDevice& QUDevDevice::operator=(const QUDevDevice& rhs)
{
  t++;
  qDebug() << "assigned " << t;
  if (this == &rhs)
    return *this;

  mDevice = rhs.mDevice;

  udev_device_ref(mDevice);
}

QUDev::QUDev(QObject* parent) : QObject(parent)
{
  mContext = udev_new();
  if (mContext == nullptr)
    throw new std::runtime_error("Can't get access to udev");
}

QList<QUDevDevice> QUDev::getDevices()
{
  return getDevices("");
}

QList<QUDevDevice> QUDev::getDevices(const QString& filter)
{
  QList<QUDevDevice> list;

  struct udev_enumerate* enumerate;

  enumerate = udev_enumerate_new(mContext);

  qDebug() << "Scanning udev";
  if (!filter.isEmpty())
    udev_enumerate_add_match_subsystem(enumerate, filter.toUtf8().data());

  int result = udev_enumerate_scan_devices(enumerate);
  if (result != 0)
  {
    std::cerr << "Error scanning udev devices" << std::endl;
    return list;
  }
  qDebug() << "Scan complete";

  struct udev_list_entry *devices, *dev_list_entry;
  devices = udev_enumerate_get_list_entry(enumerate);
  qDebug() << " reading each item";
  udev_list_entry_foreach(dev_list_entry, devices)
  {
    const char* path;

    path                    = udev_list_entry_get_name(dev_list_entry);
    struct udev_device* dev = udev_device_new_from_syspath(mContext, path);
    qDebug() << "path: " << udev_device_get_devnode(dev);
    list << QUDevDevice(dev);
  }
  udev_enumerate_unref(enumerate);

  return list;
}

QUDev::~QUDev()
{
  udev_unref(mContext);
}
