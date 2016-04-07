#pragma once

#include <QString>
#include <QProcess>
#include <QTimer>
#include <QKeyEvent>
#include <Qt>

#include <PropellerLoader>

#include "logging.h"
#include "colorscheme.h"

#include "ui_buildmanager.h"


class BuildManager : public QFrame
{
    Q_OBJECT

    Ui::buildManager ui;

    bool failure;

public:
    explicit BuildManager(QWidget *parent = 0);
    ~BuildManager();
    void waitClose();
    void setFont(const QFont & font);
    void setTextColor(QColor color);

    typedef struct ConfigurationType
    {
        QString compiler;
        QStringList includes;
        QString file;
        QString binary;
        QString port;

        bool load;
        bool write;

        PropellerManager * manager;
    } Configuration;
    
    Configuration config;

    void setConfiguration(BuildManager::Configuration config);

signals:
    void compilerErrorInfo(QString file, int line);
    void terminalReceived(QString text);
    void statusChanged(const QString & text);
    void finished();
    void buildError();

public slots:
    void loadSuccess();
    void loadFailure();
    void compilerFinished(int exitCode, QProcess::ExitStatus status);
    void procReadyRead();
    void runProcess(const QString & programName, const QStringList & programArgs);

    void showStatus();
    void hideStatus();
    void print(const QString & text, QColor color = Qt::black);

    bool load(const QByteArray & binary = QByteArray());

public:

    QString compilerStr;
    QString includesStr;
    QString projectFile;
    QString compileResult;

    void build();
    void getCompilerOutput();

private:
    QTimer timer;

public:
    void keyPressEvent(QKeyEvent * event);

public slots:
    void updateColors();

    void setStage(int stage);
    void setText(const QString & text);
    void handleCompilerError(QProcess::ProcessError e);

private:
    void setRun(bool active);
    void setBuild(bool active);
    void setDownload(bool active);

    ColorScheme * currentTheme;
};
