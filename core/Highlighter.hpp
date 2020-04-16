#pragma once
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
namespace CIEAssembly
{
    class CIEAsmHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT

      public:
        explicit CIEAsmHighlighter(QTextDocument *parent = nullptr);

      protected:
        void highlightBlock(const QString &text) override;

      private:
        struct HighlightingRule
        {
            QRegularExpression pattern;
            QTextCharFormat format;
        };
        QVector<HighlightingRule> highlightingRules;
    };
} // namespace CIEAssembly
