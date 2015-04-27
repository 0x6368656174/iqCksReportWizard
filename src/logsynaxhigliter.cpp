#include "logsynaxhigliter.h"

LogSyntaxHighliter::LogSyntaxHighliter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    m_dateTimeFormat.setForeground(QBrush("#404040"));
    rule.pattern = QRegExp("^<[^>]+");
    rule.format = m_dateTimeFormat;
    m_highlightingRules.append(rule);

    m_debugFormat.setForeground(QBrush("green"));
    rule.pattern = QRegExp("Debug:");
    rule.format = m_debugFormat;
    m_highlightingRules.append(rule);

    m_warningFormat.setForeground(QBrush("blue"));
    rule.pattern = QRegExp("Warning:");
    rule.format = m_warningFormat;
    m_highlightingRules.append(rule);

    m_critiacalFormat.setForeground(QBrush("red"));
    rule.pattern = QRegExp("Critical:");
    rule.format = m_critiacalFormat;
    m_highlightingRules.append(rule);
}

void LogSyntaxHighliter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, m_highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);
}
