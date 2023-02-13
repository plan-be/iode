#pragma once

#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>


class SystemItem
{
    QFileInfo sourceInfo;
    bool cut;
    bool _isDir;
    bool _isFile;
    bool overwrite_all;

private:
    QStringList recursiveEntryList(const QDir& sourceDir)
    {
        QStringList entryList;

        QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        foreach(QFileInfo fileInfo, fileInfoList)
        {
            QString path = fileInfo.absoluteFilePath();
            // if it is a directory, go deeper
            if(fileInfo.isDir())
            {   
                entryList << path;
                entryList << recursiveEntryList(QDir(path));
            }
            else
                entryList << path;
        }

        return entryList;
    }

    bool pasteFile(const QDir& destinationDir)
    {
        // --- build absolute source path
        const QDir sourceDir = sourceInfo.absoluteDir();
        const QString sourcePath = sourceInfo.absoluteFilePath();

        // --- build absolute destination path
        QString destinationPath = destinationDir.filePath(sourceInfo.fileName());

        // --- cut and paste in the same directory -> nothing to do
        if(cut && sourcePath == destinationPath) return true;

        // --- copy and paste in the same directory -> add _copy to stem
        QString fileName;
        if (sourceDir == destinationDir)
        {
            QString stem = sourceInfo.baseName();
            stem += "_copy";
            fileName = stem + "." + sourceInfo.suffix();
        }
        else fileName = sourceInfo.fileName();

        // --- build destination path
        destinationPath = destinationDir.filePath(fileName);

        // --- check if already exist before to paste
        if(QFileInfo(destinationPath).exists())
        {
            QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "Warning", "File " + 
            fileName + " already exists in directory " + destinationDir.absolutePath() + " !\n Overwrite it ?", 
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
            if (answer == QMessageBox::Cancel) return false;
            else QFile(destinationPath).remove();
        }

        // --- actually paste the file 
        bool success = QFile::copy(sourcePath, destinationPath);

        // --- delete source file if method cut() was used
        if(cut) QFile(sourcePath).remove();

        return success;
    }

    bool copyRecursiveDir(QDir& sourceDir, QDir& targetDir)
    {
        // if the target directory doesn't exist, build it
        if(!targetDir.exists())
        {
            QFileInfo targetInfo(targetDir.absolutePath());
            QDir parentDir = targetInfo.absoluteDir();
            if(!parentDir.mkdir(targetDir.dirName())) return false;
        }

        // recursive copy of files and sub-directories
        QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        QString fileName;
        foreach(QFileInfo fileInfo, fileInfoList)
        {
            QString fileName = fileInfo.fileName();

            // if it is a directory, copy recursively
            if(fileInfo.isDir())
            {   
                QDir sourceSubdir(fileInfo.absoluteFilePath());
                QDir targetSubdir(targetDir.filePath(sourceSubdir.dirName())); 
                if(!copyRecursiveDir(sourceSubdir, targetSubdir)) return false;
                sourceDir.rmdir(sourceSubdir.dirName());
            }
            // copy file
            else
            {
                QString targetFilePath = targetDir.filePath(fileName);

                if (!overwrite_all && QFileInfo(targetFilePath).exists())
                {
                    QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "Warning", "File " + 
                    fileInfo.fileName() + " already exist!\n Overwrite it ?", 
                    QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Discard, QMessageBox::Yes);
                    if(answer == QMessageBox::YesToAll) overwrite_all = true;
                    if(answer == QMessageBox::Discard) return false;
                }

                // if a file with the name newName already exists, QFile::copy() returns false
                // -> we need to delete the already existing file first
                if(QFileInfo(targetFilePath).exists()) targetDir.remove(fileInfo.fileName());

                // copy file
                if(!QFile::copy(fileInfo.filePath(), targetFilePath)) return false;
                // if cut -> remove source file
                if (cut) sourceDir.remove(fileName);
            }
        }
        return true;
    }

    bool pasteDir(QDir& destinationDir)
    {
        // reset overwrite_all
        overwrite_all = false;
        
        // --- build (parent) source dirs
        QDir sourceDir(sourceInfo.absoluteFilePath());
        QDir parentDir = sourceInfo.absoluteDir();

        // --- get dir name
        QString dirName = sourceDir.dirName();

        // --- [cut]copy/paste in the same parent directory
        if(parentDir.absolutePath() == destinationDir.absolutePath())
        {
            // cut and paste in the same parent directory -> nothing to do
            if (cut) return true;

            // copy and paste in the same parent directory -> add _copy to the directory name
            dirName += "_copy"; 
        }

        // --- prepare target dir
        QDir targetDir(destinationDir.filePath(dirName));

        // --- check if already exist before to paste
        if(targetDir.exists())
        {
            QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "Warning", "Directory " + 
            targetDir.absolutePath() + " already exists!\n Overwrite it ?", 
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
            if (answer == QMessageBox::Cancel) return false;
            else removeDir(targetDir.absolutePath());
        }

        // --- actually paste the directory 
        bool success = copyRecursiveDir(sourceDir, targetDir);

        // --- delete source directory if method cut() was used
        if(cut) removeDir();

        return success;
    }

    bool removeFile(const QString& filePath = "")
    {
        QFileInfo fileInfo = (filePath.isEmpty()) ? sourceInfo : QFileInfo(filePath);
        return QFile(fileInfo.absoluteFilePath()).moveToTrash();
    }

    bool removeDir(const QString& dirPath = "")
    {
        QFileInfo dirInfo = (dirPath.isEmpty()) ? sourceInfo : QFileInfo(dirPath);
        if(!dirInfo.exists()) return false;
        QDir sourceDir(dirInfo.absoluteFilePath());

        QDir parentDir = sourceDir;
        // if no parent directory, do nothing (Don't delete C:\\ !)
        if(!parentDir.cdUp()) return false;
        // remove all files and sub-directories
        if(!sourceDir.removeRecursively()) return false;

        // remove remaining empty directory
        // Note: according to Qt documentation, the deleted directory 
        //       must be empty before to call rmdir()
        return parentDir.rmdir(sourceDir.dirName());
    }

public:
    SystemItem(const QFileInfo& sourceInfo, const bool cut = false) : sourceInfo(sourceInfo), cut(cut), overwrite_all(false)
    {
        _isDir = sourceInfo.isDir();
        _isFile = sourceInfo.isFile();
    }

    bool isDir() const { return _isDir; }

    bool isFile() const { return _isFile; }

    QFileInfo fileInfo() const { return sourceInfo; }

    QString absoluteFilePath() const
    {
        return sourceInfo.absoluteFilePath();
    }

    QStringList recursiveEntryList()
    {
        return recursiveEntryList(QDir(sourceInfo.absoluteFilePath()));
    }

    // required for "Save Project As" option in File menu
    bool copyTo(QDir& targetDir)
    {
        bool success  = false;
        bool cut_save = cut;
        cut = false;
        if (_isFile) success = pasteFile(targetDir);
        if (_isDir)
        {
            QDir currentDir(sourceInfo.absoluteFilePath());
            success = copyRecursiveDir(currentDir, targetDir);
        }
        cut = cut_save;
        return success;
    }

    bool paste(QDir& destinationDir)
    {
        if(_isFile) return pasteFile(destinationDir);
        if(_isDir) return pasteDir(destinationDir);
        // not a directory or a file (e.g. symbolic link, executable, ...)
        return false;
    }

    bool remove()
    {
        if(_isFile) return removeFile();
        if(_isDir) return removeDir();
        // not a directory or a file (e.g. symbolic link, executable, ...)
        return false;
    }
};
