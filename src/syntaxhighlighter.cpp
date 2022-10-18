#include "syntaxhighlighter.h"

#include <QSettings>

#include <algorithm>

#include "log.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent){

    // Get the syntax highlighting settings
    QSettings settings(":/config/syntax_highlight.ini",
                       QSettings::IniFormat);

    // Iterate over every group, every group is a different type of string that needs to be highlighted
    QStringList groups = settings.childGroups();
    for(auto& group : groups){
        settings.beginGroup(group);

        // Retrieve the settings
        QStringList keys = settings.childKeys();
        HighlightingRule new_rule;
        QTextCharFormat new_format;
        // Iterate over every key in the group, each key defines a formatting parameter or the pattern to be matched
        for(auto& key: keys){
            // Parse each key and apply the style/set the pattern
            // TODO: This is awful
            if(key == "pattern"){
                new_rule.pattern = QRegularExpression(settings.value(key).toString());
                new_rule.pattern.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);
            }else if(key == "italic"){
                new_format.setFontItalic(settings.value(key) == "yes");
            }else if(key == "weight"){
                new_format.setFontWeight(settings.value(key).toInt());
            }else if(key == "underline"){
                new_format.setFontUnderline(settings.value(key) == "yes");
            }else if(key == "colour"){
                auto val = settings.value(key);
                QBrush new_foreground;
                QColor colour;
                colour.setNamedColor(val.toString());
                new_foreground.setColor(colour);
                new_foreground.setStyle(Qt::SolidPattern);
                new_format.setForeground(new_foreground);
            }else if(key == "order"){
                new_rule.order = settings.value(key).toInt();
            }else{
                Log::Warning() << "Unknown key " << key << " in group " << group << " when parsing syntax highlighting settings. Ignoring and continuing.";
            }
        }
        new_rule.format = new_format;
        rules.push_back(new_rule);
        settings.endGroup();
    }

    // Because we're formatting based off this, ordering matters. Sort the rules.
    std::sort(rules.begin(), rules.end(), [](const HighlightingRule &a, const HighlightingRule& b) -> bool{
        return a.order < b.order;
    });
}

void SyntaxHighlighter::highlightBlock(const QString &text){
    for(HighlightingRule& rule : rules){
        // For each rule,
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        // Check if the pattern applies
        while(matchIterator.hasNext()){
            // If so, apply format
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
