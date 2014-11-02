#include "terminal2.h"
#include "clock.h"
#include "graphwidget.h"
#include "graphline.h"

//#if defined(Q_WS_WIN32)
#define TERM_ENABLE_BUTTON
//#endif

Terminal2::Terminal2(QWidget *parent) : QDialog(parent), portListener(NULL)
{
    termEditor = new Console(parent);
    init();
}

void Terminal2::init()
{
    QVBoxLayout *termLayout = new QVBoxLayout();
    termLayout->setContentsMargins(4,4,4,4);
    termEditor->setReadOnly(false);

    options = new TermPrefs(this);

    QAction *copyAction = new QAction(tr("Copy"),this);
    copyAction->setShortcuts(QKeySequence::Copy);
    termEditor->addAction(copyAction);
    QAction *pasteAction = new QAction(tr("Paste"),this);
    pasteAction->setShortcuts(QKeySequence::Paste);
    termEditor->addAction(pasteAction);
    termEditor->setFont(QFont("Parallax", 14, QFont::Bold));
    termEditor->setMaxRows(options->getMaxRows());

#ifdef ENABLE_TERM_TABS
    GraphLine *graphLine = new GraphLine(this);
    connect(termEditor, SIGNAL(queueGraph(QString)), graphLine, SLOT(queueGraph(QString)));

    Clock     *termClock = new Clock(this);

    termTabs = new QTabWidget(this);
    //termTabs->setStyleSheet("QWidget { background-color: #f0e0c0; }"); // better without it.
    termTabs->setMovable(true);
    termTabs->setTabsClosable(false);
    termTabs->addTab(termEditor,tr(TabConsole));
    termTabs->addTab(new GraphWidget(graphLine, graphLine->getTools(), this),tr(TabLineGraph));
    termTabs->addTab(new GraphWidget(termClock, termClock->getTools(), this),tr(TabClock));
    termEditor->setTabWidget(termTabs);
    termLayout->addWidget(termTabs);

    termTabs->setCurrentIndex(1);
#else
    termLayout->addWidget(termEditor);
#endif

    QPushButton *buttonClear = new QPushButton(tr("Clear"),this);
    connect(buttonClear,SIGNAL(clicked()), this, SLOT(clearScreen()));
    buttonClear->setAutoDefault(false);
    buttonClear->setDefault(false);

    comboBoxBaud = new QComboBox(this);
    comboBoxBaud->setMinimumContentsLength(7);
    comboBoxBaud->addItem("115200", QVariant(BAUD115200));
    comboBoxBaud->addItem("57600", QVariant(BAUD57600));
    comboBoxBaud->addItem("38400", QVariant(BAUD38400));
    comboBoxBaud->addItem("19200", QVariant(BAUD19200));
    comboBoxBaud->addItem("9600", QVariant(BAUD9600));
    comboBoxBaud->addItem("4800", QVariant(BAUD4800));
    comboBoxBaud->addItem("2400", QVariant(BAUD2400));
    comboBoxBaud->addItem("1200", QVariant(BAUD1200));

    int baud = getBaudRate();
    for(int n = comboBoxBaud->count()-1; n > -1; n--) {
        QString bs;
        bs = comboBoxBaud->itemData(n).toString();
        if(bs.toInt() == baud) {
            comboBoxBaud->setCurrentIndex(n);
            break;
        }
    }
    connect(comboBoxBaud,SIGNAL(currentIndexChanged(int)),this,SLOT(baudRateChange(int)));

    cbEchoOn = new QCheckBox(tr("Echo On"),this);
    connect(cbEchoOn,SIGNAL(clicked(bool)),this,SLOT(echoOnChange(bool)));

    setEchoOn(options->getEchoOn());

    QPushButton *buttonOpt = new QPushButton(tr("Options"),this);
    connect(buttonOpt,SIGNAL(clicked()), this, SLOT(showOptions()));
    buttonOpt->setAutoDefault(false);
    buttonOpt->setDefault(false);

#ifdef TERM_ENABLE_BUTTON
    buttonEnable = new QPushButton(tr("Disable"),this);
    connect(buttonEnable,SIGNAL(clicked()), this, SLOT(toggleEnable()));
    buttonEnable->setAutoDefault(false);
    buttonEnable->setDefault(false);
#endif

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QPushButton* button = buttonBox->button(QDialogButtonBox::Ok);
    button->setAutoDefault(true);
    button->setDefault(true);

    QHBoxLayout *butLayout = new QHBoxLayout();
    termLayout->addLayout(butLayout);
    butLayout->addWidget(buttonClear);
    butLayout->addWidget(buttonOpt);
#ifdef TERM_ENABLE_BUTTON
    butLayout->addWidget(buttonEnable);
#endif
    butLayout->addWidget(comboBoxBaud);
    butLayout->addWidget(&portLabel);
    butLayout->addWidget(cbEchoOn);
    butLayout->addWidget(buttonBox);
    setLayout(termLayout);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/images/console.png"));
    resize(800,600); // initial size

    //show();
}

