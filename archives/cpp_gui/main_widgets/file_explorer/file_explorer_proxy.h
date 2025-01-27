#pragma once

#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>


/**
 * @brief This class is required to show the root directory of the current project as top directory in the QTreeView.
 * 
 * @note see https://stackoverflow.com/questions/53430989/pyside-qfilesystemmodel-display-show-root-item  
 *  
 */
class QiodeFileExplorerProxyModel: public QSortFilterProxyModel
{
    // Note: a QPersistentModelIndex is a model index that can be stored by an application, 
    //       and later used to access information in a model. 
    //       Unlike the QModelIndex class, it is safe to store a QPersistentModelIndex 
    //       since the model will ensure that references to items will continue to be valid 
    //       as long as they can be accessed by the model.
    //       See https://doc.qt.io/qt-6/qpersistentmodelindex.html  
    QPersistentModelIndex projectIndex;

public:
    QiodeFileExplorerProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

    void setProjectIndex(const QPersistentModelIndex& projectIndex)
    {
        this->projectIndex = projectIndex;
        invalidateFilter();
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        if(!projectIndex.isValid()) return false;
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        // hide all folders at the same level of the project directory
        if(sourceParent == projectIndex.parent()) 
            return index == projectIndex;
        else
        {   QFileInfo file_info = static_cast<QFileSystemModel*>(sourceModel())->fileInfo(index);
            if (file_info.suffix() == "ini") 
                return false;
            else
                return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
        }
    }

    QModelIndex index(const QString &path, int column = 0) const
    {
        return mapFromSource(static_cast<QFileSystemModel*>(sourceModel())->index(path, column));
    }

    bool isDir(const QModelIndex &proxyIndex) const
    {
        return static_cast<QFileSystemModel*>(sourceModel())->isDir(mapToSource(proxyIndex));
    }

    QString filePath(const QModelIndex &proxyIndex) const
    {
        return static_cast<QFileSystemModel*>(sourceModel())->filePath(mapToSource(proxyIndex));
    }

    QFileInfo fileInfo(const QModelIndex &proxyIndex) const
    {
        return static_cast<QFileSystemModel*>(sourceModel())->fileInfo(mapToSource(proxyIndex));
    }
};
