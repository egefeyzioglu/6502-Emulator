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
    /**
     * An individual highlighting rule
     */
    struct HighlightingRule{
        /**
         * Regex where this rule should apply
         */
        QRegularExpression pattern;
        /**
         * The formatting that should be used when this rule applies
         */
        QTextCharFormat format;
        /**
         * Order of importance of this rule
         * Higher = more important, checked later
         */
        int order;
    };

    /**
     * `HighlightingRule`s known to the syntax highlighter
     */
    std::vector<HighlightingRule> rules;
};

#endif // SYNTAXHIGHLIGHTER_H
