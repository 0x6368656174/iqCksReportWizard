#include "stringlistdelegatewidget.h"
#include <QHBoxLayout>
#include <QRegExpValidator>

StringListDelegateWidget::StringListDelegateWidget(QWidget *parent) :
    QLineEdit(parent),
    m_toolButton(new QToolButton(this)),
    m_editDialog(new AddressEditDialog(this))
{
    m_toolButton->setText("...");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_toolButton);
    layout->setMargin(0);
    setLayout(layout);

    connect(this, &StringListDelegateWidget::textEdited,
            this, &StringListDelegateWidget::validate);

    connect(m_toolButton, &QToolButton::clicked,
            this, &StringListDelegateWidget::showEditDialog);

    setFocusPolicy(Qt::StrongFocus);
}

StringListDelegateWidget::~StringListDelegateWidget()
{
}

void StringListDelegateWidget::setValue(const QStringList &value)
{
    if (m_value != value) {
        m_value = value;
        setText(value.join(", "));
        m_oldValidText = text();
    }
}

QStringList StringListDelegateWidget::value() const
{
    return m_value;
}

void StringListDelegateWidget::validate(const QString &text)
{
    if (text.trimmed().isEmpty()) {
        m_oldValidText = "";
        m_value = QStringList();
        updateValue("");
        return;
    }

    if (m_oldValidText.startsWith(text)) {
        m_oldValidText = text;
        setText(text);
        updateValue(text);
        return;
    }

    int oldCursorPosition = cursorPosition();

    QRegExp rx (tr("\\s*([a-z]{0,8}\\s*,\\s*)*[a-z]{0,8}\\s*"));
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (!rx.exactMatch(text)) {
        setText(m_oldValidText);
        if (!m_oldValidText.startsWith(text.mid(0, cursorPosition())))
            --oldCursorPosition;
        setCursorPosition(oldCursorPosition);
        return;
    }

    QString validText = text.toUpper();
    m_oldValidText = validText;
    setText(validText);
    setCursorPosition(oldCursorPosition);

    updateValue(validText);
}

void StringListDelegateWidget::updateValue(const QString &text)
{
    if (text.trimmed().isEmpty())
        m_value = QStringList();

    QStringList list = text.split(",");
    QStringList newValue;
    foreach (const QString &string, list) {
        QString trimmedString = string.trimmed();
        if (!trimmedString.isEmpty())
            newValue << trimmedString;
    }

    m_value = newValue;
}

void StringListDelegateWidget::showEditDialog()
{
    m_editDialog->setAddresses(m_value);
    if (m_editDialog->exec() == QDialog::Accepted) {
        setValue(m_editDialog->addresses());
    }
}

QMainWindow *StringListDelegateWidget::mainWindow() const
{
    return m_mainWindow;
}

void StringListDelegateWidget::setMainWindow(QMainWindow *mainWindow)
{
    if (m_mainWindow != mainWindow) {
        m_mainWindow = mainWindow;
    }
}

