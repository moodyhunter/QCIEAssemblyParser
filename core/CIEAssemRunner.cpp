#include "CIEAssemRunner.hpp"

#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>

namespace CIEAssembly
{
    void ParseAssemblyCode(const QString &code, QString *errorMessage)
    {
        CIEAssemblyCodeModel instructions;
        auto lines = code.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
        //
        QString lastLabel = "_init_";
        int labelOffset = 0;
        for (const auto &_line : lines)
        {
            auto pureLine = _line.mid(0, _line.indexOf(";")).trimmed();
            auto splited = pureLine.split(" ", Qt::SkipEmptyParts);
            if (pureLine.isEmpty() || splited.isEmpty())
            {
                continue;
            }
            //
            if (splited.count() == 1 && splited.first().contains(":"))
            {
                // Remove the rightmost ":" symbol.
                lastLabel = splited.first().trimmed().chopped(1).trimmed();
                labelOffset = 0;
            }
            else
            {
                auto x = StringToEnum<CIEAssemblyOpcode>(splited.first());
                if (x < 0)
                {
                    *errorMessage = "\"" + splited.first() + "\" is not a valid CIE assembly opcode.";
                    return;
                }
                CIEAssemblyInstruction instruction;
                instruction.opcode = x;
                // For IN and OUT, or those opcode without operands.
                instruction.operand = splited.count() == 1 ? "" : splited.at(1);
                instruction.label = lastLabel + (labelOffset == 0 ? "" : ("+" + QString::number(labelOffset)));
                //
                instructions.append(instruction);
                labelOffset++;
            }
        }
        //
        CODE_MODEL = instructions;
    }

    int FindOffsetByLabel(const QString &label)
    {
        for (auto i = 0; i < CODE_MODEL.count(); i++)
        {
            if (CODE_MODEL.at(i).label == label)
            {
                return i;
            }
        }
        return -1;
    }

    QStringList GetLabels(const QString &code)
    {
        QStringList labels;
        auto lines = code.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
        //
        QString lastLabel = "_init_";

        for (auto _line : lines)
        {
            auto double_trimmed = _line.remove(" ");
            double_trimmed = double_trimmed.trimmed();
            if (double_trimmed.endsWith(":"))
            {
                labels.append(double_trimmed.chopped(1).trimmed());
            }
        }
        return labels;
    }

    CIEAssemblyOperandType DetectNumberType(const QString &operand)
    {
        if (operand.toLower().startsWith("#b"))
            return NUMBER_BASE2;
        else if (operand.startsWith("#&"))
            return NUMBER_BASE16;
        else if (operand.startsWith("#"))
            return NUMBER_BASE10;
        else
            return INVALID_OPERAND;
    }

    CIEAssemblyOperandType DeduceOperandType(const CIEAssemblyInstruction &instruction, QString *errorMessage)
    {
        const auto &operand = instruction.operand;
        switch (instruction.opcode)
        {
            case LDM:
            case LDR:
            case LSL:
            case LSR:
            {
                if (operand.startsWith("#"))
                {
                    return DetectNumberType(operand);
                }
                else
                {
                    LOG("LDM, LDR, LSL, LSR expect a number as operand.");
                    *errorMessage = "LDM, LDR, LSL, LSR expect a number as operand.";
                    return INVALID_OPERAND;
                }
            }
            case INC:
            case DEC:
            {
                if (!QStringList{ "ACC", "IX" }.contains(instruction.operand))
                {
                    LOG("INC and DEC expect \"ACC\" or \"IX\" as the operand.");
                    *errorMessage = "INC and DEC expect \"ACC\" or \"IX\" as the operand.";
                }
                return MEMORY_LOCATION;
            }

            case LDD:
            case LDI:
            case LDX:
            case STO:
            case STI:
            case STX:
            {
                if (!operand.startsWith("#"))
                    return MEMORY_LOCATION;
                else
                {
                    LOG("LDD, LDI, LDX, STO, STI, STX expect an address as the operand.");
                    *errorMessage = "LDD, LDI, LDX, STO, STI, STX expect an address as the operand.";
                    return INVALID_OPERAND;
                }
            }

            case JMP:
            case JPE:
            case JPN:
            {
                if (!operand.startsWith("#"))
                    return LABEL;
                else
                {
                    LOG("JMP JPE and JPN expect an address as the operand.");
                    *errorMessage = "JMP JPE and JPN expect an address as the operand.";
                    return INVALID_OPERAND;
                }
            }
            case IN:
            case OUT:
            case END:
            {
                if (!operand.isEmpty())
                {
                    LOG("Dropped useless operand after IN OUT and END");
                }
                return NO_OPERAND;
            }

            case ADD:
            case AND:
            case OR:
            case CMP:
            case XOR:
            {
                return operand.startsWith("#") ? DetectNumberType(operand) : MEMORY_LOCATION;
            }
            default:
            {
                return INVALID_OPERAND;
            }
        }
    }

    QString NumberToString(int num)
    {
        switch (Base)
        {
            case BASE2: return "#b" + QString::number(num, 2);
            case BASE16: return "#&" + QString::number(num, 16);
            case ASCII: return "\"" + QString(QChar::fromLatin1(num)) + "\"";
            case BASE10: return "#" + QString::number(num, 10);
            default: return "Unknown";
        }
    }

