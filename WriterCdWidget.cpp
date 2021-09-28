// Local
#include "WriterCdWidget.h"
#include "CdBurner.h"
#include "qtutils/checkableproxymodel.h"
#include "qtutils/modelutils.h"

// Ui
#include "ui_writecdwidget.h"

// Qt
#include <QDebug>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QFileDialog>

WriterCdWidget::WriterCdWidget(const QString& dir, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::WriteCdWidget())
  , m_cdBurner(new CdBurner(dir, this))
  , m_dirToBurn(dir)
{
  ui->setupUi(this);
  ui->burnCdButton->setIcon(QIcon(QStringLiteral(":media-optical-burn.png")));
  ui->burnCdButton->setEnabled(false);
  ui->addFilesButton->setIcon(QIcon(QStringLiteral(":add.png")));
  ui->clearFilesButton->setIcon(QIcon(QStringLiteral(":edit-clear.png")));

  m_model = new QFileSystemModel(this);
  m_checkProxy = new CheckableProxyModel(this);
  m_checkProxy->setSourceModel(m_model);
  ui->allFilesTreeView->setModel(m_checkProxy);
  m_checkProxy->setDefaultCheckState(false);
  auto rootIndex = m_model->setRootPath(m_dirToBurn);
  rootIndex = m_checkProxy->mapFromSource(rootIndex);
  ui->allFilesTreeView->setRootIndex(rootIndex);
  ui->allFilesTreeView->hideColumn(2);
  ui->allFilesTreeView->hideColumn(3);

  connect(m_cdBurner, &CdBurner::burnt, this, &WriterCdWidget::burnt);
  connect(m_cdBurner, &CdBurner::error, this, &WriterCdWidget::onError);
  connect(m_cdBurner, &CdBurner::listDrivers, this, &WriterCdWidget::listDrivers);
  connect(m_cdBurner, &CdBurner::burnProgress, ui->progressBar, &QProgressBar::setValue);
  connect(m_checkProxy, &CheckableProxyModel::checkedNodesChanged, this, &WriterCdWidget::selectedItemsChanged);
}

WriterCdWidget::~WriterCdWidget()
{
  delete ui;
}

void WriterCdWidget::on_addFilesButton_clicked()
{
  if (ui->allFilesTreeView->model()->rowCount() > 0)
    ModelUtils::resetCheckedStateCheckableProxyModel(static_cast<CheckableProxyModel*>(ui->allFilesTreeView->model()),
                                                     true, ui->allFilesTreeView->model()->index(0, 0));
}

void WriterCdWidget::on_clearFilesButton_clicked()
{
  if (ui->allFilesTreeView->model()->rowCount() > 0)
    ModelUtils::resetCheckedStateCheckableProxyModel(static_cast<CheckableProxyModel*>(ui->allFilesTreeView->model()),
                                                     false, ui->allFilesTreeView->model()->index(0, 0));
}

void WriterCdWidget::listDrivers(const QHash<QString, QString>& list)
{
  qInfo() << "new lits of cdrw/dvdrw drivers" << list;
  ui->driversComboBox->clear();
  for (auto it = list.cbegin(); it != list.cend(); ++it)
    ui->driversComboBox->addItem(it.value(), it.key());

  if (list.isEmpty())
    ui->burnCdButton->setEnabled(false);
  else
   m_selectedFilesList.isEmpty() ? ui->burnCdButton->setEnabled(false) :
                                    ui->burnCdButton->setEnabled(true);
}

void WriterCdWidget::selectedItemsChanged()
{
  m_selectedFilesList.clear();
  m_selectedFilesList = ModelUtils::extractFilesFromCheckableProxyModel(static_cast<CheckableProxyModel*>(ui->allFilesTreeView->model()),
                                                                         ui->allFilesTreeView->model()->index(0, 0));

  qDebug() << "selected files" << m_selectedFilesList;

  if (!m_selectedFilesList.isEmpty() && ui->driversComboBox->count() > 0)
    ui->burnCdButton->setEnabled(ui->allFilesTreeView->isEnabled() ? true : false);
  else
    ui->burnCdButton->setEnabled(false);
}

void WriterCdWidget::on_burnCdButton_clicked()
{
  if (!m_selectedFilesList.isEmpty() && ui->driversComboBox->count() > 0)
  {
    QString dev_name = ui->driversComboBox->currentData().toString();
    if (QMessageBox::warning(this, tr("Cd burn"),
                                       tr("All data on disk will be erased."
                                          "\nDo you really want to burn current disc?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
      setEnabled(false);
      m_cdBurner->burnCd(dev_name, m_selectedFilesList);
    }
  }
}

void WriterCdWidget::on_selectRootDirButton_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly);
  ui->rootDirLineEdit->setText(dir);
  m_dirToBurn = dir;
  m_cdBurner->setRootDir(m_dirToBurn);
  m_checkProxy->setDefaultCheckState(false);
  auto rootIndex = m_model->setRootPath(m_dirToBurn);
  rootIndex = m_checkProxy->mapFromSource(rootIndex);
  ui->allFilesTreeView->setRootIndex(rootIndex);
}

void WriterCdWidget::onError(const QString& str)
{
  qDebug() << str;
}

void WriterCdWidget::burnt(int result)
{
  if (result == 0)
  {
    QMessageBox::information(this, tr("Cd burnt"), tr("Cd burnt successfully."));
  }
  else
  {
    QMessageBox::critical(this, tr("Cd burnt"), tr("Cd burning failed"));
  }

  on_clearFilesButton_clicked();
  setEnabled(true);
}

void WriterCdWidget::setEnabled(bool enabled)
{
  ui->addFilesButton->setEnabled(enabled);
  ui->clearFilesButton->setEnabled(enabled);
  ui->allFilesTreeView->setEnabled(enabled);
  ui->driversComboBox->setEnabled(enabled);
  ui->burnCdButton->setEnabled(enabled);
  ui->progressBar->setValue(0);
  ui->rootDirLineEdit->setEnabled(enabled);
  ui->selectRootDirButton->setEnabled(enabled);
}
