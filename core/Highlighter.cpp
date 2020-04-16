#include "Highlighter.hpp"

namespace CIEAssembly
{
    CIEAsmHighlighter::CIEAsmHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
#define BOLD(format)                                                                                                                            \
    {                                                                                                                                           \
        auto _font = document()->defaultFont();                                                                                                 \
        _font.setBold(true);                                                                                                                    \
        format.setFont(_font);                                                                                                                  \
    }
        QTextCharFormat keyword_IO_Format;
        QTextCharFormat keyword_Other_Format;
        QTextCharFormat keyword_Label_Format;
        QTextCharFormat keyword_Address_Format;
        QTextCharFormat keyword_Bitwise_Format;
        QTextCharFormat keyword_Comments_Format;
        QTextCharFormat keyword_Register_Format;
        QTextCharFormat keyword_Arithmetic_Format;
        QTextCharFormat keyword_DataMovement_Format;
        QTextCharFormat keyword_Comparision_Jump_Format;
        //
        keyword_IO_Format.setForeground(QColor(Qt::red));
        keyword_Address_Format.setForeground(QColor(qRgb(60, 220, 255)));
        keyword_Label_Format.setForeground(QColor(Qt::green));
        keyword_Other_Format.setForeground(QColor(Qt::yellow));
        keyword_Bitwise_Format.setForeground(QColor(Qt::cyan));
        keyword_DataMovement_Format.setForeground(QColor(Qt::gray));
        keyword_Arithmetic_Format.setForeground(QColor(Qt::magenta));
        keyword_Register_Format.setForeground(QColor(qRgb(190, 255, 55)));
        keyword_Comparision_Jump_Format.setForeground(QColor(Qt::lightGray));
        keyword_Comments_Format.setForeground(QColor(Qt::lightGray));
        //
        BOLD(keyword_Address_Format)
        //
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"(LD[MDIXR]|ST[OXI])");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_DataMovement_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"((IN|OUT))");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_IO_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"((ADD|INC|DEC))");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Arithmetic_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"((CMP|JMP|JPE|JPN))");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Comparision_Jump_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"((AND|XOR|OR|LSL|LSR))");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Bitwise_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"(\b(IX|ACC)\b)");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Register_Format;
            highlightingRules.append(rule);
        }
        {
            HighlightingRule rule;
            rule.pattern = QRegularExpression(R"(\bEND\b)");
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Other_Format;
            highlightingRules.append(rule);
        }
        {
            // auto number = R"(#(\d+))";
            auto addressing = R"(\#(\d+|[Bb][0|1]{8}|\&[\dA-Fa-f]+))";
            HighlightingRule rule;
            rule.pattern = QRegularExpression(addressing);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Address_Format;
            highlightingRules.append(rule);
        }
        {
            auto addressing = R"(\w+:)";
            HighlightingRule rule;
            rule.pattern = QRegularExpression(addressing);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Label_Format;
            highlightingRules.append(rule);
        }
        {
            auto addressing = R"(\;.*$)";
            HighlightingRule rule;
            rule.pattern = QRegularExpression(addressing);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption |
                                           QRegularExpression::CaseInsensitiveOption);
            rule.format = keyword_Comments_Format;
            highlightingRules.append(rule);
        }
    }

    void CIEAsmHighlighter::highlightBlock(const QString &text)
    {
        for (const HighlightingRule &rule : highlightingRules)
        {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

            while (matchIterator.hasNext())
            {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        setCurrentBlockState(0);
    }
} // namespace CIEAssembly
