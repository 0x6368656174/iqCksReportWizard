#include "sqltracelogsyntaxhighliter.h"

SqlTraceLogSyntaxHighliter::SqlTraceLogSyntaxHighliter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    m_keywordFormat.setForeground(QBrush("#808000"));
    QStringList keywordPatterns;
    keywordPatterns << "\\bSELECT\\b" << "\\bINSERT\\b" << "\\bUPDATE\\b"
                    << "\\bFROM\\b" << "\\bINTO\\b" << "\\bWHERE\\b"
                    << "\\bORDER\\b" << "\\bBY\\b" << "\\bASC\\b"
                    << "\\bDESC\\b" << "\\bLIMIT\\b" <<"\\bVALUES\\b"
                    << "\\bAS\\b" << "\\bGROUP\\b" << "\\bLEFT\\b"
                    << "\\bOUTER\\b" << "\\bJOIN\\b" << "\\bON\\b"
                    << "\\bCAST\\b" << "\\bPREPARE\\b" << "\\bDISTINCT\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    m_elapsedFormat.setForeground(QBrush("#000080"));
    rule.pattern = QRegExp("\\([^\\)]*ms\\)");
    rule.format = m_elapsedFormat;
    m_highlightingRules.append(rule);

    m_typeFormat.setForeground(QBrush("#2985C7"));
    rule.pattern = QRegExp(" \\[[^\\]]*\\]");
    rule.format = m_typeFormat;
    m_highlightingRules.append(rule);

    m_okFormat.setForeground(QBrush("green"));
    rule.pattern = QRegExp("\\bOk\\b");
    rule.format = m_okFormat;
    m_highlightingRules.append(rule);

    m_errorFormat.setForeground(QBrush("red"));
    rule.pattern = QRegExp(tr("\\bError\\b"));
    rule.format = m_errorFormat;
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("SQL Error");
    m_highlightingRules.append(rule);
}

void SqlTraceLogSyntaxHighliter::highlightBlock(const QString &text)
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
