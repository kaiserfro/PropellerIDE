#pragma once

class Terminal;

#include <QDialog>
#include <QWidget>
#include <QString>

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>

#include "Console.h"
#include "TermPrefs.h"

class Terminal : public QDialog
{
    Q_OBJECT
public:
    explicit Terminal(QWidget *parent);
    QString getPortName();
    void setPortName(QString name);
    void setPosition(int x, int y);
    void accept();
    void reject();

    QSerialPort::BaudRate getBaud();
    int  getBaudRate();
    bool setBaudRate(int baud);
    void setEchoOn(bool echoOn);
    void setPortLabelEnable(bool enable);

signals:
    void enablePortCombo();
    void disablePortCombo();

private:
    void init();

public slots:
    void baudRateChange(int index);
    void echoOnChange(bool value);
    void toggleEnable();
    void setPortEnabled(bool value);
    void clearScreen();
    void copyFromFile();
    void cutFromFile();
    void pasteToFile();
    void showOptions();

public:
    Console *getEditor();
private:
    Console     *termEditor;
    TermPrefs   *options;
    QComboBox   *comboBoxBaud;
    QCheckBox   *cbEchoOn;
    QLabel      portLabel;

private:
    QPushButton     *buttonEnable;
};
