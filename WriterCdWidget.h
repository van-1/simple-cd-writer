#ifndef WRITERCDWIDGET_H
#define WRITERCDWIDGET_H

// Qt
#include <QWidget>

// Local
class CdBurner;
class CheckableProxyModel;
class QFileSystemModel;

// Ui
namespace Ui
{
  class WriteCdWidget;
}

class WriterCdWidget : public QWidget
{
Q_OBJECT

public:
  WriterCdWidget(const QString& dir, QWidget* parent = nullptr);
  ~WriterCdWidget();

private slots:
  void on_addFilesButton_clicked();
  void on_clearFilesButton_clicked();
  void on_burnCdButton_clicked();
  void on_selectRootDirButton_clicked();
  void listDrivers(const QHash<QString,QString>& list);
  void selectedItemsChanged();
  void onError(const QString& str);
  void burnt(int result);
  void setEnabled(bool enabled);

private:
  Ui::WriteCdWidget* ui;
  CdBurner* m_cdBurner;
  QString m_dirToBurn;
  QStringList m_selectedFilesList;
  CheckableProxyModel* m_checkProxy;
  QFileSystemModel* m_model;

};

#endif // WRITERCDWIDGET_H
