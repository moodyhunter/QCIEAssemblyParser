#pragma once
#include <QMetaEnum>
#include <QString>
#include <iostream>
#include <string>

template<typename ENUM_TYPE>
ENUM_TYPE StringToEnum(const QString &str)
{
    return (ENUM_TYPE) QMetaEnum::fromType<ENUM_TYPE>().keyToValue(str.toStdString().c_str());
}

template<typename ENUM_TYPE>
QString EnumToString(const ENUM_TYPE &data)
{
    return QMetaEnum::fromType<ENUM_TYPE>().key(data);
}

inline void LOG(const QString &msg)
{
    std::cout << msg.toStdString() << std::endl;
}

template<typename T>
inline void LOG(const T &data)
{
    std::cout << data << std::endl;
}

namespace CIEAssembly
{
    Q_NAMESPACE
    enum CIEAssemblyOpcode
    {
        // -------------------------- Data Movement Instructions --------------------------
        /// Immediate addressing. Load the number n to ACC
        LDM,
        /// Direct addressing. Load the contents of the location at the given address to ACC
        LDD,
        /// Indirect addressing. The address to be used is at the given address. Load the contents of this second address to ACC
        LDI,
        /// Indexed addressing. Form the address from <address> + the contents of the index register. Copy the contents of this calculated
        /// address to ACC
        LDX,
        /// Immediate addressing. Load the number n to IX
        LDR,
        /// Store the contents of ACC at the given address
        STO,
        /// Indexed addressing, from the address from <address> + the contents of the index register. Copy the contents of ACC to this calculated
        /// address
        STX,
        /// Indirect addressing, The address to be used is at the given address. Store the contents of ACC at this second address.
        STI,
        // -------------------------- Arithmetic Operations --------------------------
        /// Add the contents of the given address to the ACC
        ADD,
        /// Add 1 to the contents of the register (ACC or IX)
        INC,
        /// Subtract 1 from the contents of the register (ACC or IX)
        DEC,
        // -------------------------- Comparision and Jump Instructions --------------------------
        /// Jump to the given address
        JMP,
        /// Compare the contents of ACC with the contents of <address>
        /// Compare the contents of ACC with number n
        CMP,
        /// Following a compare instruction, jump to <address> if the compare was True
        JPE,
        /// Following a compare instruction, jump to <address> if the compare was False
        JPN,
        // -------------------------- Input/Output Instructions --------------------------
        /// Key in a character and store its ASCII value in ACC
        IN,
        /// Output to the screen the character whose ASCII value is stored in ACC
        OUT,
        // -------------------------- Bitwise Instructions --------------------------
        /// Bitwise AND operation of the contents of ACC with the oprand
        AND,
        /// Bitwise XOR operation of the contents of ACC with the oprand
        XOR,
        /// Bitwise OR operation of the contents of ACC with the oprand
        OR,
        /// Bits in ACC are shifted n places to the left. Zeros are introduced on the right hand end
        LSL,
        /// Bits in ACC are shifted n places to the right. Zeros are introduced on the left hand end
        LSR,
        // -------------------------- END Instructions --------------------------
        //// Return control to the operating system
        END
    };

    Q_ENUM_NS(CIEAssemblyOpcode)

    enum NumberBase
    {
        BASE10,
        BASE16,
        BASE2,
        ASCII
    };

    enum CIEAssemblyOperandType
    {
        NUMBER_BASE10,
        NUMBER_BASE16,
        NUMBER_BASE2,
        LABEL,
        MEMORY_LOCATION,
        NO_OPERAND,
        INVALID_OPERAND = -2
    };

    enum CIEAssemblyCompareResult
    {
        RESULT_ARG1 = -1,
        RESULT_ARG2 = 1,
        RESULT_EQUAL = 0
    };

    struct CIEAssemblyInstruction
    {
        QString label;
        CIEAssemblyOpcode opcode;
        QString operand;
        const QString toString() const
        {
            return label + " - " + EnumToString(opcode) + (operand.isEmpty() ? "" : (":" + operand));
        }
    };
} // namespace CIEAssembly
