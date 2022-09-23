#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>

#include <vector>



class SyntaxHighlighter : public QSyntaxHighlighter {

public:
    SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;
private:
    struct HighlightingRule{
        //HighlightingRule(QRegularExpression pattern, QTextCharFormat format) : pattern(pattern), format(format){}
        QRegularExpression pattern;
        QTextCharFormat format;
        int order;
    };

    std::vector<HighlightingRule> rules;
};

#endif // SYNTAXHIGHLIGHTER_H
