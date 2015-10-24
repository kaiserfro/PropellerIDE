#pragma once

#include <QIcon>
#include <QMainWindow>
#include <QSplitter>
#include <QPlainTextEdit> 
#include <QTreeView> 
#include <QDesktopServices> 
#include <QDirIterator> 

#include "ui_mainwindow.h"

#include "preferences.h"
#include "editor.h"
#include "portmonitor.h"
#include "zipper.h"
#include "filemanager.h"
#include "buildmanager.h"
#include "finder.h"
#include "projectparser.h"
#include "projectview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow ui;

public:
    MainWindow(QWidget *parent = 0);

    Preferences     *propDialog;

public slots:
    void showMessage(const QString & message);

    // file menu
    void printFile();
    void zipFiles();
    void openFiles(const QStringList & files);

    // view
    void fontBigger();
    void fontSmaller();

    // help
    void openFileResource(QString const & resource);
    void propellerManual();
    void propellerDatasheet();
    void propellerQuickReference();
    void about();

    void findMultilineComment(QPoint point);
    void findMultilineComment(QTextCursor cur);
    void spawnTerminal();
    void setProject();
    void programBuild();
    void programBurnEE();
    void programRun();
    void programDebug();
    void viewInfo();
    void recolorInfo(QWidget * widget);
    void recolorProjectView();
    void recolorBuildManager();
    void closeEvent(QCloseEvent *event);
    void quitProgram();

    void updatePortWidget(const QStringList &);
    void showBrowser();

    void addRecentFile(const QString &fileName);
    void updateRecentFileActions();
    void openRecentFile();

    void highlightFileLine(QString file, int line);
    void getApplicationSettings();
    void setEnableBuild(bool enabled);

private:
    void loadSession();
    void saveSession();
    void clearSession();

    void checkAndSaveFiles();

    bool eventFilter(QObject *target, QEvent *event);

    int  loadProgram(bool write);
    int  runCompiler();

    QString     spinCompiler;
    QString     spinIncludes;
    QString     spinTerminal;
    QString     spinLoader;

    QList<QAction *> recentFiles;

    BuildManager    builder;
    Language        language;
    ProjectParser   * parser;

    QString         projectFile;

    QComboBox   * cbPort;

    PortMonitor portMonitor;

    Zipper      zipper;
};