Console *Terminal2::getEditor()
{
    return termEditor;
}

void Terminal2::setPortListener(PortListener *listener)
{
    portListener = listener;
    QString portName = portListener->getPortName();
    if(portName.length())
        portLabel.setText(portListener->getPortName());
    else
        portLabel.setText("");
}

QString Terminal2::getPortName()
{
    return portLabel.text();
}

void Terminal2::setPortName(QString name)
{
    portLabel.setText(name);
}

void Terminal2::setPosition(int x, int y)
{
    int width = this->width();
    int height = this->height();
    this->setGeometry(x,y,width,height);
}

void Terminal2::baudRateChange(int index)
{
    QVariant var = comboBoxBaud->itemData(index);
    bool ok;
    int baud = var.toInt(&ok);
    portListener->init(portListener->getPortName(), (BaudRateType) baud);
    options->saveBaudRate(baud);
}

BaudRateType Terminal2::getBaud()
{
    int index = comboBoxBaud->currentIndex();
    QVariant var = comboBoxBaud->itemData(index);
    bool ok;
    int baud = var.toInt(&ok);
    if(ok) {
        return (BaudRateType) baud;
    }
    else {
        return BAUD115200;
    }
}

int Terminal2::getBaudRate()
{
    return options->getBaudRate();
}

bool Terminal2::setBaudRate(int baud)
{
    for(int n = comboBoxBaud->count(); n > -1; n--) {
        if(comboBoxBaud->itemData(n) == baud) {
            comboBoxBaud->setCurrentIndex(n);
            return true;
        }
    }
    return false;
}

void Terminal2::echoOnChange(bool value)
{
    termEditor->setEnableEchoOn(value);
    options->saveEchoOn(value);
}

void Terminal2::setEchoOn(bool echoOn)
{
    termEditor->setEnableEchoOn(echoOn);
    cbEchoOn->setChecked(echoOn);
}

void Terminal2::accept()
{
#ifdef TERM_ENABLE_BUTTON
    buttonEnable->setText("Disable");
#endif
    // save Terminal2 geometry
    QSettings *settings = new QSettings(publisherKey, PropellerIdeGuiKey, this);
    if(settings->value(useKeys).toInt() == 1) {
        QByteArray geo = this->saveGeometry();
        settings->setValue(termGeometryKey,geo);
    }
    termEditor->setPortEnable(false);
    portLabel.setEnabled(false);
    portListener->stop();
    done(QDialog::Accepted);
}

void Terminal2::reject()
{
#ifdef TERM_ENABLE_BUTTON
    buttonEnable->setText("Disable");
#endif
    // save Terminal2 geometry
    QSettings *settings = new QSettings(publisherKey, PropellerIdeGuiKey, this);
    QByteArray geo = this->saveGeometry();
    settings->setValue(termGeometryKey,geo);
    termEditor->setPortEnable(false);
    portLabel.setEnabled(false);
    portListener->stop();
    done(QDialog::Rejected);
}

void Terminal2::clearScreen()
{
    termEditor->clear();
}

void Terminal2::toggleEnable()
{
#ifdef TERM_ENABLE_BUTTON
    if(buttonEnable->text().contains("Enable",Qt::CaseInsensitive)) {
        buttonEnable->setText("Disable");
        termEditor->setPortEnable(true);
        //portListener->open(); don't change port status
        portLabel.setText(portListener->getPortName());
        portLabel.setEnabled(true);
        termEditor->setFocus(Qt::OtherFocusReason);
        emit disablePortCombo();
    }
    else {
        buttonEnable->setText("Enable");
        termEditor->setPortEnable(false);
        //portLabel.setText("");
        portLabel.setEnabled(false);
        //portListener->close(); don't change port status
        emit enablePortCombo();
    }
    QApplication::processEvents();
#endif
}

void Terminal2::setPortEnabled(bool value)
{
    if(value) {
#ifdef TERM_ENABLE_BUTTON
        buttonEnable->setText("Disable");
#endif
        termEditor->setPortEnable(true);
        portLabel.setEnabled(true);
        this->portLabel.setText(portListener->getPortName());
    }
    else {
#ifdef TERM_ENABLE_BUTTON
        buttonEnable->setText("Enable");
#endif
        termEditor->setPortEnable(false);
        portLabel.setEnabled(false);
        this->portLabel.setText("NONE");
    }
    QApplication::processEvents();
}

void Terminal2::setPortLabelEnable(bool enable)
{
    portLabel.setEnabled(enable);
}

void Terminal2::copyFromFile()
{
    termEditor->copy();
}
void Terminal2::cutFromFile()
{
    termEditor->cut();
}
void Terminal2::pasteToFile()
{
    termEditor->paste();
}

void Terminal2::showOptions()
{
    options->showDialog();
}