    QString ExecuteSingleInstruction(const CIEAssemblyInstruction &instruction, QString *errorMessage, QStringList *changedMemory)
    {
        auto operandType = DeduceOperandType(instruction, errorMessage);
        if (operandType == INVALID_OPERAND || !errorMessage->isEmpty())
        {
            QMessageBox::warning(qApp->activeWindow(), "Error", *errorMessage);
            return {};
        }
        //
        auto operand = instruction.operand;
        long operandNumber;
        switch (operandType)
        {
            case LABEL:
            case NO_OPERAND:
            case MEMORY_LOCATION:
            {
                break;
            }
            case NUMBER_BASE2:
            {
                operandNumber = operand.remove(0, 2).toLong(nullptr, 2);
                break;
            }
            case NUMBER_BASE10:
            {
                operandNumber = operand.remove(0, 1).toLong(nullptr, 10);
                break;
            }
            case NUMBER_BASE16:
            {
                operandNumber = operand.remove(0, 2).toLong(nullptr, 16);
                break;
            }
            case INVALID_OPERAND:
            {
                Q_UNREACHABLE();
                break;
            }
        }
        //
        switch (instruction.opcode)
        {
            // -------------------------- Data Movement Instructions --------------------------
            case LDM:
            {
                *changedMemory << "ACC";
                ACC = operandNumber;
                break;
            }
            case LDD:
            {
                *changedMemory << "ACC";
                ACC = MEMORY[operand];
                break;
            }
            case LDI: break;
            case LDX:
            {
                *changedMemory << "ACC";
                ACC = MEMORY[operand + (IX == 0 ? "" : ("+" + QString::number(IX)))];
                break;
            }
            case LDR:
            {
                *changedMemory << "IX";
                IX = operandNumber;
                break;
            }
            case STO:
            {
                *changedMemory << operand;
                MEMORY[operand] = ACC;
                break;
            }
            case STX:
            {
                auto addr = operand + (IX == 0 ? "" : ("+" + QString::number(IX)));
                *changedMemory << addr;
                MEMORY[addr] = ACC;
                break;
            }
            case STI:
                break;
                // :------------------------- Arithmetic Operations --------------------------
            case ADD:
            {
                *changedMemory << operand;
                ACC += MEMORY[operand];
                break;
            }
            case INC:
            {
                *changedMemory << operand;
                operand == "ACC" ? ACC++ : IX++;
                break;
            }
            case DEC:
            {
                *changedMemory << operand;
                operand == "ACC" ? ACC-- : IX--;
                break;
            }
                // :------------------------- Comparision and Jump Instructions --------------------------
            case JMP:
            {
                return operand;
            }
            case CMP:
            {
                auto operand2 = (operandType == MEMORY_LOCATION) ? MEMORY[operand] : operandNumber;
                COMPARE_RESULT = (ACC > operand2) ? CIEAssemblyCompareResult::RESULT_ARG1 : (ACC == operand2) ? RESULT_EQUAL : RESULT_ARG2;
                break;
            }
            case JPE:
            {
                return COMPARE_RESULT == RESULT_EQUAL ? operand : "";
            }
            case JPN:
            {
                return COMPARE_RESULT != RESULT_EQUAL ? operand : "";
            }
                // :------------------------- Input/Output Instructions --------------------------
            case IN:
            {
                *changedMemory << "ACC";
                QString buf;
                while (buf.isEmpty())
                {
                    buf = QInputDialog::getText(qApp->activeWindow(), "Input", "Input");
                }
                ACC = buf.at(0).toLatin1();
                break;
            }
            case OUT:
            {
                QMessageBox::information(qApp->activeWindow(), "Program Output", NumberToString(ACC));
                break;
            }
                // :------------------------- Bitwise Instructions --------------------------
            case LSL:
            {
                *changedMemory << "ACC";
                ACC = ACC << operandNumber;
                break;
            }
            case LSR:
            {
                *changedMemory << "ACC";
                ACC = ACC >> operandNumber;
                break;
            }
            case AND:
            {
                *changedMemory << "ACC";
                auto operand2 = (operandType == MEMORY_LOCATION) ? MEMORY[operand] : operandNumber;
                ACC = operand2 & ACC;
                break;
            }
            case XOR:
            {
                *changedMemory << "ACC";
                auto operand2 = (operandType == MEMORY_LOCATION) ? MEMORY[operand] : operandNumber;
                ACC = operand2 ^ ACC;
                break;
            }
            case OR:
            {
                *changedMemory << "ACC";
                auto operand2 = (operandType == MEMORY_LOCATION) ? MEMORY[operand] : operandNumber;
                ACC = operand2 | ACC;
                break;
            }
            // :------------------------- END Instructions --------------------------
            case END: return {};
            default:
            {
                // Should not touch this line.
                auto msg = "Assembly instruction \"" + EnumToString(instruction.opcode) + "\" is not supported ";
                Q_ASSERT_X(false, Q_FUNC_INFO, msg.toStdString().c_str());
            }
        }
        return {};
    }
} // namespace CIEAssembly
