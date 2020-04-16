#pragma once

#include "Common.hpp"

#include <QMap>
#include <QMetaEnum>
#include <QObject>
#include <QStringList>
#include <optional>

namespace CIEAssembly
{
    typedef QList<CIEAssemblyInstruction> CIEAssemblyCodeModel;
//
#define ACC MEMORY["ACC"]
#define IX MEMORY["IX"]
    inline CIEAssemblyCompareResult COMPARE_RESULT;
    //
    inline QMap<QString, char> MEMORY;
    inline CIEAssemblyCodeModel CODE_MODEL;
    inline NumberBase Base = BASE10;
    //
    [[nodiscard]] int FindOffsetByLabel(const QString &label);
    QString NumberToString(int num);
    //
    [[nodiscard]] QString ExecuteSingleInstruction(const CIEAssemblyInstruction &instruction, QString *errorMessage, QStringList *changedMemory);
    CIEAssemblyOperandType DetectNumberType(const QString &operand);
    CIEAssemblyOperandType DeduceOperandType(const CIEAssemblyInstruction &instruction);
    //
    QStringList GetLabels(const QString &code);
    void ParseAssemblyCode(const QString &code, QString *errorMessage);
} // namespace CIEAssembly

using namespace CIEAssembly;
