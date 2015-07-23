#ifndef PHOTOSTAGE_NULLABLE_H
#define PHOTOSTAGE_NULLABLE_H

#include <cstddef>

template <typename T>
class Nullable final
{
    template <typename T2>
    friend bool operator==(const Nullable<T2>& op1, const Nullable<T2>& op2);

    template <typename T2>
    friend bool operator==(const Nullable<T2>& op, const T2& value);

    template <typename T2>
    friend bool operator==(const T2& value, const Nullable<T2>& op);

    template <typename T2>
    friend bool operator==(const Nullable<T2>& op, std::nullptr_t nullpointer);

    template <typename T2>
    friend bool operator==(std::nullptr_t nullpointer, const Nullable<T2>& op);

    public:

        Nullable();
        Nullable(const T& value);
        Nullable(std::nullptr_t nullpointer);
        const Nullable<T>& operator=(const Nullable<T>& value);

        const Nullable<T>& operator=(const T& value);

        const Nullable<T>& operator=(std::nullptr_t nullpointer);

    private:

        bool m_hasValue;
        T    m_value;

    public:

        const T& Value;
};

#endif // PHOTOSTAGE_NULLABLE_H
