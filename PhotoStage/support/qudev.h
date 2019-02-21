#ifndef QUDEV_H
#define QUDEV_H

#include <memory>

#include <QObject>

#include <libudev.h>

class QUDevDevice
{
public:
  explicit QUDevDevice(udev_device* device);
  QUDevDevice(const QUDevDevice& other);
  ~QUDevDevice();

  QUDevDevice& operator=(const QUDevDevice& rhs);

private:
  udev_device* mDevice;
};

class QUDev : public QObject
{
  Q_OBJECT
public:
  explicit QUDev(QObject* parent = nullptr);
  ~QUDev();

  QList<QUDevDevice> getDevices();
  QList<QUDevDevice> getDevices(const QString& filter);

signals:

public slots:

private:
  struct udev* mContext;
};

#endif // QUDEV_H
