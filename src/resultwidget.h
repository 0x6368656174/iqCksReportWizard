#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <QGroupBox>
#include <IqOrmModel>
#include "archiverecord.h"
#include <QSplitter>

namespace Ui {
class ResultWidget;
}

class ResultWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit ResultWidget(QWidget *parent = 0);
    ~ResultWidget();

    IqOrmModel<ArchiveRecord> *findModel() const;

    void loadSettings();
    void saveSettings();

private:
    void showItemText(const QModelIndex &currentIndex, const QModelIndex &previosIndex);

private:
    Ui::ResultWidget *ui;
    IqOrmModel<ArchiveRecord> *m_findModel;
    QSplitter *m_verticalSplitter;
};

#endif // RESULTWIDGET_H
