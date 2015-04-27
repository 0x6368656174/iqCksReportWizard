#ifndef SQLTRACELOGSYNTAXHIGHLITER_H
#define SQLTRACELOGSYNTAXHIGHLITER_H

#include <QSyntaxHighlighter>

class SqlTraceLogSyntaxHighliter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    SqlTraceLogSyntaxHighliter(QObject *parent = Q_NULLPTR);

protected:
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_elapsedFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_okFormat;
    QTextCharFormat m_errorFormat;
};

#endif // SQLTRACELOGSYNTAXHIGHLITER_H
