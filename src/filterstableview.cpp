#include "filterstableview.h"
#include "filtersmodel.h"
#include "filteritemdelegate.h"

FiltersTableView::FiltersTableView(QWidget *parent) :
    QTreeView(parent)
{
    setItemDelegate(new FilterItemDelegate(this));

    connect(this, &FiltersTableView::clicked,
            this, [this](const QModelIndex & index){
            edit(index);
    });
}

FiltersTableView::~FiltersTableView()
{

}

void FiltersTableView::setMainWindow(QMainWindow *mainWindow)
{
    FilterItemDelegate *delegate = dynamic_cast<FilterItemDelegate *>(itemDelegate());
    Q_CHECK_PTR(delegate);
    delegate->setMainWindow(mainWindow);
}


