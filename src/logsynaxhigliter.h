#ifndef LOGSYNAXHIGLITER_H
#define LOGSYNAXHIGLITER_H

#include <QObject>

#include <QSyntaxHighlighter>

class LogSyntaxHighliter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    LogSyntaxHighliter(QObject *parent = Q_NULLPTR);

protected:
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;

    QTextCharFormat m_dateTimeFormat;
    QTextCharFormat m_debugFormat;
    QTextCharFormat m_warningFormat;
    QTextCharFormat m_critiacalFormat;
};

#endif // LOGSYNAXHIGLITER_H
